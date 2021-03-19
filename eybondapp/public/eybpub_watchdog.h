/**********************    COPYRIGHT 2014-2100,  Eybond ************************ 
 * @File    : eybpub_watchdog.h
 * @Author  : CGQ
 * @Date    : 2017-12-05
 * @Brief   : 
 ******************************************************************************/
#ifndef __EYBPUB_WATCHDOG_H_
#define __EYBPUB_WATCHDOG_H_

extern u8_t FeedFlag;

void Watchdog_init(void);
void Watchdog_feed(void);
void Watchdog_stop(void);

#endif //__EYBPUB_WATCHDOG_H_
/******************************************************************************/
