/**********************    COPYRIGHT 2014-2015, EYBOND    ************************ 
  * @File	 : FlashEquilibria.h
  * @Author  : Unarty
  * @Date	 : 2017-04-23
  * @Brief	 : Flash Equilibria Write
**********************************************************************************/
#ifndef __FLASH_EPUILIBRIA_H
#define __FLASH_EPUILIBRIA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"




typedef struct 
{
	const u32_t 	allotAddr;	//allot address
	const u32_t   	allotSize;	//allot size
	const u8_t 	    size;		//struct size
	const u16_t		addr;    //next Write addr
}FlashEquilibria_t;  //




void FlashEquilibria_init(FlashEquilibria_t *head, u32_t areaAddr, \
                                     u32_t areaSize, mcu_t userSize, void *val);
void FlashEquilibria_clear(FlashEquilibria_t *head);
void FlashEquilibria_write(FlashEquilibria_t *head, void const * const val);
void FlashEquilibria_reliableWrite(FlashEquilibria_t *head, void const * const val);
void FlashEquilibria_read(FlashEquilibria_t *head, void *val);

#ifdef __cplusplus
    }
#endif

#endif  //__FLASH_EPUILIBRIA_H
