#ifndef STP_PRINT_H
#define STP_PRINT_H

#include<stdint.h>
#include<string.h>

typedef int (*stp_print_value_func)(char* buf, int buf_size, void* ptr, char* fmt);
typedef int (*stp_print_func)(char* buf, int buf_size, 
                              void* data, 
                              char* data_name,
                              char* data_type_name,
                              int level, 
                              int max_name_colum_width);
typedef struct {
    char* name;
    char* type_name;
    uint8_t is_value;
    union {
        struct {
            void* ptr;
            stp_print_value_func func;
        } val;
        struct {
            void* ptr;
            stp_print_func func;
        } st;
    } v;
    char* format;
} stp_print_table_t;

typedef struct {
    char* buf;
    int buf_size;
    stp_print_table_t *table;
    int num_field;
    int index;
    void* data;
    const char* data_name;
    const char* data_type_name;
    int max_name_colum_width;
} stp_print_context_t;

extern int stp_do_print(stp_print_context_t* ctx, int level);

extern int stp_do_print_int(char* buf, int buf_size, void* ptr, char* fmt);
extern int stp_do_print_short(char* buf, int buf_size, void* ptr, char* fmt);
extern int stp_do_print_char(char* buf, int buf_size, void* ptr, char* fmt);

extern int stp_do_print_pchar_val(char* buf, int buf_size, void* ptr, char* fmt);
extern int stp_do_print_ptr(char* buf, int buf_size, void* ptr, char* fmt);


#define DEFAULT_MAX_NAME_COLUMN_WIDTH (24)

#define stp_snprintf(TYPE, buf, buf_total_size, data, data_name, level) \
    stp_print_ ## TYPE(buf,                                             \
                       buf_total_size,                                  \
                       data,                                            \
                       data_name,                                       \
                       #TYPE,                                           \
                       level,                                           \
                       DEFAULT_MAX_NAME_COLUMN_WIDTH)

#define stp_print_begin(TYPE, NUM_FIELD)                            \
    int stp_print_ ## TYPE(char* buf, int buf_total_size,           \
                           void* data,                              \
                           char* data_name,                         \
                           char* data_type_name,                    \
                           int level, int max_name_colum_width) {   \
    stp_print_context_t ctx_;                                       \
    stp_print_context_t* ctx = &ctx_;                               \
    stp_print_table_t stp_table[NUM_FIELD];                         \
    TYPE* data_ptr = (TYPE*)data;                                   \
    memset(stp_table,0,sizeof(stp_table));                          \
    ctx->buf                  = buf;                                \
    ctx->buf_size             = buf_total_size;                     \
    ctx->table                = &stp_table[0];                      \
    ctx->num_field            = NUM_FIELD;                          \
    ctx->index                = 0;                                  \
    ctx->data                 = data;                               \
    ctx->data_name            = data_name;                          \
    ctx->data_type_name       = (data_type_name==NULL) ? #TYPE : data_type_name; \
    ctx->max_name_colum_width = max_name_colum_width;

#define stp_print_end()                         \
    ctx->num_field = ctx->index;                \
    return stp_do_print(ctx,level);             \
    }

#define stp_print_field_next(FIELD)             \
    ctx->table[ctx->index].name = #FIELD;       \
    ctx->index++;

#define stp_print_field_value(FIELD,KIND,FUNC,FMT)                      \
    if(data_ptr!=NULL && ctx->index < ctx->num_field) {                 \
        ctx->table[ctx->index].is_value     = 1;                        \
        ctx->table[ctx->index].v.val.ptr  = (void*)(&(data_ptr)->FIELD); \
        ctx->table[ctx->index].v.val.func = FUNC;                       \
        ctx->table[ctx->index].type_name    = #KIND;                    \
        ctx->table[ctx->index].format       = FMT;                      \
        stp_print_field_next(FIELD); }
    
#define stp_print_field_intf(FIELD,FMT) stp_print_field_value(FIELD,int32_t,stp_do_print_int,FMT)
#define stp_print_field_int(FIELD) stp_print_field_value(FIELD,int32_t,stp_do_print_int,"%0d")
#define stp_print_field_uintf(FIELD,FMT) stp_print_field_value(FIELD,uint32_t,stp_do_print_int,FMT)
#define stp_print_field_uint(FIELD) stp_print_field_value(FIELD,uint32_t,stp_do_print_int,"%0u")

#define stp_print_field_charf(FIELD,FMT) stp_print_field_value(FIELD,int8_t,stp_do_print_char,FMT)
#define stp_print_field_char(FIELD) stp_print_field_value(FIELD,int8_t,stp_do_print_char,"\'%c\'")
#define stp_print_field_ucharf(FIELD) stp_print_field_value(FIELD,uint8_t,stp_do_print_char,FMT)
#define stp_print_field_uchar(FIELD) stp_print_field_value(FIELD,uint8_t,stp_do_print_char,"%02x")

#define stp_print_field_pchar(FIELD)                                \
    stp_print_field_value(FIELD,char*,stp_do_print_pchar_val,NULL)

#define stp_print_field_ptr(FIELD,KIND)                     \
    stp_print_field_value(FIELD,KIND,stp_do_print_ptr,NULL)

#define stp_print_field_any(FIELD,KIND,FUNC)    \
    stp_print_field_value(FIELD,KIND,FUNC,NULL)

#define stp_print_field_struct(FIELD,DATA_TYPE)                         \
    if(data_ptr!=NULL && ctx->index < ctx->num_field) {                 \
        ctx->table[ctx->index].is_value        = 0;                     \
        ctx->table[ctx->index].v.st.ptr  = (void*)(&(data_ptr)->FIELD); \
        ctx->table[ctx->index].v.st.func = (stp_print_func)stp_print_ ## DATA_TYPE; \
        ctx->table[ctx->index].type_name       = #DATA_TYPE;            \
        stp_print_field_next(FIELD);                                    \
    }

#define stp_print_field_structp(FIELD,DATA_TYPE)                        \
    if(data_ptr!=NULL && ctx->index < ctx->num_field) {                 \
        ctx->table[ctx->index].is_value        = 0;                     \
        ctx->table[ctx->index].v.st.ptr  = (void*)(data_ptr)->FIELD;    \
        ctx->table[ctx->index].v.st.func = (stp_print_func)stp_print_ ## DATA_TYPE; \
        ctx->table[ctx->index].type_name       = #DATA_TYPE "*";        \
        stp_print_field_next(FIELD);                                    \
    }

#endif
