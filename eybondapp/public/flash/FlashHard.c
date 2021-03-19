/**********************    COPYRIGHT 2014-2015, EYBOND    ************************ 
  * @File	 : FlashHard.c
  * @Author  : Unarty
  * @Date	 : 2017-09-02
  * @Brief	 : Flash硬件接口定义 
**********************************************************************************/
#include "x25qxx.h"
#include "memory.h"
#include "crc.h"
#include "r_stdlib.h"

#define FLASH_PAGE_SIZE         (0x1000)	
#define FLASH_DEFAULT_VALUE		(0xFF)

#define FLASH_READ(falshAddr, size, ramAddr)		x25Qxx_read(falshAddr, size, ramAddr)	
#define FLASH_WRITE(falshAddr, size, ramAddr)		x25Qxx_wrtie(falshAddr, size, ramAddr)
#define FLASH_ERASE_AREA(falshAddr, len)            x25Qxx_earseBlock(falshAddr, len)
#define FLASH_ERASE_PAGE(falshAddr)                 x25Qxx_earse(falshAddr)
#define FLASH_RAM_MALLOC(size)						memory_apply(size)
#define FLASH_RAM_FREE(addr)		                memory_release(addr)
#define FLASH_RAM_CRC(buf, len)	                    crc16_standard(CRC_CCITI, buf, len)
#define FLASH_RAM_SET(dest, val, len)	            r_memset(dest, val, len)
#define FLASH_RAM_COPY(dest, src, len)              r_memcpy(dest, src, len)
#define FLASH_RAM_CMP(dest, src, len)               r_memcmp(dest, src, len)


/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
u32_t Flash_pageSize(void)
{
    return FLASH_PAGE_SIZE;
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
u8_t Flash_defaultVal(void)
{
    return FLASH_DEFAULT_VALUE;
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
void Flash_read(u32_t addr, u32_t size, u8_t *buf)
{
    FLASH_READ(addr, size, buf);
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
void Flash_write(u32_t addr, u32_t size, u8_t *buf)
{
    FLASH_WRITE(addr, size, buf);
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/    
void Flash_earsePage(u32_t addr)
{
    FLASH_ERASE_PAGE(addr);
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
void Flash_earseArea(u32_t addr, u32_t size)
{
    FLASH_ERASE_AREA(addr, size);
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/    
void *Flash_ramMalloc(u32_t size)
{
    return FLASH_RAM_MALLOC(size);
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/    
void Flash_ramFree(void *addr)
{
    FLASH_RAM_FREE(addr);
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/    
u16_t Flash_ramCRC(u8_t *buf, u32_t len)
{
    return FLASH_RAM_CRC(buf, len);
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
void Flash_ramset(void *dest, u8_t val, u32_t len)
{
    FLASH_RAM_SET(dest, val, len);
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/    
void Flash_ramcpy(void *dest, void const *val, u32_t len)
{
    FLASH_RAM_COPY(dest, val, len);
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/    
int Flash_ramcmp(void *dest, void const *val, u32_t len)
{
    return FLASH_RAM_CMP(dest, val, len);
}

/******************************************************************************/

