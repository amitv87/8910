/**********************    COPYRIGHT 2014-2100,  Eybond ************************
 * @File    : DeviceUpdate.h
 * @Author  : CGQ
 * @Date    : 2017-12-14
 * @Brief   :
 ******************************************************************************/
#ifndef __DEVICE_UPDATE_
#define __DEVICE_UPDATE_

// #include "ql_type.h"     // mike 20201211
#include "eyblib_typedef.h"
#include "eybpub_File.h"

#ifdef _PLATFORM_M26_
#define DEVICE_UPDATE_READY_ID    (0x0F0000)
#define DEVICE_UPDATE_END_ID      (0x0F1000)
#endif

#ifdef _PLATFORM_L610_
#define MSG_ID_USER_DEVICE_UPDATE_START    0x9000
#define DEVICE_UPDATE_READY_ID         MSG_ID_USER_DEVICE_UPDATE_START+0xF10
#define DEVICE_UPDATE_END_ID           MSG_ID_USER_DEVICE_UPDATE_START+0xF20
#endif

typedef struct {
  u8_t addr;
  u8_t type;
  u16_t bandrate;
} DeviceType_t;

int  Update_startDevice(File_t *file, DeviceType_t *type);
void Update_ready(void);
u8_t   Update_state(void);
void Update_end(void);

#endif //__DEVICE_UPDATE_
/******************************************************************************/
