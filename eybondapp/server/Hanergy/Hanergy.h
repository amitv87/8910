/**************Copyright(C)��2015-2026��Shenzhen Eybond Co., Ltd.***************
  *@brief   : Hanergy.c 
  *@notes   : 2017.06.03 CGQ establish
*******************************************************************************/
#ifndef __HANERGY_H
#define __HANERGY_H	 


#include "CommonServer.h"


#define HANERGY_DEVICE_CODE         (0x1000)
#define HANERGY_DEFAULT_LENGHT      (0x02)

#define HANERGY_HEARTBEA               (0x30)
#define HANERGY_DEVICE_PARA            (0x31)
#define HANERGY_DEVICE_DATA            (0x32)
#define HANERGY_DEVICE_HISTORY_DATA    (0x33)
#define HANERGY_DEVICE_SET             (0x34)


#define HANERGY_HISTORY_AREA_ADDR		(FLASH_HANERGY_HISTORY_ADDR)
#define HANERGY_HISTORY_AREA_SIZE 		(FLASH_HANERGY_HISTORY_SIZE)

#define HSP_WAIT_CNT    15

typedef enum
{
    HANERGY_OK = 0,
    HANERGY_PARA_LEN_ERR = 10,
    HANERGY_PARA_VAL_ERR = 11,
}Hanergy_e; 

typedef struct
{
    u16_t serial;   //Serial number
    u16_t code;     //device code  
    u16_t msgLen;    //meseger lenght
    u8_t  addr;      //device addresss
    u8_t  func;      //funcation code
}HanergyHeader_t;


typedef struct
{
    u16_t PDULen;    //user Data Lenght;
    u16_t waitCnt;  //command wait process time
    DataAck ack;
	HanergyHeader_t head;
	u8_t PDU[];
}HSP_t;  //hanergy server data protocol 

typedef u8_t (*hanergyCodeExe)(HSP_t *esp);

typedef struct
{
    u16_t code;
    hanergyCodeExe fun;
}HanergyTab_t;

extern const CommonServer_t HanergyServer;

int Hanergy_check(void);

#endif //__HANERGY_H
/******************************************************************************/

