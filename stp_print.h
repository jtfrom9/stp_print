#ifndef ST_PRINT_H
#define ST_PRINT_H

#include<string.h>

typedef int (*st_print_any_func)(char* buf, int bufsize, int v);
typedef int (*st_print_func)(char* buf, int buf_size, 
                             void* data, 
                             char* data_name,
                             int level, 
                             int max_name_colum_width);
typedef struct {
    char* name;
    char* type_name;
    enum {
        stp_int,
        stp_char,
        stp_ptr,
        stp_any,
        stp_struct,
    } type;
    union {
        int v_int;
        char v_char;
        void* v_ptr;
        struct {
            int v;
            st_print_any_func func;
        } v_any;
        struct {
            void* ptr;
            st_print_func func;
        } v_struct;
    } v;
    char* format;
} st_print_table_t;

typedef struct {
    const char* buf_top;
    int buf_total_size;
    char* buf;
    int buf_size;
    st_print_table_t *table;
    int num_field;
    int index;
    void* data;
    const char* data_name;
    const char* data_type_name;
    int max_name_colum_width;
} st_print_context_t;

int st_do_print(st_print_context_t* ctx, int level);
int st_do_print_pchar_val(char* buf, int bufsize, int v);

#define DEFAULT_MAX_NAME_COLUMN_WIDTH (24)

#define st_print(type, buf, buf_total_size, data, data_name)    \
    st_print_ ## type(buf,                                      \
                     buf_total_size,                            \
                     data,                                      \
                     data_name,                                 \
                     0,                                         \
                     DEFAULT_MAX_NAME_COLUMN_WIDTH)

#define st_print_define(TYPE, NUM_FIELD)                            \
    int st_print_ ## TYPE(char* buf, int buf_total_size,            \
                          void* data, char* data_name,              \
                          int level, int max_name_colum_width) {    \
    st_print_context_t ctx_;                                        \
    st_print_context_t* ctx = &ctx_;                                \
    st_print_table_t stp_table[NUM_FIELD];                          \
    TYPE* data_ptr = (TYPE*)data;                                   \
    memset(stp_table,0,sizeof(stp_table));                          \
    ctx->buf_top              = buf;                                \
    ctx->buf_total_size       = buf_total_size;                     \
    ctx->buf                  = buf;                                \
    ctx->buf_size             = buf_total_size;                     \
    ctx->table                = &stp_table[0];                      \
    ctx->num_field            = NUM_FIELD;                          \
    ctx->index                = 0;                                  \
    ctx->data                 = data;                               \
    ctx->data_name            = data_name;                          \
    ctx->data_type_name       = #TYPE;                              \
    ctx->max_name_colum_width = max_name_colum_width;

#define st_print_end()                          \
    ctx->num_field = ctx->index;                \
    return st_do_print(ctx,level);              \
    }

#define st_print_field_next(FIELD)              \
    ctx->table[ctx->index].name = #FIELD;       \
    ctx->index++;

#define st_print_field(FIELD,KIND,FMT)                                  \
    if(ctx->index < ctx->num_field) {                                   \
        ctx->table[ctx->index].type = stp_ ## KIND;                     \
        ctx->table[ctx->index].v.v_ ## KIND = (KIND)((data_ptr)->FIELD); \
        ctx->table[ctx->index].format = FMT;                            \
        ctx->table[ctx->index].type_name = #KIND;                       \
        st_print_field_next(FIELD); }

#define st_print_field_int(FIELD)  st_print_field(FIELD,int,"%0d")
#define st_print_field_uint(FIELD) st_print_field(FIELD,int,"%0u")
#define st_print_field_char(FIELD) st_print_field(FIELD,char,"\'%c\'")

#define st_print_field_ptr(FIELD,KIND)                                  \
    if(ctx->index < ctx->num_field) {                                   \
        ctx->table[ctx->index].type = stp_ptr;                          \
        ctx->table[ctx->index].v.v_ptr = (void*)((data_ptr)->FIELD);    \
        ctx->table[ctx->index].format = "%p";                           \
        ctx->table[ctx->index].type_name = #KIND "*";                   \
        st_print_field_next(FIELD) }

#define st_print_field_any(FIELD,KIND,FUNC)                             \
    if(ctx->index < ctx->num_field) {                                   \
        ctx->table[ctx->index].type = stp_any;                          \
        ctx->table[ctx->index].v.v_any.v = (int)((data_ptr)->FIELD);    \
        ctx->table[ctx->index].v.v_any.func = FUNC;                     \
        ctx->table[ctx->index].type_name = #KIND;                       \
        st_print_field_next(FIELD) }

#define st_print_field_pchar(FIELD) st_print_field_any(FIELD,char*,st_do_print_pchar_val);
    
#define st_print_field_struct(FIELD,DATA_TYPE)                          \
    if(ctx->index < ctx->num_field) {                                   \
        ctx->table[ctx->index].type = stp_struct;                       \
        ctx->table[ctx->index].v.v_struct.ptr  = (void*)(&(data_ptr)->FIELD); \
        ctx->table[ctx->index].v.v_struct.func = (st_print_func)st_print_ ## DATA_TYPE; \
        ctx->table[ctx->index].type_name = #DATA_TYPE;                  \
        st_print_field_next(FIELD);                                     \
    }

#endif
