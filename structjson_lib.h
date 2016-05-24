#ifndef JCEJSON_LIB_H

#define JCEJSON_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

    enum {
        TYPE_INT = 1,
        TYPE_UINT,
        TYPE_STRING,
        TYPE_CUSTOM,
        TYPE_VECTOR,
    };

    #define NAME_SIZE 128

    typedef struct tag_symbol
    {
        char name[NAME_SIZE];
    } symbol_t;
    symbol_t *new_symbol(char *name);

    typedef struct tag_type
    {
        symbol_t *name;
        int type;
        int vectype;
    } type_t;
    type_t *new_type(int type, int vectype, symbol_t *name);

    typedef struct tag_member
    {
        symbol_t *name;
        type_t *type;
        struct tag_member *next;
    } member_t;
    member_t *new_member(type_t *type, symbol_t *name);
    member_t *merge_member(member_t *l, member_t *r);

    typedef struct tag_struct
    {
        symbol_t *name;
        member_t *members;
        struct tag_struct *next;
    } struct_t;
    struct_t *new_struct(symbol_t *name, member_t *mbr);
    struct_t *merge_struct(struct_t *l, struct_t *r);

    typedef struct tag_module
    {
        symbol_t *name;
        struct_t *structs;
    } module_t;
    module_t *new_module(symbol_t *name, struct_t *srt);

int yylex();
void yyerror(char *s, ...);

module_t *print_module(module_t *m);
int translate(module_t *m);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: JCEJSON_LIB_H */
