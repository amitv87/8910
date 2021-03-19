/**********************    COPYRIGHT 2014-2100,  Eybond ************************ 
 * @File    : Xmodem.h
 * @Author  : CGQ
 * @Date    : 2017-12-14
 * @Brief   : 
 ******************************************************************************/
#ifndef __X_MODEM_TRANS__
#define __X_MODEM_TRANS__

// #include "ql_type.h"
#include "eyblib_typedef.h"
#include "eybpub_File.h"

#pragma pack(1)
typedef struct 
{
	u8_t head;
	u8_t num;
	u8_t numF;
	u8_t data[1024];
	u8_t crcH;
	u8_t crcL;
}Xmodem_t;
#pragma pack()


void Xmodem_start(File_t *file);
void Xmodem_end(void);
u8_t Xmodem_progress(void);
int Xmodem_ack(Buffer_t *sendBuf, Buffer_t *ack);

#endif //__X_MODEM_TRANS__
/******************************************************************************/
