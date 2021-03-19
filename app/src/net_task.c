#include "fibo_opencpu.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "elog.h"
#include "net_task.h"
#include "data_collector_parameter_table.h"
#include "parameter_number_and_value.h"
#include "uart_operate.h"
#include "run_log.h"
#include "eybond_modbus_tcp_protocol.h"
#include "big_little_endian_swap.h"
#include "gpio_operate.h"
// #include "time_function.h"

// static void     un_modbus_tcp_Transparent_Transm();

// extern UINT32 g_queue_id_net;
INT32           g_connectret      = -1;
extern UINT8 	g_29;           	//29号参数
extern UINT8 	g_34;           	//34号参数
extern UINT8 	g_54;           	//54号参数
extern int 		number_of_array_elements;
extern UINT32 	g_EventFlag;		//事件标识
extern UINT8 	g_RemoteIp[];    	//IP
extern UINT8 	g_RemotePort[];    	//端口号 0-65535
INT8            socketid;
char 			g_RecvData[2*1024] 	= {0};
UINT16 			g_RecvDataLen 		=  0;
char 			g_SendData[2*1024] 	= {0};
UINT16 			g_SendDataLen 		=  0;

extern int 	    g_stop_dog_flag;
static uint32_t g_i = 0;	//重启计数器

//开机后先读卡
static void read_card()
{
    uint8_t simstatus  =  0;
    INT32   simret     = -1;

    //获取sim卡插拔状态 
    //1 == simstatus 代表已插卡
    //0 == simstatus 代表未插卡
    simret =  fibo_get_sim_status(&simstatus);
    // 0 成功
    //<0 失败

    int i = 0;
    while((0 == simret)&&(0 == simstatus)) //获取sim卡插拔状态成功且状态为未插卡
    {
		fibo_taskSleep(1000);       //延时1S执行一次
		i++;
		if(10 == i)                 //10S              
		{
            net_lamp_off();
            BUZZER_on(5);           //读不到卡响5声
            log_d("read_sim_fail");
            break;
		}
        simret =  fibo_get_sim_status(&simstatus);
    }

    if(1 == simstatus)//已插卡
    {
        log_d("read_sim_succed\r\n");   //调试信息
    }

    if(0 == simstatus)//未插卡
    {
        log_d("read_sim_fail");
    }
}

void registered_network()
{
    read_card();

    char    *buf            = NULL;//APN信息
    UINT16  len             = 64;    
    //57号参数：APN配置信息
    for (int j = 0; j < number_of_array_elements; j++)
    {
        if(57 == PDT[j].num)
        {
            buf = fibo_malloc(sizeof(char)*64);
            memset(buf, 0, sizeof(char)*64);
            PDT[j].rFunc(&PDT[j],buf, &len);
            log_d("\r\nAPN CFG INFO is %s\r\n",buf); 
            log_d("\r\nlen is %d\r\n",len);  
        }
    }

    UINT8       ip[50]          = {0};
    reg_info_t  sim_reg_info;
    INT32       regret          =  -1;
    INT32       pdpret          =  -1;
    int         reg_cycle_flag  =   1;
    int         ret_try_count   =   0;
    while(reg_cycle_flag)
    {
	    regret = fibo_getRegInfo(&sim_reg_info, single_sim_card);

        if(0 == regret)
        {
            log_d("\r\nreg_info is valid data,try_count is %d\r\n",ret_try_count);
        }
        
        if(regret < 0)
        {
            log_d("\r\nsim register failure,try_count is %d\r\n",ret_try_count);
        }
    
		fibo_taskSleep(2000);

		if(1 == sim_reg_info.nStatus)
		{
            log_d("\r\nregister network success,try_count is %d\r\n",ret_try_count);
			
			pdpret = fibo_PDPActive(1, (UINT8*)buf, NULL, NULL, 0, single_sim_card, ip);
            if(0 == pdpret)
            {
                log_d("\r\npdp active success,try_count is %d\r\n",ret_try_count);
                reg_cycle_flag = 0;
                net_lamp_on();
            }
            else
            {
                net_lamp_off();
                log_d("\r\npdp active failed,try_count is %d\r\n",ret_try_count);
            }
		}
        else
        {
            net_lamp_off();
            log_d("\r\nregister network processing,try_count is %d\r\n",ret_try_count);
        }

        ret_try_count++;

        if(10 == ret_try_count)//20s未注册并激活网络成功则退出
        {
            reg_cycle_flag = 0;   
        }
    }

    fibo_free(buf);
}

