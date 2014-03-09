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

static int print_field_value(stp_print_context_t* ctx,
                             stp_print_table_t* table,
                             int level,
                             const char* line_fmt)
{
    int bytes = 0;
    char valbuf[32];
    char name_col[32];

    table->v.val.func(valbuf,
                      sizeof(valbuf),
                      table->v.val.ptr,
                      table->format);

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

static int print_field_struct(stp_print_context_t* ctx,
                              stp_print_table_t* table,
                              int level)
{
    int bytes = 0;
    bytes = table->v.st.func(ctx->buf,
                             ctx->buf_size,
                             table->v.st.ptr,
                             table->name,
                             table->type_name,
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
    if(ctx->data!=NULL)
        sprintf(valbuf,"(%p)",ctx->data);
    else
        sprintf(valbuf,"%p",ctx->data);

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

    memset(ctx->buf, 0, ctx->buf_size);

    snprintf(line_fmt,
             sizeof(line_fmt),
             "%%-%0ds  %%-12s  %%-s\n",
             ctx->max_name_colum_width);
    
    bytes += print_top(ctx,level,line_fmt);
    for(int i=0; i<ctx->num_field; i++) {
        if(ctx->table[i].is_value) {
            bytes += print_field_value(ctx,
                                       &ctx->table[i],
                                       level+1,
                                       line_fmt);
        } else {
            bytes += print_field_struct(ctx,
                                        &ctx->table[i],
                                        level+1);
        }
    }
    return bytes;
}

int stp_do_print_int(char* buf, int buf_size, void* ptr, char* fmt)
{
    return snprintf(buf,buf_size,fmt,*((int*)ptr));
}

int stp_do_print_short(char* buf, int buf_size, void* ptr, char* fmt)
{
    return snprintf(buf,buf_size,fmt,*((short*)ptr));
}

int stp_do_print_char(char* buf, int buf_size, void* ptr, char* fmt)
{
    return snprintf(buf,buf_size,fmt,*((char*)ptr));
}

int stp_do_print_string(char* buf, int buf_size, void* ptr, char* fmt)
{
    return snprintf(buf,buf_size,"\"%s\"",*((char**)ptr));
}

int stp_do_print_ptr(char* buf, int buf_size, void* ptr, char* fmt)
{
    return snprintf(buf,buf_size, "%p", *((void**)ptr));
}

