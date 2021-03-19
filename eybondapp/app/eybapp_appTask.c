/**********************    COPYRIGHT 2014-2100,  Eybond ************************
 * @File    : appTask.c
 * @Author  : CGQ
 * @Date    : 2017-12-01
 * @Brief   :
 ******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ril.h"
#include "ril_util.h"

#include "ql_system.h"
#include "ql_stdlib.h"
#include "ql_power.h"
#include "ql_memory.h"
#include "ql_timer.h"
#include "ql_error.h"
#include "ql_fs.h"
#define g_recName_parameter_aa  "paraconfig_file_a.ini"
#define g_recName_parameter_ab  "paraconfig_file_b.ini"
#define run_log_aa  "run_log_a.ini"
#endif

#ifndef __TEST_FOR_UFS__
#define __TEST_FOR_UFS__
#endif

#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#define g_recName_parameter_aa  "/paraconfig_file_a.ini"
#define g_recName_parameter_ab  "/paraconfig_file_b.ini"
//#define run_log_aa  "/run_log_a.ini"
#define run_log_aa  "/log_file.ini"
#endif
#include "eybpub_Debug.h"
#include "eybpub_Status.h"
#include "eybpub_utility.h"
#include "eybpub_watchdog.h"
#include "eybpub_run_log.h"
#include "eybpub_Key.h"
#include "eybpub_adc.h"
#include "eybpub_SysPara_File.h"
#include "eybpub_Clock.h"

#include "eyblib_typedef.h"
#include "eyblib_swap.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_memory.h"
#include "eybapp_appTask.h"

#include "4G_net.h"

#include "Device.h"
// #include "DeviceIO.h"
#include "eybond.h"

#include "grid_tool.h"
// #include "x25Qxx.h"  // mike 20200805
// #include "Eybond.h"  // mike 20200805
// #include "CommonServer.h"  // mike 20200805
// #include "antiReflux.h"  // mike 20200805
// #include "sarnath.h"  // mike 20200805

static void_fun_bufp outputFun;
static DeviceInfo_t cmdHead;
static u16_t deviceLockTime;
static u16_t logGetFlag;

static u32_t WDG_timer = APP_WDG_TIMER_ID;
static u32_t APP_timer = APP_USER_TIMER_ID;
static u32_t APP_time_Interval = 500;
static u32_t WDG_time_Interval = 1000;

static u32_t m_wdgCnt = 0;

#ifdef _PLATFORM_BC25_
static void UserTimerAPPscallback(u32_t timerId, void *param);    // 应用消息定时器callback
static void UserTimerWDGcallback(u32_t timerId, void *param);     // 看门狗定时器callback
#endif

#ifdef _PLATFORM_L610_
static void UserTimerAPPscallback(void *param);    // 应用消息定时器callback
static void UserTimerWDGcallback(void *param);     // 看门狗定时器callback
#endif

static void strCmp(Buffer_t *strBuf, void_fun_bufp output);
static void outputCh(Buffer_t *buf);
static void UanrtCh(DeviceAck_e e);
static int logGet(Buffer_t *buf);
static u32_t m_timeCnt = 0;

#ifdef _PLATFORM_BC25_
static s32 APP_ATResponse_Handler(char *line, u32 len, void *userData);

/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
void proc_app_task(s32_t taskId) {
//  u32 timer = 0;  // unit 1s
  ST_MSG msg;
  Buffer_t *buf;
  s32_t ret = QL_RET_OK;

  outputFun = null;
  logGetFlag = 0;
  deviceLockTime = 0;

  // Disable sleep mode.
  Ql_SleepDisable();
  Debug_init();
  APP_PRINT("App task run...\r\n");

  Watchdog_init();

  NetLED_Init();
  GSMLED_Init();
  deviceLEDInit();
  Beep_Init();
  Key_init();
  ADC_Init();

  ret = Ql_Timer_Register(WDG_timer, UserTimerWDGcallback, NULL);  // 注册外部看门狗Timer
  if (ret < QL_RET_OK) {
    log_save("Register WGD timer(%d) failed!! ret = %d", WDG_timer, ret);
  }
  ret = Ql_Timer_Start(WDG_timer, WDG_time_Interval, TRUE);
  if (ret < 0) {
    log_save("Start WGD timer failed, ret = %d", ret);
  }
#ifdef __TEST_FOR_UFS__
  Enum_FSStorage storage = Ql_FS_UFS;
  APP_DEBUG("QuecOpen: FILE(UFS) TEST!\r\n");
  //  format
  //  ret = Ql_FS_Format(storage);
  //  APP_DEBUG("Ql_FS_Format(storage=%d)ret =%d \r\n", storage, ret);
  s32_t space = 0;      // 注意不要做%和/运算
  // check freespace
  space  = Ql_FS_GetFreeSpace(storage);
  APP_DEBUG("Ql_FS_GetFreeSpace(storage=%d) =%lld \r\n", storage, space);
  // check total space
  space = Ql_FS_GetTotalSpace(storage);
  APP_DEBUG("Ql_FS_GetTotalSpace(storage=%d)=%lld \r\n", storage, space);
  APP_DEBUG("Check para_file_a \r\n");
  s32_t file_a_size = Ql_FS_GetSize(g_recName_parameter_aa);
  APP_DEBUG("file_a_size is %ld\r\n", file_a_size);
  if (file_a_size > 0) {
    APP_DEBUG("para_file_a is exist, size:%d\r\n", file_a_size);
    //    Ql_FS_Delete(g_recName_parameter_aa);   // for testing
  }
  APP_DEBUG("Check para_file_b \r\n");
  s32_t file_b_size = Ql_FS_GetSize(g_recName_parameter_ab);
  APP_DEBUG("file_b_size is %ld\r\n", file_b_size);
  if (file_b_size > 0) {
    APP_DEBUG("para_file_b is exist, size:%d\r\n", file_b_size);
    //    Ql_FS_Delete(g_recName_parameter_ab);   // for testing
  }
  s32_t file_log_size = Ql_FS_GetSize(run_log_aa);
  APP_DEBUG("file_log_size is %ld\r\n", file_log_size);
  if (file_log_size > 0) {
    APP_DEBUG("run_log_a is exist, size:%d\r\n", file_log_size);
    if (file_log_size >  152756) {
      APP_DEBUG("file_log_size is too big, delete it!\r\n");
      Ql_FS_Delete(run_log_aa);
    }
  }
#endif
//  SysPara_init();  // mike 重点死机问题函数, RIL库完成加载后初始化所有现场参数
  log_init();
  Clock_init();

  Ql_OS_SendMessage(EYBDEVICE_TASK, APP_MSG_UART_READY, 1, 0);
  Ql_OS_SendMessage(EYBNET_TASK, APP_MSG_UART_READY, 1, 0);
  Ql_OS_SendMessage(EYBOND_TASK, APP_MSG_UART_READY, 1, 0);

  while (TRUE) {
    Ql_OS_GetMessage(&msg);
    switch (msg.message) {
      case NET_MSG_RIL_READY:
        APP_DEBUG("Get NET_MSG_RIL_READY MSG\r\n");
        SysPara_init();  // mike 重点死机问题函数, RIL库完成加载后初始化所有现场参数
        Ql_OS_SendMessage(EYBDEVICE_TASK, NET_MSG_RIL_READY, 1, 0);
        ret = Ql_Timer_Register(APP_timer, UserTimerAPPscallback, &m_timeCnt);    // 注册APPTimer
        if (ret < QL_RET_OK) {
          log_save("Register app timer(%d) fail, ret = %d", APP_timer, ret);
        }
        break;
      case NET_MSG_RIL_FAIL:
        APP_DEBUG("Get NET_MSG_RIL_FAIL MSG\r\n");
        break;
      case NET_MSG_SIM_READY:   // SIM卡插入了
        APP_DEBUG("Get NET_MSG_SIM_READY MSG\r\n");
        ret = Ql_Timer_Start(APP_timer, APP_time_Interval, TRUE);
        if (ret < QL_RET_OK) {
          log_save("Start app timer failed, ret = %d", ret);
        }
        Ql_OS_SendMessage(EYBDEVICE_TASK, NET_MSG_SIM_READY, 1, 0);
        break;
      case NET_MSG_SIM_FAIL:
        APP_DEBUG("Get NET_MSG_SIM_FAIL MSG\r\n");
        break;
      case NET_MSG_GSM_READY:  // 注网成功消息
        APP_DEBUG("Get NET_MSG_GSM_READY MSG\r\n");
        /*        ret = Ql_Timer_Start(APP_timer, APP_time_Interval, TRUE);
                if (ret < 0) {
                  log_save("Start app timer failed, ret = %d", ret);
                } else {
                  APP_DEBUG("Start app timerId(%d) successfully, timer interval = %d, ret = %d\r\n", APP_timer, APP_time_Interval, ret);
                } */
        Ql_OS_SendMessage(EYBDEVICE_TASK, NET_MSG_GSM_READY, 1, 0);
        break;
      case NET_MSG_GSM_FAIL:
        Ql_OS_SendMessage(EYBDEVICE_TASK, NET_MSG_GSM_FAIL, 1, 0);
        break;
      case NET_MSG_NET_READY:    // 连接服务器成功消息
        APP_DEBUG("Get NET_MSG_NET_READY MSG\r\n");
        Ql_OS_SendMessage(EYBDEVICE_TASK, NET_MSG_NET_READY, 1, 0);
        break;
      case NET_MSG_NET_FAIL:    // 连接服务器成功消息
        APP_DEBUG("Get NET_MSG_NET_FAIL MSG\r\n");
        Ql_OS_SendMessage(EYBDEVICE_TASK, NET_MSG_NET_FAIL, 1, 0);
        break;
      case APP_CMD_BEEP_ID:  // mike 20200817 APP蜂鸣指令
        APP_DEBUG("App task APP_CMD_BEEP_ID\r\n");
        Beep_Run();
        break;
      case DEVICE_IO_ID:
        buf = (Buffer_t *)msg.param1;
        APP_DEBUG("App task DEVICE_IO_ID:%s %d\r\n", (char *) buf->payload, buf->lenght);
