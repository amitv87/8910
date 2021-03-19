/**********************    COPYRIGHT 2014-2100,  Eybond ************************
 * @File    : L610Net_TCP_EYB.h
 * @Author  : MWY
 * @Date    : 2020-08-04
 * @Brief   :
 ******************************************************************************/
#ifndef __L610_NET_TCP_EYB_H_
#define __L610_NET_TCP_EYB_H_

#include "4G_net.h"

#define MAX_GPRS_USER_NAME_LEN 32
#define MAX_GPRS_PASSWORD_LEN  32
#define MAX_GPRS_APN_LEN       100

#define SINGLE_SIM              0
#define SECOND_SIM              1


typedef struct {
  u8_t apnName[MAX_GPRS_APN_LEN];
  u8_t apnUserId[MAX_GPRS_USER_NAME_LEN]; 
  u8_t apnPasswd[MAX_GPRS_PASSWORD_LEN]; 
  u8_t authtype; // pap or chap
  void* Reserved1;  // Qos
  void* Reserved2;  //
} ST_GprsConfig;

typedef enum {
  L610_FAIL = 0,
  L610_SUCCESS,
  L610_WAIT,
  L610_DNS_FAIL,
  L610_IP_OK,
  L610_SOCKET_FAIL,
  L610_CONNECT_FAIL,
  L610_CONNECTING,
  L610_CLOSE,
}L610NetState_e;

typedef enum {
  STATE_TOTAL_NUM = 0,
  STATE_GSM_QUERY_STATE,
  STATE_SIM_INSERTED,
  STATE_SIM_NOT_INSERTED,
  STATE_SIM_READY,
  STATE_SIM_NOT_READY,
  STATE_GSM_READY,
  STATE_GSM_NOT_READY,
  STATE_DNS_READY,
  STATE_DNS_NOT_READY,
}GSMState_e;

typedef struct {
    u8_t    flag;     // link user flag
    u8_t    mode;     // link connect mode
    u16_t   port;		// net port
    u32_t   ip;    // link inter net IP -> ___.___.___.___
    s32_t    socketID;	// socket id 
    L610NetState_e status; // link status
    char  ipStr[52]; 		// user connet IP/DNS, lake
    NetDataCallback callback;  //rcveData callback fun
}L610Net_t;

//#define EYB_SOCKET_COUNTS 6
#define EYB_SOCKET_COUNTS 2
extern L610Net_t netManage[];

extern GSMState_e m_GprsActState;
extern u32_t g_SemFlag;

void L610Net_init(void);
void L610Net_init2(void);
u8_t L610Net_open(u8_t mode, char *ip, u16_t port, NetDataCallback netCallback);
void L610Net_close(u8_t nIndex);
int  L610Net_send(u8_t nIndex, u8_t* data, u16_t len);
void L610Net_closeAll(void);
void L610Net_manage(void);
u8_t L610Net_status(u8_t nIndex);
void L610Net_ready(void);
L610Net_t *L610Socket(s8_t socketid, u8_t *offset);
int netInTest(Buffer_t *buf);
//int netInTest(Buffer_t *buf, void_fun_bufp output);

//u8_t get_eybnet_index(void);
void grid_Net_manage(void);
u8_t state_status(u8_t nIndex);
void state_close(u8_t nIndex);

#endif  // __L610_NET_TCP_EYB_H_
/******************************************************************************/
