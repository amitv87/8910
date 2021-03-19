/**********************    COPYRIGHT 2014-2100,  Eybond ************************ 
 * @File    : Xmodem.c
 * @Author  : CGQ
 * @Date    : 2017-12-14
 * @Brief   : 
 ******************************************************************************/
#include "eyblib_CRC.h"
#include "eyblib_memory.h"
#include "eyblib_r_stdlib.h"
#include "eybpub_File.h"

#include "XmodemTrans.h"

#define START	(0x43)
#define SOH 	(0x01)
#define STX  	(0x02)
#define ETX  	(0x03)
#define EOT  	(0x04)
#define ACK  	(0x06)
#define NAK  	(0x15)
#define CAN  	(0x18)
#define EOF 	(0x1A)

static File_t *s_file = null;
static u8_t commNum;
static u8_t progress;

static void Xmodem_nextPakege(Buffer_t *sendBuf);
static short Xmodem_crc(const unsigned char *data, int len);

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
void Xmodem_start(File_t *file)
{
	s_file = file;
	commNum = 0;
	progress = 0;
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
void Xmodem_end(void)
{
	memory_release(s_file);
	s_file = null;
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 0~100; 
*******************************************************************************/
u8_t Xmodem_progress(void)
{
	return progress;
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
int Xmodem_ack(Buffer_t *sendBuf, Buffer_t *ack)
{
	static u8_t cnt;
	
	u8_t code = 0xFF;
	
	int i;
	
	for (i = 0; i < ack->lenght; i++)
	{
		if (ack->payload[i] != 0)
		{
			code = ack->payload[i];
		}
	}

	i = 0;
	
	switch (code)
	{
		case ETX: // 中断操作
		case CAN: // 撤销传送
			i = -1;
			break;
		case ACK: // 认可响应
			if (progress == 99)
			{
				i = 1;
			}
			else
			{
				Xmodem_nextPakege(sendBuf);
				if (progress > 100)
				{
					i = -1;
				}
			}
			cnt = 0;
			break;
		case NAK: // 不认可响应
			if (cnt++ > 10)
			//if (cnt++ > 20)
			{
				i = -2;
			}
			break;
		case START:	  // 传输的启动
			if (sendBuf->payload == null)
			{
				commNum = 0;
				cnt = 0;
				sendBuf->size = sizeof(Xmodem_t);
				sendBuf->lenght = 0;
				sendBuf->payload = memory_apply(sendBuf->size);
				Xmodem_nextPakege(sendBuf);
			}
			break;
		default:
			break;
	}
	
	return i;
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
static void Xmodem_nextPakege(Buffer_t *sendBuf)
{
	int ret;
	
	Xmodem_t *xmodem = (Xmodem_t *)sendBuf->payload;

	xmodem->head = STX;
	xmodem->num = ++commNum;
	xmodem->numF = ~commNum;

	ret = File_read(s_file, xmodem->data, sizeof(xmodem->data));
	
	if (ret < 0 || ret > sizeof(xmodem->data))
	{
		progress = 0xFE;
	}
	else if (ret == 0)
	{
		progress = 99;
		xmodem->head = EOT;
		sendBuf->lenght = 1;
	}
	else
	{
		u16_t crc;
		r_memset(&xmodem->data[ret], EOF, sizeof(xmodem->data) - ret);;
		crc = Xmodem_crc(xmodem->data, sizeof(xmodem->data));
		xmodem->crcH = crc>>8;
		xmodem->crcL = crc;
		sendBuf->lenght = sizeof(Xmodem_t);
		progress = ((s_file->seat * 98) / s_file->size);
	}
}


/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
static short Xmodem_crc(const unsigned char *data, int len)
{    
    unsigned short i, uwCrcReg = 0;

    while (len--)
    {
        uwCrcReg ^= (*data++) << 8;
        for (i = 0; i < 8; i++)
        {
            uwCrcReg = (uwCrcReg &0x8000) == 0 ? (uwCrcReg << 1) : ((uwCrcReg << 1) ^ 0x1021);
        }
    }
    return (uwCrcReg);
}	


/*********************************FILE END*************************************/

