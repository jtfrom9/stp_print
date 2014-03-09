#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "stp_print.h"

#define update_ctx(ctx,bytes) {                 \
        ctx->buf+=bytes;                        \
        ctx->buf_size -= bytes; }

static char* indent(char* buf,
                    int buf_size,
                    const char* msg,
                    int level)
{
    memset(buf,0,buf_size);
    for(int i=0; i<level; i++) {
        strcat(buf,"  ");
    }
    strcat(buf,msg);
    return buf;
}

static int print_value_field(stp_print_context_t* ctx,
                             stp_print_table_t* table,
                             int level,
                             const char* line_fmt)
{
    int bytes = 0;
    char valbuf[32];
    char name_col[32];

    switch(table->type) {
    case stp_int:
        snprintf(valbuf,
                 sizeof(valbuf),
                 table->format,
                 table->v.v_int);
        break;
    case stp_char: 
        snprintf(valbuf,
                 sizeof(valbuf),
                 table->format,
                 table->v.v_char);
        break;
    case stp_ptr:
        snprintf(valbuf,
                 sizeof(valbuf),
                 table->format,
                 table->v.v_ptr);
        break;
    case stp_any:
        table->v.v_any.func(valbuf,
                            sizeof(valbuf),
                            table->v.v_any.v);
        break;
    }

    bytes = snprintf(ctx->buf,
                     ctx->buf_size,
                     line_fmt,
                     // name
                     indent(name_col, sizeof(name_col),
                            table->name, 
                            level),
                     // type
                     table->type_name,
                     // value
                     valbuf);

    update_ctx(ctx,bytes);
    return bytes;
}

static int print_struct_field(stp_print_context_t* ctx,
                              stp_print_table_t* table,
                              int level)
{
    int bytes = 0;
    bytes = table->v.v_struct.func(ctx->buf,
                                   ctx->buf_size,
                                   table->v.v_struct.ptr,
                                   table->name,
                                   level,
                                   ctx->max_name_colum_width);
    update_ctx(ctx,bytes);
    return bytes;
}

static int print_top(stp_print_context_t* ctx, 
                     int level,
                     const char* line_fmt)
{
    int bytes;
    char namebuf[16];
    char valbuf[16];
    sprintf(valbuf,"(%p)",ctx->data);
    bytes = snprintf(ctx->buf, ctx->buf_size,
                     line_fmt,
                     indent(namebuf,sizeof(namebuf),ctx->data_name,level),
                     ctx->data_type_name,
                     valbuf);
    update_ctx(ctx,bytes);
    return bytes;
}

int stp_do_print(stp_print_context_t* ctx, int level)
{
    char line_fmt[32];
    int bytes = 0;
    int n;
    int flag=0;
    memset(ctx->buf, 0, ctx->buf_size);

    snprintf(line_fmt,
             sizeof(line_fmt),
             "%%-%0ds  %%-12s  %%-s\n",
             ctx->max_name_colum_width);
    
    bytes += print_top(ctx,level,line_fmt);
    for(int i=0; i<ctx->num_field; i++) {
        if(ctx->table[i].type!=stp_struct) {
            n = print_value_field(ctx,
                                  &ctx->table[i],
                                  level+1,
                                  line_fmt);
        } else {
            n = print_struct_field(ctx,
                                   &ctx->table[i],
                                   level+1);
        }
        bytes += n;
    }
    return bytes;
}

int stp_do_print_pchar_val(char* buf, int bufsize, int v)
{
    return snprintf(buf,bufsize,"\"%s\"",(char*)v);
}

int stp_do_print_ptr(char* buf, int bufsize, int v)
{
    return snprintf(buf,bufsize, "%p", (void*)v);
}