//        Beep_On(2);  // mike 20200814
        if (buf->lenght > 2 && 0 == Ql_strncmp((char *)buf->payload, "AT", 2)) {
          Ql_memcpy(&buf->payload[buf->lenght], "\r\n", 3);
          buf->lenght += 3;
          ret = Ql_RIL_SendATCmd((char *)buf->payload, buf->lenght, APP_ATResponse_Handler, (void_fun_bufp)((void *)msg.param2),
                                 0);  // AT指令的返回反馈给输入的串口
        } else {
          strCmp(buf, (void_fun_bufp)((void *)msg.param2));
        }
        break;
      case DEBUG_MSG_ID:
        buf = (Buffer_t *)msg.param1;
        APP_DEBUG("App task DEBUG_MSG_ID:%s %d\r\n", (char *) buf->payload, buf->lenght);
//        Beep_On(1);  // mike 20200814
        if (buf->lenght > 2 && 0 == Ql_strncmp((char *)buf->payload, "AT", 2)) {
          Ql_memcpy(&buf->payload[buf->lenght], "\r\n", 3);
          buf->lenght += 3;
          ret = Ql_RIL_SendATCmd((char *)buf->payload, buf->lenght, APP_ATResponse_Handler, (void_fun_bufp)((void *)msg.param2),
                                 0);
        } else {
          strCmp(buf, (void_fun_bufp)((void *)msg.param2));
        }
        break;
      case APP_MSG_DEVTIMER_ID:
