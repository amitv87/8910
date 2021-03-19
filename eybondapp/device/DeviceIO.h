/**********************    COPYRIGHT 2014-2100,  Eybond ************************
 * @File    : DeviceIO.h
 * @Author  : CGQ
 * @Date    : 2017-12-05
 * @Brief   :
 ******************************************************************************/
#ifndef __DEVICE_IO_
#define __DEVICE_IO_

#ifdef _PLATFORM_BC25_  // 移远BC25 SDK
#include "ql_uart.h"
#include "eyblib_typedef.h"
#define DEVICE_IO_PORT    (UART_PORT1)
#define DEVICE_OVERTIME_ID  (TIMER_ID_USER_START + 2)
#define DEVICE_IO_ID    (0x00FC00)
#endif

#ifdef _PLATFORM_L610_  // 广和通L610 SDK
#include "oc_uart.h"
#include "eyblib_typedef.h"

#define  DEVICE_IO_PORT      0 //设备串口
#define  DEVICE_PORT_BITRATE 9600

#endif

typedef enum {
  DEVICE_ACK_FINISH     = 0x00,
  DEVICE_ACK_OVERTIME   = 0x01,
  DEVICE_ACK_OVERFLOW   = 0x02,
  DEVICE_ACK_BUSY       = 0x10,
  DEVICE_ACK_PRAR_ERR   = 0x11,
  DEVICE_ACK_HARD_FAULT = 0x12,
  DEVICE_ACK_LOCK       = 0x20,
  DEVICE_ACK_READY      = 0x80,
} DeviceAck_e;

typedef struct {
  mcu_t waitTime;
  Buffer_t *buf;
  void (*callback)(DeviceAck_e ack);
} DeviceInfo_t;

#ifdef _PLATFORM_BC25_
void DeviceIO_init(ST_UARTDCB *cfg);
ST_UARTDCB *DeviceIO_cfgGet(void);
#endif

#ifdef _PLATFORM_L610_
#include "L610_UARTConfig.h"
#include "oc_uart.h"

void DeviceIO_init(ST_UARTDCB *cfg);
void DevIO_halGPIO();
void DevIO_stcfg(ST_UARTDCB* hardcfg);
#endif

void DeviceIO_reset(void);
DeviceAck_e DeviceIO_write(DeviceInfo_t *hard, u8_t *pData, mcu_t lenght);
void    DeviceIO_lock(DeviceInfo_t *hard);
void    DeviceIO_unlock(void);
DeviceInfo_t *DeviceIO_lockState(void);
void  Uart_write(u8_t *data, u16_t len);
ST_UARTDCB *DeviceIO_cfgGet(void);
extern s8_t g_UARTIO_AT_enable;


#endif //__DEVICE_IO_
/******************************************************************************/
