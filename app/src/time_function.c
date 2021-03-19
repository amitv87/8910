#include "fibo_opencpu.h"
#include "elog.h"
#include "time_function.h"

//全局变量
char g_compile_date[9] ={0};
char g_compile_time[7] ={0};

/*-----------------------------------------------------------------------------
Function Name	:	set_get_localtime
Author			:	wangkang
Created Time	:	2019.09.11
Description 	:	设置本地时间和获取本地时间程序
Input Argv		:
Output Argv 	:
Return Value	:	
-----------------------------------------------------------------------------*/	
void set_get_localtime()
{
	hal_rtc_time_t current = {
					.year 		= 1995,
					.month 		= 12,
					.day 		= 25,
					.wDay		= 1,
					.hour		= 2,
					.min		= 3,
					.sec		= 4,
	};
	fibo_setRTC(&current);//设置本地时间 参数：time 时间结构体
	fibo_getRTC(&current);//获取本地时间 参数：time 时间结构体
	log_d("Time:%d%d%d%d%d%d%d",current.year,current.month,current.day,current.dayofweek,current.hour,current.min,current.sec);
	
}

/*-----------------------------------------------------------------------------
Function Name	:	getsysuptime
Author			:	wangkang
Created Time	:	2019.09.11
Description 	:	系统运行时间程序
Input Argv		:
Output Argv 	:
Return Value	:	
-----------------------------------------------------------------------------*/	
// void getsysuptime()
// {
// 	u32 ut = 0;
// 	ut = cm_getsysuptime();//获取系统启动后运行的时间，单位 ms
// 	log_d("cm_getsysuptime:%d",ut);
// }

/*-----------------------------------------------------------------------------
Function Name	:	sleep
Author			:	wangkang
Created Time	:	2019.09.11
Description 	:	睡眠时间程序
Input Argv		:
Output Argv 	:
Return Value	:	
-----------------------------------------------------------------------------*/	
void sleep(UINT32 ms)
{
	fibo_taskSleep(ms);//睡眠指定时间
	// log_d("\r\nsleep:%d\r\n",ms);
}

//编译时间
void build_moment(char *moment)
{
	UINT8 *p  = (UINT8 *)moment;
	UINT8 cnt           = 0; //计数
    UINT8 underline     = 0; //下划线
 
    while(0 != *p)
    {
        if('_'== *p)
        {
            underline = cnt;   
        }
        p++;
        cnt++;
    }

	memcpy(g_compile_date,(UINT8 *)moment,8);
	memcpy(g_compile_time,(UINT8 *)moment+underline+1,6);
}