//        Ql_OS_SendMessage(EYBDEVICE_TASK, APP_MSG_DEVTIMER_ID, 0, 0);
//        Ql_OS_SendMessage(EYBOND_TASK, APP_MSG_DEVTIMER_ID, 0, 0);
        break;
      case APP_CMD_WDG_ID:
        APP_DEBUG("App task APP_CMD_WDG_ID\r\n");
        ret = Ql_Timer_Stop(WDG_timer);
        if (ret != QL_RET_OK) {
          APP_DEBUG("Stop WDG_timer error!! ret:%d\r\n", ret);
        } else {
          APP_DEBUG("Stop WDG_timer Success!!\r\n");
        }
        break;
      case APP_MSG_WDG_ID:
        APP_DEBUG("App task APP_MSG_WDG_ID %d\r\n", m_wdgCnt);
        Watchdog_feed();
        m_wdgCnt++;
        if (m_wdgCnt == 1) {  // 开机后三秒跑马灯
          deviceLEDOff();
        } else if (m_wdgCnt == 2) {
          GSMLED_Off();
        } else if (m_wdgCnt == 3) {
          NetLED_Off();
        } else if (m_wdgCnt > 3) {
          m_wdgCnt = 4;
        }
        break;
      case APP_MSG_TIMER_ID:
