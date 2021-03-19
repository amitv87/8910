/**************Copyright(C)��2015-2026��Shenzhen Eybond Co., Ltd.***************
  *@brief   : SAJ.c 
  *@notes   : 2018.09.10 CGQ establish
*******************************************************************************/
#ifndef __SAJ_H
#define __SAJ_H	 

#include "CommonServer.h"




#define SAJ_ACK_OVERTIME        (20000)
#define SAJ_WAIT_CNT            (SAJ_ACK_OVERTIME/SERVER_PROCESS_FREP)

typedef struct
{
  u16_t PDULen;    //user Data Lenght;
  u16_t waitCnt;  //command wait process time
  DataAck ack;
	u8_t PDU[];
}SAJ_t;  //SAJ server data protocol 

extern const CommonServer_t SAJ_API;
int SAJ_check(void);
#endif //__SAJ_H
/******************************************************************************/

