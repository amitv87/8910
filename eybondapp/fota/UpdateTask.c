/**********************    COPYRIGHT 2014-2100,  Eybond ************************ 
 * @File    : UpdateTask.c
 * @Author  : CGQ
 * @Date    : 2017-12-14
 * @Brief   : 
 ******************************************************************************/
#ifdef _PLATFORM_M26_
#include "ql_system.h"
#endif

#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#endif

#include "eyblib_typedef.h"
#include "eyblib_r_stdlib.h"

#include "eybpub_utility.h"
#include "eybpub_File.h"
#include "eybpub_run_log.h"
#include "eybpub_Debug.h"

#include "eybapp_appTask.h"
#include "UpdateTask.h"
#include "SelfUpdate.h"

#include "DeviceUpdate.h"
#include "Device.h"

#ifdef _PLATFORM_M26_
/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
void proc_update_task(s32_t taskId)
{
	int ret;
	ST_MSG msg;
	
	APP_DEBUG("Update task run!!\r\n");
	

	while (TRUE)
	{
		Ql_OS_GetMessage(&msg);

		switch(msg.message)
		{
			case SELF_UPDATE_ID:
				Ql_Sleep(3000);
				log_save("App satrt update");
				ret = Update_Self((File_t*)msg.param1);
				if (ret != 0)
				{
					log_saveAbnormal("M26 app update fail: ", ret);
				}
				break;
			case DEVICE_UPDATE_ID:
				ret = Update_startDevice((File_t*)msg.param1, (DeviceType_t*)msg.param2);
				if (ret != 0)
				{
					log_saveAbnormal("Device update fail: ", ret);
					break;
				}
			case DEVICE_UPDATE_READY_ID:
				log_save("Device update ready");
			    Ql_Sleep(2000);
				Ql_OS_SendMessage(DEVICE_TASK, DEVICE_UPDATE_READY_ID, 0, 0);
				break;
			case DEVICE_UPDATE_END_ID:
				Update_end();
				break;//*/
			default:
				break;
		}
	}
}
#endif

#ifdef _PLATFORM_L610_

#include "grid_tool.h"
#include "eybpub_watchdog.h"
/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
void proc_update_task(s32_t taskId) {
  APP_DEBUG("Update task run...\r\n");
  int ret = 0;
  ST_MSG msg;
  r_memset(&msg, 0, sizeof(ST_MSG));

	//若APP升级重启后要停电一次，确认所有参数清0
	if(logpara_read(APP_UPDATA)){
		logpara_write(APP_UPDATA,0);
		Watchdog_stop();
	}
		

  while (1) {
    fibo_queue_get(UPDATE_TASK, (void *)&msg, 0);
    switch(msg.message) {
      case SELF_UPDATE_ID:
        fibo_taskSleep(3000);
		log_save("App start update");
		ret = Update_Self((File_t*)msg.param1);
		if (ret != 0) {
          log_save("L610 app update fail: %d", ret);
        }
        break;
      case DEVICE_UPDATE_ID:
        ret = Update_startDevice((File_t*)msg.param1, (DeviceType_t*)msg.param2);
        if (ret != 0) {
          log_save("Device update fail: %d", ret);
          break;
        }
      case DEVICE_UPDATE_READY_ID:
        log_save("Device update ready");
        fibo_taskSleep(2000);
        Eybpub_UT_SendMessage(EYBDEVICE_TASK, DEVICE_UPDATE_READY_ID, 0, 0);
        break;
      case DEVICE_UPDATE_END_ID:
        Update_end();
        break;
      case APP_MSG_TIMER_ID:
        break;
      default:
        break;
    }
  }
}
#endif
/*********************************FILE END*************************************/

