/**********************    COPYRIGHT 2014-2015, EYBOND    ************************
  *@brief   : ModbusDevice.h  MOSOEC Modbus
  *@notes   : 2017.08.04 CGQ
**********************************************************************************/
#ifndef __DEVICE_RPOTOCOL_H
#define __DEVICE_RPOTOCOL_H

#ifdef _PLATFORM_BC25_
#include "ql_uart.h"
#endif

#ifdef _PLATFORM_L610_
#include "oc_uart.h"
#include "L610_UARTConfig.h"
#endif

#include "eyblib_typedef.h"
#include "Modbus.h"

#define  PROTOCOL_TABER(tab, flag, code)        {sizeof(tab)/sizeof(tab[0]), flag, code,  (ModbusGetCmd_t*)tab}
#define  CONVERT_TYPE                           const ModbusGetCmdTab_t**

typedef u8_t (*cmdProtocol)(void *cmd, void *cmdTab);

typedef struct {
  u8_t count;
  u8_t flag;  // bit 7: 1(must merg type) bit2~bit5(reg spilt)  reg merge flag, bit0: 0(yes),1(No)
  u16_t code;
  ModbusGetCmd_t *cmdTab;
} ModbusGetCmdTab_t;

typedef struct {
  const ST_UARTDCB        *hardCfg;
  const ModbusGetCmd_t    *findCmd;     // Find Cmd;
  const ModbusGetCmdTab_t *procmd;      // protocol find command
  const cmdProtocol       devicePro;
  const cmdProtocol       ackDataPro;   // Onceage process device command ack data
                                        // return 0: data is OK ,other device ack abnormal
} ModbusDeviceHead_t;

typedef struct {
  u8_t startAddr;
  u8_t endAddr;
  ST_UARTDCB *cfg;
  const ModbusDeviceHead_t *head;
} ADDDevice_t;

typedef struct {
  const u8_t *addrTab;
  ST_UARTDCB *cfg;
  const ModbusDeviceHead_t *head;
  const int monitorCount;
} ModbusDevice_t;

typedef struct {
  u16_t code;
  u8_t startAddr;
  u8_t endAddr;
//  ST_UARTDCB *cfg;    // mike 20200924
  ST_UARTDCB cfg;
} ProtocolAttr_t;

extern ModbusDevice_t ModbusDevice;
extern ADDDevice_t MeterDevice;
extern const ST_UARTDCB UART_2400_N1;
extern const ST_UARTDCB UART_4800_N1;
extern const ST_UARTDCB UART_9600_N1;
extern const ST_UARTDCB UART_9600_E1;
extern const ST_UARTDCB UART_19200_N1;
extern const ST_UARTDCB UART_38400_N1;
extern const ST_UARTDCB UART_115200_N1;

void Protocol_init(void);
void Protocol_clean(void);

int protocolAttrGet(u8_t num, ProtocolAttr_t *attr);

#endif //__DEVICE_RPOTOCOL_H end
/******************************************************************************/

