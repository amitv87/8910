/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
  *@brief   : eyblib_algorithm.h
  *@notes   : 2020.08.04 MWY
*******************************************************************************/
#ifndef __EYBLIB_ALGORITHM_H_
#define __EYBLIB_ALGORITHM_H_

#include "eyblib_typedef.h"
#define BINARY_FIND(addr)       (void*)addr, (void*)(&addr + 1), sizeof(addr[0])    //

void *ALG_binaryFind(u16_t arm, void *srcStart, void *srcEnd, u16_t onceSize);

#endif //__EYBLIB_ALGORITHM_H_
/*******************************************************************************/

