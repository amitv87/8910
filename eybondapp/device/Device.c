/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
  *@brief   : Device.c
  *@notes   : 2017.12.05 CGQ
*******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_system.h"
#include "ql_stdlib.h"
#include "ql_timer.h"
#include "ql_memory.h"
#include "ql_error.h"
#endif
#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#endif

#include "eyblib_list.h"
#include "eyblib_swap.h"
#include "eyblib_memory.h"
#include "eyblib_r_stdlib.h"

#include "eybpub_Debug.h"
#include "eybpub_Status.h"
#include "eybpub_watchdog.h"
#include "eybpub_SysPara_File.h"
#include "eybpub_run_log.h"
#include "eybpub_utility.h"
#include "eybpub_Clock.h"

#include "eybapp_appTask.h"
#ifdef _PLATFORM_BC25_
#include "NB_Net.h"
#endif
#ifdef _PLATFORM_L610_
#include "4G_net.h"
// #include "ali_data_packet.h"
#endif

#include "Device.h"
#include "DeviceIO.h"
#include "Protocol.h"

#include "DeviceUpdate.h"
// #include "selfUpdate.h"
// #include "UpdateTask.h"
#include "Sineng.h"
// #include "CommonServer.h"     //mike 20200804
#include "eybpub_Debug.h"

#define DEVICE_LOCK     (0x5A)
#define DEVICE_UNLOCK   (0x00)

#define DEVICE_MAX_OVERTIME 300
//#define DEVICE_MAX_OVERTIME 360   //Luee_d
ListHandler_t DeviceList;  //

