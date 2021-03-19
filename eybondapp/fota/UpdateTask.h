/**********************    COPYRIGHT 2014-2100,  Eybond ************************ 
 * @File    : UpdateTask.h
 * @Author  : CGQ
 * @Date    : 2017-12-14
 * @Brief   : 
 ******************************************************************************/
#ifndef __UPDATE_TASK_
#define __UPDATE_TASK_

#include "eyblib_typedef.h"

#ifdef _PLATFORM_M26_
#define APP_TASK_ID		5

#define UPDATE_TASK			subtask4_id
#define SELF_UPDATE_ID      (0x00F600)
#define DEVICE_UPDATE_ID	(0x00F000)
#endif

#ifdef _PLATFORM_L610_
#define MSG_ID_USER_UPDATE_START    0x8000
#define SELF_UPDATE_ID          MSG_ID_USER_UPDATE_START+0xF60
#define DEVICE_UPDATE_ID        MSG_ID_USER_UPDATE_START+0xF00
#endif
void proc_update_task(s32_t taskId);

#endif //__UPDATE_TASK_
/******************************************************************************/
