/**********************    COPYRIGHT 2014-2015, EYBOND    ************************
  *@brief   : TransDevice.h  MOSOEC Modbus
  *@notes   : 2017.12.28 CGQ
**********************************************************************************/
#ifndef __TRANS_DEVICE_H
#define __TRANS_DEVICE_H

#include "eyblib_typedef.h"

#ifdef _PLATFORM_BC25_
#include "ql_uart.h"
#endif

#ifdef _PLATFORM_L610_
#include "L610_UARTConfig.h"
#endif

typedef struct {
  char narration[32];
  ST_UARTDCB *cfg;
} TransCfgTab_t;

void TransDevice_init(char *str, ST_UARTDCB *cfg);
void TransDevice_clear(void);

#endif //__TRANS_DEVICE_H end
/******************************************************************************/

