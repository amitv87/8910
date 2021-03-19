/**********************    COPYRIGHT 2014-2100,  Eybond ************************
 * @File    : eybpub_Debug.c
 * @Author  : MWY
 * @Date    : 2020-08-13
 * @Brief   :
 ******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_system.h"
#include "ql_error.h"
#include "ql_uart.h"
#include "ql_stdlib.h"
#include "ql_memory.h"
#include "ql_trace.h"

#include "eybpub_Debug.h"
#include "eybpub_utility.h"
#include "eyblib_typedef.h"
#include "eybapp_appTask.h"

#ifdef  EYBOND_DEBUG_ENABLE
static Buffer_t UARTDEBUG_buf;

static void UARTDEBUG_CallBack(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void *customizedPara);

/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
static void UARTDEBUG_CallBack(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void *customizedPara) {
  switch (msg) {
    case EVENT_UART_READY_TO_READ: {
      APP_DEBUG("Read data in DEBUG_PORT buffer!\r\n");
      if (UARTDEBUG_buf.payload != NULL) {  // clean debug buffer DEBUG串口接收的数据统一在这里释放内存
        Ql_MEM_Free(UARTDEBUG_buf.payload);
        UARTDEBUG_buf.payload = NULL;
      }
      UARTDEBUG_buf.payload = Ql_MEM_Alloc(SERIAL_RX_BUFFER_LEN);
      if (UARTDEBUG_buf.payload == NULL) {
        APP_DEBUG("memory alloc Fail\r\n");
        return;
      }
      UARTDEBUG_buf.size = SERIAL_RX_BUFFER_LEN;
      UARTDEBUG_buf.lenght = 0;
      Ql_memset(UARTDEBUG_buf.payload, 0x0, UARTDEBUG_buf.size);
      
      if (DEBUG_PORT == port) {
        // read data from debug UART        
//        UARTDEBUG_buf.lenght = ReadSerialPort(port, UARTDEBUG_buf.payload, UARTDEBUG_buf.size);
        UARTDEBUG_buf.lenght =  Ql_UART_Read(port, UARTDEBUG_buf.payload, UARTDEBUG_buf.size);
        if (UARTDEBUG_buf.lenght <= 0) {
          APP_DEBUG("No data in DEBUG_PORT buffer!\r\n");
          return;
        } else {
#if DEBUG_INPUT_EHCO > 0
          char *pCh = NULL;
          // Echo
          Ql_UART_Write(port, UARTDEBUG_buf.payload, UARTDEBUG_buf.lenght);

          pCh = Ql_strstr((char *)UARTDEBUG_buf.payload, "\r\n");
          if (pCh) {
            *(pCh + 0) = '\0';
            *(pCh + 1) = '\0';
          }
#endif
          // No permission for single <cr><lf>
          if (Ql_strlen((char *)UARTDEBUG_buf.payload) == 0) {
            APP_DEBUG("read error data from DEBUG_PORT buffer!\r\n");
            return;
          }
            Ql_OS_SendMessage(EYBAPP_TASK, DEBUG_MSG_ID, (u32)(&UARTDEBUG_buf), (u32)((void*)Debug_buffer));   // mike 20200814
//          Ql_OS_SendMessage(EYBAPP_TASK, DEBUG_MSG_ID, (u32)(&UARTDEBUG_buf), (u32)((void *)DebugBuffer));   // mike 20200827
        }
      }
    break;
    }
/*    case EVENT_UART_DTR_IND: { // DTR level changed, developer can wake up the module in here
      if (0 == level) {
        // APP_DEBUG("DTR set to low =%d  wake !!\r\n", level);
        Ql_SleepDisable();
      } else {
        // APP_DEBUG("DTR set to high =%d  Sleep \r\n", level);
        Ql_SleepEnable();
      }
      break;
    } */
    case EVENT_UART_READY_TO_WRITE:
      break;
    default:
      break;
  }
}
#endif

