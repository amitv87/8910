/**************Copyright(C)£¬2015-2026£¬QIYI Team *****************************
  *@brief   : ParaSave.h
  *@notes   : 2017.07.15  Unarty establish define Item powerDown save Para
*******************************************************************************/
#ifndef __PARA_SAVE_H
#define __PARA_SAVE_H	 

#include "Typedef.h"


typedef struct
{
    u16_t offset;
    u8_t  size;
    void *val;
}Para_t;


int Para_init(void);
int Para_read(u16_t offset, Buffer_t *buf);
int Para_write(u16_t offset, Buffer_t *buf);

#endif //__PARA_SAVE_H
/******************************************************************************/

