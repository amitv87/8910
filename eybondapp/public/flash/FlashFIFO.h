/**********************    COPYRIGHT 2014-2015, EYBOND    ************************ 
  * @File	 : historySave.c
  * @Author  : Unarty
  * @Date	 : 2017-03-28
  * @Brief	 : 2017.04.08 Unarty 将其抽像成统一库。注: 前后尾指针存储空间默认占用
  				一个扇区的存储区域，最大单位存储区域为65536个
**********************************************************************************/
#ifndef __HISTORY_SAVE_H
#define __HISTORY_SAVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "FlashEquilibria.h"


#define POINT_SAVE_AREA_SIZE		(Flash_pageSize())	//指针存储区域大小
   



typedef struct
{
    u32_t size;      //结束地址
	u32_t addr;	    //起始地址
    FlashEquilibria_t frontAddr;	//前指针
    u32_t front;    //前指针地址
    FlashEquilibria_t rearAddr;	//尾指针
    u32_t rear;     //尾指针地址 
}FlashFIFOHead_t; //Flash队列存储结构体

void FlashFIFO_clear(FlashFIFOHead_t *head);
void FlashFIFO_init(FlashFIFOHead_t *head, u32_t startAddr, u32_t areaSize);
void FlashFIFO_put(FlashFIFOHead_t *head, Buffer_t *buf);
u16_t FlashFIFO_get(FlashFIFOHead_t *head, Buffer_t *buf);
u16_t FlashFIFO_see(FlashFIFOHead_t *head, Buffer_t *buf);

#ifdef __cplusplus
    }
#endif

#endif 

/******************************************************************************
 End of the File (EOF). Do not put anything after this part!
******************************************************************************/