/*******************************************************************************
 Brief    : Debug_init
 Parameter:
 return   :
*******************************************************************************/
void Debug_init(void) {
#ifdef  EYBOND_DEBUG_ENABLE
  Ql_UART_Close(DEBUG_PORT);
  Ql_UART_Register(DEBUG_PORT, UARTDEBUG_CallBack,  NULL);
  Ql_UART_Open(DEBUG_PORT, DEBUG_PORT_BITRATE, FC_NONE);
  if (UARTDEBUG_buf.payload != NULL)
  {
    Ql_MEM_Free(UARTDEBUG_buf.payload);
    UARTDEBUG_buf.payload = null;
    UARTDEBUG_buf.size = 0;
    UARTDEBUG_buf.lenght = 0;
  }
#endif
}

/*******************************************************************************
 Brief    : Debug_buffer
 Parameter:
 return   :
*******************************************************************************/
#ifdef  EYBOND_DEBUG_ENABLE
void  Debug_buffer(Buffer_t *buf)
{  
//  Debug_output(buf->payload, buf->lenght);
  Ql_UART_Write((Enum_SerialPort)DEBUG_PORT, (u8_t *)(buf->payload), buf->lenght);
/*  if(buf->payload != NULL) {      // mike 打开会死机
    Ql_MEM_Free(buf->payload);
    buf->payload = null;
    buf->lenght = 0;
    buf->size = 0;
  } */
}

/*******************************************************************************
 Brief    : Debug_output
 Parameter:
 return   :
*******************************************************************************/

void Debug_output(u8_t *p, u16_t len) {
/* #if 0//(UART_PORT2 == DEBUG_PORT) 
  Ql_Debug_Trace((u8 *)(p));
#else
  Ql_UART_Write((Enum_SerialPort)DEBUG_PORT, (u8 *)(p), len);
#endif */
#ifdef EYBOND_TRACE_ENABLE
  Ql_Debug_Trace((char *)(p));
#else
  Ql_UART_Write((Enum_SerialPort)DEBUG_PORT, (u8 *)(p), len);
#endif
}

void Print_output(u8_t *p, u16_t len) {
  Ql_UART_Write((Enum_SerialPort)DEBUG_PORT, (u8 *)(p), len);
}

void Debug_trace(u8_t *p, u16_t len) {
  Ql_Debug_Trace((char *)(p));
}
#endif

#endif

#ifdef _PLATFORM_L610_ // 添加L610 SDK相关接口
#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('Q', 'E', 'K', 'J')
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fibo_opencpu.h"

#include "eybpub_Debug.h"
#include "eybpub_utility.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_memory.h"
#include "eyblib_typedef.h"
#include "eyblib_swap.h"

#include "eybapp_appTask.h"

#ifdef  EYBOND_DEBUG_ENABLE
Buffer_t UARTDEBUG_buf;
#endif

void UARTDEBUG_CallBack(hal_uart_port_t uart_port, UINT8 *data, UINT16 len, void *arg) {
  APP_DEBUG("uartapi recv uart_port=%d len=%d, data len=%ld\r\n", uart_port, len, r_strlen((char *)data));
  switch (uart_port) {
    case DEBUG_PORT: {
      if (len > SERIAL_RX_BUFFER_LEN || len == 0) {
        APP_DEBUG("UART get data len is big than %d\r\n", SERIAL_RX_BUFFER_LEN);
        return;
      }
      u16_t nTemp_i = 0;
      for (nTemp_i=0; nTemp_i < len; nTemp_i ++) {
        if (data[nTemp_i] < 10 || data[nTemp_i] > 126) {  // DEBUG串口只接收可见字符
          APP_DEBUG("DEBUG UART get a data less than 0x0A or large than 0x7E\r\n");
          return;
        }
      }
      if (UARTDEBUG_buf.payload != NULL) {
        memory_release(UARTDEBUG_buf.payload);  // clean debug buffer DEBUG串口接收的数据统一在这里释放内存
        UARTDEBUG_buf.payload = NULL;
      }
      UARTDEBUG_buf.payload = memory_apply(SERIAL_RX_BUFFER_LEN);
      if (UARTDEBUG_buf.payload == NULL) {
        APP_DEBUG("memory alloc Fail\r\n");
        return;
      }
      UARTDEBUG_buf.size = SERIAL_RX_BUFFER_LEN;
      UARTDEBUG_buf.lenght = 0;
      r_memset(UARTDEBUG_buf.payload, '\0', UARTDEBUG_buf.size);
      r_memcpy(UARTDEBUG_buf.payload, data, len);
      UARTDEBUG_buf.lenght = len;

      Eybpub_UT_SendMessage(EYBAPP_TASK, APP_DEBUG_MSG_ID, (u32_t)(&UARTDEBUG_buf), (u32_t)((void*)Debug_buffer));
      break;
    }
    default:
      break;
  }
}

