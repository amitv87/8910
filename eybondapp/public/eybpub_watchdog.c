/**********************    COPYRIGHT 2014-2100,  Eybond ************************ 
 * @File    : eybpub_watchdog.c
 * @Author  : CGQ
 * @Date    : 2017-12-05
 * @Brief   : 
 ******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_system.h"
#include "ql_wtd.h"			//mike include ql_gpio.h
#include "ql_error.h"
#endif

#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#endif

#include "eybpub_utility.h"
#include "eybpub_Debug.h"
#include "eybpub_watchdog.h"
#include "eybpub_Clock.h"
#include "eybapp_appTask.h"
#include "eybpub_run_log.h"

u8_t FeedFlag;

#ifdef _PLATFORM_BC25_
#ifndef WATCHDOG_PIN	//mike 20200805
#define WATCHDOG_PIN (PINNAME_GPIO5)
#endif

/*******************************************************************************
 Brief    : Watchdog_init
 Parameter: 
 return   : 
*******************************************************************************/
void Watchdog_init(void)
{    
	FeedFlag = 0;
  s32_t ret = 0;
//  ret = Ql_GPIO_Init(DEVICE_LED, PINDIRECTION_OUT, PINLEVEL_HIGH, PINPULLSEL_PULLDOWN);
  ret = Ql_GPIO_Init(WATCHDOG_PIN, PINDIRECTION_OUT, PINLEVEL_HIGH, PINPULLSEL_PULLDOWN);
}

/*******************************************************************************
 Brief    : Watchdog_feed
 Parameter: 
 return   : 
*******************************************************************************/
void Watchdog_feed(void)
{
	if (FeedFlag == 0)
	{
//	    APP_DEBUG("feed!!\r\n");
      s32_t ret = 0;
//      ret = Ql_GPIO_SetLevel(DEVICE_LED, PINLEVEL_LOW);
      ret = Ql_GPIO_SetLevel(WATCHDOG_PIN, PINLEVEL_LOW);
      Ql_Delay_ms(500);
//      ret = Ql_GPIO_SetLevel(DEVICE_LED, PINLEVEL_HIGH);
      ret = Ql_GPIO_SetLevel(WATCHDOG_PIN, PINLEVEL_HIGH);
	}
}

/*******************************************************************************
 Brief    : Watchdog_stop
 Parameter: 
 return   : 
*******************************************************************************/
void Watchdog_stop(void)
{
	int i;

	for (i = 0; i < 6; i++)
	{
		Clock_Add();
	}
	FeedFlag = 1;
    s32_t ret = 0;
//    ret = Ql_GPIO_SetLevel(DEVICE_LED, PINLEVEL_HIGH);
    ret = Ql_GPIO_SetLevel(WATCHDOG_PIN, PINLEVEL_HIGH);
	Clock_save();
    Ql_OS_SendMessage(EYBAPP_TASK, APP_CMD_WDG_ID, 0, 0);
	APP_DEBUG("feed stop!!\r\n");
}
#endif

#ifdef _PLATFORM_L610_
/*******************************************************************************
 Brief    : Watchdog_init
 Parameter: 
 return   : 
*******************************************************************************/
void Watchdog_init(void) {    
  FeedFlag = 0;
  s32_t ret = 0;
  ret = fibo_gpio_mode_set(WATCHDOG_PIN, 0);
  ret = fibo_gpio_cfg(WATCHDOG_PIN,PINDIRECTION_OUT);
  ret = fibo_gpio_set(WATCHDOG_PIN,PINLEVEL_HIGH);
}

/*******************************************************************************
 Brief    : Watchdog_feed
 Parameter: 
 return   : 
*******************************************************************************/
void Watchdog_feed(void) {
  if (FeedFlag == 0) {
    s32_t ret = 0;
    ret = fibo_gpio_set(WATCHDOG_PIN,PINLEVEL_LOW);
    fibo_taskSleep(500);  //500ms翻转一次
	ret = fibo_gpio_set(WATCHDOG_PIN,PINLEVEL_HIGH);
  }
}

/*******************************************************************************
 Brief    : Watchdog_stop
 Parameter: 
 return   : 
*******************************************************************************/
void Watchdog_stop(void) {
  int i;
  for (i = 0; i < 6; i++) {
    Clock_Add();
  }
  FeedFlag = 1;
  s32_t ret = 0;
  ret = fibo_gpio_set(WATCHDOG_PIN,PINLEVEL_HIGH);
  Clock_save();
  APP_DEBUG("feed stop!!\r\n");
  log_save("wdt feed stop!!!\r\n");
}
#endif

/*********************************FILE END*************************************/

