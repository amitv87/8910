/**********************    COPYRIGHT 2014-2100,  Eybond ************************ 
 * @File    : eybpub_utility.h
 * @Author  : MWY
 * @Date    : 2020-08-05
 * @Brief   : 
 ******************************************************************************/
#ifndef __EYBPUB_UTILITY_H_
#define __EYBPUB_UTILITY_H_

#ifdef _PLATFORM_BC25_
#include "ql_type.h"
#include "ql_uart.h"

#define NET_LED			(PINNAME_NETLIGHT)
#define GSM_LED			(PINNAME_UART2_RTS)
#define DEVICE_LED		(PINNAME_RI)
#define BEEP_PORT		(PINNAME_DCD)

#define KEY_PORT		(PINNAME_UART2_CTS)
#define WATCHDOG_PIN	(PINNAME_GPIO5)
// #define WATCHDOG_PIN	(PINNAME_GPIO2)
#define SERIAL_RX_BUFFER_LEN  1024
#define MAX_NET_BUFFER_LEN    512

s32 ReadSerialPort(Enum_SerialPort port,u8* pBuffer, u32 bufLen);
#endif

#ifdef _PLATFORM_L610_
#include "oc_uart.h"

#define NET_LED			(24)
#define GSM_LED			(26)
#define DEVICE_LED		(30)
#define BEEP_PORT		(27)

#define KEY_PORT		(31)
#define WATCHDOG_PIN	(25)
#define ADC0_PORT	    (45)

#define SERIAL_RX_BUFFER_LEN  1024
#define MAX_NET_BUFFER_LEN    1024

#define DEBUG_UART_TXD       28
#define DEBUG_UART_RXD       29

#define DEVICE_UART_TXD      67
#define DEVICE_UART_RXD      68


#define PINDIRECTION_IN   0
#define PINDIRECTION_OUT  1

#define PINLEVEL_LOW      0
#define PINLEVEL_HIGH     1

s32_t ReadSerialPort(hal_uart_port_t port,u8_t* pBuffer, u32_t bufLen);
extern UINT32 EYBAPP_TASK;
extern UINT32 EYBNET_TASK;
extern UINT32 EYBDEVICE_TASK;
extern UINT32 EYBOND_TASK;
extern UINT32 ALIYUN_TASK;
extern UINT32 FOTA_TASK;
extern UINT32 UPDATE_TASK;
extern UINT32 COMMON_SERVER_TASK;

/* typedef struct {
  u8_t  message;    // message
  void  *param1;    // 参数1
  void  *param2;    // 参数2
} ST_MSG; */

typedef struct {
  u32_t  message;
  u32_t  param1;
  u32_t  param2;
  u32_t  srcTaskId;
} ST_MSG;

s32_t Eybpub_UT_SendMessage(s32_t destTaskId, u32_t msgId, u32_t param1, u32_t param2);
#endif

#endif //__EYBPUB_UTILITY_H_
/******************************************************************************/
