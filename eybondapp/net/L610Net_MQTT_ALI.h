/**********************    COPYRIGHT 2014-2100,  Eybond ************************
 * @File    : L610Net_MQTT_ALI.h
 * @Author  : MWY
 * @Date    : 2020-08-04
 * @Brief   :
 ******************************************************************************/
#ifndef __L610_NET_MQTT_ALI_H_
#define __L610_NET_MQTT_ALI_H_

#include "4G_net.h"

/*****************************************************************
* define OceanConnect state for timer state_step
******************************************************************/
typedef enum {
  STATE_NW_QUERY_STATE = 0,
  STATE_OC_NCFG,
  STATE_OC_GET_NCFG,
  STATE_OC_NNMI,
  STATE_OC_NCDPOPEN,
  STATE_OC_NMSTATUS,
  STATE_OC_GET_NMSTATUS,
  STATE_OC_NMGS,
  STATE_OC_NMGR,
  STATE_OC_NCDPCLOSE,
  STATE_TOTAL_NUM
} OC_State_e;

typedef struct 
{
    u8_t    flag;       // link user flag
    u8_t    mode;       // link connect mode
    u16_t   port;		// net port
    u32_t   ip;         // link inter net IP -> ___.___.___.___
    s32_t   socketID;	// socket id 
    OC_State_e status;  // link status
    char  ipStr[52]; 	// user connet IP/DNS, lake
    NetDataCallback callback;  //rcveData callback fun
}L610Net_MQTT_ALI_t;

extern OC_State_e m_OCActState;

#endif  // __L610_NET_MQTT_ALI_H_
/******************************************************************************/
