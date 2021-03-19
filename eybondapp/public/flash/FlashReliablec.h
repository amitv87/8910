/**********************    COPYRIGHT 2014-2015,QIYI TEAM   ************************ 
  * @File	 : FlashReliable
  * @Author  : Unarty
  * @Date	 : 2017-04-15
  * @Brief	 : Flash数据可靠存储，注意一般页为单位
**********************************************************************************/
#ifndef __HISTORY_RELIABLE_H
#define __HISTORY_RELIABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"




typedef struct 
{
	u16_t updateTime;	//
	u16_t userSize;	//使用长度
	u32_t resave;	//保留区域
}FlashRecrode_t;  //flash可靠存储标记结构体

typedef struct
{
	const u32_t	startAddr;	//
	const u32_t	areaSize;	
	u32_t	userAddr;	
	FlashRecrode_t recrode;
}FlashReliable_t;



u8_t FlashReliable_init(FlashReliable_t *head, u32_t addr, u32_t size);
int FlashReliable_write(FlashReliable_t *head, u16_t offset, Buffer_t *buf);
int FlashReliable_read(FlashReliable_t *head, u16_t offset, Buffer_t *buf);

#ifdef __cplusplus
    }
#endif

#endif  //__HISTORY_RELIABLE_H

/******************************************************************************/