static DeviceCmd_t *currentCmd;     //
static u8_t currentStep;            //
static u16_t DeviceOvertime;        // device overtime cnt
static int watiTime;
static u32_t m_timeCheck_DEV = 0;
static u8_t device_cmp(void *src, void *dest);
static void device_callback(DeviceAck_e ack);
static void deviceCmdSend(void);

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
#ifdef _PLATFORM_BC25_
void proc_device_task(s32 taskId) {
  ST_MSG msg;
  int deviceResetCnt = 0;
  u32_t uStackSize = 0;

  deviceResetCnt = 0;
  currentStep = 0;
  DeviceOvertime = 0;
  watiTime = 5;   // wait 5sec start get device date
  m_timeCheck_DEV = 0;

  while (TRUE) {
    Ql_OS_GetMessage(&msg);
    switch (msg.message) {
      case APP_MSG_UART_READY:  // DEBUG串口OK
        APP_DEBUG("Get APP_CMD_UART_READY MSG\r\n");
        APP_PRINT("Devce task run...\r\n");
        list_init(&DeviceList);
//        DeviceIO_init(null);
//        Protocol_init();
        break;
      case NET_MSG_RIL_READY:
        APP_DEBUG("Get NET_MSG_RIL_READY MSG\r\n");
        DeviceIO_init(null);
        Protocol_init();
        break;
      case NET_MSG_RIL_FAIL:
        break;
      case NET_MSG_SIM_READY:
        APP_DEBUG("Get NET_MSG_SIM_READY MSG\r\n");
        break;
      case NET_MSG_SIM_FAIL:
        break;
      case NET_MSG_GSM_READY:    // 注网OK
        APP_DEBUG("Get NET_MSG_GSM_READY MSG\r\n");
        break;
      case NET_MSG_GSM_FAIL:    // 注网FAIL
        break;
      case NET_MSG_NET_READY:   // 服务器OK
        APP_DEBUG("Get NET_MSG_NET_READY MSG\r\n");
        break;
      case NET_MSG_NET_FAIL:    // 服务器FAIL
        break;
      case DEVICE_RESTART_ID:
        APP_DEBUG("deviceResetCnt %d!!\r\n", deviceResetCnt);
        /*        if (0 != runTimeCheck(4, 19)) { // 路灯是晚上工作的，所以白天没有数据判断是正常的
                  deviceResetCnt = 0;
                  DeviceOvertime = 0;
                  watiTime = 10;
                  break;
                } else if (++deviceResetCnt > 12) {
                  APP_DEBUG("Device no command ack reboot!!\r\n");
                  log_save("Device no command ack reboot!!");
                  Watchdog_stop();
                  Ql_Reset(0);  // mike 20200909
                  break;
                } else {
                  log_save("Device modular reset!!");
                  APP_DEBUG("Device modular reset!!\r\n");
                  msg.param1 = DEVICE_MONITOR_NUM;
                } */
        if (++deviceResetCnt >= 12) {
          if (0 == runTimeCheck(4, 19)) {
            APP_PRINT("Time is from 4 to 19, not reboot!!\r\n");
            deviceResetCnt = 0;
            DeviceOvertime = 0;
            watiTime = 10;
            break;
          }
          APP_DEBUG("Device no command ack, reboot!!\r\n");
          log_save("Device no command ack, reboot!!");
          Watchdog_stop();
          Ql_Reset(0);  // mike 20200909
          break;
        } else {
          log_save("Device modular reset!!");
          APP_DEBUG("Device modular reset!!\r\n");
          msg.param1 = DEVICE_MONITOR_NUM;
        }
        DeviceOvertime = 0;
      case SYS_PARA_CHANGE:   //system parameter change
        if (g_UARTIO_AT_enable == 1) {
          APP_DEBUG("Device IO UART AT mode is enable, don't reset device!!\r\n");
          break;
        }
//        if (msg.param1 == DEVICE_MONITOR_NUM || msg.param1 == DEVICE_PROTOCOL || msg.param1 == METER_PROTOCOL // mike 20201029
          if (msg.param1 == DEVICE_MONITOR_NUM || msg.param1 == DEVICE_PROTOCOL
            || msg.param1 == DEVICE_UART_SETTING || msg.param1 == DEVICE_SYSTEM) {
          DeviceIO_reset();
          Protocol_clean();
          Device_clear();
//          memory_trans(Debug_output);   //mike 20200805
//          CommonServer_DeviceInit();    //mike 20200805
          Protocol_init();
          deviceLEDOff();
        }
        watiTime = 10;
        break;
      case APP_MSG_DEVTIMER_ID:     //mike 20200915
        m_timeCheck_DEV++;
        if (m_timeCheck_DEV >= 5) {
          uStackSize = Ql_OS_GetCurrenTaskLeftStackSize();
          APP_DEBUG("Device task stack size:%ld!!\r\n", uStackSize);
          m_timeCheck_DEV = 0;
        }
//        APP_DEBUG("Get APP_MSG_DEVTIMER_ID MSG,watiTime:%04X step:%d\r\n", watiTime, currentStep);
        if ((watiTime & 0x8000) == 0x8000 || --watiTime > 0) {
          break;
        }
      case DEVICE_CMD_ID:
        watiTime = 0x8000;
        deviceCmdSend();
        break;
//    case DEVICE_UPDATE_READY_ID: //Device update    //mike 20200804
//      Update_ready();
//      break;
      case DEVICE_PV_SCAN_ID:    //Device PV scan
        PV_Scan();
        break;
      case DEVICE_PV_GET_ID:   //Device PV Data Get
        PV_dataGet();
        break;
      default:
        break;
    }
  }
}
#endif
/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t deviceClear(void *payload, void *point) {
  APP_DEBUG("deviceClear\r\n");
  DeviceCmd_clear((Device_t *)payload);
  return 1;
}
void Device_clear(void) {
  APP_DEBUG("Device_clear\r\n");
  list_trans(&DeviceList, deviceClear, null);
  list_delete(&DeviceList);
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t deviceCmdClear(void *payload, void *point) {
  DeviceCmd_t *cmd = (DeviceCmd_t *)payload;
  APP_DEBUG("deviceCmdClear\r\n");
  if (cmd->ack.payload != NULL) {
    memory_release(cmd->ack.payload);
  }
  if (cmd->cmd.payload != NULL) {
    memory_release(cmd->cmd.payload);
  }
  return 1;
}

void DeviceCmd_clear(Device_t *dev) {
  APP_DEBUG("DeviceCmd_clear\r\n");
  list_trans(&dev->cmdList, deviceCmdClear, null);
  list_delete(&dev->cmdList);
//  if (dev->explain != NULL) {
//    memory_release(dev->explain); // mike 20201231 打开透传会死机 与deviceClear冲突
//  }                               // modbus dev的explain 和ESP dev的explain申请和释放有冲突
  currentStep = 0;
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void Device_add(Device_t *device) {
  APP_DEBUG("Device_add\r\n");
  list_bottomInsert(&DeviceList, device);
  device->lock = DEVICE_UNLOCK;
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void Device_inset(Device_t *device) {
  APP_DEBUG("Device_inset\r\n");
  list_topInsert(&DeviceList, device);
  device->lock = DEVICE_UNLOCK;
  currentStep = 0;
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void Device_remove(Device_t *device) {
  APP_DEBUG("Device_remove\r\n");
  Device_t *dev = list_find(&DeviceList, device_cmp, device);

  if (dev != null) {
    APP_DEBUG("dev Clear\r\n");
    DeviceCmd_clear(dev);
    list_nodeDelete(&DeviceList, dev);
  }
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t deviceTypeRemove(void *payload, void *point) {
  Device_t *dev = (Device_t *)payload;

  if (dev->type == *(u16_t *)point) {
    DeviceCmd_clear(dev);
    list_nodeDelete(&DeviceList, dev);
  }
  return 1;
}
void Device_removeType(u16_t type) {
  list_trans(&DeviceList, deviceTypeRemove, &type);
}

/*******************************************************************************
  * @brief  device_cmp
  * @note   None
  * @param  None
  * @retval
*******************************************************************************/
static u8_t device_cmp(void *src, void *dest) {
  Device_t *pSrc = (Device_t *)src;
  Device_t *pDest = (Device_t *)dest;

  return  r_memcmp(&pSrc->callBack, &pDest->callBack, (int)(pSrc + 1) - (int)(&pSrc->callBack));
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
// DeviceInfo_t deviceInfo;
static void deviceCmdSend(void) {
  static DeviceInfo_t deviceInfo;
  static Device_t *currentDevice;     //

//  APP_DEBUG("Device Step %d waitTime is %04X!!\r\n", currentStep, watiTime);
  switch (currentStep) {
    case 0:
      currentDevice = null;
      currentCmd = null;
      deviceInfo.callback = device_callback;
    case 1:
      currentDevice = list_nextData(&DeviceList, currentDevice);    // 定时获取列表中需要执行指令的设备节点
      if (currentDevice == null || currentDevice->lock == DEVICE_LOCK) {
        if (++DeviceOvertime >= DEVICE_MAX_OVERTIME) {
          APP_DEBUG("Device no command in %d sec(DeviceOvertime), reset!!\r\n", DeviceOvertime);
          //log_save("Device no command in %d sec, reset!!", DeviceOvertime);
#ifdef _PLATFORM_BC25_
          Ql_OS_SendMessage(EYBDEVICE_TASK, DEVICE_RESTART_ID, 0, 0);
#endif
#ifdef _PLATFORM_L610_
          Eybpub_UT_SendMessage(EYBDEVICE_TASK, DEVICE_RESTART_ID, 0, 0);
#endif
        }
        APP_DEBUG("currentDevice is null,DeviceOvertime:%d!!\r\n", DeviceOvertime);
        watiTime = 1;
        //watiTime = 2;   //Luee_d
      }
      else if (DeviceIO_lockState() == null) {
        APP_DEBUG("DeviceIO_lockState is null!!Step from 1 to 3!!\r\n");
        currentStep = 3;
        currentCmd = null;
        if (currentDevice->cfg != null) {
          APP_DEBUG("currentDevice->cfg is %ld!!\r\n", currentDevice->cfg->baudrate);
          DeviceIO_init(currentDevice->cfg);
          watiTime = 1;
        } else if (DeviceIO_cfgGet() == null) {
          APP_DEBUG("Uart No Init!!\r\n");
          log_save("Uart No Init!!");
          currentStep = 1;
        }
      } else {
        APP_DEBUG("Device output is lock!!\r\n");   //mike 20200817
        log_save("Device output is lock!!");
        watiTime = 10;
      }
      break;
    case 2:
      currentDevice->lock = DEVICE_LOCK;
      if (currentDevice->callBack == null || currentDevice->callBack(currentDevice) == 0) {
        APP_DEBUG("Callback is null or return 0, Step from 2 to 1!!\r\n");
        currentStep = 1;
      } else {
        APP_DEBUG("Step from 2 to 3!!\r\n");
        currentStep  = 3; //设备数据再次更新，增快数据上线速度 CGQ 2019.04.30
      }
      break;
    case 3:
      currentCmd = list_nextData(&currentDevice->cmdList, currentCmd);  // 找到当前执行设备需要执行的指令
      if (currentCmd == null) {
        currentStep--;
        APP_DEBUG("Step from 3 to 2!!\r\n");
        break;
      }
      currentStep++;
      watiTime = 0;
      APP_DEBUG("Step from 3 to 4!!\r\n");
      break;
    case 4:
      if (null != currentCmd) {
        DeviceAck_e e;
        deviceInfo.waitTime = currentCmd->waitTime;
        deviceInfo.buf = &currentCmd->ack;    // mike 20200926
        APP_DEBUG("Device ack return buf size:%d\r\n", deviceInfo.buf->size);
        if (deviceInfo.buf->size == 0) {
          deviceInfo.buf->size = DEVICE_ACK_SIZE;
          deviceInfo.buf->payload = memory_apply(DEVICE_ACK_SIZE);
          currentCmd->ack.size = deviceInfo.buf->size;
          currentCmd->ack.payload = deviceInfo.buf->payload;
        }
//        APP_DEBUG("Device buf size:%d\r\n", deviceInfo.buf->size);
//        APP_DEBUG("Device waittime:%d\r\n", deviceInfo.waitTime);
//        APP_DEBUG("Device buf length:%d\r\n", deviceInfo.buf->lenght);
//        APP_DEBUG("Device buf:%s\r\n", deviceInfo.buf->payload);
//        DeviceIO_init(null);  // 测试 下发指令前重新配置串口，接收完数据后关闭串口
        e = DeviceIO_write(&deviceInfo, currentCmd->cmd.payload, currentCmd->cmd.lenght);   // 把指令写到串口
//      APP_PRINT("currentCmd->cmd:");
//	    out_put_buffer(currentCmd->cmd.payload,currentCmd->cmd.lenght);	
//		APP_PRINT("currentCmd->cmd len: %d\r\n",currentCmd->cmd.lenght);
        watiTime = 10;
        if (DEVICE_ACK_FINISH != e) {
          APP_DEBUG("Device command send fail %02X\r\n", e);
//          log_saveAbnormal("Device command send fail ", e);     //mike 20200824
          log_save("Device command send fail %02X", e);
          if (e == DEVICE_ACK_HARD_FAULT) {
            DeviceIO_reset();
          }
          currentStep = 1;
        }
//        else {
//          APP_DEBUG("Device command send finish %02X\r\n", e);
//        }

        if (++DeviceOvertime > 180) {
          //if (++DeviceOvertime > 380) {   //Luee_d
#ifdef _PLATFORM_BC25_
          Ql_OS_SendMessage(EYBDEVICE_TASK, DEVICE_RESTART_ID, 0, 0);
#endif
#ifdef _PLATFORM_L610_
          Eybpub_UT_SendMessage(EYBDEVICE_TASK, DEVICE_RESTART_ID, 0, 0);
#endif
        }
      }
      break;
    default:
      currentStep = 0;
      break;
  }

//  APP_DEBUG("watiTime %04X\r\n", watiTime);
  if (watiTime == 0x8000) {
#ifdef _PLATFORM_BC25_    
    Ql_OS_SendMessage(EYBDEVICE_TASK, DEVICE_CMD_ID, 0, 0);
#endif
#ifdef _PLATFORM_L610_
    Eybpub_UT_SendMessage(EYBDEVICE_TASK, DEVICE_CMD_ID, 0, 0);
#endif
  }
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static void device_callback(DeviceAck_e ack) {
  APP_DEBUG("ack %04X Step: %d state:%d DeviceOvertime:%d\r\n", ack, currentStep, currentCmd->state, DeviceOvertime);
  if (ack == DEVICE_ACK_OVERTIME) {
    deviceLEDOff();
    ++DeviceOvertime;
/* #if TEST_RF
    if (++DeviceOvertime > 180) {
#ifdef _PLATFORM_BC25_        
      Ql_OS_SendMessage(EYBDEVICE_TASK, DEVICE_RESTART_ID, 0, 0);
#endif
#ifdef _PLATFORM_L610_
      Eybpub_UT_SendMessage(EYBDEVICE_TASK, DEVICE_RESTART_ID, 0, 0);
#endif
      return;
    }
#endif */
    if (currentCmd->state != 0) {
      currentStep--;
    }
  } else {
    APP_DEBUG("ack %04X:not overtime, DeviceOvertime:%d\r\n", ack, DeviceOvertime);
    currentStep--;
    DeviceOvertime = 0;
    deviceLEDOn();
  }
  currentCmd->state = ack;
#ifdef _PLATFORM_BC25_
  Ql_OS_SendMessage(EYBDEVICE_TASK, DEVICE_CMD_ID, 0, 0);
#endif
#ifdef _PLATFORM_L610_
  Eybpub_UT_SendMessage(EYBDEVICE_TASK, DEVICE_CMD_ID, 0, 0);
#endif
}


#ifdef _PLATFORM_L610_    
void proc_device_task (s32_t taskId) {
  ST_MSG msg;
  int deviceResetCnt = 0;
  APP_PRINT("Devce task run...\r\n");
  DevIO_halGPIO();  // INIT set URAT GPIO
  r_memset(&msg, 0, sizeof(ST_MSG));
  deviceResetCnt = 0;
  currentStep = 0;
  DeviceOvertime = 0;
  watiTime = 5;   // wait 5sec start get device date
  m_timeCheck_DEV = 0;
  list_init(&DeviceList);
    
  while (1) {
    fibo_queue_get(EYBDEVICE_TASK, (void *)&msg, 0);  
//    APP_PRINT("msg.message = %x\r\n",msg.message);
    switch (msg.message) {
      case APP_MSG_UART_READY:  // 串口OK
        APP_DEBUG("Get APP_MSG_UART_READY MSG\r\n");
        DeviceIO_init(null);
        Protocol_init();
        break;
      case NET_MSG_SIM_READY:
        APP_DEBUG("Get NET_MSG_SIM_READY MSG\r\n");
        break;
      case NET_MSG_SIM_FAIL:
        break;
      case NET_MSG_GSM_READY:   // 注网OK
        APP_DEBUG("Get NET_MSG_GSM_READY MSG\r\n");
        break;
      case NET_MSG_GSM_FAIL:    // 注网FAIL
        break;
      case NET_MSG_NET_READY:   // 服务器OK
        APP_DEBUG("Get NET_MSG_NET_READY MSG\r\n");
        break;
      case NET_MSG_NET_FAIL:    // 服务器FAIL
        break;
      case DEVICE_RESTART_ID:
        APP_DEBUG("deviceResetCnt %d!!\r\n", deviceResetCnt);
        if (++deviceResetCnt >= 12) {
          if (0 != runTimeCheck(4, 19)) {
            APP_PRINT("Time is not from 4 to 19, not reboot!!\r\n");
            deviceResetCnt = 0;
            DeviceOvertime = 0;
            watiTime = 10;
            break;
          }
          APP_DEBUG("Device no command ack, reboot!!\r\n");
          log_save("Device no command ack, reboot!!");
          Watchdog_stop();
          //fibo_softReset();
          break;
        } else {
          log_save("Device modular reset!!");
          APP_DEBUG("Device modular reset!!\r\n");
          msg.param1 = DEVICE_MONITOR_NUM;
        }
        DeviceOvertime = 0;
      case SYS_PARA_CHANGE:   // system parameter change
        if (g_UARTIO_AT_enable == 1) {
          APP_DEBUG("Device IO UART AT mode is enable, don't reset device!!\r\n");
          break;
        }
        if (msg.param1 == DEVICE_MONITOR_NUM || msg.param1 == DEVICE_PROTOCOL
          || msg.param1 == DEVICE_UART_SETTING || msg.param1 == DEVICE_SYSTEM) {
            APP_DEBUG("Reset device!!\r\n");
            DeviceIO_reset();   // save
            Protocol_clean();   // save
            Device_clear();
            Protocol_init();
            deviceLEDOff();     // save
        }
        watiTime = 10;
        break;
      case APP_MSG_DEVTIMER_ID:  // mike 20200915
        m_timeCheck_DEV++;
        if (m_timeCheck_DEV >= 10) {
          u32_t heepsize = 0, heep_avail = 0, heep_maxblock =0;
          fibo_get_heapinfo(&heepsize, &heep_avail, &heep_maxblock);
          u32_t uTick = fibo_getSysTick();
          u32_t freesize = fibo_file_getFreeSize();
          APP_DEBUG("SysTick:%ld ROM free size:%ld heep size:%ld avail:%ld maxblock:%ld!!\r\n", uTick/16384, freesize, heepsize, heep_avail, heep_maxblock);
          u32_t app_queue_num = fibo_queue_space_available(EYBAPP_TASK);
          u32_t net_queue_num = fibo_queue_space_available(EYBNET_TASK);
          u32_t device_queue_num = fibo_queue_space_available(EYBDEVICE_TASK);
          u32_t eybond_queue_num = fibo_queue_space_available(EYBOND_TASK);
          u32_t ali_queue_num = fibo_queue_space_available(ALIYUN_TASK);
          u32_t fota_queue_num = fibo_queue_space_available(FOTA_TASK);
          u32_t update_queue_num = fibo_queue_space_available(UPDATE_TASK);
          APP_DEBUG("Queue num app:%ld net:%ld device:%ld eybond:%ld ali:%ld fota:%ld update:%ld!!\r\n", \
            app_queue_num, net_queue_num, device_queue_num, eybond_queue_num, ali_queue_num, fota_queue_num, update_queue_num);
          m_timeCheck_DEV = 0;
        }
        APP_DEBUG("Get APP_MSG_DEVTIMER_ID MSG,waitTime:%04X step:%d\r\n", watiTime, currentStep);
        if ((watiTime & 0x8000) == 0x8000 || --watiTime > 0) {
          break;
        }
      case DEVICE_CMD_ID:
//        APP_DEBUG("will send cmd\r\n");
        watiTime = 0x8000;
        deviceCmdSend();
        break;
      case DEVICE_UPDATE_READY_ID: // Device update    //mike 20200804
        Update_ready();
        break;
      case DEVICE_PV_SCAN_ID:    // Device PV scan
//        PV_Scan();
        break;
      case DEVICE_PV_GET_ID:   // Device PV Data Get
//        PV_dataGet();
        break;
      default:
        break;
    }
  }
  fibo_thread_delete();
}

#endif

/******************************************************************************/



