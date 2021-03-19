/**********************    COPYRIGHT 2014-2100,  Eybond ************************
 * @File    : eybpub_Debug.h
 * @Author  : MWY
 * @Date    : 2020-08-13
 * @Brief   :
 ******************************************************************************/
#ifndef __EYBPUB_DEBUG_H_
#define __EYBPUB_DEBUG_H_

#define EYBOND_DEBUG_ENABLE    // 打开DEBUG log
//#define EYBOND_TRACE_ENABLE    // 将DEBUG log输出改为TRACE模式,不能直接用TRACE_ENABLE做开关,会影响SDK里面的接口
#define DBG_BUF_LEN     1024

#ifdef _PLATFORM_BC25_
#include "ql_stdlib.h"
#include "ql_uart.h"      //include ql_common.h -- include ql_gpio.h
#include "eyblib_typedef.h"

#ifdef __EYBOND_MUS01NB_CODE__  //mike 20200804
#define DEBUG_PORT      (UART_PORT2)
#else
#define DEBUG_PORT      (UART_PORT3)
#endif
#define DEBUG_MSG_ID    (0x00FB00)

#define DEBUG_PORT_BITRATE 57600

#define DEBUG_INPUT_EHCO   0

#ifdef EYBOND_DEBUG_ENABLE
static char DebugBuffer[DBG_BUF_LEN];
void  Debug_buffer(Buffer_t *buf);
extern void Debug_output(u8_t *p, u16_t len);

#define APP_DEBUG(FORMAT,...) {\
    Ql_memset(DebugBuffer, 0, DBG_BUF_LEN);\
    Ql_snprintf(DebugBuffer, DBG_BUF_LEN, "%s:%d %s::"FORMAT, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    Debug_output((u8_t*)DebugBuffer, Ql_strlen(DebugBuffer));\
}

extern void Print_output(u8_t *p, u16_t len);
#define APP_PRINT(FORMAT,...) {\
    Ql_memset(DebugBuffer, 0, DBG_BUF_LEN);\
    Ql_snprintf(DebugBuffer, DBG_BUF_LEN, "%s:%d %s::"FORMAT, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    Print_output((u8_t*)DebugBuffer, Ql_strlen(DebugBuffer));\
}

extern void Debug_trace(u8_t *p, u16_t len);
#define APP_TRACE(FORMAT,...) {\
    Ql_memset(DebugBuffer, 0, DBG_BUF_LEN);\
    Ql_snprintf(DebugBuffer, DBG_BUF_LEN, "%s:%d %s::"FORMAT, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    Debug_trace((u8_t*)DebugBuffer, Ql_strlen(DebugBuffer));\
}

#else
#define APP_DEBUG(FORMAT,...)
#define APP_PRINT(FORMAT,...)
#define APP_TRACE(FORMAT,...)
#endif

#endif

#ifdef _PLATFORM_L610_   // 添加L610 SDK相关接口
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "eyblib_typedef.h"
#include "eyblib_r_stdlib.h"

#define DEBUG_PORT      2 //调试串口
char DebugBuffer[DBG_BUF_LEN];
#define DEBUG_PORT_BITRATE 115200

#define DEBUG_INPUT_EHCO   0

#ifdef EYBOND_DEBUG_ENABLE
void  Debug_buffer(Buffer_t *buf);
extern void Debug_output(u8_t *p, u16_t len);

#define APP_DEBUG(FORMAT,...)  {\
  r_memset(DebugBuffer, 0, DBG_BUF_LEN);\
  snprintf(DebugBuffer, DBG_BUF_LEN, "%s:%d %s::"FORMAT, __FILE__, __LINE__, __func__, ##__VA_ARGS__);\
  Debug_output((u8_t*)DebugBuffer, r_strlen(DebugBuffer));\
}
extern void Print_output(u8_t *p, u16_t len);
#define APP_PRINT(FORMAT,...) {\
  r_memset(DebugBuffer, 0, DBG_BUF_LEN);\
  snprintf(DebugBuffer, DBG_BUF_LEN, "%s:%d %s::"FORMAT, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
  Print_output((u8_t*)DebugBuffer, r_strlen(DebugBuffer));\
}
extern void Debug_trace(u8_t *p, u16_t len);
#define APP_TRACE(FORMAT,...) {\
  r_memset(DebugBuffer, 0, DBG_BUF_LEN);\
  snprintf(DebugBuffer, DBG_BUF_LEN, "%s:%d %s::"FORMAT, __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
  Debug_trace((u8_t*)DebugBuffer, r_strlen(DebugBuffer));\
}
#else
#define APP_DEBUG(FORMAT,...)
#define APP_PRINT(FORMAT,...)
#define APP_TRACE(FORMAT,...)
#endif

#endif

void Debug_init(void);
#endif //__EYBPUB_DEBUG_H_
/******************************************************************************/
