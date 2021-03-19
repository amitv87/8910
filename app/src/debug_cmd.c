#include "common_data.h"
#include "elog.h"
#include "net_task.h"
#include <string.h>
#include "data_collector_parameter_table.h"
#include <stdlib.h>
#include "debug_cmd.h"
#include "fibo_opencpu.h"
#include "uart_operate.h"

#define DEF_IP_MAX_SIZE         46
#define DEF_PORT_MAX_SIZE       6

/*----------------------------------------------------------------------------*
**                             Global Vars                                    *
**----------------------------------------------------------------------------*/
extern char g_compile_date[];
extern char g_compile_time[];

extern int number_of_array_elements;
extern uint8_t g_29;           //29号参数 系统操作
extern uint8_t g_34;           //34号参数 串口配置

UINT8 g_RemoteIp[DEF_IP_MAX_SIZE]       = {0};    //IP
UINT8 g_RemotePort[DEF_PORT_MAX_SIZE]   = {0};    //端口号 0-65535

//大写字母转小写
u8_t  Capital_To_Lowercase(u8_t bHex)
{
    if ((bHex >= (0x61-0x20)) && (bHex <= (0x7A-0x20)))
    {
        (bHex += 0x20);
    }
    return bHex;
}

//将字符串中的大写字母转小写
void Capital2Lowercase_str(void *buf, u16_t len)
{
    u8_t *p = buf;
    for (u16_t wCycle = 0; wCycle < (len); wCycle++)
    {
        *(p + wCycle) = Capital_To_Lowercase(*(p + wCycle));
    }
}

/***********************************************************************************
 函数名  : STRCMD_AtTest
 函数功能: "AT+TEST=tool.eybond.com,502\r\n"指令处理
 输入    : NULL
 输出    : NULL
 返回    : NULL
 作者    : wangkang
 时间    : 2019年10月19日
 说明    : NULL
************************************************************************************/
void STRCMD_AtTest(void *para)
{
    log_d("\r\nSTRCMD_AtTest\r\n");
    UINT8 *p            = (UINT8 *)para;
    UINT8 cnt           = 0;
    UINT8 equalPlace    = 0;                //等号
    UINT8 commaPlace    = 0;                //逗号
    UINT8 newlinePlace  = 0;                //换行符

    int flag=1;
    
    memset(g_RemoteIp, 0, sizeof(g_RemoteIp));
    memset(g_RemotePort, 0, sizeof(g_RemotePort));
    
    while(0 != *p)
    {
        if('=' == *p)
        {
            equalPlace = cnt;
        }
        if(',' == *p)
        {
            commaPlace = cnt;
        }
        if(('\r' == *p) || ('\n' == *p))
        {
            newlinePlace = cnt;
            break;
        }
        p++;
        cnt++;
    }

    log_d("\r\nnewlinePlace is %d\r\n",newlinePlace);
    
    memcpy(g_RemoteIp,(UINT8 *)para+equalPlace+1,commaPlace-equalPlace-1);      //等号 逗号-等号
   
    memcpy(g_RemotePort,(UINT8 *)para+commaPlace+1,newlinePlace-commaPlace-1);  //逗号 换行符-逗号
 
    if(strlen((char*)g_RemoteIp) >= 7 || strlen((char*)g_RemotePort) > 0)
    {
        COMMON_DataInit();
        COMMON_SetEvent(flag);
        tcp_connection();    	//发起TCP连接
    } 
}

