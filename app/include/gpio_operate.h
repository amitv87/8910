#ifndef __GPIO_OPERATE_H__
#define __GPIO_OPERATE_H__

#include "fibo_opencpu.h"

#define input       0
#define output      1

#define low         0
#define high        1

#define SRV         24
#define WDI         25
#define NET         26
#define BUZZER      27

#define M_TXD       28
#define M_RXD       29

#define COM         30
#define RECOVERY    31

#define ADC0        45

//硬件引脚 45 ADC0  对应通道2
//硬件引脚 44 ADC1  对应通道3
//硬件引脚 43 ADC2  对应通道1

// #define SPI_CS      37
// #define SPI_MOSI    38
// #define SPI_MISO    39
// #define SPI_CLK     40

void gpio_init(void);
void gpio_test(void);

void net_lamp_on();     //网络灯开
void net_lamp_off();    //网络灯关

void srv_lamp_on();     //服务灯开
void srv_lamp_off();    //服务灯关

void com_lamp_on();     //设备灯开
void com_lamp_off();    //设备灯关

void BUZZER_on(int i);  //蜂鸣器响 i为次数
void BUZZER_off();      //蜂鸣器灭

#endif // __GPIO_OPERATE_H__