/**************Copyright(C)£¬2015-2026£¬QIYI Team *****************************
  *@brief   : x25Qxx.h flash 
  *@notes   : 2017.07.12  Unarty establish
*******************************************************************************/
#ifndef __X25QXX_H
#define __X25QXX_H	 

#include "Typedef.h"


#define X25Q_WRITE_EN           0x06
#define X25Q_WRITE_DIS		    0x04 
#define X25Q_READ_STATE 		0x05 
#define X25Q_WRITE_STATE		0x01 
#define X25Q_READ_DATA			0x03 
#define X25Q_WRITE_DATA  		0x02  
#define X25Q_BLOCK_ERASE		0xD8 
#define X25Q_SECTOR_ERASE		0x20 
#define X25Q_CHIP_ERASE			0xC7  
#define X25Q_POWER_DOWN			0xB9  
#define X25Q_DEVICE_ID			0xAB 



#define X25Q_SPI_PORT       (1)
#define X25Q_SPI_TYPE       (1)  //spi_usr_type = 0 is analog spi, spi_usr_type = 1 is hardware spi.

#define X25Q_CHIP_SIZE           (0x00800000)
#define X25Q_BLOCK_SIZE          (0x00010000)
#define X25Q_SECTOR_SIZE         (0x00001000)
#define X25Q_PAGE_SIZE           (0x00000100)
#define X25Q_READ_SIZE           (0x00000200)



void x25Qxx_init(void);
void x25Qxx_earse(u32_t addr);
void x25Qxx_earseBlock(u32_t addr, u32_t len);
void x25Qxx_read(u32_t addr, u32_t len, u8_t *payload);
void x25Qxx_wrtie(u32_t addr, u32_t len, u8_t *payload);

#endif //__X25QXX_H
/******************************************************************************/

