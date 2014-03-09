#include <stdio.h>
#include "stp_print.h"

typedef enum {
    A = 1 << 0,
    B = 1 << 1,
    C = 1 << 2,
    D = 1 << 3,
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
    push_str(msg,(e&D),"D",&flag);
    sprintf(msg, "%s }",msg);

    return snprintf(buf,bufsize,"%s",msg);
}

stp_print_define(sub_t, 4) {
    stp_print_field_int(sub_dat1);
    stp_print_field_uint(sub_dat2);
    stp_print_field_int(sub_dat3);
    stp_print_field(sub_dat2,int,"0x%x");
    stp_print_end();
}

stp_print_define(data_t, 6) {
    stp_print_field_int(data_int);
    stp_print_field_struct(sub,sub_t);
    stp_print_field_char(c);
    stp_print_field_ptr(addr,void*);
    stp_print_field_any(e,enum_t,print_enum_t);
    stp_print_field_pchar(text);
    stp_print_end();
} 

void print_data(data_t* ptr, int level)
{
    char msg[1024];
    printf("----\n");
    stp_snprintf(data_t,msg,sizeof(msg),ptr,"data", level);
    printf("%s",msg);
}

int main()
{
    int hoge;
    data_t d = {
        .data_int = 1, 
        .c        = 'x',
        .addr     = &hoge,
        .e        = A|C|D,
        .sub = { 
            .sub_dat1 = 123,
            .sub_dat2 = -1,
            .sub_dat3 = 0xa,
        },
        .text = "hogehoge" 
    };
    print_data(&d, 0);
    print_data(&d, 2);
    return 0;
}
