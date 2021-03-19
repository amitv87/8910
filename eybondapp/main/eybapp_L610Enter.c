/******************************************************************************           
* name:                       
* introduce:        app enter
* author:           Luee                                     
******************************************************************************/ 
#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('Q', 'E', 'K', 'J')

#include "eyblib_r_stdlib.h"

#include "eybpub_Debug.h"
#include "eybpub_Status.h"
#include "eybpub_utility.h"
#include "eybpub_watchdog.h"
#include "eybpub_run_log.h"
#include "eybpub_Key.h"
#include "eybpub_adc.h"

#include "4G_net.h"
#include "eybapp_appTask.h"
#include "Device.h"
#include "DeviceIO.h"
#include "eybond.h"
#include "ESP_Update_L610.h"
#include "UpdateTask.h"
#include "L610_conn_ali_net.h"
#include "http_fota.h"
#include "CommonServer.h"
#include "L610Net_SSL.h"

UINT32 EYBAPP_TASK = 0;
UINT32 EYBNET_TASK = 0;
UINT32 EYBDEVICE_TASK = 0;
UINT32 EYBOND_TASK = 0;
UINT32 ALIYUN_TASK = 0;
UINT32 FOTA_TASK = 0;
UINT32 UPDATE_TASK = 0;
UINT32 COMMON_SERVER_TASK = 0;

static void prvInvokeGlobalCtors(void) {
  extern void (*__init_array_start[])();
  extern void (*__init_array_end[])();

  size_t count = __init_array_end - __init_array_start;
  for (size_t i = 0; i < count; ++i) {
    __init_array_start[i]();
  }
}

static void sig_res_callback(GAPP_SIGNAL_ID_T sig, va_list arg) {
  APP_DEBUG("sig_res_callback sig:%d\r\n", sig);
  switch (sig) {
    // fibo_PDPActive  ip address resopnse event
    case GAPP_SIG_PDP_ACTIVE_ADDRESS: {
      UINT8 cid = (UINT8)va_arg(arg, int);
      char *addr = (char *)va_arg(arg, char *);
      if(addr != NULL) {
        OSI_PRINTFI("GAPP_SIG_PDP_ACTIVE_ADDRESS  cid = %d, addr = %s ", cid, addr);
        APP_DEBUG("GAPP_SIG_PDP_ACTIVE_ADDRESS cid = %d, addr = %s\r\n", cid, addr);
      } else  {
        OSI_PRINTFI("[%s-%d]", __FUNCTION__, __LINE__);
        APP_DEBUG("GAPP_SIG_PDP_ACTIVE_ADDRESS addr is NULL\r\n");
      }
      va_end(arg);
	  break;
    }
    // fibo_getHostByName event
	case GAPP_SIG_DNS_QUERY_IP_ADDRESS: {   
      char *host = (char *)va_arg(arg, char *);
      char *ipv4_addr = (char *)va_arg(arg, char *);
      char *ipv6_addr = (char *)va_arg(arg, char *);
	  if(host != NULL) {
        APP_DEBUG("GAPP_SIG_DNS_QUERY_IP_ADDRESS, host = %s\r\n", host);
      }
      if(ipv4_addr != NULL) {
        APP_DEBUG("GAPP_SIG_DNS_QUERY_IP_ADDRESS, ipv4_addr = %s\r\n", ipv4_addr);
      } else {
        APP_DEBUG("GAPP_SIG_DNS_QUERY_IP_ADDRESS, ipv4_addr is NULL\r\n");
      }
      if(ipv6_addr != NULL) {
        APP_DEBUG("GAPP_SIG_DNS_QUERY_IP_ADDRESS, ipv6_addr = %s", ipv6_addr);
      } else {
	    APP_DEBUG("GAPP_SIG_DNS_QUERY_IP_ADDRESS, ipv6_addr is NULL\r\n");
      }
      va_end(arg);
      break;
	}
    // fibo_PDPRelease /fibo_asyn_PDPRelease pdp deactive status report
	case GAPP_SIG_PDP_RELEASE_IND: {
	  UINT8 cid = (UINT8)va_arg(arg, int);
	  APP_DEBUG("GAPP_SIG_PDP_RELEASE_IND  cid = %d\r\n", cid);
	  va_end(arg);
      break;
    }
    // fibo_PDPActive /fibo_asyn_PDPActive pdp active status report
	case GAPP_SIG_PDP_ACTIVE_IND: {
      UINT8 cid = (UINT8)va_arg(arg, int);
      APP_DEBUG("GAPP_SIG_PDP_ACTIVE_IND  cid = %d\r\n", cid);
	  va_end(arg);
      break;
    }    
    // GAPP_SIG_PDP_ACTIVE_OR_DEACTIVE_FAIL_IND
	case GAPP_SIG_PDP_ACTIVE_OR_DEACTIVE_FAIL_IND: {
      UINT8 cid = (UINT8)va_arg(arg, int);
      UINT8 state = (UINT8)va_arg(arg, int);
      APP_DEBUG("GAPP_SIG_PDP_ACTIVE_OR_DEACTIVE_FAIL_IND cid = %d,state = %d\r\n", cid, state);
      va_end(arg);
      break;
	}
	// PDP in active state, deactive indicator received from modem  
    // PDP 异常断开
    case GAPP_SIG_PDP_DEACTIVE_ABNORMALLY_IND: {
      UINT8 cid = (UINT8)va_arg(arg, int);
      APP_DEBUG("GAPP_SIG_PDP_DEACTIVE_ABNORMALLY_IND cid = %d\r\n", cid);
      va_end(arg);
      Eybpub_UT_SendMessage(EYBNET_TASK, NET_MSG_GSM_FAIL, 0, 0);   //Luee
//    fibo_taskSleep(1000);
//	  UINT8 ip[50];	
//	  int ret;
//	  ret = fibo_PDPActive(1, NULL, NULL, NULL, 0, 0, ip);
//	  APP_DEBUG("ret = %d,ip=%s\r\n", ret, ip);
      break;
	}
    case GAPP_SIG_MPING_RECV: {
      char *value = (char *)va_arg(arg, char *);
      APP_DEBUG("GAPP_SIG_MPING_RECV %s\r\n", value);
      va_end(arg);
      break;
    }
    case GAPP_SIG_MPINGSTAT_RECV: {
      char *value = (char *)va_arg(arg, char *);
      APP_DEBUG("GAPP_SIG_MPINGSTAT_RECV %s\r\n", value);
      va_end(arg);
      break;
    }
    case GAPP_SIG_REPORT_URC_TEXT: {
      char *value = (char *)va_arg(arg, char *);
      if(value != NULL) {
        APP_DEBUG("GAPP_SIG_REPORT_URC_TEXT, value = %s\r\n", value);
      }
	  va_end(arg);
//      if (r_strncmp(value, "CGREG: 1,0", 10) == 0) {
//        Eybpub_UT_SendMessage(EYBNET_TASK, NET_MSG_GSM_READY, 0, 0);
//      } else if (r_strncmp(value, "CGREG: 1,7", 10) == 0) {
//        Eybpub_UT_SendMessage(EYBNET_TASK, NET_MSG_GSM_READY, 0, 0);
//      } else if (r_strncmp(value, "CGREG: 0", 8) == 0) {
//        Eybpub_UT_SendMessage(EYBNET_TASK, NET_MSG_GSM_FAIL, 0, 0);
//      }
      break;
    }
    default:
      break;
  }
}

