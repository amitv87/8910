/**************Copyright(C) 2015-2026 eybond TEMP *************************
  *@brief   : SysAttr.h 
  *@notes   : 2017.12.21 CGQ
*******************************************************************************/
#ifndef __SYS_ATTR_H
#define __SYS_ATTR_H	 

#include "eyblib_typedef.h"

int Attr_onlineCnt(Buffer_t *buf);
int Attr_timeGet(Buffer_t *buf);
int Attr_option(Buffer_t *buf);
int Attr_bandrateGet(Buffer_t *buf);
int Attr_bandrateSet(Buffer_t *buf);
int Attr_onlineType(Buffer_t *buf);
int Attr_logGet(Buffer_t *buf);
int Attr_logSet(Buffer_t *buf);
int Attr_CSQ(Buffer_t *buf);
int Attr_CPUID(Buffer_t *buf);

char stateFlag;

#endif //__SYS_ATTR_H

/******************************************************************************/

