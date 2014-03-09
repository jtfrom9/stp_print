#include <stdio.h>

#include "stp_print.h"

typedef enum {
    A = 1 << 0,
    B = 1 << 1,
    C = 1 << 2,
} enum_t;

typedef struct {
    int sub_dat1;
    int sub_dat2;
    int sub_dat3;
} sub_t;

typedef struct {
    int data_int;
    char c;
    void *addr;
    enum_t e;
    sub_t sub;
    char* text;
} data_t;

void push_str(char* buf, int expr, char* val, int* flag)
{
    if(expr) {
        if(*flag) sprintf(buf,"%s | ", buf);
        *flag = 1;
        sprintf(buf,"%s%s",buf,val);
    }
}

int print_enum_t(char* buf, int bufsize, int v)
{
    char msg[32];
    enum_t e = (enum_t)v;
    memset(msg,0,sizeof(msg));
    int flag=0;

    sprintf(msg, "%s{ ", msg);
    push_str(msg,(e&A),"A",&flag);
    push_str(msg,(e&B),"B",&flag);
    push_str(msg,(e&C),"C",&flag);
    sprintf(msg, "%s }",msg);

    return snprintf(buf,bufsize,"%s",msg);
}

st_print_define(sub_t, 4) {
    st_print_field_int(sub_dat1);
    st_print_field_uint(sub_dat2);
    st_print_field_int(sub_dat3);
    st_print_field(sub_dat2,int,"0x%x");
    st_print_end();
}

st_print_define(data_t, 6) {
    st_print_field_int(data_int);
    st_print_field_struct(sub,sub_t);
    st_print_field_char(c);
    st_print_field_ptr(addr,void);
    st_print_field_any(e,enum_t,print_enum_t);
    st_print_field_pchar(text);
    st_print_end();
} 

void dump_data(data_t* ptr)
{
    char msg[1024];
    st_print(data_t,msg,sizeof(msg),ptr,"data");
    printf("%s",msg);
}

int main()
{
    int hoge;
    data_t d = { 1, 'x' , &hoge, A|C,
                 { 123, -1, 0xa },
                 "hogehoge" };

    dump_data(&d);
    return 0;
}