void uart_set_get_para(void *command)
{
    UINT8 *p            = (UINT8 *)command;
    UINT8 cnt           = 0;       //计数
    UINT8 underline     = 0;       //下划线
    UINT8 equalPlace    = 0;       //等号
    UINT8 newlinePlace  = 0;       //换行符
    
    char *buf  = NULL;
    UINT16 len = 64;                                                //长度

    int8_t  para_num           =  0;
    char para_num_char[3]      = {0};    //参数编号
    char para_value[64]        = {0};    //参数值
    char operate[3]            = {0};    //操作类型 set or get

    INT32 CSQ;  
    INT32 ber;  
    char CSQ_char[3]={0};                                         //长度

    char CCID[21]={0};

    char IMEI[16]={0};

    // uint8_t device_online = 0;
    // char device_online_char[2] = {0};

    // uint8_t creg = 0;
    // uint8_t cgreg = 0;
    // char creg_char[2]  = {0};
    // char cgreg_char[2] = {0};

    memset(para_num_char,   0,  sizeof(para_num_char)); //参数编号
    memset(para_value,      0,  sizeof(para_value));    //参数值
    memset(operate,         0,  sizeof(operate));       //操作类型 set or get
    
    while(0 != *p)
    {
        if(('_'== *p)
         ||('-'== *p))
        {
            underline = cnt;   
        }
        if('=' == *p)
        {
            equalPlace = cnt;
        }
        if(('\r' == *p) || ('\n' == *p))
        {
            newlinePlace = cnt;
            break;
        }
        p++;
        cnt++;
    }
    // log_d("\r\nunderline is %d\r\n",underline);
    // log_d("\r\nequalPlace is %d\r\n",equalPlace);
    // log_d("\r\nnewlinePlace is %d\r\n",newlinePlace);
	// log_d("\r\ncommand:%s\r\n",command);  
    memcpy(operate,(UINT8 *)command,3);                                       //操作类型 set or get
    if(0 == equalPlace)//get
    {
        memcpy(para_num_char,(UINT8 *)command+underline+1,newlinePlace-underline-1);//下划线 等号-下划线
    }
    else//set
    {
        memcpy(para_num_char,(UINT8 *)command+underline+1,equalPlace-underline-1); //下划线 等号-下划线
        memcpy(para_value,(UINT8 *)command+equalPlace+1,newlinePlace-equalPlace-1);//等号 换行符-等号
    }
  
    if(
      (0==memcmp("set", operate, 3))
    ||(0==memcmp("SET", operate, 3))
    )
    {
        para_num = atoi(para_num_char);
        log_d("para_num is %d",para_num);

        if( ( 0 == para_num)||  //Reserved 特别预留，作为广播编号
            ( 4 == para_num)||  //协议版本 数采器支持的本协议的版本，固定3个字符。如版本号为1.2，则数据传输时为：0x31 0x2E 0x32
            ( 5 == para_num)||  //固件版本 固定7个字符，采用四段表示法，如：1.2.3.4
            ( 6 == para_num)||  //硬件版本 固定7个字符，采用四段表示法，如：1.2.3.4
            (11 == para_num)||  //在线设备数 当前处于监控状态下的设备个数，初始值为0
            (20 == para_num)||  //本端端口号 最大5个字符（1-65535）
            (27 == para_num)||  //系统告警信息 每个字符只有0和1两种状态，参见“告警信息编码表”
            (28 == para_num)||  //系统故障信息 每个字符只有0和1两种状态，参见“故障信息编码表”
            (44 == para_num)||  //无线网卡的MAC地址 固定12个字符，如：ACCF23734685
            (45 == para_num)||  //射频版本号 固定7个字符，采用四段表示法，如：1.2.3.4
            (55 == para_num)||  //信号强度CSQ
            (56 == para_num)||  //通信卡CCID
            (58 == para_num))   //IMEI  CPUID
        {
            log_d("\r\nOperation is not allowed\r\n"); //操作不被允许 
        }
        else
        {
            if(29 == para_num)
            {
                g_29 = atoi(para_value);
            }
            else
            {
                for (int j = 0; j < number_of_array_elements; j++)       
                {
                    if(para_num == PDT[j].num)
                    {
                        memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                        PDT[j].wFunc(&PDT[j],para_value, &len);
                        log_d("%d=%s;",PDT[j].num,para_value); 
                    } 
                }

                if(34 == para_num)//串口通讯速率
                {
                    g_34 = 1;
                }
            }
            parameter_check();
        }
    }
    else if(
            (0==memcmp("get", operate, 3))
          ||(0==memcmp("GET", operate, 3))
          )
    {
        if(
          (0==memcmp("all", para_num_char, 3))
        ||(0==memcmp("ALL", para_num_char, 3))
        )
        {
            for (int j = 0; j < number_of_array_elements; j++)
            {
                // if(11 == PDT[j].num)//是否有设备在线
                // {
                //     device_online = cm_gpio_read(COM);
                //     device_online = !(device_online);
                //     cm_itoa(device_online,&device_online_char[0],10);
                //     memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                //     PDT[j].wFunc(&PDT[j],device_online_char, &len);
                // }

    // reg_info_t reg_info1;
    // INT32 retreg = fibo_getRegInfo(&reg_info1,0);

    //             if(49 == PDT[j].num)//查询网络注册信息
    //             {
    //                 cm_gprs_getcregstate(&creg);
    //                 cm_itoa(creg,creg_char,10);
    //                 memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
    //                 PDT[j].wFunc(&PDT[j],creg_char, &len);
    //             }

    //             if(50 == PDT[j].num)//GPRS 网络注册状态
    //             {
    //                 cm_gprs_getcgregstate(&cgreg);
    //                 cm_itoa(cgreg,cgreg_char,10);
    //                 memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
    //                 PDT[j].wFunc(&PDT[j],cgreg_char, &len);
    //             }

                // if(51 == PDT[j].num)//固件编译日期
                // {
                //     memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                //     PDT[j].wFunc(&PDT[j],g_compile_date, &len);
                // }

                // if(52 == PDT[j].num)//固件编译时间
                // {
                //     memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                //     PDT[j].wFunc(&PDT[j],g_compile_time, &len);
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
                    fibo_get_ccid((uint8_t *)CCID);
                    memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                    PDT[j].wFunc(&PDT[j],CCID, &len);
                }

                if(58 == PDT[j].num)//CPUID IMEI
                {
                    fibo_get_imei((UINT8*)IMEI,0);
                    memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                    PDT[j].wFunc(&PDT[j],IMEI, &len);
                }

                buf = fibo_malloc(sizeof(char)*64);
                memset(buf, 0, sizeof(char)*64);
                PDT[j].rFunc(&PDT[j],buf, &len);
                memset(para_value, 0, 64);      
                memcpy(para_value,buf, len);
                fibo_free(buf);
                log_d("%d=%s;\r\n",PDT[j].num,para_value); 
            }
        }
        else
        {
            para_num = atoi(para_num_char);
            log_d("para_num is %d\r\n",para_num);
            for (int j = 0; j < number_of_array_elements; j++)
            {
                if(para_num == PDT[j].num)
                {
                    // if(11 == para_num)//是否有设备在线
                    // {
                    //     device_online = cm_gpio_read(COM);
                    //     device_online = !(device_online);
                    //     cm_itoa(device_online,&device_online_char[0],10);
                    //     memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                    //     PDT[j].wFunc(&PDT[j],device_online_char, &len);
                    // }

                    // if(49 == para_num)//查询网络注册信息
                    // {
                    //     cm_gprs_getcregstate(&creg);
                    //     cm_itoa(creg,creg_char,10);
                    //     memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                    //     PDT[j].wFunc(&PDT[j],creg_char, &len);
                    // }

                    // if(50 == para_num)//GPRS 网络注册状态
                    // {
                    //     cm_gprs_getcgregstate(&cgreg);
                    //     cm_itoa(cgreg,cgreg_char,10);
                    //     memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                    //     PDT[j].wFunc(&PDT[j],cgreg_char, &len);
                    // }

                    // if(51 == para_num)//固件编译日期
                    // {
                    //     memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                    //     PDT[j].wFunc(&PDT[j],g_compile_date, &len);
                    // }

                    // if(52 == para_num)//固件编译时间
                    // {
                    //     memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                    //     PDT[j].wFunc(&PDT[j],g_compile_time, &len);
                    // }

                    if(55 == para_num)
                    {
                        fibo_get_csq(&CSQ,&ber);
                        itoa(CSQ,CSQ_char,10);
                        memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                        PDT[j].wFunc(&PDT[j],CSQ_char, &len);
                    }

                    if(56 == para_num)//通信卡CCID
                    {
                        fibo_get_ccid((uint8_t*)CCID);
                        memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                        PDT[j].wFunc(&PDT[j],CCID, &len);
                    }

                    if(58 == para_num)//CPUID IMEI
                    {
                        fibo_get_imei((UINT8*)IMEI,0);
                        memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                        PDT[j].wFunc(&PDT[j],IMEI, &len);
                    }
                    
                    buf = fibo_malloc(sizeof(char)*64);
                    memset(buf, 0, sizeof(char)*64);
                    PDT[j].rFunc(&PDT[j],buf, &len);
                    memset(para_value, 0, 64);      
                    memcpy(para_value,buf, len);
                    fibo_free(buf);
                    log_d("%d=%s;\r\n",para_num,para_value); 
                }
            }
        }
    }
}