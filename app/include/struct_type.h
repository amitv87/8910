#ifndef __STRUCT_TYPE_H__
#define __STRUCT_TYPE_H__

#include "fibo_opencpu_comm.h"

typedef struct _ParamDescripTable_t{
    UINT8 num;                                                  
    char a[64];                                                
    INT8 (*wFunc)(struct _ParamDescripTable_t *, void *, UINT16 *);    
    INT8 (*rFunc)(struct _ParamDescripTable_t *, void *, UINT16 *);    
}name;

#endif // __STRUCT_TYPE_H__