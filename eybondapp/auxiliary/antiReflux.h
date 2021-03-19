/**************Copyright(C)��2015-2026��Shenzhen Eybond Co., Ltd.***************
  *@brief   : anti-reflux.h
  *@notes   : 2018.11.06 CGQ establish
*******************************************************************************/
#ifndef __ANTIREFLUX_H
#define __ANTIREFLUX_H	 

#include "typedef.h"
#include "Device.h"
#include "Protocol.h"

#define   ANTI_REFLUX_TASK        subtask6_id


typedef struct
{
    u8_t state;
    s32_t power;
}MeterValue_t;

typedef struct
{
    ModbusGetCmdTab_t tab;
    void (*func)(u16_t deviceCount, Device_t *dev, MeterValue_t *meter);
}AntiReflux_t;

typedef struct
{
    u8_t addr;
    AntiReflux_t *aux;
}AntiRefluxDeviceInfo_t;

void proc_antiReflux_task(s32_t taskId);
u8_t modbusDataCheck(void *load, void *changeData);
int float2int(u32_t data);
#endif      //__ANTIREFLUX_H
/******************************************************************************/

