/******************************************************************************           
* name:             CommonServer.h           
* introduce:        头文件
* author:           Luee                                     
******************************************************************************/ 
#ifndef __COMMON_SERVER_H
#define __COMMON_SERVER_H

#ifdef _PLATFORM_M26_
#include "typedef.h"
#include "ql_type.h"
#include "SysPara.h"
#include "list.h"

#define COMMON_SERVER_TASK        subtask5_id
#define COMMON_SERVER__DATA_PROCESS        (0x010000)
#endif

#ifdef _PLATFORM_L610_
#include "eyblib_typedef.h"
#include "eyblib_list.h"
#include "eybpub_SysPara_File.h"
#include "grid_tool.h"
#define MSG_ID_USER_COMMONSERVER_START     0xA000
#define COMMON_SERVER__DATA_PROCESS        MSG_ID_USER_COMMONSERVER_START + 0xF10
#define COMMON_SERVER__EYBNET_READY        MSG_ID_USER_COMMONSERVER_START + 0xF11
#endif

#define SERVER_PROCESS_FREP                (1000)       //ms

typedef void (*DataAck)(Buffer_t *buf);
typedef ServerAddr_t *(*serverAddrGet)(void);
typedef void (*DataProcess)(ListHandler_t *cmdList);
typedef u8_t (*DataAdd)(Buffer_t *buf, DataAck ack);

typedef struct {
  char name[32] ;         // server name
  int waitTime;
  void_fun_void init;     // server Init
  void_fun_void deviceInit; // device Init;
  void_fun_u8 run;        // server run
  void_fun_void process;  // server data process
  DataAdd       add;      // server rcvedata Add
  serverAddrGet getAddr;  // Get server connect addr
  void_fun_void sclose;    // server close
  void_fun_void clean;    // server config or log clean
} CommonServer_t;

//结构体
//server net status
typedef struct{
  u8 inwork_status;   //=1 in work success
	u8 send_status;     //=1 sending =0 send finish
  u16 send_counter;   //send required time
  u8 rec_status;      //=1 receiving =0 receive finish
  u16 rec_counter;    //receiving required time
}net_para_t;

extern net_para_t eybnet_para;
extern net_para_t statenet_para;

void proc_commonServer_task(s32_t taskId);
void CommonServerDataSend(Buffer_t *buf);
void CommonServer_DeviceInit(void);
void CommonServer_close(void);
void CommonServerData_clean(void);

void ssl_relink(void);
void soft_reset_en(void);
void clear_overtime(void);

void state_rec_process(Buffer_t *buf);
void sslrec_task(void *param);

#endif //__COMMON_SERVER_H
/******************************************************************************/

