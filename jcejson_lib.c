
#include "jcejson_lib.h"

#ifdef DEBUG
#define debug(fmt, args...) fprintf(stderr, fmt, ##args)
#else
#define debug(fmt, args...) 
#endif

symbol_t *new_symbol(char *name)
{
    symbol_t *ret = calloc(1, sizeof(*ret));
    strncpy(ret->name, name, NAME_SIZE-1);
    debug("%s ret %p, name=%s\n", __func__, ret, ret->name);
    return ret;
}
type_t *new_type(int type, int vectype, symbol_t *name)
{
    type_t *ret = calloc(1, sizeof(*ret));
    ret->type = type;
    ret->vectype = vectype;
    ret->name = name;
    debug("%s ret %p\n", __func__, ret);
    return ret;
}
member_t *new_member(type_t *type, symbol_t *name)
{
    member_t *ret = calloc(1, sizeof(*ret));
    ret->type = type;
    ret->name = name;
    debug("%s ret %p\n", __func__, ret);
    return ret;
}
member_t *merge_member(member_t *l, member_t *r)
{
    debug("%s merge %p %p\n", __func__, l, r);
    if (!l)
        return r;
    member_t *ret = l;
    while (l->next)
    {
        l = l->next;
    }
    l->next = r;
    return ret;
}
struct_t *new_struct(symbol_t *name, member_t *mbr)
{
    struct_t *ret = calloc(1, sizeof(*ret));
    ret->name = name;
    ret->members = mbr;
    debug("%s ret %p\n", __func__, ret);
    return ret;
}
struct_t *merge_struct(struct_t *l, struct_t *r)
{
    debug("%s merge %p %p\n", __func__, l, r);
    if (!l)
        return r;
    struct_t *ret = l;
    while (l->next)
    {
        l = l->next;
    }
    l->next = r;
    return ret;
}
module_t *new_module(symbol_t *name, struct_t *srt)
{
    module_t *ret = calloc(1, sizeof(*ret));
    ret->name = name;
    ret->structs = srt;
    debug("%s ret %p\n", __func__, ret);
    return ret;
}


module_t *print_module(module_t *m)
{
    debug("module: %s\n", m->name->name);
    struct_t *srt = m->structs;
    while (srt)
    {
        debug("        ");
        debug("struct: %s\n", srt->name->name);
        member_t *mbr = srt->members;
        while (mbr)
        {
            debug("                ");
            debug("member: %s\n", mbr->name->name);
            mbr = mbr->next;
        }
        srt = srt->next;
    }
    return m;
}

