/**********************    COPYRIGHT 2014-2100,  Eybond ************************
 * @File    : eybapp_appTask.h
 * @Author  : CGQ
 * @Date    : 2017-12-01
 * @Brief   :
 ******************************************************************************/
#ifndef __EYBAPP_APPTASK_H_
#define __EYBAPP_APPTASK_H_

#include "eyblib_typedef.h"

#ifdef _PLATFORM_BC25_
#include "ql_timer.h"

#define  EYBAPP_TASK  subtask1_id
#define  APP_WDG_TIMER_ID   (TIMER_ID_USER_START + 1)
#define  APP_USER_TIMER_ID  (TIMER_ID_USER_START + 4)
#define  APP_DEVICE_TIMER_ID  (TIMER_ID_USER_START + 8)

#define  APP_CMD_WDG_ID     0x00FAFF
#define  APP_CMD_BEEP_ID    0x00FA01      // mike 20200817
#define  APP_CMD_GSM_READY  0x00FA03      // mike 20200826 注网成功消息

#define  APP_MSG_UART_READY MSG_ID_USER_START+0xFA4
#define  APP_MSG_TIMER_ID   MSG_ID_USER_START+0xFA5
#define  APP_MSG_DEVTIMER_ID   MSG_ID_USER_START+0xFA6
#define  APP_MSG_WDG_ID     MSG_ID_USER_START+0xFA7
#endif

#ifdef _PLATFORM_L610_
#define  MSG_ID_USER_APP_START    0x1000
#define  APP_WDG_TIMER_ID   (0)
#define  APP_USER_TIMER_ID  (0)

#define  APP_CMD_WDG_ID     0x00FAFF
#define  APP_CMD_BEEP_ID    0x00FA01
#define  APP_CMD_GSM_READY  0x00FA03

#define  APP_MSG_UART_READY        MSG_ID_USER_APP_START+0xFA4  // 调试串口OK
#define  APP_MSG_TIMER_ID          MSG_ID_USER_APP_START+0xFA5
#define  APP_MSG_DEVTIMER_ID       MSG_ID_USER_APP_START+0xFA6
#define  APP_MSG_WDG_ID            MSG_ID_USER_APP_START+0xFA7
#define  APP_DEBUG_MSG_ID          MSG_ID_USER_APP_START+0xFA8  // DEBUG串口指令输入
#define  APP_MSG_DEVUART_READY     MSG_ID_USER_APP_START+0xFA9  // 设备串口OK
#define  APP_DEVICE_IO_ID          MSG_ID_USER_APP_START+0xFAA  // 设备串口指令输入
#define  MODBUS_DATA_GET           MSG_ID_USER_APP_START+0xFAB  // 收到逆变器数据
#define  FIRMWARE_UPDATE           MSG_ID_USER_APP_START+0xFAC  // 固件升级

#endif

void proc_app_task(s32_t taskId);
#endif  // __EYBAPP_APPTASK_H_
/******************************************************************************/
