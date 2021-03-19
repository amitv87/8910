/********************** COPYRIGHT 2014-2100, eybond ***************************
 * @File    : eybpub_Status.c
 * @Author  : MWY
 * @Date    : 2020-08-11
 * @Brief   :
 ******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_type.h"
#include "ql_system.h"
#include "ql_stdlib.h"
#include "ql_memory.h"
#endif

#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#endif

#include "eybpub_utility.h"
#include "eybpub_Status.h"
#include "eybpub_Debug.h"
#include "eybpub_SysPara_File.h"

#include "eybapp_appTask.h"

static char beepCnt = 0;

#ifdef _PLATFORM_BC25_
#ifndef BEEP_PORT  //mike 20200805
#define BEEP_PORT (PINNAME_DCD)
#endif

#ifndef NET_LED  // mike 20200814
#define NET_LED (PINNAME_NETLIGHT)
#endif

#ifndef GSM_LED  // mike 20200814
#define GSM_LED (PINNAME_UART2_RTS)
#endif

#ifndef DEVICE_LED  //mike 20200814
#define DEVICE_LED (PINNAME_RI)
#endif

/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
void Beep_Init(void) {
  s32_t ret = 0;
  beepCnt = 0;
//  ret = Ql_GPIO_Init(BEEP_PORT, PINDIRECTION_OUT, PINLEVEL_HIGH, PINPULLSEL_PULLDOWN);
  ret = Ql_GPIO_Init(BEEP_PORT, PINDIRECTION_OUT, PINLEVEL_LOW, PINPULLSEL_PULLUP);
  APP_DEBUG("BEEP PORT GPIO Init ret:%d direction: %d\r\n", ret, Ql_GPIO_GetDirection(BEEP_PORT));

  Beep_On(1);
}

/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
void Beep_On(char cnt) {
  if (cnt < 10 && cnt > 0) {
    beepCnt = cnt;
    Ql_OS_SendMessage(EYBAPP_TASK, APP_CMD_BEEP_ID, 0, 0);
  }
}

/*******************************************************************************
 Brief    :void
 Parameter:
 return   :
*******************************************************************************/
void Beep_Run(void) {
  Buffer_t buf;     //mike 20200805
  Ql_memset(&buf, 0, sizeof(Buffer_t));

/*  parametr_get(BUZER_EN_ADDR, &buf);      // 蜂鸣器设置开关？
  if (buf.payload != null && buf.lenght > 0) {
    int num = 0;
    num = Swap_charNum((char *)buf.payload);
    if (buf.payload != NULL) {
      Ql_MEM_Free(buf.payload);
      buf.lenght = 0;
      buf.size = 0;
    }

    if (num == 1) {
      return;
    }
  } */

  s32_t ret = 0;

  ret = Ql_GPIO_SetLevel(BEEP_PORT, PINLEVEL_HIGH);

//  Ql_GPIO_SetLevel(DEVICE_LED, PINLEVEL_LOW);

  Ql_Delay_ms(500);  //mike 20200805

  Ql_GPIO_SetLevel(BEEP_PORT, PINLEVEL_LOW);

//  Ql_GPIO_SetLevel(DEVICE_LED, PINLEVEL_HIGH);
  
  if (--beepCnt > 0) {
    Ql_Delay_ms(500);  //mike 20200805
    Ql_OS_SendMessage(EYBAPP_TASK, APP_CMD_BEEP_ID, 0, 0);
  }
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void NetLED_Init(void) {
  s32_t ret = 0;
  ret = Ql_GPIO_Init(NET_LED, PINDIRECTION_OUT, PINLEVEL_HIGH, PINPULLSEL_PULLDOWN);
  APP_DEBUG("NET LED Init ret:%d direction: %d\r\n", ret, Ql_GPIO_GetDirection(NET_LED));
  NetLED_On();
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void NetLED_On(void) {
  s32_t ret = 0;
  ret = Ql_GPIO_SetLevel(NET_LED, PINLEVEL_LOW);
//  APP_DEBUG("NET LED set low ret:%d new level: %d\r\n", ret, Ql_GPIO_GetLevel(NET_LED));
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void NetLED_Off(void) {
  s32_t ret = 0;
  ret = Ql_GPIO_SetLevel(NET_LED, PINLEVEL_HIGH);
//  APP_DEBUG("NET LED set high ret:%d new level: %d\r\n", ret, Ql_GPIO_GetLevel(NET_LED));
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void GSMLED_Init(void) {
  s32_t ret = 0;
  ret = Ql_GPIO_Init(GSM_LED, PINDIRECTION_OUT, PINLEVEL_HIGH, PINPULLSEL_PULLDOWN);
  APP_DEBUG("GSM LED Init ret:%d direction: %d\r\n", ret, Ql_GPIO_GetDirection(GSM_LED));
  GSMLED_On();
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void GSMLED_On(void) {
  s32_t ret = 0;
  ret = Ql_GPIO_SetLevel(GSM_LED, PINLEVEL_LOW);
//  APP_DEBUG("GSM LED set low ret:%d new level: %d\r\n", ret, Ql_GPIO_GetLevel(GSM_LED));
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void GSMLED_Off(void) {
  s32_t ret = 0;
  ret = Ql_GPIO_SetLevel(GSM_LED, PINLEVEL_HIGH);
//  APP_DEBUG("GSM LED set high ret:%d new level: %d\r\n", ret, Ql_GPIO_GetLevel(GSM_LED));
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void deviceLEDInit(void) {
  s32_t ret = 0;
  ret = Ql_GPIO_Init(DEVICE_LED, PINDIRECTION_OUT, PINLEVEL_LOW, PINPULLSEL_PULLUP);
  APP_DEBUG("DEVICE LED Init ret:%ld direction: %ld\r\n", ret, Ql_GPIO_GetDirection(DEVICE_LED));
  deviceLEDOn();
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void deviceLEDOn(void) {
  s32_t ret = 0;
  ret = Ql_GPIO_SetLevel(DEVICE_LED, PINLEVEL_LOW);
//  APP_DEBUG("DEVICE LED set low,ret:%ld new level: %ld\r\n", ret, Ql_GPIO_GetLevel(DEVICE_LED));
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void deviceLEDOff(void) {
  s32_t ret = 0;
  ret = Ql_GPIO_SetLevel(DEVICE_LED, PINLEVEL_HIGH);
//  APP_DEBUG("DEVICE LED set high,ret:%ld new level: %ld\r\n", ret, Ql_GPIO_GetLevel(DEVICE_LED));
}
#endif

#ifdef _PLATFORM_L610_
#ifndef BEEP_PORT
#define BEEP_PORT (0)
#endif

#ifndef NET_LED
#define NET_LED (0)
#endif

#ifndef GSM_LED
#define GSM_LED (0)
#endif

#ifndef DEVICE_LED
#define DEVICE_LED (0)
#endif

/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
void Beep_Init(void) {
  s32_t ret = 0;
  beepCnt = 0;
  fibo_gpio_mode_set(BEEP_PORT, 0);
  fibo_gpio_cfg(BEEP_PORT,PINDIRECTION_OUT);
  fibo_gpio_set(BEEP_PORT,PINLEVEL_HIGH);
  UINT8 nBeep_Level = 0;
  ret = fibo_gpio_get(BEEP_PORT, &nBeep_Level);
//  APP_DEBUG("BEEP PORT GPIO Init ret:%ld direction: %d\r\n", ret, nBeep_Level);
  Beep_On(1);
}

/*******************************************************************************
 Brief    : void
 Parameter:
 return   :
*******************************************************************************/
void Beep_On(char cnt) {
  if (cnt < 10 && cnt > 0) {
    beepCnt = cnt;
    Eybpub_UT_SendMessage(EYBAPP_TASK, APP_CMD_BEEP_ID, 0, 0);
  }
}

/*******************************************************************************
 Brief    :void
 Parameter:
 return   :
*******************************************************************************/
void Beep_Run(void) {
  s32_t ret = 0;
  ret = fibo_gpio_set(BEEP_PORT, PINLEVEL_HIGH);
  fibo_taskSleep(500);
  fibo_gpio_set(BEEP_PORT, PINLEVEL_LOW);  
  if (--beepCnt > 0) {
    fibo_taskSleep(500);
    Eybpub_UT_SendMessage(EYBAPP_TASK, APP_CMD_BEEP_ID, 0, 0);
  }
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void NetLED_Init(void) {
  s32_t ret = 0;
  fibo_gpio_mode_set(NET_LED, 0);
  fibo_gpio_cfg(NET_LED,PINDIRECTION_OUT);
  fibo_gpio_set(NET_LED,PINLEVEL_HIGH);
  NetLED_On();
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void NetLED_On(void) {
  s32_t ret = 0;
  fibo_gpio_set(NET_LED,PINLEVEL_LOW);
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void NetLED_Off(void) {
  s32_t ret = 0;
  fibo_gpio_set(NET_LED,PINLEVEL_HIGH);
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void GSMLED_Init(void) {
  s32_t ret = 0;
  fibo_gpio_mode_set(GSM_LED, 0);
  fibo_gpio_cfg(GSM_LED,PINDIRECTION_OUT);
  fibo_gpio_set(GSM_LED,PINLEVEL_HIGH);
  GSMLED_On();
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void GSMLED_On(void) {
  s32_t ret = 0;
  fibo_gpio_set(GSM_LED,PINLEVEL_LOW);
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void GSMLED_Off(void) {
  s32_t ret = 0;
  fibo_gpio_set(GSM_LED,PINLEVEL_HIGH);
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void deviceLEDInit(void) {
  s32_t ret = 0;
  fibo_gpio_mode_set(DEVICE_LED, 1);
  fibo_gpio_cfg(DEVICE_LED,PINDIRECTION_OUT);
  fibo_gpio_set(DEVICE_LED,PINLEVEL_HIGH);
  deviceLEDOn();
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void deviceLEDOn(void) {
  s32_t ret = 0;
  fibo_gpio_set(DEVICE_LED,PINLEVEL_LOW);
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void deviceLEDOff(void) {
  s32_t ret = 0;
  fibo_gpio_set(DEVICE_LED,PINLEVEL_HIGH);
}

#endif
/******************************************************************************/
