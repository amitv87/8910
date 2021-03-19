#ifndef __EYBPUB_STRUCT_TYPE_H__
#define __EYBPUB_STRUCT_TYPE_H__

#include "eyblib_typedef.h"

typedef struct
{
    int num;
    char para[64];
}DefaultPara_List_t;

typedef struct _ParamDescripTable_t{
    u8_t num;
    char a[64];
    u8_t (*wFunc)(struct _ParamDescripTable_t *, void *, u16_t *);
    u8_t (*rFunc)(struct _ParamDescripTable_t *, void *, u16_t *);
}name;

#endif // __EYBPUB_STRUCT_TYPE_H__