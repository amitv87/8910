#ifndef _COMMON_DATA_H_
#define _COMMON_DATA_H_

#include "fibo_opencpu_comm.h"

#define DEF_EVENT_DEVICE_START_UPDATE       (1<<2)
#define DEF_EVENT_RUN_XMODEM                (1<<3)
#define DEF_EVENT_DEVICE_UPDATING           (1<<4)

void COMMON_DataInit(void);
void COMMON_SetEvent(UINT32 CommonEvent);
void COMMON_ResetEvent(UINT32 CommonEvent);

#endif // __COMMON_DATA_H__