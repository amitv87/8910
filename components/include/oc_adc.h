#ifndef COMPONENTS_FIBOCOM_OPENCPU_OC_ADC_H
#define COMPONENTS_FIBOCOM_OPENCPU_OC_ADC_H

typedef int hal_adc_channel_t;

/**
* @brief 
* 
* @return INT32 
 */
INT32 fibo_hal_adc_init(void);

/**
* @brief 
* 
* @return INT32 
 */
INT32 fibo_hal_adc_deinit(void);

/**
* @brief 
* 
* @param channel 
* @param data 
* @return INT32 
 */
INT32 fibo_hal_adc_get_data_polling(hal_adc_channel_t channel, UINT32 *data);
INT32 fibo_hal_adc_get_data(hal_adc_channel_t channel,UINT32 scale_value);
#endif /* COMPONENTS_FIBOCOM_OPENCPU_OC_ADC_H */


INT32 fibo_gpio_cfg(SAPP_IO_ID_T id,SAPP_GPIO_CFG_T cfg);
INT32 fibo_gpio_get(SAPP_IO_ID_T id,UINT8 * level);
INT32 fibo_gpio_mode_set(SAPP_IO_ID_T id,UINT8 mode);
INT32 fibo_gpio_pull_disable(SAPP_IO_ID_T id);
INT32 fibo_gpio_set(SAPP_IO_ID_T id,UINT8 level);

INT32 fibo_lpg_switch(UINT8 ucLpgSwitch);
INT32 fibo_setSleepMode(UINT8 time);