unsigned short htons_special(unsigned short n)
{
    return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

int launch_tcp_connection()
{
    char 		*buf  			= NULL;							//参数值buff
    UINT16		len 			= 64;   
    UINT16 		u16Port			= 502;   
    ip_addr_t   addr_para;
    INT32       dns_ret         = -1;

    if(1 == g_EventFlag)
	{
		u16Port = atoi((char*)g_RemotePort); 
        dns_ret = fibo_getHostByName((char*)g_RemoteIp,&addr_para,1,single_sim_card);//0成功 小于0失败 

        if(0 == dns_ret)
        {
            log_d("\r\nproduction test dns getHostByName success\r\n");
        }

        if(0 < dns_ret)
        {
            log_d("\r\nproduction test dns getHostByName fail\r\n");
        }

		log_d("\r\nproduction test u16Port:%d\r\n",u16Port);
		log_d("\r\nproduction test g_RemoteIp:%s\r\n",g_RemoteIp); 
	}
    else
    {
        //服务器端口号
        for (int j = 0; j < number_of_array_elements; j++)
        {
            if(24 == PDT[j].num)
            {
                buf = fibo_malloc(sizeof(char)*64);
                memset(buf, 0, sizeof(char)*64);
                PDT[j].rFunc(&PDT[j],buf, &len);
                u16Port = atoi(buf);
                log_d("\r\nu16Port is %d\r\n",u16Port);
                fibo_free(buf);
            }
        }

        //服务器ip地址
        for (int j = 0; j < number_of_array_elements; j++)
        {
            if(21 == PDT[j].num)
            {
                buf = fibo_malloc(sizeof(char)*64);
                memset(buf, 0, sizeof(char)*64);
                PDT[j].rFunc(&PDT[j],buf, &len);
                dns_ret = fibo_getHostByName(buf,&addr_para,1,single_sim_card);//0成功 小于0失败 

                if(0 == dns_ret)
                {
                    log_d("\r\ndns getHostByName success\r\n");
                }
                
                if(0 < dns_ret)
                {
                    log_d("\r\ndns getHostByName fail\r\n");
                }

                log_d("\r\nbuf is %s\r\n",buf);
                fibo_free(buf);
            }
        }
    }

    //sockets建立
    GAPP_TCPIP_ADDR_T addr;
    memset(&addr, 0, sizeof(GAPP_TCPIP_ADDR_T));
    socketid                    = fibo_sock_create(0);//0-TCP,1-UDP
    addr.sin_port               = htons_special(u16Port);
    addr.sin_addr.u_addr.ip4    = addr_para.u_addr.ip4;
    addr.sin_addr.type          = AF_INET;

    g_connectret = fibo_sock_connect(socketid, &addr);

    if(1 == g_EventFlag)
    {
        uart_write(UART1, (UINT8 *)"+OK\r\n", strlen("+OK\r\n"));	
        log_d("\r\nuart1_write output +OK\r\n");
    }

    if(0 == g_connectret)
    {
        log_d("\r\nfibo_sock_connect success\r\n");
        return 0;
    }
    else
    {
        log_d("\r\nfibo_sock_connect fail\r\n");
        fibo_sock_close(socketid);
        return -1;
    } 
}

void tcp_connection()
{
    log_d("\r\ntcp connecting\r\n");

    int tcp_cycle_flag = 1;
    int tcp_try_count  = 1;
    int launch_tcp_ret = 1;

    while(tcp_cycle_flag)
    {
        launch_tcp_ret = launch_tcp_connection();

        if(0 == launch_tcp_ret)
        {
            log_d("\r\nlaunch tcp success,tcp_try_count is %d\r\n",tcp_try_count);
            tcp_cycle_flag = 0;
            srv_lamp_on();
        }

        if(-1 == launch_tcp_ret)
        {
            log_d("\r\nlaunch tcp fail,tcp_try_count is %d\r\n",tcp_try_count);
            srv_lamp_off();
        }

        if(1 == g_EventFlag)
        {
           tcp_cycle_flag = 0; 
        }

        if(0 == g_EventFlag)
        {
            tcp_try_count++;
            fibo_taskSleep(1000);   //不能删
            if(60 == tcp_try_count) //60*1000 1分钟未联网成功则退出
            {
                tcp_cycle_flag = 0;   
            }
        }

    }
}

/*机器大小端测试*/
/*
内存存储的大小端模式：
高字节位存储在高地址   ---称为小端
高字节位存储在低地址   ---称为大端
----无论大小端，都可以使用，但一个地方只能用一种否则会出错.
*/
int big_little_endian_test()
{
	int a;
	a = 0x10000001;			//指针测试
	char b = *((char*)(&a));

	if(b==0x10)
    {
	    log_d("\r\nbig mode\r\n");
    }

	if(b==0x01)
    {
	    log_d("\r\nlittle mode\r\n");
    }

	return 0;
}

//网络任务
void net_task(void *param)
{
    log_d("\r\n%s()\r\n",__func__);
	log_d("\r\nSRNE AT init ok\r\n"); 

    uart1_cfg_update();             //更新串口配置
    uint32_t relink = 0;            //重连计数器

    net_lamp_off();
	registered_network();			//注册网络
    
    srv_lamp_off();
    tcp_connection(); 			    //连接网络

    while(1)
    {
        fibo_taskSleep(1000);//不能删除、给其他任务运行时间
        log_d("\r\ng_RecvDataLen is %d\r\n",g_RecvDataLen); 
        if(g_RecvDataLen > 0)
        {
            relink  = 0;		//重连计数器清零
            g_i     = 0;		//重启计数器清零
            if(TRUE == data_frame_legal_checking((UINT8 *)g_RecvData))	                            /*判断数据帧是否为modbus_tcp*/
            {
                log_hex((UINT8 *)g_RecvData,g_RecvDataLen);
                receiving_processing(g_RecvData[7]);
                memset(g_RecvData, 0, sizeof(g_RecvData));		//接收buff清零
                g_RecvDataLen = 0;								//接收长度清零
                fibo_sock_send(socketid, (UINT8 *)g_SendData, g_SendDataLen);
                log_hex((UINT8 *)g_SendData,g_SendDataLen);
            }
            else
            {
                // un_modbus_tcp_Transparent_Transm();	
                // g_RecvDataLen = 0;	
                memset(g_RecvData, 0, sizeof(g_RecvData));		//接收buff清零
                g_RecvDataLen = 0;								//接收长度清零
            }
        }
        else
        {
            memset(g_RecvData, 0, sizeof(g_RecvData));		//接收buff清零
            g_RecvDataLen = 0;								//接收长度清零
            if(0 == g_EventFlag)
            {
                g_i 	= g_i+1;		    //重启计数器
                relink	= relink+1;		    //重连计数器
                log_d("g_i      is     %d",g_i); 
                log_d("relink   is     %d",relink); 
                if(relink >= 75)   	        //75s左右           
                {
                    relink = 0;		        //重连计数器清零
                    srv_lamp_off();
                    log_d("\r\ntimeout\r\n"); 
                    log_d("\r\nreconnection tcp\r\n"); 
                    tcp_connection();	
                }
                //重连网络10次不成功就重启
                if(g_i >= 750)   		    //750s左右           
                {
                    g_i = 0;		        //重启计数器
                    log_d("softReset"); 
                    fibo_softReset();
                }
            }
        }
    }
}

#if 0
//非modbus tcp透传
static void un_modbus_tcp_Transparent_Transm()
{
    log_d("\r\nun_modbus_tcp_Transparent_Transm\r\n"); 
	memset(g_SendData, 0, sizeof(g_SendData));
	memset(uart1_recv_data, 0, sizeof(uart1_recv_data));              //接收清零
	uart1_recv_len = 0;
	uart_write(UART1, (UINT8 *)g_RecvData, g_RecvDataLen);	//接收到服务器发送的数据及长度
	memset(g_RecvData, 0, sizeof(g_RecvData));
	fibo_taskSleep(500);
	// log_d("\r\nuart1_recv_len is %d\r\n",uart1_recv_len); 
	log_hex((UINT8 *)uart1_recv_data, uart1_recv_len);
	memcpy(g_SendData,uart1_recv_data,uart1_recv_len);
	for(int i = 0; i < uart1_recv_len; i++)
	{
		g_SendData[i] = uart1_recv_data[i];
	}
	log_hex((UINT8 *)g_SendData, uart1_recv_len);
	g_SendDataLen =  uart1_recv_len;
	fibo_sock_send(socketid, (UINT8 *)g_SendData, g_SendDataLen);

	memset(g_SendData, 0, sizeof(g_SendData));
	g_SendDataLen =  0;
	if(uart1_recv_len > 0)
	{
		com_lamp_on();
        log_d("\r\ncom_lamp_on()\r\n"); 
	}
	else
	{
		com_lamp_off();
        log_d("\r\ncom_lamp_off()\r\n"); 
	}
}
#endif

void parameter_check()
{
	if(1 == g_34)
	{
		log_d("\r\ng_34 is %d\r\n",g_34);
		g_34 = 0;
		uart1_cfg_update();		//更新串口配置
	}

	if((0 == g_29)&&(0 == g_54))
	{
        parameter_a_module();
        a_copy_to_b();
		parameter_init();       //保持统一
	}

	if(1 == g_29)				// 系统操作
	{
		log_d("\r\nthe system_operation is %d restart system\r\n",g_29);
		g_29 = 0;
		g_stop_dog_flag = 1;
	}
	
	if(1 == g_54)				// 系统操作
	{
		g_54 = 0;
		log_clean();            //清除运行日志
	}
}

void update_version()
{
    char firmware_version[] = "6.0.0.0";

	UINT16 		len 			= 64;    						//参数值长度

    char CSQ_char[3]={0};
    INT32 CSQ  = 0;
    INT32 ber  = 0;

    char CCID[21]={0};

    char IMEI[16]={0};

	for(int j = 0; j < number_of_array_elements; j++)
	{
		if(5 == PDT[j].num)//固件版本
		{
			memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
			PDT[j].wFunc(&PDT[j],firmware_version, &len);
		}

		// if(51 == PDT[j].num)//固件编译日期
		// {
		// 	memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
		// 	PDT[j].wFunc(&PDT[j],g_compile_date, &len);
		// }

		// if(52 == PDT[j].num)//固件编译时间
		// {
		// 	memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
		// 	PDT[j].wFunc(&PDT[j],g_compile_time, &len);
		// }

		if(55 == PDT[j].num)
		{
            fibo_get_csq(&CSQ,&ber);
			itoa(CSQ,CSQ_char,10);
			memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
			PDT[j].wFunc(&PDT[j],CSQ_char, &len);
		}

		if(56 == PDT[j].num)//通信卡CCID
		{
			fibo_get_ccid((UINT8*)CCID);
			memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
			PDT[j].wFunc(&PDT[j],CCID, &len);
		}

		if(58 == PDT[j].num)//CPUID IMEI
		{
			fibo_get_imei((UINT8 *)IMEI,0);
			memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
			PDT[j].wFunc(&PDT[j],IMEI, &len);
		}
	}

	parameter_check();//更新版本号
}

void netrecv_task(void *param)
{
    while(1)
    {
        fibo_taskSleep(1000);//不能删除、给其他任务运行时间
        g_RecvDataLen = fibo_sock_recv(socketid, (UINT8 *)g_RecvData, sizeof(g_RecvData));
    }
}