#include "gpio_operate.h"
#include "watch_dog.h"

void gpio_init(void)
{
	fibo_gpio_mode_set(NET,0);  	
	fibo_gpio_cfg(NET,output);   		
	fibo_gpio_set(NET,high);		

	fibo_gpio_mode_set(SRV,0);  		
	fibo_gpio_cfg(SRV,output);   	
	fibo_gpio_set(SRV,high);	

	fibo_gpio_mode_set(COM,1);  	
	fibo_gpio_cfg(COM,output);   		
	fibo_gpio_set(COM,high);	

	fibo_gpio_mode_set(WDI,0);  		
	fibo_gpio_cfg(WDI,output);   	
	fibo_gpio_set(WDI,high);	
	feed_the_dog();				//上电先喂一次狗

	fibo_gpio_mode_set(BUZZER,0);  		
	fibo_gpio_cfg(BUZZER,output);   			
	BUZZER_on(1);				//开机上电响一下

	fibo_gpio_mode_set(RECOVERY,1);  		
	fibo_gpio_cfg(RECOVERY,input);   		

								//调试串口
	fibo_gpio_mode_set(M_TXD,6);  		
	fibo_gpio_cfg(M_TXD,output);   	
	fibo_gpio_set(M_TXD,high);			

	fibo_gpio_mode_set(M_RXD,6);  		
	fibo_gpio_cfg(M_RXD,input);   		
	fibo_gpio_set(M_RXD,high);			
}

void gpio_test(void)
{
	for (int j = 0; j < 2; j++)
    {
		fibo_gpio_set(NET,high);
		fibo_taskSleep(500);
		fibo_gpio_set(NET,low);	
		fibo_taskSleep(500);  

		fibo_gpio_set(SRV,high);
		fibo_taskSleep(500); 
		fibo_gpio_set(SRV,low);
		fibo_taskSleep(500); 
		
		fibo_gpio_set(COM,high);	
		fibo_taskSleep(500); 
		fibo_gpio_set(COM,low);	
		fibo_taskSleep(500); 
	}
}

void net_lamp_on()//网络灯开
{
	fibo_gpio_set(NET,low);				
}

void net_lamp_off()//网络灯关
{
	fibo_gpio_set(NET,high);	
}

void srv_lamp_on()//服务器灯 开
{
	fibo_gpio_set(SRV,low);	
}

void srv_lamp_off()//服务器灯 关
{
	fibo_gpio_set(SRV,high);		
}

void com_lamp_on()//数据传输灯 开
{
	fibo_gpio_set(COM,low);			
}

void com_lamp_off()//数据传输灯 关
{
	fibo_gpio_set(COM,high);		
}

void BUZZER_on(int i)//蜂鸣器响
{
	int j = 0;
	for(j = 0;j<i;j++)
	{
		fibo_gpio_set(BUZZER,high);		
		fibo_taskSleep(500);
		fibo_gpio_set(BUZZER,low);	
		fibo_taskSleep(500);
	}
}

void BUZZER_off()
{
	fibo_gpio_set(BUZZER,low);	
}