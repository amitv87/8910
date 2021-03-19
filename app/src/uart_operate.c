#include "uart_operate.h"
#include "fibo_opencpu_comm.h"
#include "oc_uart.h"
#include "elog.h"
#include "debug_cmd.h"
#include "data_collector_parameter_table.h"
#include "fibo_opencpu.h"
#include "device_access_information.h"
#include "net_task.h"

extern INT32           g_connectret;

extern int 	g_stop_dog_flag;
extern int 	number_of_array_elements;

UINT16 	uart1_recv_len					    =	 0;			//返回实际接收的字节数
char 	uart1_recv_data[UART_BUFFER_SIZE]   =   {0}; 

UINT16	uart3_recv_len	                    =    0;			//返回实际接收的字节数
char	uart3_recv_data[UART_BUFFER_SIZE]	=	{0}; 

//串口1默认配置
hal_uart_config_t   uart1_cfg  = {
    .baud                 = 9600,                           //波特率9600
    .data_bits            = HAL_UART_DATA_BITS_8,           //8位数据位
    .stop_bits            = HAL_UART_STOP_BITS_1,           //1位停止位
    .parity               = HAL_UART_NO_PARITY,             //无校验
    .rx_buf_size          = UART_RX_BUF_SIZE,               //接收缓冲区大小
    .tx_buf_size          = UART_TX_BUF_SIZE,               //发送缓冲区大小
};

//串口3默认配置
hal_uart_config_t   uart3_cfg  = {
    .baud                 = 115200,                         //波特率115200
    .data_bits            = HAL_UART_DATA_BITS_8,           //8位数据位
    .stop_bits            = HAL_UART_STOP_BITS_1,           //1位停止位
    .parity               = HAL_UART_NO_PARITY,             //无校验
    .rx_buf_size          = UART_RX_BUF_SIZE,               //接收缓冲区大小
    .tx_buf_size          = UART_TX_BUF_SIZE,               //发送缓冲区大小
};

//串口1更新配置
hal_uart_config_t   uart1_newcfg;

void serial_communication_information(void)
{
    UINT8 *p            = NULL;
    UINT8 cnt           = 0;       //计数
    UINT8 linkPlace     = 0;       //连接符
    UINT8 newlinePlace  = 0;       //换行符

    char *buf  = NULL;
    UINT16 len = 64;    

    char *baud_rate_char  = NULL;
    char *data_bits_char  = NULL;
    char *stop_bits_char  = NULL;
    char *check_bits_char = NULL; 

    //串口通讯信息获取
    for (int j = 0; j < number_of_array_elements; j++)
    {
        if(34 == PDT[j].num)
        {
            buf = fibo_malloc(sizeof(char)*64);
            memset(buf, 0, sizeof(char)*64);
            PDT[j].rFunc(&PDT[j],buf, &len);
            p   = (UINT8 *)buf;
        }
    }

    while(0 != *p)
    {
        if('-'== *p)
        {
            linkPlace = cnt;   
        }
        if(('\r' == *p) || ('\n' == *p))
        {
            newlinePlace = cnt;
        }
        p++;
        cnt++;
    }

    log_d("\r\nnewlinePlace is %d\r\n",newlinePlace);

    if(0 == linkPlace)                                                      //只有波特率
    {
        baud_rate_char  = fibo_malloc(sizeof(char)*(len));  
        memcpy(baud_rate_char,(UINT8 *)buf,len);             
        uart1_newcfg.baud = atoi(baud_rate_char);                          //波特率                          
        log_d("\r\nbaud_rate is %d\r\n",uart1_newcfg.baud); 

		uart1_newcfg.data_bits      = 8;                                   //数据位
        uart1_newcfg.stop_bits      = 1;                                   //停止位
		uart1_newcfg.parity         = 0;                                   //校验位
        fibo_free(baud_rate_char);
    }
    else
    {
        baud_rate_char  = fibo_malloc(sizeof(char)*(linkPlace-4));          //波特率字符
        data_bits_char  = fibo_malloc(sizeof(char)*(1));                    //数据位字符
        stop_bits_char  = fibo_malloc(sizeof(char)*(1));                    //停止位字符
        check_bits_char = fibo_malloc(sizeof(char)*(1));                    //校验位字符
                                 
        memcpy(baud_rate_char,  (UINT8 *)buf,               linkPlace-4);   //波特率字符
        memcpy(data_bits_char,  (UINT8 *)buf+linkPlace-3,   1);             //数据位字符
        memcpy(stop_bits_char,  (UINT8 *)buf+linkPlace-1,   1);             //停止位字符
        memcpy(check_bits_char, (UINT8 *)buf+linkPlace+1,   1);             //校验位字符

        uart1_newcfg.baud        = atoi(baud_rate_char);                   //波特率
        uart1_newcfg.data_bits   = atoi(data_bits_char);                   //数据位
        uart1_newcfg.stop_bits   = atoi(stop_bits_char);                   //停止位
        uart1_newcfg.parity      = atoi(check_bits_char);                  //校验位

        log_d("\r\nbaud_rate  is %d\r\n",uart1_newcfg.baud);               //波特率
        log_d("\r\ndata_bits  is %d\r\n",uart1_newcfg.data_bits);          //数据位
        log_d("\r\nstop_bits  is %d\r\n",uart1_newcfg.stop_bits);          //停止位
        log_d("\r\ncheck_bits is %d\r\n",uart1_newcfg.parity );   		   //校验位
        fibo_free(baud_rate_char);
        fibo_free(data_bits_char);  
        fibo_free(stop_bits_char);
        fibo_free(check_bits_char);  
    } 
    uart1_newcfg.rx_buf_size          = UART_RX_BUF_SIZE,               //接收缓冲区大小
    uart1_newcfg.tx_buf_size          = UART_TX_BUF_SIZE,               //发送缓冲区大小
    fibo_free(buf);
}

