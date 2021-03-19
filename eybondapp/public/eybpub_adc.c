/**********************    COPYRIGHT 2014-2100,  Eybond ************************
 * @File    : adc.c
 * @Author  : MWY
 * @Date    : 2020-08-05
 * @Brief   :
 ******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_system.h"
#include "ql_adc.h"
#include "ql_timer.h"
#include "ql_error.h"
#define ADC_CHECKTIME_ID  (TIMER_ID_USER_START + 3)
#endif

#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#include "eybapp_appTask.h"
#define ADC_CHECKTIME_ID  (0)
#endif
#include "eybpub_run_log.h"
#include "eybpub_Debug.h"
// #include "eybond.h"  // mike 20201116
// #include "eyblib_memory.h"    //mike 20200828

#define   ADC_COUNT    10

u8_t g_ret_flag = 1;
static u32_t ADC_timer = ADC_CHECKTIME_ID;
static u32_t ADC_time_Interval = 5000;
static s32_t m_param = 0;

#ifdef _PLATFORM_BC25_
Enum_ADCPin adcPin = PIN_ADC0;
// timer callback function
void ADC_Timer_handler(u32 timerId, void *param) {
  static int count_1 = 0;
  static int count_2 = 0;
  u16_t adcvalue = 0;

//    *((s32_t*)param) += 1;
  if (ADC_timer == timerId) {
    // stack_timer repeat
    Ql_ADC_Read(adcPin, &adcvalue);
    APP_DEBUG("read adc value = %d mV)\r\n", adcvalue);
    if (adcvalue < 100) {   // mike 忽略极低电压的情况，适配没有断电挽救电路的设计
      return;
    }
    if ((adcvalue < 700) && (1 == g_ret_flag) && (0 == (++count_1) % 3)) {
      APP_DEBUG("power_interrupted\r\n");
      specialData_send();
      count_1 = 0;
    }
    if ((adcvalue > 700) && (0 == (++count_2) % 3)) {
      g_ret_flag = 1;
      count_2 = 0;
    }
  }
}

void ADC_Init(void) {
  APP_DEBUG("ADC init...\r\n");
  s32_t ret = QL_RET_OK;
  //open  adc
  ret = Ql_ADC_Open(adcPin);
  if (ret < QL_RET_OK) {
    APP_DEBUG("Open adc failed, ret = %d\r\n", ret);
  } else {
    APP_DEBUG("Open adc successfully\r\n");
  }

  //register  a timer
  ret = Ql_Timer_Register(ADC_timer, ADC_Timer_handler, &m_param);
  if (ret < QL_RET_OK) {
    APP_DEBUG("Register adc timer failed!!, Ql_Timer_Register: timer(%d) fail ,ret = %d\r\n", ADC_timer, ret);
  } else {
    APP_DEBUG("Register adc timer successfully: timerId = %d, param = %d,ret = %d\r\n", ADC_timer, m_param, ret);
  }
  //start a timer,repeat=true;
  ret = Ql_Timer_Start(ADC_timer, ADC_time_Interval, TRUE);
  if (ret < 0) {
    APP_DEBUG("Start adc timer failed, ret = %d -->\r\n", ret);
  } else {
    APP_DEBUG("Start adc timer successfully, timerId = %d, timer interval = %d, ret = %d\r\n", ADC_timer, ADC_time_Interval,
              ret);
  }
}
#endif

#ifdef _PLATFORM_L610_
void ADC_Timer_handler(void *param) {
  static int count_1 = 0;
  static int count_2 = 0;
  u16_t adcvalue = 0;

  // stack_timer repeat
  fibo_hal_adc_get_data_polling(2, (UINT32 *)&adcvalue); // ADC0: channel 2, ADC1: channel 3, ADC2: channel 1
//  APP_DEBUG("read adc value = %d mV)\r\n", adcvalue);
  if (adcvalue < 500) {   // mike 忽略极低电压的情况，适配没有断电挽救电路的设计
    return;
  }
  if ((adcvalue < 700) && (1 == g_ret_flag) && (0 == (++count_1) % 3)) {
    APP_DEBUG("power_interrupted\r\n");
//    specialData_send();   // mike 20201116
    count_1 = 0;
  }
  if ((adcvalue > 700) && (0 == (++count_2) % 3)) {
    g_ret_flag = 1;
    count_2 = 0;
  }
}

void ADC_Init(void) {
  APP_DEBUG("ADC init...\r\n");
  s32_t ret = fibo_hal_adc_init();
  if (ret < 0) {
    APP_DEBUG("Open adc failed, ret = %ld\r\n", ret);
  } else {
    APP_DEBUG("Open adc successfully\r\n");
  }

  ADC_timer = fibo_timer_period_new(ADC_time_Interval, ADC_Timer_handler, &m_param);
  if (ADC_timer == 0) {
    APP_DEBUG("Register adc timer failed!!\r\n");
  } else {
    APP_DEBUG("Register adc timer successfully, param = %ld\r\n", m_param);
  }
}
#endif
