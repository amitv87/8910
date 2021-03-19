/**********************    COPYRIGHT 2014-2015, EYBOND    ************************ 
  *@brief   : sineng.h 
  *@notes   : 2017.12.29 CGQ   
**********************************************************************************/
#ifndef __SINENG_H
#define __SINENG_H

#include "Protocol.h"
#include "DeviceIO.h"
#include "eyblib_list.h"

typedef struct
{
	u16_t pointCount;  //PV scan piont count;
	u16_t startAddr;   //PV scan start addr;
	u16_t startRegNum;	//PV scan start opt reg num;
	u16_t *startCmd;   //PV scan start opt reg val
	u16_t readyAddr;   //PV scan ready addr
	u16_t readyRAMSize;	//PV scan RAM byte width
	u16_t RAMWide;		//RAM byte wide
	u16_t dataAddr;     //PV scan data addr
	u16_t dataRegNum;	//PV scan start opt reg num;
	u16_t *dataCmd;		//PV scan data get command
	u16_t passwordAddr;     //PV scan password addr
	u16_t passwordRegNum;	//PV scan start opt reg num;
	u16_t *passwordCmd;		//PV scan data get command
}SinengPV_t;

typedef struct 
{
	u32_t addr;		//
	u16_t size; 	//
	u16_t user; 	//
}SinengPVRAM_t;

typedef struct
{
	u8_t addr;
	u8_t tryCnt;
	u8_t pointSize;
	u16_t state;
	u16_t code;
	u16_t upPointCount;
	SinengPVRAM_t RAM;
	Buffer_t buf;
	DeviceInfo_t deviceHead;
	ListHandler_t cmdList;
}SinengPVScan_t;

typedef enum
{
	PV_READY	= 0,
	PV_START	= 1,
	PV_RAM_ADDR_GET = 2,
	PV_PASSWORD		= 3,
	PV_RAM_DATA_GET = 4,
	PV_RAM_DATA_UP = 5,
	PV_RAM_DATA_WAIT = 6,
}PVScanState_e;

extern const ModbusDeviceHead_t SinengDevice;

void Sineng_PVData(u8_t addr, u8_t state, u16_t *code, Buffer_t *ack);
void PV_Scan(void);
void PV_dataGet(void);

#endif //__SINENG_H end
/******************************************************************************/

