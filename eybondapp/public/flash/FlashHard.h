/**********************    COPYRIGHT 2014-2015, EYBOND    ************************ 
  * @File	 : FlashHard.h
  * @Author  : Unarty
  * @Date	 : 2017-04-15
  * @Brief	 : Flash硬件接口定义 
**********************************************************************************/
#ifndef __HISTORY_HARD_H
#define __HISTORY_HARD_H

#include "typedef.h"

/*****Flash area map ***************/
#define FLASH_TEST_ADDR			(0x00000000)
#define FLASH_TEST_SIZE			(0x00001000)

/***update file save area*/
#define FLASH_UPDATE_FILE_ADDR	(0x00002000)
#define FLASH_UPDATE_FILE_SIZE	(0x00380000)


/**system para save area*/
#define FLASH_SYSPARA_ADDR		(0x00480000)
#define FLASH_SYSPARA_SIZE		(0x00010000)

/*eybond hoistory data area*/
#define FLASH_EYBOND_HISTORY_ADDR	(0x00500000)
#define FLASH_EYBOND_HISTORY_SIZE	(0x00100000)

/*Hanergy hoistory data area*/
#define FLASH_HANERGY_HISTORY_ADDR	(0x00600000)
#define FLASH_HANERGY_HISTORY_SIZE	(0x00100000)

/*run log save area*/
#define FLASH_LOG_DATA_ADDR		(0x00700000) //log recode flash start addr
#define FLASH_LOG_DATA_SIZE		(0x00100000) //log recode flash user size

/*****************************/


u32_t Flash_pageSize(void);
u8_t  Flash_defaultVal(void);
void  Flash_read(u32_t addr, u32_t size, u8_t *buf);
void  Flash_write(u32_t addr, u32_t size, u8_t *buf);
void  Flash_earsePage(u32_t addr);
void  Flash_earseArea(u32_t addr, u32_t size);
void  *Flash_ramMalloc(u32_t size);
void  Flash_ramFree(void *addr);
u16_t Flash_ramCRC(u8_t *buf, u32_t len);
void  Flash_ramset(void *dest, u8_t val, u32_t len);
void  Flash_ramcpy(void *dest, void const *val, u32_t len);
int   Flash_ramcmp(void *dest, void const *val, u32_t len);

#endif  //__HISTORY_HARD_H

/******************************************************************************/