static void at_res_callback(UINT8 *buf, UINT16 len) {
  OSI_PRINTFI("FIBO <--%s", buf);
  fibo_hal_uart_put(DEBUG_PORT, buf, len);
}

static FIBO_CALLBACK_T user_callback = {
  .fibo_signal = sig_res_callback,
  .at_resp = at_res_callback
};

void * appimg_enter(void *param) {
  OSI_LOGI(0, "application image enter");
  prvInvokeGlobalCtors();

  Debug_init(); // 上电配置DEBUG串口
  fibo_taskSleep(1000);

  u8_t bootcase = fibo_getbootcause();
  switch (bootcase) {
    case 0:
      APP_PRINT("Booting from Soft reboot!!\r\n");
      break;
    case 1:
      APP_PRINT("Booting from RST reboot!!\r\n");
      break;
    case 2:
      APP_PRINT("Booting from Power on!!\r\n");
      break;
    case 3:
      APP_PRINT("Booting from USB plug!!\r\n");
      break;
    default:
      APP_PRINT("Booting from unkonw mode!!\r\n");
      break;
  }

  APP_PRINT("Eybond 4G L610 Application!!\r\n");
  APP_PRINT("Version: %s\r\n", FWVERSION);
  APP_PRINT("Time: %s\r\n", MAKE_TIME);
  fibo_set_app_version(FWVERSION);

  INT8 *hardware_version = NULL;
  INT8 *software_version = NULL;

  fibo_at_send((UINT8 *)"AT+MSTART=0,0\r\n", r_strlen("AT+MSTART=0,0\r\n"));  // 关闭Start message notification

  hardware_version   = fibo_get_hw_verno();  // 获取当前的硬件版本(客户定制)
  software_version   = fibo_get_sw_verno();  // 获取当前的软件版本(客户定制)
  APP_PRINT("hardware_version %s\r\n",hardware_version);
  APP_PRINT("SDK version %s\r\n",software_version);
  INT32 enret = fibo_watchdog_enable(60);  // 60秒=1分钟 无任何语句执行则重启      
  if(0 == enret) {
    APP_DEBUG("ninside watchdog enable success\r\n"); 
  }
  if(enret < 0) {
    APP_DEBUG("ninside watchdog enable fail\r\n"); 
  }
  
  Watchdog_init();
  NetLED_Init();
  GSMLED_Init();
  deviceLEDInit();
  Beep_Init();
  Key_init();
  ADC_Init();  

  s32_t ret = 0;
  u32_t nFreeSize = fibo_file_getFreeSize();
  if (nFreeSize < 0x80000) {
    APP_DEBUG("File system is full, Clean it\r\n");
    if (fibo_file_getSize(FOTA_FILE_NAME) >= 0 || fibo_file_exist(FOTA_FILE_NAME) == 1) {
        ret = fibo_file_delete(FOTA_FILE_NAME);
        if (ret < 0) {
          APP_DEBUG("Delete %s file fail\r\n", FOTA_FILE_NAME);
        }
    }
    if (fibo_file_getSize(FOTA_STATUSFILE_NAME) >= 0 || fibo_file_exist(FOTA_STATUSFILE_NAME) == 1) {
        ret = fibo_file_delete(FOTA_STATUSFILE_NAME);
        if (ret < 0) {
          APP_DEBUG("Delete %s file fail\r\n", FOTA_STATUSFILE_NAME);
        }
    }
  }
  
  UINT32 net_thread_id = 0;
  UINT32 app_thread_id = 0;
  UINT32 dev_thread_id = 0;
  UINT32 eyb_thread_id = 0;
  UINT32 ali_thread_id = 0;
  UINT32 fota_thread_id = 0;
  UINT32 upd_thread_id = 0;
  UINT32 com_thread_id = 0;

  EYBAPP_TASK = fibo_queue_create(10, sizeof(ST_MSG));
  EYBNET_TASK = fibo_queue_create(10, sizeof(ST_MSG));
  EYBDEVICE_TASK = fibo_queue_create(10, sizeof(ST_MSG));
  EYBOND_TASK = fibo_queue_create(10, sizeof(ST_MSG));
  ALIYUN_TASK = fibo_queue_create(10, sizeof(ST_MSG));
  FOTA_TASK = fibo_queue_create(10, sizeof(ST_MSG));
  UPDATE_TASK = fibo_queue_create(10, sizeof(ST_MSG));
  COMMON_SERVER_TASK = fibo_queue_create(10, sizeof(ST_MSG));

  fibo_thread_create_ex(proc_app_task,          "Eybond APP TASK",     1024*8*2, NULL, OSI_PRIORITY_REALTIME, &app_thread_id);
  fibo_taskSleep(100);
  fibo_thread_create_ex(proc_net_task,          "Eybond NET TASK",     1024*8*3, NULL, OSI_PRIORITY_ABOVE_NORMAL,   &net_thread_id);
  fibo_taskSleep(100);
  fibo_thread_create(sslrec_task,      "SSL RECV TASK",     1024*8*3, NULL, OSI_PRIORITY_NORMAL);
  fibo_taskSleep(100);
  fibo_thread_create_ex(proc_device_task,       "Eybond DEVICE TASK",  1024*8*4, NULL, OSI_PRIORITY_LOW,      &dev_thread_id);
  fibo_taskSleep(100);
  fibo_thread_create_ex(proc_eybond_task,       "Eybond CMD TASK",     1024*8*6, NULL, OSI_PRIORITY_LOW,      &eyb_thread_id);
  fibo_taskSleep(100);
  fibo_thread_create_ex(proc_update_task,       "Eybond UPDATE TASK",  1024*8*3, NULL, OSI_PRIORITY_NORMAL,   &upd_thread_id);
  fibo_taskSleep(100);
  //fibo_thread_create_ex(mqtt_conn_ali_task,     "MQTT CONN ALI TASK",  1024*8*4, NULL, OSI_PRIORITY_LOW,      &ali_thread_id);
  //fibo_taskSleep(100);
  //fibo_thread_create_ex(proc_http_fota_task,    "Http fota TASK",      1024*8*2, NULL, OSI_PRIORITY_NORMAL,   &fota_thread_id);
  //fibo_taskSleep(100);
  fibo_thread_create_ex(proc_commonServer_task, "Common Server TASK",  1024*8*4, NULL, OSI_PRIORITY_LOW,      &com_thread_id);
  fibo_taskSleep(100);
  APP_PRINT("Net %X APP %X Dev %X Eyb %X MQTT %X UPd %X JLFota %X COM %X\r\n", \
    net_thread_id, app_thread_id, dev_thread_id, eyb_thread_id, ali_thread_id, upd_thread_id, fota_thread_id, com_thread_id);
  Eybpub_UT_SendMessage(EYBAPP_TASK, APP_MSG_UART_READY, 0, 0);

  return (void *)&user_callback;
}

void appimg_exit(void) {
  OSI_LOGI(0, "application image exit");
}

#endif