//        APP_DEBUG("App task APP_MSG_TIMER_ID\r\n");
        Ql_OS_SendMessage(EYBNET_TASK, APP_MSG_TIMER_ID, 0, 0);
        Key_scan();
        Clock_Add();
        if ((DeviceIO_lockState() == &cmdHead) && deviceLockTime++ > (60 * 2)) {
          APP_DEBUG("DeviceIO_unlock\r\n");
          DeviceIO_unlock();
          if (cmdHead.buf != NULL) {
            Ql_MEM_Free(cmdHead.buf);
            cmdHead.buf = null;
          }
        }

        if (logGetFlag == 0x5AA5) { // 按条显示log信息
          Buffer_t log;
          int logIndex = 0;
          while (logIndex < 100) {
            Ql_memset(&log, 0, sizeof(Buffer_t));
            logGet(&log);
            if (log.lenght == 0 || log.payload == null) {  // 读完log后就不再显示了
              logGetFlag = 0;
              if (log.payload != NULL) {
                Ql_MEM_Free(log.payload);
                log.lenght = 0;
                log.size = 0;
              }
              break;
            } else {
              Print_output(log.payload, log.lenght);
              Print_output((u8_t *)"\r\n", 2);
              logIndex++;
            }
            if (log.payload != NULL) {
              Ql_MEM_Free(log.payload);
              log.lenght = 0;
              log.size = 0;
            }
          }
        }
        break;
      default:
        break;
    }
  }
}

/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
static void UserTimerAPPscallback(u32 timerId, void *param) {
  if (timerId == APP_timer) {
    if (*((s32_t *)param) == 0) {
      Ql_OS_SendMessage(EYBDEVICE_TASK, APP_MSG_DEVTIMER_ID, 0, 0);
      Ql_OS_SendMessage(EYBOND_TASK, APP_MSG_DEVTIMER_ID, 0, 0);
      *((s32_t *)param) += 1;
    } else {
      Ql_OS_SendMessage(EYBAPP_TASK, APP_MSG_TIMER_ID, 0, 0);
      *((s32 *)param) -= 1;;
    }
  }
}
/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
static void UserTimerWDGcallback(u32 timerId, void *param) {
  if (timerId == WDG_timer) {
    Ql_OS_SendMessage(EYBAPP_TASK, APP_MSG_WDG_ID, 0, 0);
  }
}

static s32 APP_ATResponse_Handler(char *line, u32 len, void *userData) {
  if (userData != null) {   // mike 利用callback把AT的返回反馈给AT输入的串口
    Buffer_t buf;
    void_fun_bufp AToutput = (void_fun_bufp)(userData);

    buf.lenght = len;
    buf.payload = (u8_t *)line;
    AToutput(&buf);
  }

//  Ql_UART_Write(DEBUG_PORT, (u8 *)line, len); // debug口重复显示一次 mike 20200817
  if (Ql_RIL_FindLine(line, len, "OK")) {
    return  RIL_ATRSP_SUCCESS;
  } else if (Ql_RIL_FindLine(line, len, "ERROR")) {
    return  RIL_ATRSP_FAILED;
  } else if (Ql_RIL_FindString(line, len, "+CME ERROR")) {
    return  RIL_ATRSP_FAILED;
  } else if (Ql_RIL_FindString(line, len, "+CMS ERROR:")) {
    return  RIL_ATRSP_FAILED;
  }
  return RIL_ATRSP_CONTINUE;  // continue wait
}
#endif

