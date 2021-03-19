/**************Copyright(C)��2015-2026��QIYI Temp *****************************
  *@brief   : x25Qxx.c flash 
  *@notes   : 2017.07.12  Unarty establish
*******************************************************************************/
#include "x25Qxx.h"
#include "ql_type.h"
#include "ql_gpio.h"
#include "ql_spi.h"
#include "Debug.h"
#include "memory.h"
#include "r_stdlib.h"

static void writeEnable(void);
static u8_t status(void);
static u8_t busyWait(u32_t time);
static void pageRead(u32_t addr, u32_t len, u8_t *payload);
static void pageWrite(u32_t addr, u32_t len, u8_t *payload);
static void SPICS(u8_t val);


/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void x25Qxx_init(void)
{
	int ret;
	
	ret = Ql_SPI_Init(X25Q_SPI_PORT,PINNAME_PCM_IN,PINNAME_PCM_SYNC,PINNAME_PCM_OUT,PINNAME_PCM_CLK,X25Q_SPI_TYPE);
    if(ret <0)
    {
        APP_DEBUG("\r\n<-- Failed!! Ql_SPI_Init fail , ret =%d-->\r\n",ret)
    }
	ret = Ql_SPI_Config(X25Q_SPI_PORT, 1, 0, 0, 10000); //config sclk about 10MHz;
    if(ret <0)
    {
        APP_DEBUG("\r\n<--Failed!! Ql_SPI_Config fail  ret=%d -->\r\n",ret)
    }
	
	SPICS(1); //CS Hight
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void x25Qxx_earse(u32_t addr)
{
	u8_t writeBuf[4];
	
    writeEnable();
	addr &= ~(X25Q_SECTOR_SIZE-1);
	writeBuf[0] = X25Q_SECTOR_ERASE;
	writeBuf[1] = (addr>>16)&0xff;
	writeBuf[2] = (addr>>8)&0xff;
	writeBuf[3] = addr & 0xff;
    SPICS(0);
    Ql_SPI_Write(X25Q_SPI_PORT, writeBuf, sizeof(writeBuf));
    SPICS(1);   
	busyWait(0x00FFFFFF);
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void x25Qxx_earseBlock(u32_t addr, u32_t len)
{
    u8_t cmd;
    u32_t earseSize = 0;
    u8_t writeBuf[4];
	
    len += addr&(X25Q_SECTOR_SIZE-1);
    addr &= ~(X25Q_SECTOR_SIZE - 1);
    
    while (len > 0)
    {
        addr += earseSize;
        if (len >= X25Q_BLOCK_SIZE)
        {
            cmd = X25Q_BLOCK_ERASE;
            len -= X25Q_BLOCK_SIZE;
            earseSize = X25Q_BLOCK_SIZE;
        }
        else if (len > X25Q_SECTOR_SIZE)
        {
            cmd = X25Q_SECTOR_ERASE;
            len -= X25Q_SECTOR_SIZE;
            earseSize = X25Q_SECTOR_SIZE;
        }
        else 
        {
            cmd = X25Q_SECTOR_ERASE;
            len = 0;
        }
        writeBuf[0] = cmd;
		writeBuf[1] = (addr>>16)&0xff;
		writeBuf[2] = (addr>>8)&0xff;
		writeBuf[3] = addr & 0xff;
        writeEnable();
        SPICS(0); 	
        Ql_SPI_Write(X25Q_SPI_PORT, writeBuf, sizeof(writeBuf)); 
        SPICS(1);  
		busyWait(0x00FFFFFF);
    }        
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void x25Qxx_read(u32_t addr, u32_t len, u8_t *payload)
{    
	u16_t count;

	while (len)
	{
        count = X25Q_READ_SIZE - (addr&(X25Q_READ_SIZE-1));
		if (count > len)
        {
            count = len;
        }
		pageRead(addr, count, payload);
        addr += count;
        len -= count;
        payload += count;
	}
}

/*******************************************************************************
  * @brief  
  * @param  None
  * @retval None
*******************************************************************************/
static void pageRead(u32_t addr, u32_t len, u8_t *payload)
{
	int ret;
	u8_t writeBuf[4];
	
	writeBuf[0] = X25Q_READ_DATA;
	writeBuf[1] = (addr>>16)&0xff;
	writeBuf[2] = (addr>>8)&0xff;
	writeBuf[3] = addr & 0xff;
    SPICS(0);
	ret = Ql_SPI_WriteRead(X25Q_SPI_PORT, writeBuf, sizeof(writeBuf), payload, len);
    SPICS(1);
	busyWait(0x0000FFFF);
}


/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void x25Qxx_wrtie(u32_t addr, u32_t len, u8_t *payload)
{
    u16_t count;
	while (len)
	{
        count = X25Q_PAGE_SIZE - (addr&(X25Q_PAGE_SIZE-1));
		if (count > len)
        {
            count = len;
        }
		pageWrite(addr, count, payload);
        addr += count;
        len -= count;
        payload += count;
	}
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void pageWrite(u32_t addr, u32_t len, u8_t *payload)
{
	u8_t *buf = memory_apply(len + 4);

	buf[0] = X25Q_WRITE_DATA;
	buf[1] = (addr>>16)&0xff;
	buf[2] = (addr>>8)&0xff;
	buf[3] = addr & 0xff;
	r_memcpy(buf+4, payload, len);
    writeEnable();
    SPICS(0);
    Ql_SPI_Write(X25Q_SPI_PORT, buf, len + 4);
    SPICS(1);
	busyWait(0x0000FFFF);
	memory_release(buf);
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void writeEnable(void)
{
	int ret;
	u8_t cmd = X25Q_WRITE_EN;
    
    SPICS(0);
    ret = Ql_SPI_Write(X25Q_SPI_PORT, &cmd, 1);
	SPICS(1);
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void writeDisable(void)
{
	int ret;
	u8_t cmd = X25Q_WRITE_EN;
    
    SPICS(0);
    ret = Ql_SPI_Write(X25Q_SPI_PORT, &cmd, 1);
	SPICS(1);
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t busyWait(u32_t time)
{
    while ((0x01 == (status()&0x01)) && ((time--) > 0))
    {}

    return time > 0 ? 0 : 1;
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t status(void)
{
	int ret;
    u8_t state;
	u8_t cmd = X25Q_READ_STATE;

    SPICS(0);
	ret = Ql_SPI_WriteRead(X25Q_SPI_PORT, &cmd, 1, &state, 1);
    SPICS(1);
	

	if (ret == 1)
	{
		return state;
	}
}

/*******************************************************************************
  * @brief  
  * @param  None
  * @retval None
*******************************************************************************/
static void SPICS(u8_t cs)
{

#if X25Q_SPI_TYPE
#else
	if (cs)
	{
		Ql_GPIO_SetLevel(PINNAME_PCM_CLK,PINLEVEL_HIGH);
	}
	else
	{
		Ql_GPIO_SetLevel(PINNAME_PCM_CLK,PINLEVEL_LOW);
	}
#endif
}



/******************************************************************************/

