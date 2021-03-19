/**********************    COPYRIGHT 2014-2015, EYBOND    ************************
  *@brief   : ModbusDevice.h  MOSOEC Modbus
  *@notes   : 2017.08.04 CGQ
**********************************************************************************/
#ifndef __MODBUS_DEVICE_H_
#define __MODBUS_DEVICE_H_

#include "eyblib_typedef.h"
#include "eyblib_list.h"
#include "Modbus.h"
#include "Device.h"
#include "Protocol.h"
// #include "ql_uart.h"

#define DEVICE_TAB_FLAG         (0x5A5A55AA)
#define DEVICE_TAB_ADDR         (0x0800A000)

typedef struct {
  u8_t addr;   // device addr
  u8_t tryCnt; // error try count
  u8_t flag;
  u16_t code;  // device ocde
  ModbusDeviceHead_t *head;
} DeviceExplain_t;

typedef struct {
  u8_t state;
  u8_t fun;
  u16_t startAddr;
  u16_t endAddr;
  u16_t crc;
  Buffer_t buf;
} CmdBuf_t;

typedef struct {
  u8_t addr;
  u8_t flag;
  u16_t code;
  ListHandler_t cmdList;
} DeviceOnlineHead_t;

extern ListHandler_t onlineDeviceList;

void ModbusDevice_init(void);
void ModbusDevice_clear(void);

#endif  // __MODBUS_DEVICE_H_ end
/******************************************************************************/