#ifdef _PLATFORM_L610_
void proc_app_task(s32_t taskId) {
  ST_MSG msg;
  Buffer_t *buf;

  outputFun = null;
  logGetFlag = 0;
  deviceLockTime = 0;
  int err = 0;

  // Disable sleep mode.
  fibo_setSleepMode(0);
  APP_PRINT("App task run...\r\n");
  r_memset(&msg, 0, sizeof(ST_MSG));


#ifdef __TEST_FOR_UFS__
  APP_DEBUG("FILE System TEST!\r\n");
  u32_t space = 0;      // 注意不要做%和/运算
  // check freespace
  space  = fibo_file_getFreeSize();
  APP_DEBUG("fibo_file_getFreeSize=%ld \r\n", space);
  // check total space
  s32_t file_a_size = fibo_file_getSize(g_recName_parameter_aa);
  APP_DEBUG("file_a_size is %ld\r\n", file_a_size);
  if (file_a_size > 0) {
    APP_DEBUG("para_file_a is exist, size:%ld\r\n", file_a_size);
  // fibo_file_delete(g_recName_parameter_aa);   // for testing
  }
  APP_DEBUG("Check para_file_b \r\n");
  s32_t file_b_size = fibo_file_getSize(g_recName_parameter_ab);
  APP_DEBUG("file_b_size is %ld\r\n", file_b_size);
  if (file_b_size > 0) {
    APP_DEBUG("para_file_b is exist, size:%ld\r\n", file_b_size);
    //    fibo_file_delete(g_recName_parameter_ab);   // for testing
  }
  s32_t file_log_size = fibo_file_getSize(run_log_aa);
  APP_DEBUG("file_log_size is %ld\r\n", file_log_size);
  if (file_log_size > 0) {
    APP_DEBUG("run_log_a is exist, size:%ld\r\n", file_log_size);
    if (file_log_size >  152756) {
      APP_DEBUG("file_log_size is too big, delete it!\r\n");
      fibo_file_delete(run_log_aa);
    }
  }
#endif
  log_init();
  Clock_init();
 // SysPara_init();  // mike 重点死机问题函数, RIL库完成加载后初始化所有现场参数

 // 系统参数初始化
  if(SysPara_init() != 0) {
    // 参数初始化失败
    while(1) {
      if (++err > 10) {
        fibo_softReset();
        err = 0;
      }
      fibo_taskSleep(1000);
    }
  }

  WDG_timer = fibo_timer_period_new(WDG_time_Interval, UserTimerWDGcallback, NULL);  // 注册外部看门狗Timer
  if (WDG_timer == 0) {
    log_save("Register hard wdt timer(%d) failed!!\r\n");
  }

  while (1) {
    fibo_queue_get(EYBAPP_TASK, (void *)&msg, 0);
    switch (msg.message) {
      case APP_MSG_UART_READY:
        APP_DEBUG("App task APP_MSG_UART_READY\r\n");
        Eybpub_UT_SendMessage(EYBNET_TASK, APP_MSG_UART_READY, 0, 0);
        Eybpub_UT_SendMessage(EYBDEVICE_TASK, APP_MSG_UART_READY, 0, 0);
        Eybpub_UT_SendMessage(EYBOND_TASK, APP_MSG_UART_READY, 0, 0);
        //Eybpub_UT_SendMessage(ALIYUN_TASK, APP_MSG_UART_READY, 0, 0);
        APP_timer = fibo_timer_period_new(APP_time_Interval, UserTimerAPPscallback, &m_timeCnt);    // 注册APPTimer
        if (APP_timer == 0) {
          log_save("Register app timer(%ld) fail", APP_timer);
        }
        break;
      case NET_MSG_SIM_READY:   // SIM卡插入了
        APP_DEBUG("Get NET_MSG_SIM_READY MSG\r\n");
        Eybpub_UT_SendMessage(EYBDEVICE_TASK, NET_MSG_SIM_READY, 0, 0);
        Eybpub_UT_SendMessage(EYBOND_TASK, NET_MSG_SIM_READY, 0, 0);
        break;
      case NET_MSG_SIM_FAIL:
        APP_DEBUG("Get NET_MSG_SIM_FAIL MSG\r\n");
        Eybpub_UT_SendMessage(EYBDEVICE_TASK, NET_MSG_SIM_FAIL, 0, 0);
        Eybpub_UT_SendMessage(EYBOND_TASK, NET_MSG_SIM_FAIL, 0, 0);
        break;
      case NET_MSG_GSM_READY:  // 注网成功消息
        APP_DEBUG("Get NET_MSG_GSM_READY MSG\r\n");
        Eybpub_UT_SendMessage(EYBDEVICE_TASK, NET_MSG_GSM_READY, 0, 0);
        Eybpub_UT_SendMessage(EYBOND_TASK, NET_MSG_GSM_READY, 0, 0);
        break;
      case NET_MSG_GSM_FAIL:
        Eybpub_UT_SendMessage(EYBDEVICE_TASK, NET_MSG_GSM_FAIL, 0, 0);
        Eybpub_UT_SendMessage(EYBOND_TASK, NET_MSG_GSM_FAIL, 0, 0);
        break;
      case NET_MSG_DNS_READY:
        Eybpub_UT_SendMessage(EYBDEVICE_TASK, NET_MSG_DNS_READY, 0, 0);
        Eybpub_UT_SendMessage(EYBOND_TASK, NET_MSG_DNS_READY, 0, 0);
        //Eybpub_UT_SendMessage(ALIYUN_TASK, NET_MSG_DNS_READY, 0, 0);
        APP_DEBUG("Net task NET_MSG_DNS_READY\r\n");
        break;
      case NET_MSG_DNS_FAIL:
        Eybpub_UT_SendMessage(EYBDEVICE_TASK, NET_MSG_DNS_FAIL, 0, 0);
        Eybpub_UT_SendMessage(EYBOND_TASK, NET_MSG_DNS_FAIL, 0, 0);
        //Eybpub_UT_SendMessage(ALIYUN_TASK, NET_MSG_DNS_FAIL, 0, 0);
        APP_DEBUG("Net task NET_MSG_DNS_FAIL\r\n");
        break;
      case NET_MSG_NET_READY:    // 连接服务器成功消息
        APP_DEBUG("Get NET_MSG_NET_READY MSG\r\n");
        Eybpub_UT_SendMessage(EYBDEVICE_TASK, NET_MSG_NET_READY, 0, 0);
        Eybpub_UT_SendMessage(EYBOND_TASK, NET_MSG_NET_READY, 0, 0);
        break;
      case NET_MSG_NET_FAIL:    // 连接服务器成功消息
        APP_DEBUG("Get NET_MSG_NET_FAIL MSG\r\n");
        Eybpub_UT_SendMessage(EYBDEVICE_TASK, NET_MSG_NET_FAIL, 0, 0);
        Eybpub_UT_SendMessage(EYBOND_TASK, NET_MSG_NET_FAIL, 0, 0);
        break;      
      case APP_CMD_BEEP_ID:  // mike 20200817 APP蜂鸣指令
        APP_DEBUG("App task APP_CMD_BEEP_ID\r\n");
        Beep_Run();
        break;
      case APP_MSG_WDG_ID:
//        APP_DEBUG("App task APP_MSG_WDG_ID %ld\r\n", m_wdgCnt);
        Watchdog_feed();
        m_wdgCnt++;
        if (m_wdgCnt == 1) {  // 开机后三秒跑马灯
          deviceLEDOff();
        } else if (m_wdgCnt == 2) {
          GSMLED_Off();
        } else if (m_wdgCnt == 3) {
          NetLED_Off();
        } else if (m_wdgCnt > 3) {
          m_wdgCnt = 4;
        }
        break;
      case APP_MSG_TIMER_ID: {
//        APP_DEBUG("App task APP_MSG_TIMER_ID\r\n");
        Eybpub_UT_SendMessage(EYBNET_TASK, APP_MSG_TIMER_ID, 0, 0);
        Key_scan();
        Clock_Add();
        if ((DeviceIO_lockState() == &cmdHead) && deviceLockTime++ > (60 * 2)) {
          APP_DEBUG("DeviceIO_unlock\r\n");
          DeviceIO_unlock();
          if (cmdHead.buf != NULL) {
            memory_release(cmdHead.buf);
            cmdHead.buf = null;
          }
        }

        if (logGetFlag == 0x5AA5) { // 按条显示log信息
          Buffer_t log;
          int logIndex = 0;
          while (logIndex < 100) {
            r_memset(&log, 0, sizeof(Buffer_t));
            logGet(&log);
            if (log.lenght == 0 || log.payload == null) {  // 读完log后就不再显示了
              logGetFlag = 0;
              memory_release(log.payload);
              log.lenght = 0;
              log.size = 0;
              break;
            } else {
              Print_output(log.payload, log.lenght);
              Print_output((u8_t *)"\r\n", 2);
              logIndex++;
            }
            memory_release(log.payload);
            log.lenght = 0;
            log.size = 0;
          }
        }
        break;
      }
      case APP_DEBUG_MSG_ID: {
        buf = (Buffer_t *)msg.param1;
        APP_DEBUG("App task APP_DEBUG_MSG_ID:%s %d\r\n", (char *) buf->payload, buf->lenght);
        if (buf->lenght > 2 && 0 == r_strncmp((char *)buf->payload, "AT", 2)) {
          r_memcpy(&buf->payload[buf->lenght], "\r\n", 3);
          buf->lenght += 3;
          fibo_at_send(buf->payload, buf->lenght);
        } else {
          strCmp(buf, (void_fun_bufp)((void *)msg.param2));
        }
        break;
      }
      case APP_DEVICE_IO_ID: {
        buf = (Buffer_t *)msg.param1;
        APP_DEBUG("App task APP_DEVICE_IO_ID:%s %d\r\n", (char *) buf->payload, buf->lenght);
        if (buf->lenght > 2 && 0 == r_strncmp((char *)buf->payload, "AT", 2)) {
          r_memcpy(&buf->payload[buf->lenght], "\r\n", 3);
          buf->lenght += 3;
          fibo_at_send(buf->payload, buf->lenght);
        } else {
          strCmp(buf, (void_fun_bufp)((void *)msg.param2));
        }
        break;
      }
      default:
        break;
    }
  }

  fibo_thread_delete();
}

