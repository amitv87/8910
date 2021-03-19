/**
 * 运行日志
 * 每次记录运行日志需加上时间戳
 * 运行日志记录系统：断网、连接成功、重启等信息
*/

#include "run_log.h"
#include "data_collector_parameter_table.h"
#include "elog.h"
#include "string.h"
#include "fibo_opencpu.h"

#define run_log_a  "/run_log_a.ini"         //文件名
#define run_log_b  "/run_log_b.ini"         //文件名

int32_t     iFd_run_log_a = 0;                //文件描述符
int32_t     iFd_run_log_b = 0;                //文件描述符

extern int number_of_array_elements;

void log_init(void)
{
    log_d("\r\n%s()\r\n",__func__);

    int check_a = 0;
    check_a = fibo_file_exist(run_log_a);
    if(1 == check_a)//文件存在
    {
        log_d("\r\nrun_log_a is exist\r\n");
        iFd_run_log_a = fibo_file_open(run_log_a, FS_O_RDWR|FS_O_APPEND);//读写、追加
    }
    if(check_a<0)//文件不存在
    {
        log_d("\r\nrun_log_a  creat\r\n");
        iFd_run_log_a = fibo_file_open(run_log_a, FS_O_RDWR|FS_O_CREAT|FS_O_APPEND);//读写、创建、追加
    }
    fibo_file_close(iFd_run_log_a);

    int check_b = 0;
    check_b = fibo_file_exist(run_log_b);
    if(1 == check_b)//文件存在
    {
        log_d("\r\nrun_log_b is exist\r\n");
        iFd_run_log_b = fibo_file_open(run_log_b, FS_O_RDWR|FS_O_APPEND);//读写、追加
    }
    if(check_b<0)//文件不存在
    {
        log_d("\r\nrun_log_b  creat\r\n");
        iFd_run_log_b = fibo_file_open(run_log_b, FS_O_RDWR|FS_O_CREAT|FS_O_APPEND);//读写、创建、追加
    }
    fibo_file_close(iFd_run_log_b);
}

void log_save(char *log)
{
    UINT8 *p = (UINT8 *)log;

    int32_t run_log_size_a = 0;
    int32_t run_log_size_b = 0;

    //时间戳
    char time_stamp[30]={0};
    hal_rtc_time_t local_time;
    fibo_getRTC(&local_time);//该函数用于获取本地时间
    sprintf(time_stamp,"\r\n%04d.%02d.%02d.%02d.%02d.%02d\r\n",local_time.year,local_time.month,local_time.day,local_time.hour,local_time.min,local_time.sec);
   
    iFd_run_log_a  = fibo_file_open(run_log_a, FS_O_RDONLY);
    run_log_size_a = fibo_file_getSize(run_log_a);
    fibo_file_close(iFd_run_log_a);

    iFd_run_log_b  = fibo_file_open(run_log_b, FS_O_RDONLY);
    run_log_size_b = fibo_file_getSize(run_log_b);
    fibo_file_close(iFd_run_log_b);

    log_d("\r\nrun_log_size_a is %d\r\n",run_log_size_a);
    log_d("\r\nrun_log_size_b is %d\r\n",run_log_size_b);

    if(run_log_size_a < 65536)//64k
    {
        iFd_run_log_a = fibo_file_open(run_log_a, FS_O_WRONLY|FS_O_APPEND);     //只写、追加
        fibo_file_write(iFd_run_log_a, (UINT8 *)time_stamp, sizeof(time_stamp));
        while(0 != *p)
        {
            fibo_file_write(iFd_run_log_a,p,1);
            p++;
        }
        fibo_file_write(iFd_run_log_a, (UINT8 *)"\r\n", 2);
        fibo_file_close(iFd_run_log_a);
    }
    else
    {
        if(run_log_size_b < 65536)//64k
        {
            iFd_run_log_b = fibo_file_open(run_log_b, FS_O_WRONLY|FS_O_APPEND);     //只写、追加
            fibo_file_write(iFd_run_log_b, (UINT8 *)time_stamp, sizeof(time_stamp));
            while(0 != *p)
            {
                fibo_file_write(iFd_run_log_b,p,1);
                p++;
            }
            fibo_file_write(iFd_run_log_b, (UINT8 *)"\r\n", 2);
            fibo_file_close(iFd_run_log_b);
        }
        else
        {
            log_clean();
            iFd_run_log_a = fibo_file_open(run_log_a, FS_O_WRONLY|FS_O_APPEND);     //只写、追加
            fibo_file_write(iFd_run_log_a, (UINT8 *)time_stamp, sizeof(time_stamp));
            while(0 != *p)
            {
                fibo_file_write(iFd_run_log_a,p,1);
                p++;
            }
            fibo_file_write(iFd_run_log_a, (UINT8 *)"\r\n", 2);
            fibo_file_close(iFd_run_log_a);
        }  
    }
}

void log_clean(void)
{
    fibo_file_delete(run_log_a);
    fibo_file_delete(run_log_b);
    log_init();
}