void uart1_cfg_update()
{
	serial_communication_information();
	uart_deinit(UART1);
	uart_init(UART1,&uart1_newcfg, uart1_recv_cb, NULL);
}

//串口1接收回调
void uart1_recv_cb(hal_uart_port_t uart_port, UINT8 *data, UINT16 len, void *arg)
{
    log_d("uartapi recv uart_port=%d len=%d, data=%s", uart_port, len, (char *)data);
	uart1_recv_len = len;
    memcpy(uart1_recv_data,data,uart1_recv_len); 

    log_hex((UINT8 *)uart1_recv_data,uart1_recv_len);
	int cmp = 1;
	char *buf="AT+TEST";
	cmp = memcmp(uart1_recv_data,buf,7);

    static int production_test = 0;//进入生产测试的次数

	if(0 == cmp)
	{
        if(production_test < 3)
        {
            production_test++;
            char uart1_recv_data_copy[UART_BUFFER_SIZE]={0};
            memcpy(uart1_recv_data_copy,uart1_recv_data,uart1_recv_len); 
            STRCMD_AtTest(uart1_recv_data_copy);
            memset(uart1_recv_data, 0, sizeof(uart1_recv_data)); 
            uart1_recv_len = 0;
        }
        else
        {
            if(0 != g_connectret)//连接测试服不成功
            {
                tcp_connection();    	//发起TCP连接
            }
            memset(uart1_recv_data, 0, sizeof(uart1_recv_data)); 
            uart1_recv_len = 0;  
        }
	}

	dev_access_info();  //硕日专用
}  

//串口3接收回调
void uart3_recv_cb(hal_uart_port_t uart_port, UINT8 *data, UINT16 len, void *arg)
{
    log_d("uartapi recv uart_port=%d len=%d, data=%s", uart_port, len, (char *)data);

	uart3_recv_len = len;

	char uart3_recv_data_copy[UART_BUFFER_SIZE]={0};

	if(uart3_recv_len>5)
	{
		memcpy(uart3_recv_data_copy,data,uart3_recv_len); 
		uart_set_get_para(uart3_recv_data_copy);	
	}
}

//串口开
INT32 uart_init(hal_uart_port_t uart_port, hal_uart_config_t *uart_config, uart_input_callback_t recv_cb, void *arg)
{
    INT32   ret = fibo_hal_uart_init(uart_port,uart_config,recv_cb,arg);
    return  ret;
}

//串口写
INT32 uart_write(hal_uart_port_t uart_port, UINT8 *buff, UINT32 len)
{
    INT32   ret = fibo_hal_uart_put(uart_port,buff,len);
    return  ret;
}

//串口关
INT32 uart_deinit(hal_uart_port_t uart_port)
{
    INT32   ret = fibo_hal_uart_deinit(uart_port);
    return  ret;
}