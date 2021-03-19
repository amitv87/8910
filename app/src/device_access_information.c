#include "device_access_information.h"
#include "data_collector_parameter_table.h"
#include "uart_operate.h"
#include "gpio_operate.h"
#include "string.h"
#include "stdio.h"
#include "elog.h"
#include "net_task.h"

extern char 	uart1_recv_data[UART_BUFFER_SIZE];
extern int 		number_of_array_elements;
extern uint16_t uart1_recv_len;

void dev_access_info()
{
	log_d("\r\n%s()\r\n", __func__);
    UINT8 AT_buf[25] = {0};

	//1.1 采集器序列号（PN）14或18位
	if(0 == memcmp(uart1_recv_data,"AT+PN\r\n",5))
	{
		log_d("\r\ndev_access PN\r\n");
		UINT16 len=0;
		for (int j = 0; j < number_of_array_elements; j++)       
		{
			if(2 == PDT[j].num)
			{
				PDT[j].rFunc(&PDT[j],(void*)AT_buf, &len);
				UINT8 PN_buf[21]={0};//包含回车换行及0
				sprintf((char*)PN_buf,"PN:%s\r\n",AT_buf);
				uart_write(UART1, PN_buf, strlen((char*)PN_buf));
				log_d("\r\n%s\r\n",PN_buf);
				break;
			} 
		}
		memset(uart1_recv_data, 0, sizeof(uart1_recv_data)); 
		uart1_recv_len = 0;
	}
	//1.2 通讯模组设备识别码（IMEI）15位
	if(0 == memcmp(uart1_recv_data,"AT+IMEI\r\n",7))
	{
		log_d("\r\ndev_access IMEI\r\n");
		fibo_get_imei((UINT8 *)AT_buf,0);
		UINT8 imei_buf[18]={0};//包含回车换行及0
		sprintf((char*)imei_buf,"IMEI:%s\r\n",AT_buf);
		uart_write(UART1, imei_buf, strlen((char*)imei_buf));
		log_d("\r\n%s\r\n",imei_buf);
		memset(uart1_recv_data, 0, sizeof(uart1_recv_data)); 
		uart1_recv_len = 0;
	}
	//1.3 SIM卡唯一识别码（CCID）20位
	if(0 == memcmp(uart1_recv_data,"AT+CCID\r\n",7))
	{
		log_d("\r\ndev_access CCID\r\n");
		fibo_get_ccid((UINT8*)AT_buf);
		UINT8 iccid_buf[23]={0};//包含回车换行及0
		sprintf((char*)iccid_buf,"CCID:%s\r\n",AT_buf);
		uart_write(UART1, iccid_buf, strlen((char*)iccid_buf));
		log_d("\r\n%s\r\n",iccid_buf);
		memset(uart1_recv_data, 0, sizeof(uart1_recv_data));
		uart1_recv_len = 0; 
	}
	//1.4 无线信号强度 （CSQ）1-2位
	if(0 == memcmp(uart1_recv_data,"AT+CSQ\r\n",6))
	{
		log_d("\r\ndev_access CSQ\r\n");
		INT32 CSQ  = 0;
		INT32   ber  = 0;
		fibo_get_csq(&CSQ,&ber);
		char CSQ_char[3]={0};  
		itoa(CSQ,CSQ_char,10);
		UINT8 CSQ_buf[9]={0};//包含回车换行及0
		sprintf((char*)CSQ_buf,"CSQ:%s\r\n",CSQ_char);
		uart_write(UART1, CSQ_buf, strlen((char*)CSQ_buf));
		log_d("\r\n%s\r\n",CSQ_buf);
		memset(uart1_recv_data, 0, sizeof(uart1_recv_data)); 
		uart1_recv_len = 0;
	}
	//1.5 采集器与服务的连接状态 （LINK）1位
	if(0 == memcmp(uart1_recv_data,"AT+LINK\r\n",7))
	{
		log_d("\r\ndev_access LINK\r\n");
		UINT8 link = 0;
		//1为连接服务器，0为断连服务器
		fibo_gpio_get(SRV,&link);
		link = !link;
		char link_char[2]={0};  
		itoa(link,link_char,10);
		UINT8 link_buf[9]={0};//包含回车换行及0
		sprintf((char*)link_buf,"LINK:%s\r\n",link_char);
		uart_write(UART1,link_buf, strlen((char*)link_buf));
		log_d("\r\n%s\r\n",link_buf);
		memset(uart1_recv_data, 0, sizeof(uart1_recv_data)); 
		uart1_recv_len = 0;
	}

	//1.6 获取APN
	if(0 == memcmp(uart1_recv_data,"AT+APN\r\n",6))
	{
		log_d("\r\ndev_access APN\r\n");
		UINT16 len=0;
		for (int j = 0; j < number_of_array_elements; j++)       
		{
			if(57 == PDT[j].num)
			{
				PDT[j].rFunc(&PDT[j],(void*)AT_buf, &len);
				UINT8 APN_buf[64]={0};						//包含回车换行及0
				sprintf((char *)APN_buf,"APN:%s\r\n",AT_buf);
				uart_write(UART1,APN_buf, strlen((char *)APN_buf));
				log_d("\r\n%s\r\n",APN_buf);
				break;
			} 
		}
		memset(uart1_recv_data, 0, sizeof(uart1_recv_data)); 
		uart1_recv_len = 0;
	}

	//1.6 设置APN
	if(0 == memcmp(uart1_recv_data,"AT-APN=",7))
	{
		log_d("\r\ndev_set APN\r\n");
		UINT16 len = 0;

		int branch_flag = 1;

		for(int i=0; i<uart1_recv_len; i++)
		{
			if(';' == uart1_recv_data[i])
			{
				branch_flag = 0;
			}
		}

		if(1 == branch_flag)
		{
			char *uart1_recv_APN = NULL;

			uart1_recv_APN = fibo_malloc(sizeof(char)*((uart1_recv_len-7)+1));
			memset(uart1_recv_APN, 0, sizeof(char)*((uart1_recv_len-7)+1));
			
			memcpy(uart1_recv_APN,&uart1_recv_data[7],(uart1_recv_len-7)); 

			for (int j = 0; j < number_of_array_elements; j++)       
			{
				if(57 == PDT[j].num)
				{
					memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
					PDT[j].wFunc(&PDT[j],(void*)uart1_recv_APN, &len);
					log_d("\r\n%d=%s;\r\n",PDT[j].num,uart1_recv_APN); 
					log_d("\r\nlen=%d;\r\n",len); 
				} 
			}
			fibo_free(uart1_recv_APN);
			memset(uart1_recv_data, 0, sizeof(uart1_recv_data)); 
			uart1_recv_len = 0;
			parameter_check();
		}
	}
}   