/*******************************************************************************
 Brief    : Debug_init
 Parameter:
 return   :
*******************************************************************************/
void Debug_init(void) {
  fibo_gpio_mode_set(DEBUG_UART_TXD,6);
  fibo_gpio_cfg(DEBUG_UART_TXD,PINDIRECTION_OUT);
  fibo_gpio_set(DEBUG_UART_TXD,PINLEVEL_HIGH);

  fibo_gpio_mode_set(DEBUG_UART_RXD,6);
  fibo_gpio_cfg(DEBUG_UART_RXD,PINDIRECTION_IN);
  fibo_gpio_set(DEBUG_UART_RXD,PINLEVEL_HIGH);
  
  hal_uart_config_t drvcfg;
  fibo_hal_uart_deinit(DEBUG_PORT);
  fibo_taskSleep(1000);
  r_memset(&drvcfg,0,sizeof(hal_uart_config_t));
  drvcfg.baud = DEBUG_PORT_BITRATE;
  drvcfg.parity = HAL_UART_NO_PARITY;
  drvcfg.data_bits = HAL_UART_DATA_BITS_8;
  drvcfg.stop_bits = HAL_UART_STOP_BITS_1;
  drvcfg.rx_buf_size = UART_RX_BUF_SIZE;
  drvcfg.tx_buf_size = UART_TX_BUF_SIZE;
//  drvcfg.cts_enable = false;
//  drvcfg.rts_enable = false;
  drvcfg.recv_timeout = 500;
  fibo_hal_uart_init(DEBUG_PORT, &drvcfg, UARTDEBUG_CallBack, NULL);

#ifdef  EYBOND_DEBUG_ENABLE
  if (UARTDEBUG_buf.payload != NULL) {
    memory_release(UARTDEBUG_buf.payload);
  }
  UARTDEBUG_buf.payload = NULL;
  UARTDEBUG_buf.size = 0;
  UARTDEBUG_buf.lenght = 0;
#endif  
}
/*******************************************************************************
 Brief    : Debug_printf
 Parameter:
 return   :
*******************************************************************************/
#ifdef  EYBOND_DEBUG_ENABLE
void  Debug_buffer(Buffer_t *buf) {
  fibo_hal_uart_put(DEBUG_PORT, (UINT8 *)buf->payload, buf->lenght);
}

/*******************************************************************************
 Brief    : Debug_output
 Parameter:
 return   :
*******************************************************************************/
void Debug_output(u8_t *p, u16_t len) {
#ifdef EYBOND_TRACE_ENABLE
  fibo_textTrace("%s", (UINT8 *)p);
//  OSI_PRINTFI("%s", (UINT8 *)p);
#else
  fibo_hal_uart_put(DEBUG_PORT, (UINT8 *)p, len);
#endif
}

void Print_output(u8_t *p, u16_t len) {
  fibo_hal_uart_put(DEBUG_PORT, (UINT8 *)p, len);
}

void Debug_trace(u8_t *p, u16_t len) {
  OSI_PRINTFI("%s", (UINT8 *)p);
}
#else
void Print_output(u8_t *p, u16_t len) {
  fibo_hal_uart_put(DEBUG_PORT, (UINT8 *)p, len);
}

void Debug_output(u8_t *p, u16_t len) {
  fibo_hal_uart_put(DEBUG_PORT, (UINT8 *)p, len);
}

#endif
#endif
/*********************************FILE END*************************************/

