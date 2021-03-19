/**************Copyright(C)��2015-2026��Shenzhen Eybond Co., Ltd.***************
  *@brief   : StateGridModbus.h
  *@notes   : 2018.10.30 CGQ establish
*******************************************************************************/
#ifndef __STATE_GRID_MODBUS_H
#define __STATE_GRID_MODBUS_H

#include "eyblib_typedef.h"
#include "eyblib_list.h"
#include "StateGridData.h"

typedef struct {
  u32_t id;
  u16_t rsv;
  u8_t  type;
  u8_t  lenght;
  u8_t  addr;
  u8_t  reg;
  u16_t  regAddr;
} FileModbusPoint_t;

typedef struct {
  u8_t  addr;
  u8_t  reg;
  u16_t  regAddr;
} ModbusPoint_t;

typedef struct {
  ModbusPoint_t point;
  StateGridPoint_t *src;
} ModbusCollectPoint_t;

typedef struct {
  int count;
  ModbusCollectPoint_t tab[];
} ModbusCollectTab_t;

typedef struct {
  int count;
  FileModbusPoint_t point[];
} ModbusPointFile_t;

typedef struct {
  u8_t addr;
  ListHandler_t cmdList;
} PointTabCmd_t;

typedef struct {
  u8_t addr;
} StateGridDeviceInfo_t;

extern const StateGridDevice_t StateGridModbusDevice;

#endif //__STATE_GRID_MODBUS_H
/******************************************************************************/

