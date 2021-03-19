/**********************    COPYRIGHT 2014-2100, Eybond ************************ 
 * @File    : eybpub_Status.h
 * @Author  : MWY
 * @Date    : 2020-08-11
 * @Brief   : 
 ******************************************************************************/
#ifndef __EYBPUB_STATUS_H_
#define __EYBPUB_STATUS_H_

void Beep_Init(void);
void Beep_On(char cnt);
void Beep_Run(void);

void NetLED_Init(void);
void NetLED_On(void);
void NetLED_Off(void);

void GSMLED_Init(void);
void GSMLED_On(void);
void GSMLED_Off(void);

void deviceLEDInit(void);
void deviceLEDOn(void);
void deviceLEDOff(void);

#endif //__EYBPUB_STATUS_H_
/******************************************************************************/