/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
static void UserTimerAPPscallback(void *param) {
  Eybpub_UT_SendMessage(COMMON_SERVER_TASK, APP_MSG_TIMER_ID, 0, 0);
  //if (*((s32_t *)param) == 0) {
  //  if (m_wdgCnt > 3) {
      Eybpub_UT_SendMessage(EYBDEVICE_TASK, APP_MSG_DEVTIMER_ID, 0, 0);
      Eybpub_UT_SendMessage(EYBOND_TASK, APP_MSG_DEVTIMER_ID, 0, 0);
      //Eybpub_UT_SendMessage(ALIYUN_TASK, APP_MSG_TIMER_ID, 0, 0);
      //Eybpub_UT_SendMessage(FOTA_TASK, APP_MSG_TIMER_ID, 0, 0);
      Eybpub_UT_SendMessage(UPDATE_TASK, APP_MSG_TIMER_ID, 0, 0);
      //Eybpub_UT_SendMessage(COMMON_SERVER_TASK, APP_MSG_TIMER_ID, 0, 0);
    //}
    *((s32_t *)param) += 1;
  //} else {
    Eybpub_UT_SendMessage(EYBAPP_TASK, APP_MSG_TIMER_ID, 0, 0);
    *((s32_t *)param) -= 1;;
 // }
}
/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
static void UserTimerWDGcallback(void *param) {
  Eybpub_UT_SendMessage(EYBAPP_TASK, APP_MSG_WDG_ID, 0, 0);
}
#endif

