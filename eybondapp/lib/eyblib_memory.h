/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
  *@brief   : eyblib_memory.h
  *@notes   : 2020.08.04 MWY
*******************************************************************************/
#ifndef __EYBLIB_MEMORY_H_
#define __EYBLIB_MEMORY_H_

#include "eyblib_typedef.h"

void memory_init(void *memAddr, mcu_t memSize);
void *memory_apply(mcu_t size);
void memory_release(void *addr);
void memory_trans(void_fun_u8p_u16 fun);

#endif//__EYBLIB_MEMORY_H_
/******************************************************************************/

