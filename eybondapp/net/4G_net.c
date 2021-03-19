/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
  *@brief   : net.c for 4G modem
  *@notes   : 2020.08.04 MWY
******************************************************************************/
#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#endif

#include "eyblib_list.h"
#include "eyblib_typedef.h"
#include "eyblib_swap.h"
#include "eyblib_r_stdlib.h"

#include "eybpub_Debug.h"
#include "eybpub_Status.h"
#include "eybpub_utility.h"
// #include "eybpub_watchdog.h"
// #include "eybpub_run_log.h"
#include "4G_net.h"
#include "L610Net_TCP_EYB.h"
#include "eybapp_appTask.h"
// #include "restart_net.h"

#include "L610Net_SSL.h"
#include "CommonServer.h"

typedef struct {
  u8_t nIndex;
  Buffer_t buf;
} NetSend_t;

// #define NET_OVERTIME  (60*10)
#define NET_OVERTIME  (60)  // 1 min overtime for GSM register
// #define MEMOTY_SIZE  (160 * 1024)  // unit 1K
// static char testIP[64] = {0};
// static u16_t testPort;

static u32_t NetOvertime = 0;
static u32_t m_timeCheck = 0;

static ListHandler_t netSendPakege;
// static int netInTest(Buffer_t *buf, void_fun_bufp output);
// static s32_t m_simStat = SIM_STAT_UNSPECIFIED;
static void Net_sendData(void);

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void proc_net_task(s32_t taskId) {
  int ret = -1;
  ST_MSG msg;
  u8_t tmpbuf[64];

  static int netResetCnt = 0;
  s32_t cereg = 0;
  u32_t uStackSize = 0;

  // variable Initialization
  netResetCnt = 0;
  NetOvertime = 0;
  m_timeCheck = 0;
  r_memset(&msg, 0, sizeof(ST_MSG));

  g_SemFlag = fibo_sem_new(0);
  if (g_SemFlag < 0) {
	APP_DEBUG("sem create failure\r\n");
  } else {
	APP_DEBUG("sem create success\r\n");
  }

  APP_PRINT("Net task run...\r\n");
  list_init(&netSendPakege);    // 初始化网络发送队列
  while (1) {
    fibo_queue_get(EYBNET_TASK, (void *)&msg, 0);
    switch (msg.message) {
      case APP_MSG_UART_READY:
        APP_DEBUG("Net task APP_MSG_UART_READY\r\n");
        L610Net_init();
        Beep_On(1); // 上电后响1声
        //ssl_relink();
        break;
      case NET_MSG_SIM_INSERTED:
        APP_DEBUG("Net task NET_MSG_SIM_INSERTED\r\n");
        Eybpub_UT_SendMessage(EYBAPP_TASK, NET_MSG_SIM_INSERTED, 0, 0);
        break;
      case NET_MSG_SIM_NOT_INSERTED:
        APP_DEBUG("Net task NET_MSG_SIM_NOT_INSERTED\r\n");
        Eybpub_UT_SendMessage(EYBAPP_TASK, NET_MSG_SIM_NOT_INSERTED, 0, 0);
        Beep_On(5); // 未插卡响5声
        break;
      case NET_MSG_SIM_READY:
        APP_DEBUG("Net task NET_MSG_SIM_READY\r\n");
        Eybpub_UT_SendMessage(EYBAPP_TASK, NET_MSG_SIM_READY, 0, 0);
        break;
      case NET_MSG_SIM_FAIL:
        APP_DEBUG("Net task NET_MSG_SIM_FAIL\r\n");
        Eybpub_UT_SendMessage(EYBAPP_TASK, NET_MSG_SIM_FAIL, 0, 0);
        break;
      case NET_MSG_GSM_READY:
        APP_DEBUG("Net task NET_MSG_GSM_READY\r\n");
        GSMLED_On();
        L610Net_ready();
        Eybpub_UT_SendMessage(EYBAPP_TASK, NET_MSG_GSM_READY, 0, 0);

        //SSL_init();  
        ssl_relink(); 
        break;
      case NET_MSG_GSM_FAIL:
        APP_DEBUG("Net task NET_MSG_GSM_FAIL\r\n");
        GSMLED_Off();
        Eybpub_UT_SendMessage(EYBAPP_TASK, NET_MSG_GSM_FAIL, 0, 0);
        break;
      case NET_MSG_DNS_READY:
        Eybpub_UT_SendMessage(EYBAPP_TASK, NET_MSG_DNS_READY, 0, 0);
        APP_DEBUG("Net task NET_MSG_DNS_READY\r\n");
        break;
      case NET_MSG_DNS_FAIL:
        Eybpub_UT_SendMessage(EYBAPP_TASK, NET_MSG_DNS_FAIL, 0, 0);
        APP_DEBUG("Net task NET_MSG_DNS_FAIL\r\n");
        break;
      case APP_MSG_TIMER_ID:  // 从APP task传递过来的定时器(1000 ms)消息
//      APP_DEBUG("Net task get APP_USER_TIMER_ID:%ld\r\n", NetOvertime);
        L610Net_manage();
        grid_Net_manage();
        break;
      case NET_CMD_RESTART_ID:
        break;
      case NET_CMD_SENDDATA_ID:
        Net_sendData();
        break;
      default:
        break;
    }
  }
  fibo_thread_delete();
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
u8_t Net_connect(u8_t mode, char *ip, u16_t port, NetDataCallback netCallback) {
  return L610Net_open(mode, ip, port, netCallback);
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void Net_close(u8_t nIndex) {
  L610Net_close(nIndex);
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
u8_t Net_status(u8_t nIndex) {
  return L610Net_status(nIndex);
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void Net_send(u8_t nIndex, u8_t *pData, u16_t len) {
  NetSend_t *sendData = list_nodeApply(sizeof(NetSend_t) + len);
  if (sendData != null) {
    sendData->nIndex = nIndex;
    sendData->buf.lenght = len;
    sendData->buf.payload = (u8_t *)(sendData + 1);
    r_memcpy(sendData->buf.payload, pData, len);
    list_bottomInsert(&netSendPakege, sendData);    
    Eybpub_UT_SendMessage(EYBNET_TASK, NET_CMD_SENDDATA_ID, 0, 0);
  } else {
    APP_DEBUG("memory apply full!!!");
  }
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void Net_sendData(void) {
  NetSend_t *send = (NetSend_t *)list_nextData(&netSendPakege, null);
  if (send == null) {
    APP_DEBUG("No netSendPakege any more.\r\n");
    return;
  } else if (send->buf.lenght == 0) {
    list_nodeDelete(&netSendPakege, send);
  } else {
    int ret = 0;
    ret = L610Net_send(send->nIndex, send->buf.payload, send->buf.lenght);
    if (ret == send->buf.lenght) {
	  list_nodeDelete(&netSendPakege, send);
      APP_DEBUG("nIndex %d Send %d data successfully.\r\n", send->nIndex, ret);
      NetOvertime = 0;
    } else if (ret > 0 && ret < send->buf.lenght) {
	  send->buf.lenght -= ret;
	  send->buf.payload = send->buf.payload + ret;
	} else if (ret == -520) {
      APP_DEBUG("port close state.\r\n");
	  list_nodeDelete(&netSendPakege, send);
	  L610Net_close(send->nIndex);      
	} else {
	  L610Net_close(send->nIndex);
	  APP_DEBUG("Fail to connect to server, cause=%d\r\n", ret);	
	}	
  }
  Eybpub_UT_SendMessage(EYBNET_TASK, NET_CMD_SENDDATA_ID, 2, 0);    // 确保netSendPackage里面没有数据了
}

/******************************************************************************/