/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
static void strCmp(Buffer_t *strBuf, void_fun_bufp callback_output) {
  APP_DEBUG("strCmp:%s %d\r\n", (char *) strBuf->payload, strBuf->lenght);
  const char testServer[] = "solar.eybond.com";
  char *str = NULL;
  int offset = 0;
  u32_t num = 0;
  Buffer_t buf;
  char *P1 = NULL;

  if (strBuf == null || strBuf->payload == null) {
    return;
  }

  str = (char *)strBuf->payload;
  str[strBuf->lenght] = '\0';

  if ((r_strncmp(str, "SET-", 4) == 0)
      || (r_strncmp(str, "SET_", 4) == 0)
      || (r_strncmp(str, "set_", 4) == 0)) {
    str += 4;
    num = Swap_charNum(str);
    P1 = r_strstr(str, "=");
    APP_DEBUG("%s\r\n", P1);
    if (num == 0 || P1 == NULL) {
      char setNote[32] = {0};
      r_memset(setNote, 0, 32);
      r_strcpy(setNote, "Para error!! (SET_*=***)\r\n");
      buf.lenght = r_strlen(setNote);
      buf.payload = (u8_t *)setNote;
      callback_output(&buf);
    } else {
//    SysPara_auth();     // mike 20200824
      offset = P1 - str;
      APP_DEBUG("%d \r\n", offset);
      if (num == 21 && (r_strstr(str, "TEST") != NULL)) {
        buf.lenght = r_strlen(testServer);
        buf.payload = (u8_t *)testServer;
      } else {
        str = &str[offset] + 1;
        P1 = r_strstr(str, "\r");
        if (P1) {
          *P1 = '\0';
          offset = r_strlen(str);
          buf.lenght = offset;
        } else {
          buf.lenght = r_strlen(str);
        }
        buf.payload = (u8_t *)str;
      }
      offset = (int)parametr_set(num, &buf);
      if (offset == 0) {
        char setOk[32] = {0};
        r_memset(setOk, 0, 32);
#ifdef _PLATFORM_BC25_
        Ql_sprintf(setOk, "%d,OK", num);
#endif
#ifdef _PLATFORM_L610_
        snprintf(setOk, sizeof(setOk), "%ld,OK", num);
#endif
        r_strcat(setOk, "\r\n");
        buf.lenght = r_strlen(setOk);
        buf.payload = (u8_t *)setOk;
      } else {
        char setFail[32] = {0};
        r_memset(setFail, 0, 32);
#ifdef _PLATFORM_BC25_
        Ql_sprintf(setFail, "%d,NG,%d", num, offset);
#endif
#ifdef _PLATFORM_L610_
        snprintf(setFail, sizeof(setFail), "%ld,NG,%d", num, offset);
#endif
        r_strcat(setFail, "\r\n");
        buf.lenght = r_strlen(setFail);
        buf.payload = (u8_t *)setFail;
      }
      callback_output(&buf);
    }
  } else if ((r_strncmp(str, "GET-", 4) == 0)
             || (r_strncmp(str, "GET_", 4) == 0)
             || (r_strncmp(str, "get_", 4) == 0)) {
    if (r_strstr(str, "ALL") != NULL) {   // mike 20200828
      GET_ALL_data();
    } else if (r_strstr(str, "LOG") != NULL || r_strstr(str, "log") != 0) {
      logGetFlag = 0x5AA5;
      sw_loghead_rp = 0;
    } else {
      num = Swap_charNum(str + 4);
      APP_DEBUG("get num:%ld\r\n", num);
      if (num == 0 || (r_strstr(str, "=?") == NULL)) {
        char getNote[32] = {0};
        r_memset(getNote, 0, 32);
        r_strcpy(getNote, "Para error!! (GET_*=?)\r\n");
        buf.lenght = r_strlen(getNote);
        buf.payload = (u8_t *)getNote;
        callback_output(&buf);
      } else {
        buf.lenght = 0;
        buf.payload = NULL;
        parametr_get(num, &buf);
        if (buf.lenght > 0 && buf.payload != null) {
          u8_t *pd = buf.payload;       // mike 格式打印输出num=xxxxxxxx
          buf.payload = (u8_t *)memory_apply(buf.lenght + 16);
          Swap_numChar((char *)buf.payload, num);
          r_strcat((char *)buf.payload, "=");
          r_strcat((char *)buf.payload, (char *)pd);
          r_strcat((char *)buf.payload, "\r\n");
          buf.lenght = r_strlen((char *)buf.payload);
          memory_release(pd);
          callback_output(&buf);
          memory_release(buf.payload);
          buf.lenght = 0;
          buf.size = 0;
        } else {
          char getFail[32] = {0};
          r_memset(getFail, 0, 32);
#ifdef _PLATFORM_BC25_
          Ql_sprintf(getFail, "%d=NG", num);
#endif
#ifdef _PLATFORM_L610_
          snprintf(getFail, sizeof(getFail), "%ld=NG", num);
#endif
          r_strcat(getFail, "\r\n");
          buf.lenght = r_strlen(getFail);
          buf.payload = (u8_t *)getFail;
          callback_output(&buf);
        }
      }
    }
  } else if (ESP_check(strBuf) == 0) {
    outputFun = callback_output;
    ESP_cmd(strBuf, outputCh);
    cmdHead.buf = memory_apply(1056);
    cmdHead.buf->size = 1040;
    cmdHead.buf->lenght = 0;
    cmdHead.buf->payload = (u8_t *)(cmdHead.buf + 1);
    strBuf->payload = null;
    strBuf->size = 0;
    cmdHead.waitTime = 2000;
    cmdHead.callback = UanrtCh;
    DeviceIO_lock(&cmdHead);
  } else {
    callback_output(strBuf);
    Net_send(0xFF, strBuf->payload, r_strlen((char *)strBuf->payload));
  }
}

/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
static void outputCh(Buffer_t *buf) {
  if (outputFun != null && buf != null && buf->payload != null) {
    deviceLockTime = 0;
    outputFun(buf);
    memory_release(buf->payload);
    buf->lenght = 0;
    buf->size = 0;
  }
}

/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
static void UanrtCh(DeviceAck_e e) {
  if (e == DEVICE_ACK_FINISH) {
    ESP_cmd(cmdHead.buf, outputCh);
    cmdHead.buf->lenght = 0;
  }
  deviceLockTime += 10;
}

/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
static int logGet(Buffer_t *buf) {
  int ret = 0;
  Buffer_t getBuf;

  buf->size = 256;
  buf->payload = memory_apply(buf->size);
  buf->lenght = 0;

  getBuf.size = buf->size - buf->lenght;
  getBuf.payload = &buf->payload[buf->lenght];
  ret = log_get(&getBuf);
  if (ret > 0) {
    buf->lenght += ret - 1;
  }

//  APP_DEBUG("logGet: %d\r\n", buf->lenght);
  return 0;
}

/*********************************FILE END*************************************/

