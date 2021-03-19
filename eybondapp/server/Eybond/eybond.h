/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
  *@brief   : Eybond
  *@notes   : 2017.12.09 CGQ
*******************************************************************************/
#ifndef __EYBOND_H
#define __EYBOND_H

#ifdef _PLATFORM_BC25_
#include "ql_timer.h"
#define EYBOND_TASK   subtask3_id
#endif

#ifdef _PLATFORM_L610_
#define EYBOND_DEFAULT_SERVER        "www.shinemonitor.com"
#define EYBOND_SOLAR_SERVER          "solar.eybond.com"
#define EYBOND_TEST_SERVER           "tool.eybond.com"
#define EYBOND_DEFAULT_SERVER_PORT   502
#endif
#include "eyblib_typedef.h"

#ifdef _PLATFORM_BC25_
// #define EYBOND_DATA_PROCESS  (0x010001)
#define EYBOND_DATA_PROCESS  MSG_ID_USER_START+0xFB0
#define EYBOND_CMD_REPORT    MSG_ID_USER_START+0xFB1
#endif

#ifdef _PLATFORM_L610_
#define MSG_ID_USER_EYB_START    0x4000
#define EYBOND_DATA_PROCESS  MSG_ID_USER_EYB_START+0xFB0
#define EYBOND_CMD_REPORT    MSG_ID_USER_EYB_START+0xFB1
#endif

#define ESP_PROCESS_FREP        (1000)       //ms
#define EPS_ACK_OVERTIME        (15000)
#define ESP_WAIT_CNT            (EPS_ACK_OVERTIME/ESP_PROCESS_FREP)

extern  u8_t device_data_geting;

typedef enum {
  EYBOND_HEARTBEAT = 0x01,
  EYBOND_GET_PARA  = 0x02,
  EYBOND_SET_PATA  = 0x03,
  EYBOND_TRANS     = 0x04,
  EYBOND_GET_DEVICE_PARA = 0x11,
  EYBOND_SET_DEVICE_PARA = 0x12,
  EYBOND_GET_DEVICE_HISTORY = 0x13,
  EYBOND_REPORT_SPECIAL       = 0x16,
  EYBOND_GET_COMMUNICATION = 0x1F,
  EYBOND_FILE_UPDATE        = 0x20,
  EYBOND_SOFT_UPDATE        = 0x21,
  EYBOND_DEVICE_UPDATE      = 0x22,
  EYBOND_DEVICE_UPDATE_STATE = 0x23,
  EYBOND_DEVICE_UPDATE_CANCEL = 0x24,
  EYBOND_UPDATE_INFO          = 0x25,
  EYBOND_UPDATE_DATA_SEND     = 0x26,
  EYBOND_UPDATE_DATA_STATE    = 0x27,
  EYBOND_UPDATE_DATA_CHECK    = 0x28,
  EYBOND_UPDATE_EXIT          = 0x29,
} EybondCode_e;

typedef struct {
  u16_t serial;   //Serial number
  u16_t code;     //device code
  u16_t msgLen;    //meseger lenght
  u8_t  addr;      //device addresss
  u8_t  func;      //funcation code
} EybondHeader_t;

typedef void (*AckCh)(Buffer_t *buf);

typedef struct {
  u16_t PDULen;    //user Data Lenght;
  u16_t waitCnt;  //command wait process time
  AckCh ack;      //commad ack  road function
  EybondHeader_t head;    //protocol head
  u8_t PDU[];             //user Data
} ESP_t;

typedef u8_t (*funCodeExe)(ESP_t *esp);

typedef struct {
  u16_t code;
  funCodeExe fun;
} funcationTab_t;

void proc_eybond_task(s32_t taskId);
u8_t ESP_cmd(Buffer_t *buf, AckCh ch);
void ESP_callback(u8_t nIndex, Buffer_t *buf);
void TEST_esp_callback(u8_t nIndex, Buffer_t *buf);
u8_t ESP_check(Buffer_t *buf);
void HistoryDataClean(void);
void specialData_send(void);
void tcp_relink(void);
#endif //__EYBOND_H
/******************************************************************************/

