#include "fibo_opencpu.h"
#include "eybond_modbus_tcp_protocol.h"
#include "struct_type.h"
#include "data_collector_parameter_table.h"
#include "stdio.h"
#include "elog.h"
#include "string.h"
#include "uart_operate.h"
#include "net_task.h"
#include "modbus_crc.h"
#include "modbus_md5.h"
#include "md5_test.h"
#include "big_little_endian_swap.h"
#include "xmodem_server.h"
#include "data_block_burning_record_table.h"
#include "hextostr.h"
#include "gpio_operate.h"
#include "run_log.h"

// extern char g_compile_date[]; 
// extern char g_compile_time[]; 

extern int g_device_update_flag;
// // extern int g_para_save_flag;

// #ifndef SEEK_SET
// #define SEEK_SET 0
// #endif /* SEEK_SET */
// #define DEF_TRANSPARENT_UNLOCK        (1)

// #define run_log_a  "/run_log_a.ini"         //文件名
// #define run_log_b  "/run_log_b.ini"         //文件名

// /*----------------------------------------------------------------------------*
// **                             Global Vars                                    *
// **----------------------------------------------------------------------------*/

extern int      number_of_array_elements;
extern char     uart1_recv_data[UART_BUFFER_SIZE];
extern uint16_t uart1_recv_len;
extern char     g_RecvData[2*1024];
extern UINT16   g_RecvDataLen;
extern UINT16   g_SendDataLen;

extern INT8 socketid;

// extern int      iFd_run_log_a;
// extern int      iFd_run_log_b;
// extern UINT8    UpgradeProgress;
 UINT8    UpgradeProgress;

// static UINT32 dwOff_54_a  = 0;
// static UINT32 dwOff_54_b  = 0;

// UINT32 DataBlock_Len_54_a = 512;
// UINT32 DataBlock_Len_54_b = 512;
// /*----------------------------------------------------------------------------*
// **                             Local Vars                                     *
// **----------------------------------------------------------------------------*/

//4G_L610本地烧录和远程升级不是同一个文件：本地烧录为编译生成的.pac文件、远程升级为编译生成的.img文件
static const INT8 *g_recName =  "data_collector_firmware.img";                  //采集器固件文件名
const INT8 *g_device_recName =  "sub_equipment_firmware.bin";       //设备固件文件名
// //传输状态
// //0x00:成功
// //0x01:失败
// //0x02:中断
UINT8    firmware_transmission_status= 0x00;
uint8_t  g_29                        = 0; //29号参数
uint8_t  g_34                        = 0; //34号参数
uint8_t  g_54                        = 0; //54号参数
int      g_upgrade_mode_flag         = 0; //升级模式标志
INT32    g_iFd                       = 0; //采集器固件文件描述符
INT32    g_device_iFd                = 0; //设备固件文件描述符
UINT8    g_device_FW_type            = 0; //设备固件类型
UINT16   g_device_baudrate           = 0; //设备升级波特率
UINT8    g_device_AddressField       = 0; //设备地址
UINT32   g_device_firmware_real_size = 0; //设备固件实际大小
uint16_t g_block_size                = 0; //每个数据块长度
uint16_t g_block_amount              = 0; //数据区-数据块个数
UINT32   firmware_real_size          = 0; //固件的实际大小

static UINT8 HeartBeat_Package(void);               //0x01心跳包
static UINT8 GetDAU_Param(void);                    //0x02数采器参数查询
static UINT8 SetDAU_Param(void);                    //0x03数采器参数设置
static UINT8 Transparent_Transm(void);              //0x04完全透传
// static UINT8 ReatTimeDataUpload(void);              //0x11设备实时数据上传   
// static UINT8 HistoryDataUpload(void);               //0x13设备历史数据上传
static UINT8 TriggerDataLogUpGrade(void);           //0x21触发数采器固件升级
static UINT8 TriggerEquipmentUpGrade(void);         //0x22触发设备固件升级
static UINT8 ReadDeviceUpdateProgess(void);         //0x23查询设备的固件升级进度
static UINT8 CancelDeviceUpdate(void);              //0x24取消设备升级                 
static UINT8 GetFirmWareInformation(void);          //0x25获取固件信息             
static UINT8 SendFirmWareDataBlock(void);           //0x26发送固件数据块     
static UINT8 QueryFWBlockToReceiveInform(void);     //0x27查询固件数据块接收信息     
static UINT8 Verify_FirmWare(void);                 //0x28校验固件写入情况               
static UINT8 ExitFileTransferState(void);           //0x29退出文件传输状态
static INT32 MD5Verify_Func (UINT8 bHash[], UINT32 Verify_Len, UINT32 DataBlock_Len);
static INT32 device_MD5Verify_Func (UINT8 bHash[], UINT32 Verify_Len, UINT32 DataBlock_Len);
static void  correction_time(void);                 //校时

BOOL  data_frame_legal_checking(UINT8 a[])             //数据帧合法性检查
{
    UINT16 b = 6 + (a[4]*(1<<8))+a[5];
    if((0x01 == a[3])&&(b == g_RecvDataLen))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void receiving_processing(UINT8 function_code)
{
	switch(function_code)
	{
		case 0x01:								    //心跳包
			HeartBeat_Package();
			break;
		case 0x02:									//数采器参数查询
			GetDAU_Param();
			break;
		case 0x03:									//数采器参数设置
			SetDAU_Param();
            break;
        case 0x04:                                  //完全透传命令
            Transparent_Transm();                   
            break;
        // case 0x11:                                  //设备实时数据上传
        //     ReatTimeDataUpload();                       
        //     break;
        // case 0x13:                                  //设备历史数据上传
        //     HistoryDataUpload();                        
        //     break; 
		case 0x21:                                  //触发数采器固件升级
            g_upgrade_mode_flag = 0;       
			TriggerDataLogUpGrade();
			break;
		case 0x22:									//触发设备固件升级
            g_upgrade_mode_flag = 0;      	
		    TriggerEquipmentUpGrade();
			break;
		case 0x23:									//查询设备的固件升级进度
            if(2 == g_upgrade_mode_flag)
            {
                ReadDeviceUpdateProgess();
            }
            if(100 == UpgradeProgress)
            {
                fibo_file_close(g_device_iFd);
                fibo_file_delete(g_device_recName);
                fibo_file_exist(g_device_recName);
                g_upgrade_mode_flag = 0;                            //升级模式标志
                UpgradeProgress = 0;
                g_device_update_flag = 0;
            }
			break;
        case 0x24:                                  //取消设备升级
            if(2 == g_upgrade_mode_flag)
            {
                CancelDeviceUpdate();                       
            }
            break;
        case 0x25:                                  //获取固件信息
            GetFirmWareInformation();                   
            break;
        case 0x26:                                  //发送固件数据块
            if(0 != g_upgrade_mode_flag)
            {
                SendFirmWareDataBlock();                    
            }
            break; 
        case 0x27:                                  //查询固件数据块接收信息
            if(0 != g_upgrade_mode_flag)
            {
                QueryFWBlockToReceiveInform();              
            }
            break;
        case 0x28:                                  //校验固件写入情况
            if(0 != g_upgrade_mode_flag)
            {
                Verify_FirmWare();                          
            }
            break;
        case 0x29:                                  //退出文件传输状态
            log_d("\r\ng_upgrade_mode_flag is %d\r\n",g_upgrade_mode_flag); 
            if(0 != g_upgrade_mode_flag)
            {
                ExitFileTransferState();                    
            }
            memset(g_RecvData, 0, sizeof(g_RecvData));		//接收buff清零
            g_RecvDataLen = 0;								//接收长度清零
            fibo_sock_send(socketid, (UINT8 *)g_SendData, g_SendDataLen);

            //释放内存
            data_block_burning_record_table_destroy();
            if(0x00 == firmware_transmission_status)//成功
            {
                log_d("\r\nFile transfer completed\r\n"); 
                
                if(1 == g_upgrade_mode_flag)//采集器升级状态
                {
                    log_d("\r\n1 == g_upgrade_mode_flag\r\n"); 
                    char *data = NULL;
                    data = fibo_malloc(firmware_real_size);
                    memset(data, 0, firmware_real_size);
                    g_iFd = fibo_file_open((INT8*)g_recName, FS_O_RDONLY);
                    fibo_file_seek(g_iFd, 0, 0);
                    fibo_file_read(g_iFd,(UINT8 *)data,firmware_real_size); 
                    fibo_file_close(g_iFd);
                    if(0 == fibo_app_check((INT8*)data, firmware_real_size))
                    {
                        INT32 app_ota_ret = fibo_ota_handle((INT8*)data, firmware_real_size);
                        log_d("\r\napp ota_ret is %d\r\n",app_ota_ret); 
                    }
                    else
                    {
                        INT32 sdk_ota_ret = fibo_firmware_handle((INT8*)data, firmware_real_size);
                        log_d("\r\nsdk ota_ret is %d\r\n",sdk_ota_ret); 
                    }
                    fibo_free(data);
                }

                if(2 == g_upgrade_mode_flag)//设备升级状态
                {
                    g_device_update_flag = 1;
                    //DeviceUpdateTask();//Xmodem-1k                         //升级模式标志
                }
            }
            if(0x01 == firmware_transmission_status)//失败
            {
                log_d("\r\nFile transfer failure (such as final file MD5 verification failure)\r\n");
            }
            if(0x02 == firmware_transmission_status)//中断
            {
                log_d("\r\nUser canceled file transfer\r\n");
            }
            break; 
		default:            
            break;
	}
}

#pragma pack(push)                                  //保存对齐状态   
#pragma pack(1)                                     //设定为 1 字节对齐
typedef struct
{
    UINT16 msgNum;                                  //通讯编号 
    UINT16 deviceCode;                              //设备编码 
    UINT16 Datalen;                                 //数据长度 
    UINT8  deviceAddr;                              //设备地址 
}MBAP_Type;                                         //MBAP报文头
#pragma pack(pop)                                   //恢复对齐状态

static UINT8 HeartBeat_Package(void)                //0x01心跳包
{
    correction_time();                              //校时
    #pragma pack(push)                              //保存对齐状态   
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef struct 
    {
        MBAP_Type MBAP;                             //MBAP报文头
        UINT8     PDU_Func;                         //功能码
        UINT8     Time_Year;                        //年
        UINT8     Time_Month;                       //月
        UINT8     Time_Day;                         //日
        UINT8     Time_Hour;                        //时
        UINT8     Time_Min;                         //分
        UINT8     Time_Sec;                         //秒
        UINT8     Heart_Cyc_H;                      //心跳周期高字节
        UINT8     Heart_Cyc_L;                      //心跳周期低字节
    }ADU_t;                                         //服务器发送的数据格式
    typedef struct 
    {
        MBAP_Type MBAP;                             //MBAP报文头
        UINT8     PDU_Func;                         //功能码
        UINT8     PN[18];                           //数据区 支持14、18位PN
    }res_ADU_t;                                     //数采器响应的数据格式
    #pragma pack(pop)                               //恢复对齐状态

    ADU_t       *ADU            = (ADU_t *)g_RecvData;
	res_ADU_t   *res            = (res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));

    memset(res, 0, sizeof(res_ADU_t));  

	res->MBAP.msgNum            = ADU->MBAP.msgNum;             //1.报文编号

    // char        *deviceCode_buf = cm_malloc(sizeof(char)*64);
    // memset(deviceCode_buf, 0, sizeof(char)*64);  

    // UINT16      deviceCode_len  = 64;                           //长度
    // PDT[1].rFunc(&PDT[1],deviceCode_buf, &deviceCode_len);
    // UINT16      deviceCode      = 0x0100+atoi(deviceCode_buf);  //设备编码
    // res->MBAP.deviceCode        = swap_endian(deviceCode);	    //设备编码据实填充
    // cm_free(deviceCode_buf);

    res->MBAP.deviceCode        = ADU->MBAP.deviceCode;          //2.设备编码

 
	res->MBAP.deviceAddr        = ADU->MBAP.deviceAddr;         //4.设备地址
	res->PDU_Func		        = ADU->PDU_Func;	            //5.功能码

    char        *PN_buf         = NULL;
    UINT16      PN_len          = 64;                           //长度
	
	for (int j = 0; j < number_of_array_elements; j++)
	{
		if(2 == PDT[j].num)
		{
			PN_buf = fibo_malloc(sizeof(char)*64);
			memset(PN_buf, 0, sizeof(char)*64);
			PDT[j].rFunc(&PDT[j],PN_buf, &PN_len);

            if(14 == PN_len)
            {
                memcpy(res->PN,PN_buf,14);    // 支持14、18位PN  
                res->MBAP.Datalen	        = swap_endian(0x0010);	        //数据长度  
            }

            if(18 == PN_len)
            {
                memcpy(res->PN,PN_buf,18);    // 支持14、18位PN 
                res->MBAP.Datalen	        = swap_endian(0x0014);	        //数据长度   
            }
            
            fibo_free(PN_buf);
		}
	}

    g_SendDataLen               = 6 + swap_endian(res->MBAP.Datalen);
    memcpy(g_SendData,(char*)res,g_SendDataLen); 
    fibo_free(res);
    return 0;
} 

static UINT8 GetDAU_Param(void)                     //0x02数采器参数查询
{
    log_d("GetDAU_Param");  
    #pragma pack(push)                              //保存对齐状态  
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef  struct
    {
        MBAP_Type MBAP;                             //MBAP报文头
        UINT8     PDU_Func;                         //功能码
        UINT8     param[];                          //数据区------参数编号    
    }ADU_t;                                         //服务器发送的数据格式
    typedef  struct
    {
        MBAP_Type MBAP;                             //MBAP报文头
        UINT8     PDU_Func;                         //功能码
        UINT8     respondCode;                      //数据区------响应码
        UINT8     paramCode;                        //数据区------参数编号
        UINT8     data[1450];                       //数据区------查询数据
    }res_ADU_t;                                     //数采器响应的数据格式
    #pragma pack(pop)                               //恢复对齐状态

    ADU_t 		*ADU            = (ADU_t *)g_RecvData;
	res_ADU_t 	*res 	        = (res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));
    memset(res, 0, sizeof(res_ADU_t)); 
	res->MBAP.msgNum		    = ADU->MBAP.msgNum;	              //1通讯编号

    char        *deviceCode_buf = fibo_malloc(sizeof(char)*64);
    memset(deviceCode_buf, 0, sizeof(char)*64); 
    UINT16      deviceCode_len  = 64;                             //长度
    PDT[1].rFunc(&PDT[1],deviceCode_buf, &deviceCode_len);
    UINT16      deviceCode      = 0x0100+atoi(deviceCode_buf);    //2设备编码
    res->MBAP.deviceCode        = swap_endian(deviceCode);	      //设备编码据实填充
    fibo_free(deviceCode_buf);

	res->MBAP.deviceAddr	    = ADU->MBAP.deviceAddr;           //4设备地址
	res->PDU_Func			    = ADU->PDU_Func;		          //5功能码

    if(1 == g_upgrade_mode_flag)
    {
        res->respondCode            = 0x02;                      //0x02 繁忙 表示当前有更重要的任务要完成，比如正在进行自身固件的升级或设备固件的升级。
    }

    UINT8 parameter_number_n    = swap_endian(ADU->MBAP.Datalen)-2; //参数编号总数

    char *buf  = NULL;
    UINT16 len = 64;

    INT32 CSQ;  
    INT32 ber;  
    char CSQ_char[3]={0};                                         //长度
    
    char     CCID[21]    ={0};

    char     IMEI[16]    ={0};

    // uint8_t  device_online           = 0;
    // char     device_online_char[2]   = {0};

    // int32_t  run_log_size_a        = 0;
    // int32_t  run_log_size_b        = 0;
    // UINT8    s_pTemp_log[512]   = {0};

    //当进行多参数同时查询时，数采器需逐个参数进行响应。
    for(int i = 0; i < parameter_number_n; i++)
    {
        res->paramCode = ADU->param[i];                             //7参数编号
        for (int j = 0; j < number_of_array_elements; j++)          
        {
            if(res->paramCode == PDT[j].num)
            {  
                // if(11 == res->paramCode)//是否有设备在线
                // {
                //     device_online = cm_gpio_read(COM);
                //     device_online = !(device_online);
                //     cm_itoa(device_online,&device_online_char[0],10);
                //     memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                //     PDT[j].wFunc(&PDT[j],device_online_char, &len);
                // }

                // if(51 == res->paramCode)//固件编译日期
                // {
                //     memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                //     PDT[j].wFunc(&PDT[j],g_compile_date, &len);
                // }

                // if(52 == res->paramCode)//固件编译时间
                // {
                //     memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                //     PDT[j].wFunc(&PDT[j],g_compile_time, &len);
                // }

                // //读取采集器运行日志
                // if(54 == res->paramCode)
                // {
                //     iFd_run_log_a  = cm_file_open(run_log_a, O_RDONLY);
                //     run_log_size_a = cm_file_getsize(iFd_run_log_a);
                //     log_d("\r\nrun_log_size_a = %d\r\n",run_log_size_a);
                //     log_d("\r\ndwOff_54_a = %d\r\n",dwOff_54_a);
                //     if(dwOff_54_a < run_log_size_a)
                //     {
                //         cm_file_seek(iFd_run_log_a, dwOff_54_a, 0); 
                //         log_d("\r\ndwOff_54_a = %d\r\n",dwOff_54_a);
                //         if ((run_log_size_a - dwOff_54_a) < DataBlock_Len_54_a) 
                //         {
                //             DataBlock_Len_54_a = run_log_size_a - dwOff_54_a;
                //             log_d("\r\nDataBlock_Len_54_a = %d\r\n",DataBlock_Len_54_a);
                //         }
                //         cm_file_read(iFd_run_log_a, s_pTemp_log, DataBlock_Len_54_a);
                //         memset(res->data, 0, 1450);      
                //         memcpy(res->data,s_pTemp_log, DataBlock_Len_54_a);
                //         dwOff_54_a += DataBlock_Len_54_a;
                //         cm_file_close(iFd_run_log_a);
                //         res->respondCode = 0x00; //完成 成功获取参数数据
                //         len = DataBlock_Len_54_a;
                //         break;
                //     }
                //     else
                //     {
                //         iFd_run_log_b  = cm_file_open(run_log_b, O_RDONLY);
                //         run_log_size_b = cm_file_getsize(iFd_run_log_b);
                //         log_d("\r\nrun_log_size_b = %d\r\n",run_log_size_b);
                //         log_d("\r\ndwOff_54_b = %d\r\n",dwOff_54_b);
                //         if(dwOff_54_b < run_log_size_b)
                //         {
                //             cm_file_seek(iFd_run_log_b, dwOff_54_b, 0); 
                //             log_d("\r\ndwOff_54 = %d\r\n",dwOff_54_b);
                //             if ((run_log_size_b - dwOff_54_b) < DataBlock_Len_54_b) 
                //             {
                //                 DataBlock_Len_54_b = run_log_size_b - dwOff_54_b;
                //                 log_d("\r\nDataBlock_Len_54_b = %d\r\n",DataBlock_Len_54_b);
                //             }
                //             cm_file_read(iFd_run_log_b, s_pTemp_log, DataBlock_Len_54_b);
                //             memset(res->data, 0, 1450);      
                //             memcpy(res->data,s_pTemp_log, DataBlock_Len_54_b);
                //             dwOff_54_b += DataBlock_Len_54_b;
                //             cm_file_close(iFd_run_log_b);
                //             res->respondCode = 0x00; //完成 成功获取参数数据
                //             len = DataBlock_Len_54_b;
                //             break;
                //         }
                //         else
                //         {
                //             log_clean();
                //             memset(res->data, 0, 1450); 
                //             res->respondCode = 0x00; //完成 成功获取参数数据
                //             len = 1;
                //             break;
                //         }
                //     }
                // }

                if(55 == res->paramCode)//信号强度CSQ
                {
                    fibo_get_csq(&CSQ,&ber);
                    itoa(CSQ,CSQ_char,10);
                    memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                    PDT[j].wFunc(&PDT[j],CSQ_char, &len);
                }

                if(56 == res->paramCode)//通信卡CCID
                {
                    fibo_get_ccid((uint8_t *)CCID);
                    memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                    PDT[j].wFunc(&PDT[j],CCID, &len);
                }

                if(58 == res->paramCode)//CPUID IMEI
                {
                    fibo_get_imei((UINT8 *)IMEI,0);
                    memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                    PDT[j].wFunc(&PDT[j],IMEI, &len);
                }
                
                buf = fibo_malloc(sizeof(char)*64);
                memset(buf, 0, sizeof(char)*64);
                PDT[j].rFunc(&PDT[j],buf, &len);
                memset(res->data, 0, 64);      
                memcpy(res->data,buf, len);
                fibo_free(buf);
                res->respondCode            = 0x00;                 //完成 成功获取参数数据
                break;
            }
            else
            {
                res->respondCode            = 0x01;//错误 参数不存在，或本类数采器不支持该参数
            } 
        }

        res->MBAP.Datalen = swap_endian(len+4);                      //数据长度
        g_SendDataLen = 6 + swap_endian(res->MBAP.Datalen);
        memcpy(g_SendData,(char*)res,g_SendDataLen); 
        
        if(i < parameter_number_n-1)
        {
            fibo_sock_send(socketid, (UINT8 *)g_SendData, g_SendDataLen);
            log_hex((UINT8 *)g_SendData,g_SendDataLen);
        }
    }
    fibo_free(res);
    log_d("GetDAU_Param run end");  
    return 0;
}

static UINT8 SetDAU_Param(void)                     //0x03数采器参数设置
{
    #pragma pack(push)                              //保存对齐状态  
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef struct 
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               paramCode;              //数据区------参数编号
        UINT8               param[];                //设置数据     
    }ADU_t;                                         //服务器发送的数据格式
    typedef struct 
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               respondCode;            //响应码
        UINT8               paramCode;              //参数编号
    }res_ADU_t;                                     //数采器响应的数据格式
    #pragma pack(pop)                               //恢复对齐状态
  
    ADU_t       *ADU        = (ADU_t *)g_RecvData;   
    res_ADU_t   *res        = (res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));
	res->MBAP.msgNum	    =	ADU->MBAP.msgNum;	    //通讯编号
                                                        //设备编码
    char        *deviceCode_buf = fibo_malloc(sizeof(char)*64);
    UINT16      deviceCode_len  = 64;                       
    PDT[1].rFunc(&PDT[1],deviceCode_buf, &deviceCode_len);
    UINT16      deviceCode      = 0x0100+atoi(deviceCode_buf);
    res->MBAP.deviceCode        = swap_endian(deviceCode);	     
    fibo_free(deviceCode_buf);

	res->MBAP.Datalen		=	swap_endian(0x0004);    //数据长度
	res->MBAP.deviceAddr	=	ADU->MBAP.deviceAddr;   //设备地址
	res->PDU_Func			=	ADU->PDU_Func;		    //功能码
                                                        //响应码
	res->paramCode			=   ADU->paramCode;		    //数据区->参数编号

    if((1 == g_upgrade_mode_flag)||(1 == g_device_update_flag))
    {
        res->respondCode = 0x04;//0x04 繁忙 表示当前有更重要的任务要完成，比如正在进行自身固件的升级或设备固件的升级。
    }

    UINT16  len   = 64;                                //长度

    for (int j = 0; j < number_of_array_elements; j++)
    {
        if(res->paramCode == PDT[j].num)
        {
            if( ( 0 == res->paramCode)|| //Reserved 特别预留，作为广播编号
                ( 4 == res->paramCode)|| //协议版本 数采器支持的本协议的版本，固定3个字符。如版本号为1.2，则数据传输时为：0x31 0x2E 0x32
                ( 5 == res->paramCode)|| //固件版本 固定7个字符，采用四段表示法，如：1.2.3.4
                ( 6 == res->paramCode)|| //硬件版本 固定7个字符，采用四段表示法，如：1.2.3.4
                (11 == res->paramCode)|| //在线设备数 当前处于监控状态下的设备个数，初始值为0
                (20 == res->paramCode)|| //本端端口号 最大5个字符（1-65535）
                (27 == res->paramCode)|| //系统告警信息 每个字符只有0和1两种状态，参见“告警信息编码表”
                (28 == res->paramCode)|| //系统故障信息 每个字符只有0和1两种状态，参见“故障信息编码表”
                (44 == res->paramCode)|| //无线网卡的MAC地址 固定12个字符，如：ACCF23734685
                (45 == res->paramCode)|| //射频版本号 固定7个字符，采用四段表示法，如：1.2.3.4
                (51 == res->paramCode)|| //固件编译日期
                (52 == res->paramCode)|| //固件编译时间
                (55 == res->paramCode)|| //信号强度CSQ
                (56 == res->paramCode)|| //通信卡CCID
                (58 == res->paramCode))  //GPRS系统中使用IMEI代替CPUID
            {
                res->respondCode        =   0x02;//操作不被允许
                break;
            }
            else
            {
                res->respondCode        =   0x00;//成功
            //     if(54 == res->paramCode)
            //     {
            //         g_54 = atoi(ADU->param);
            //         log_d("\r\ng_54：%d\r\n",g_54);
            //         if(1 == g_54)
            //         {
            //             dwOff_54_a  = 0;
            //             dwOff_54_b  = 0;
            //         }
            //     }

                if(29 == res->paramCode)
                {
                    g_29 = atoi((char *)ADU->param);
                    log_d("\r\ng_29：%d\r\n",g_29);
                }

                if((29 != res->paramCode)&&(54 != res->paramCode))
                {
                    log_d("\r\nADU->param：%s\r\n",ADU->param);
                    memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
                    PDT[j].wFunc(&PDT[j],ADU->param, &len);
                    if(34 == res->paramCode)//串口通讯速率
                    {
                        g_34 = 1;
                    }
                }
                
                parameter_check();//保存参数
                break;
            } 
        } 
        else
        {
            res->respondCode        =   0x03;//不支持该项参数
        }
    }

    g_SendDataLen= 6 + swap_endian(res->MBAP.Datalen);
    memcpy(g_SendData,(char*)res,g_SendDataLen); 
    fibo_free(res);
    return 0;
}

static UINT8 Transparent_Transm(void)               //0x04完全透传命令
{
    #pragma pack(push)                              //保存对齐状态  
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef struct 
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               Transparent;            //数据区（透传数据区）     
    }ADU_t;                                         //服务器发送的数据格式
    typedef struct 
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               Transparent[2048];      //数据区（透传数据区 ）   
    }res_ADU_t;                                     //数采器响应的数据格式
    #pragma pack(pop)                               //恢复对齐状态
  
    ADU_t   *ADU = (ADU_t *)g_RecvData;   
                                                                        //透传发送
    int write_len = swap_endian(ADU->MBAP.Datalen)-2;
    char* serial_write_buffer = fibo_malloc(sizeof(char)*write_len);
    memcpy(serial_write_buffer,(char*)&ADU->Transparent,write_len);
    memset(uart1_recv_data, 0, sizeof(uart1_recv_data));                //接收清零
	uart1_recv_len = 0;
    uart_write(UART1, (UINT8 *)serial_write_buffer, write_len);
    fibo_taskSleep(500);
    res_ADU_t 	*res 	    =   (res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));
    res->MBAP.msgNum	    =	ADU->MBAP.msgNum;               //通讯编号
    res->MBAP.deviceCode	=	swap_endian(0x0103);            //设备编码
    res->MBAP.deviceAddr	=	ADU->MBAP.deviceAddr;	        //设备地址
    res->PDU_Func			=	ADU->PDU_Func;		            //功能码
    memcpy((char*)res->Transparent,uart1_recv_data,uart1_recv_len);
    res->MBAP.Datalen		=	swap_endian(uart1_recv_len + 2); //数据长度
    g_SendDataLen= 6 +swap_endian(res->MBAP.Datalen);
    memcpy(g_SendData,(char*)res,g_SendDataLen); 
    fibo_free(res);
    if(uart1_recv_len > 0)
    {
        com_lamp_on();
    }
    else
    {
        com_lamp_off();
    }
    fibo_free(serial_write_buffer);
    return 0;
}

// static UINT8  ReatTimeDataUpload(void)              //0x11 查询设备实时参数 
// {
//     #pragma pack(push)                              //保存对齐状态   
//     #pragma pack(1)                                 //设定为 1 字节对齐
//     typedef struct 
//     {
//         MBAP_Type      MBAP;                        //MBAP报文头
//         UINT8          PDU_Func;                    //功能码
//         UINT8          data_type;                   //数据区
//     }ADU_t;                                         //服务器发送的数据格式
//     typedef struct 
//     {
//         MBAP_Type           MBAP;                   //MBAP报文头
//         UINT8               PDU_Func;               //功能码
//         UINT8               respondCode;            //数据区------响应码
//         UINT8               data_type;              //数据区------数据类型
//         UINT8               data[5];                //数据区------查询数据
//     }res_ADU_t;                                     //数采器响应的数据格式
//     #pragma pack(pop)                               //恢复对齐状态

//     ADU_t       *ADU        = (ADU_t *)g_RecvData;
// 	res_ADU_t 	*res        = (res_ADU_t *)cm_malloc(sizeof(res_ADU_t));

//     res->MBAP.msgNum		=	ADU->MBAP.msgNum;	    //1通讯编号
// 	res->MBAP.deviceCode	=	swap_endian(0x0103);    //2设备编码
//     res->MBAP.Datalen       =   swap_endian(0x0004);    //3数据长度
// 	res->MBAP.deviceAddr	=	ADU->MBAP.deviceAddr;   //4设备地址
// 	res->PDU_Func			=	ADU->PDU_Func;		    //5功能码
//     res->respondCode        =   0x02;                   //6响应码
//     res->data_type          =   0x00;                   //7数据类型

//     g_SendDataLen = 6 + swap_endian(res->MBAP.Datalen);
//     log_d("\r\ng_SendDataLen：%d\r\n",g_SendDataLen);
//     memcpy(g_SendData,(char*)res,g_SendDataLen); 
//     cm_free(res);
//     return 0;
// }

// static UINT8 HistoryDataUpload(void)                //0x13设备历史数据上传
// {
//     #pragma pack(push)                              //保存对齐状态
//     #pragma pack(1)                                 //设定为 1 字节对齐
//     typedef struct
//     {
//         MBAP_Type   MBAP;                           //MBAP报文头
//         UINT8       PDU_Func;                       //功能码
//         UINT8       requestCode;                    //数据区---请求码
//         UINT16      intervalTime;                   //数据区---数据间隔（秒）
//     } ADU_t;                                        //服务器发送的数据格式
//     typedef struct
//     {
//         MBAP_Type   MBAP;                           //MBAP报文头
//         UINT8       PDU_Func;                       //功能码
//         UINT8       respondCode;                    //响应码
//         UINT8       Time_Year;                      //时间戳-年
//         UINT8       Time_Month;                     //时间戳-月
//         UINT8       Time_Day;                       //时间戳-日
//         UINT8       Time_Hour;                      //时间戳-时
//         UINT8       Time_Min;                       //时间戳-分
//         UINT8       Time_Sec;                       //时间戳-秒
//         UINT8       datatype;                       //数据类型
//         UINT8       data[5];                        //数据区段内容
//     } res_ADU_t;                                    //数采器响应的数据格式
//     #pragma pack(pop)                               //恢复对齐状态
//     ADU_t       *ADU        = (ADU_t *)g_RecvData;
// 	res_ADU_t 	*res        = (res_ADU_t *)cm_malloc(sizeof(res_ADU_t));

//     res->MBAP.msgNum	    = ADU->MBAP.msgNum;		//1通讯编号
// 	res->MBAP.deviceCode	= swap_endian(0x0103);	//2设备编码
//     res->MBAP.Datalen       = swap_endian(0x0003);  //3数据长度
// 	res->MBAP.deviceAddr	= ADU->MBAP.deviceAddr;	//4设备地址
// 	res->PDU_Func			= ADU->PDU_Func;	    //5功能码
//     res->respondCode        = 0x00;                 //6响应码
//     g_SendDataLen           = 6 + swap_endian(res->MBAP.Datalen);
//     log_d("\r\ng_SendDataLen：%d\r\n",g_SendDataLen);
//     memcpy(g_SendData,(char*)res,g_SendDataLen); 
//     cm_free(res);
//     return 0;
// }

static UINT8 TriggerDataLogUpGrade(void)            //0x21触发数采器固件升级
{
    #pragma pack(push)                              //保存对齐状态
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef struct 
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT16              block;                  //数据区-数据块个数
        UINT16              block_size;             //数据区-每个数据块长度       
    }ADU_t;                                         //服务器发送的数据格式
    typedef struct 
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               respondCode;            //响应码
    }res_ADU_t;                                     //数采器响应的数据格式
    #pragma pack(pop)                               //恢复对齐状态
    ADU_t     *ADU     = (ADU_t *)g_RecvData;

                                                        //固件大小
    UINT32 firmware_size = (swap_endian(ADU->block))*(swap_endian(ADU->block_size));
    log_d("\r\nfirmware_size:%d\r\n",firmware_size);
                                                        //数据块个数
    log_d("\r\nswap_endian:ADU->block：%d\r\n",swap_endian(ADU->block));
                                                        //每个数据块长度
    log_d("\r\nswap_endian:ADU->block_size：%d\r\n",swap_endian(ADU->block_size));
                                                    //数据块烧写记录表大小确认
    data_block_burning_record_table_init(ADU->block);                                         
    //文件系统使用情况
    uint32_t file_free_size  = 0;                       //文件系统可用大小

    file_free_size = fibo_file_getFreeSize();
    log_d("\r\nfile_free_size：%d\r\n",file_free_size);

    res_ADU_t 	*res        =(res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));

    if(file_free_size < firmware_size)                  //文件系统可用空间 小于 固件大小
    {
        log_clean();
        file_free_size = fibo_file_getFreeSize();
        if(file_free_size < firmware_size)
        {
            //0x02 错误 数据块个数和每个数据块的长度相乘过大，无对应缓冲区存储。
            //特殊地，当请求命令中的“数据块个数”为 0x0000 时，表示无完整的固件。
            res->respondCode = 0x02;                        //响应码-错误
        }
    }

    if(0 == g_upgrade_mode_flag)
    {
        g_upgrade_mode_flag = 1;                        //进入升级模式
        //0x00 成功 数采器已经转入升级模式，做好了升级准备，即将接收来自服务器的文件传输请求。
        res->respondCode = 0x00;                        //响应码-成功
        fibo_file_close(g_iFd);
        fibo_file_delete(g_recName);
        fibo_file_exist(g_recName);
        g_iFd = fibo_file_open(g_recName, FS_O_RDWR|FS_O_CREAT);//打开文件  创建文件														//打开文件
        if (g_iFd < 0)
        {
            log_d("\r\nCreate test.txt failed\r\n");
        }

        fibo_file_close(g_iFd);                           //关闭文件
    }
    else
    {
        //0x01 失败 数采器有更紧急的任务要完成（如正在给设备更新固件），暂不接受升级任务。
        res->respondCode = 0x01;//响应码-失败
    }

    g_block_amount          =   swap_endian(ADU->block);             //数据区-数据块个数
    g_block_size            =   swap_endian(ADU->block_size);        //每个数据块长度   

    res->MBAP.msgNum        =   ADU->MBAP.msgNum;       //通讯编号
    res->MBAP.deviceCode	=	swap_endian(0x0103);	//设备编码
    res->MBAP.Datalen       =   swap_endian(0x0003);    //数据长度
	res->MBAP.deviceAddr	=	ADU->MBAP.deviceAddr;	//设备地址
	res->PDU_Func			=	ADU->PDU_Func;			//功能码

    g_SendDataLen = 6 + swap_endian(res->MBAP.Datalen);
    log_d("\r\ng_SendDataLen：%d\r\n",g_SendDataLen);
    memcpy(g_SendData,(char*)res,g_SendDataLen); 
    fibo_free(res);

    return 0;
}

static UINT8 TriggerEquipmentUpGrade(void)          //0x22触发设备固件升级
{
    #pragma pack(push)                              //保存对齐状态
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef struct
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               FW_type;                //数据区-固件类型
        UINT16              baudrate;               //数据区-升级波特率
        UINT16              block;                  //数据区-数据块个数
        UINT16              block_size;             //数据区-每个数据块长度
    } ADU_t;                                        //服务器发送的数据格式
    typedef struct
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               respondCode;            //响应码
    } res_ADU_t;                                    //数采器响应的数据格式
    #pragma pack(pop)                               //恢复对齐状态

    ADU_t       *ADU        = (ADU_t *)g_RecvData;
                                                       //设备固件大小
    UINT32 firmware_size = (swap_endian(ADU->block))*(swap_endian(ADU->block_size));
    log_d("\r\nfirmware_size:%d\r\n",firmware_size);

                                                        //数据块个数
    log_d("\r\nswap_endian:ADU->block：%d\r\n",swap_endian(ADU->block));

                                                        //每个数据块长度
    log_d("\r\nswap_endian:ADU->block_size：%d\r\n",swap_endian(ADU->block_size));

                                                    //数据块烧写记录表大小确认
    data_block_burning_record_table_init(ADU->block);
  
    //特殊地，当请求命令中的“数据块个数”为 0x0000 时，表示将立即切换到 BOOT 区进行升级操作。
    if(0x0000 == ADU->block)
    {
        log_d("\r\nSwitch to the BOOT area immediately to upgrade.\r\n");
    }

    //文件系统使用情况
    uint32_t file_free_size  = 0;                       //文件系统可用大小
    file_free_size = fibo_file_getFreeSize();
    log_d("\r\nfile_free_size：%d\r\n",file_free_size);

	res_ADU_t 	*res        = (res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));

    if(file_free_size < firmware_size)                  //文件系统可用空间 小于 固件大小
    {
        log_clean();
        file_free_size = fibo_file_getFreeSize();
        if(file_free_size < firmware_size)
        {
            //0x02 错误 数据块个数和每个数据块的长度相乘过大，无对应缓冲区存储。
            //特殊地，当请求命令中的“数据块个数”为 0x0000 时，表示无完整的固件。
            res->respondCode = 0x02;                        //响应码-错误
        }
    }

    if(0 == g_upgrade_mode_flag)
    {
        g_upgrade_mode_flag = 2;                        //进入设备升级模式
        //0x00 成功 数采器已经转入升级模式，做好了升级准备，即将接收来自服务器的文件传输请求。
        res->respondCode = 0x00;                        //响应码-成功
        fibo_file_close(g_device_iFd);
        fibo_file_delete(g_device_recName);
        fibo_file_exist(g_device_recName);
        g_device_iFd = fibo_file_open(g_device_recName, FS_O_RDWR|FS_O_CREAT);//打开文件														//打开文件
        if (g_device_iFd <= 0)
        {
            log_d("\r\nCreate device.bin failed\r\n");
        }
        fibo_file_close(g_device_iFd);                           //关闭文件
    }
    else
    {
        //0x01 失败 数采器有更紧急的任务要完成（如正在给设备更新固件），暂不接受升级任务。
        res->respondCode = 0x01;//响应码-失败
    }

    g_device_FW_type        =   ADU->FW_type;           //设备固件类型
    // g_device_baudrate       =   ADU->baudrate;          //设备升级波特率
    g_device_baudrate       =   0x0000;          //设备升级波特率

    g_device_AddressField   =   ADU->MBAP.deviceAddr;	//逆变器设备地址

    g_block_amount          =   swap_endian(ADU->block);             //数据区-数据块个数
    g_block_size            =   swap_endian(ADU->block_size);        //每个数据块长度   

    res->MBAP.msgNum        =   ADU->MBAP.msgNum;       //通讯编号
    res->MBAP.deviceCode	=	ADU->MBAP.deviceCode;	//设备编码
    res->MBAP.Datalen       =   swap_endian(0x0003);    //数据长度
	res->MBAP.deviceAddr	=	ADU->MBAP.deviceAddr;	//设备地址
	res->PDU_Func			=	ADU->PDU_Func;			//功能码

    g_device_AddressField   =   ADU->MBAP.deviceAddr;	//逆变器设备地址


    g_SendDataLen = 6 + swap_endian(res->MBAP.Datalen);
    log_d("\r\ng_SendDataLen：%d\r\n",g_SendDataLen);
    memcpy(g_SendData,(char*)res,g_SendDataLen); 
    fibo_free(res);
    return 0;
}

static UINT8 ReadDeviceUpdateProgess(void)          //0x23查询设备的固件升级进度
{
    #pragma pack(push)                              //保存对齐状态
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef struct
    {
        MBAP_Type          MBAP;                    //MBAP报文头
        UINT8              PDU_Func;                //功能码
    } ADU_t;                                        //服务器发送的数据格式
    typedef struct
    {
        MBAP_Type          MBAP;                    //MBAP报文头
        UINT8              PDU_Func;                //功能码
        UINT8              Progress;                //数据区-进度值
    } res_ADU_t;                                    //数采器响应的数据格式
    #pragma pack(pop)                               //恢复对齐状态

    ADU_t       *ADU        =   (ADU_t *)g_RecvData;
	res_ADU_t 	*res        =   (res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));

    res->MBAP.msgNum        =   ADU->MBAP.msgNum;       //通讯编号
    res->MBAP.deviceCode	=   swap_endian(0x0103);    //设备编码
    res->MBAP.Datalen       =   swap_endian(0x0003);    //数据长度
	res->MBAP.deviceAddr	=   ADU->MBAP.deviceAddr;	//设备地址
	res->PDU_Func			=   ADU->PDU_Func;			//功能码
    res->Progress           =   UpgradeProgress;        //进度值


    
    g_SendDataLen = 6 + swap_endian(res->MBAP.Datalen);
    log_d("\r\ng_SendDataLen：%d\r\n",g_SendDataLen);
    memcpy(g_SendData,(char*)res,g_SendDataLen); 
    fibo_free(res);
    return 0;
}

static UINT8 CancelDeviceUpdate(void)               //0x24取消设备升级                 
{
    #pragma pack(push)                              //保存对齐状态 
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef struct
    {
        MBAP_Type          MBAP;                    //MBAP报文头
        UINT8              PDU_Func;                //功能码
        UINT8              Data;                    //数据区-强制中断标识
    } ADU_t;                                        //服务器发送的数据格式
    typedef struct
    {
        MBAP_Type          MBAP;                    //MBAP报文头
        UINT8              PDU_Func;                //功能码
        UINT8              respondCode;             //响应码
    } res_ADU_t;                                    //数采器响应的数据格式
    #pragma pack(pop)                               //恢复对齐状态

    ADU_t       *ADU        = (ADU_t *)g_RecvData;
	res_ADU_t 	*res        = (res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));
    res->MBAP.msgNum        =   ADU->MBAP.msgNum;       //通讯编号
    res->MBAP.deviceCode	=	swap_endian(0x0103);	//设备编码
    res->MBAP.Datalen       =   swap_endian(0x0003);    //数据长度
	res->MBAP.deviceAddr	=	ADU->MBAP.deviceAddr;	//设备地址
	res->PDU_Func			=	ADU->PDU_Func;			//功能码
    res->respondCode        =   0x00;                   //成功：表示数采器执行设备升级取消的指令。

    g_SendDataLen = 6 + swap_endian(res->MBAP.Datalen);
    log_d("\r\ng_SendDataLen：%d\r\n",g_SendDataLen);
    memcpy(g_SendData,(char*)res,g_SendDataLen); 
    fibo_free(res);
    return 0;
}

static UINT8 GetFirmWareInformation(void)           //0x25获取固件信息              
{
    #pragma pack(push)                              //保存对齐状态
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef struct
    {
        MBAP_Type          MBAP;                    //MBAP报文头
        UINT8              PDU_Func;                //功能码
        UINT8              FW_area;                 //数据区-固件区域
    } ADU_t;                                        //服务器发送的数据格式
    typedef struct
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               FW_area;                //数据区-固件区域
        UINT8               respondCode;            //数据区-响应码
        UINT8               MD5[32];                //数据区-MD5值
    } res_ADU_t;                                    //数采器响应的数据格式
    #pragma pack(pop)//恢复对齐状态

    ADU_t     *ADU       = (ADU_t *)g_RecvData;
	res_ADU_t *res       = (res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));
    res->MBAP.msgNum     = ADU->MBAP.msgNum;        //通讯编号
    res->MBAP.deviceCode = swap_endian(0x0001);	    //设备编码
    res->MBAP.Datalen    = swap_endian(0x0024);     //数据长度
	res->MBAP.deviceAddr = ADU->MBAP.deviceAddr;    //设备地址
	res->PDU_Func	     = ADU->PDU_Func;		    //功能码
    res->FW_area         = ADU->FW_area;            //固件区域

    //不存在 表示数采器对应区域不存在已经下载完全的固件。
    res->respondCode     =   0x02;      

    //当响应码不为 0x00 时，响应数据中的“MD5 值”区域全部用 0x00 填充。
    if(0x00 != res->respondCode)
    {
        for(int i=0;i<32;i++)
        {
            res->MD5[i]=0x00;
        }
    }

    g_SendDataLen = 6 + swap_endian(res->MBAP.Datalen);
    log_d("\r\ng_SendDataLen：%d\r\n",g_SendDataLen);
    memcpy(g_SendData,(char*)res,g_SendDataLen); 
    fibo_free(res);

    return 0;
}

static UINT8 SendFirmWareDataBlock(void)            //0x26发送固件数据块    
{
    #pragma pack(push)                              //保存对齐状态 
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef struct
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT16              block_code;             //数据区-数据块编号
        UINT8               data[];                 //数据块内容//数据块CRC值                              
    } ADU_t;                                        //服务器发送的数据格式
    typedef struct
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT16              block_code;             //数据区-数据块编号
        UINT8               respondCode;            //数据区-响应码
    } res_ADU_t;                                    //数采器响应的数据格式
    #pragma pack(pop)                               //恢复对齐状态

    ADU_t       *ADU        = (ADU_t *)g_RecvData;
	res_ADU_t 	*res        = (res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));

    //数采器在收到服务器的该功能码后，对“数据块内容”进行 CRC 校验，并与“数据块 CRC 值”
    uint8_t  *pucframe  = ADU->data ;
    uint16_t usdatalen  = swap_endian(ADU->MBAP.Datalen)-6;
    uint16_t c_checksum = crc16rtu(pucframe,usdatalen);
    log_d("\r\nc_checksum:%x\r\n",c_checksum); 

    log_d("\r\ns_checksum_1:%x\r\n",ADU->data[usdatalen]);
    log_d("\r\ns_checksum_2:%x\r\n",ADU->data[usdatalen+1]);
    uint16_t s_checksum=(uint16_t)ADU->data[usdatalen]+((uint16_t)ADU->data[usdatalen+1]<<8);
    log_d("\r\ns_checksum:%x\r\n",s_checksum);

    if(c_checksum == s_checksum)                        //校验成功
    {
        //0x00 成功 表示数采器对该数据块正确接收。
        res->respondCode = 0x00;                        //数据区-响应码
        int32_t wlen=0;

        //判断是设备固件 or 采集器固件
        if(1 == g_upgrade_mode_flag)//采集器固件
        {
            g_iFd = fibo_file_open(g_recName, FS_O_RDWR|FS_O_APPEND);
            fibo_file_write(g_iFd, (UINT8 *)ADU->data, g_block_size);   //写内容到文件
            log_d("\r\nADU->data：%s\r\n",ADU->data);
            fibo_file_close(g_iFd); 
        }
        
        if(2 == g_upgrade_mode_flag)//设备固件
        {
            g_device_iFd = fibo_file_open(g_device_recName, FS_O_RDWR|FS_O_APPEND);
            wlen  = fibo_file_write(g_device_iFd, ADU->data, g_block_size);   //写内容到文件
            log_d("\r\nADU->data：%s\r\n",ADU->data);
            fibo_file_close(g_device_iFd);
        }
        
        if (wlen < 0)
        {
            log_d("\r\nWrite file failed\r\n"); 
        }
        else
        {
            data_block_burning_record_table_set(ADU->block_code);
        }
    }
    else//校验失败
    {
        //0x01 失败 表示数采器对该数据块未正确接收，要求重传。
        res->respondCode        =   0x01;                   //数据区-响应码
    }

    res->MBAP.msgNum        =   ADU->MBAP.msgNum;       //通讯编号
    res->MBAP.deviceCode	=	swap_endian(0x0001);	//设备编码
    res->MBAP.Datalen       =   swap_endian(0x0005);    //数据长度
	res->MBAP.deviceAddr	=	ADU->MBAP.deviceAddr;	//设备地址
	res->PDU_Func			=	ADU->PDU_Func;			//功能码
    res->block_code         =   ADU->block_code;        //数据区-数据块编号

    g_SendDataLen = 6 + swap_endian(res->MBAP.Datalen);
    log_d("\r\ng_SendDataLen：%d\r\n",g_SendDataLen);
    memcpy(g_SendData,(char*)res,g_SendDataLen); 
    fibo_free(res);
    return 0;
}

static UINT8 QueryFWBlockToReceiveInform(void)      //0x27查询固件数据块接收信息      
{
    #pragma pack(push)                              //保存对齐状态 
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef struct
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT16              block_amount;           //数据区-数据块个数
    } ADU_t;                                        //服务器发送的数据格式
    typedef struct
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               state;                  //数据区-烧录状态
        UINT8               data[];                  //数据区-数据块烧写记录表
    } res_ADU_t;                                    //数采器响应的数据格式
    #pragma pack(pop)                               //恢复对齐状态

    ADU_t       *ADU     = (ADU_t *)g_RecvData;
	res_ADU_t 	*res     = (res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));

    res->MBAP.msgNum     = ADU->MBAP.msgNum;                    //通讯编号
    res->MBAP.deviceCode = ADU->MBAP.deviceCode;	            //设备编码

	res->MBAP.deviceAddr = ADU->MBAP.deviceAddr;                //设备地址
	res->PDU_Func		 = ADU->PDU_Func;			            //功能码
    res->state           = 0x00;                                //数据区-烧录状态
    res->MBAP.Datalen    = swap_endian(3+data_block_burning_record_table_size_get()); 

    g_SendDataLen = 6 + swap_endian(res->MBAP.Datalen);
    log_d("\r\ng_SendDataLen：%d\r\n",g_SendDataLen);
    memcpy(g_SendData,(char*)res,9); 
    memcpy(g_SendData+9,data_block_burning_record_table_value_get(),data_block_burning_record_table_size_get());  
    fibo_free(res);
    return 0;
}

static UINT8 Verify_FirmWare(void)                  //0x28校验固件写入情况                   
{
    #pragma pack(push)                              //保存对齐状态
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef struct
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               verify_len[4];          //数据区-校验长度
        UINT8               MD5[32];                //数据区-MD5值
    } ADU_t;                                        //服务器发送的数据格式
    typedef struct
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               calibration_result;     //数据区-校验结果
    } res_ADU_t;                                    //数采器响应的数据格式
    #pragma pack(pop)//恢复对齐状态

    ADU_t       *ADU        = (ADU_t *)g_RecvData;
	res_ADU_t 	*res        = (res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));

    res->MBAP.msgNum        = ADU->MBAP.msgNum;     //通讯编号
    res->MBAP.deviceCode	= swap_endian(0x0001);	//设备编码
    res->MBAP.Datalen       = swap_endian(0x0003);  //数据长度
	res->MBAP.deviceAddr	= ADU->MBAP.deviceAddr;	//设备地址
	res->PDU_Func			= ADU->PDU_Func;		//功能码

    UINT8       *c_MD5_1    = fibo_malloc(sizeof(UINT8)*32);   //客户端MD5值   
    UINT8       *c_MD5      = fibo_malloc(sizeof(UINT8)*32);   //客户端MD5值   
    UINT32      Verify_Len  = (UINT32)((UINT32)(ADU->verify_len[0] << 24   )|   
                                       (UINT32)(ADU->verify_len[1] << 16   )|   
                                       (UINT32)(ADU->verify_len[2] << 8    )|   
                                       (UINT32)(ADU->verify_len[3]         ));
    log_d("\r\nADU->verify_len[0] is %d\r\n",ADU->verify_len[0]); 
    log_d("\r\nADU->verify_len[1] is %d\r\n",ADU->verify_len[1]); 
    log_d("\r\nADU->verify_len[2] is %d\r\n",ADU->verify_len[2]); 
    log_d("\r\nADU->verify_len[3] is %d\r\n",ADU->verify_len[3]);                                 
    log_d("\r\nVerify_Len：%d\r\n",Verify_Len);   

    firmware_real_size   = Verify_Len ;

    if(1 == g_upgrade_mode_flag)//采集器固件升级
    {
        MD5Verify_Func(c_MD5_1,  Verify_Len, g_block_size);
    }

    if(2 == g_upgrade_mode_flag)//设备固件升级
    {
        g_device_firmware_real_size = Verify_Len;
        device_MD5Verify_Func(c_MD5_1,  Verify_Len, g_block_size);
    }                         
    
    hextostr(c_MD5_1, c_MD5, 16);
    log_d("\r\nc_MD5：%s\r\n",c_MD5);

    UINT8       *s_MD5      = fibo_malloc(sizeof(UINT8)*32);   //服务器MD5值
    memcpy(s_MD5,ADU->MD5,32);                            
    log_d("\r\ns_MD5：%s\r\n",s_MD5);

    if(0==memcmp(c_MD5,s_MD5,16))
    {
        //0x00 成功 数采器将写入的固件全部读出，计算出 MD5 值，与服务器下发的 MD5 比较，完全一致。
        res->calibration_result = 0x00;//数据区-校验结果
    }
    else
    {
        //0x01 失败 数采器将写入的固件全部读出，计算出 MD5 值，与服务器下发的 MD5 比较，二者不同。
        res->calibration_result = 0x01;//数据区-校验结果
    }

    g_SendDataLen = 6 + swap_endian(res->MBAP.Datalen);
    log_d("\r\ng_SendDataLen：%d\r\n",g_SendDataLen);
    memcpy(g_SendData,(char*)res,g_SendDataLen); 
    fibo_free(res);

    fibo_free(c_MD5_1);
    fibo_free(c_MD5);
    fibo_free(s_MD5);
    return 0;
}

static UINT8 ExitFileTransferState(void)            //0x29退出文件传输状态             
{
    #pragma pack(push)                              //保存对齐状态 
    #pragma pack(1)                                 //设定为 1 字节对齐
    typedef struct
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               transmission_status;    //数据区-传输状态
    } ADU_t;                                        //服务器发送的数据格式
    typedef struct
    {
        MBAP_Type           MBAP;                   //MBAP报文头
        UINT8               PDU_Func;               //功能码
        UINT8               respond;                //数据区-响应码
    } res_ADU_t;                                    //数采器响应的数据格式
    #pragma pack(pop)                               //恢复对齐状态

    ADU_t       *ADU        = (ADU_t *)g_RecvData;
	res_ADU_t 	*res        = (res_ADU_t *)fibo_malloc(sizeof(res_ADU_t));
  
    res->MBAP.msgNum        = ADU->MBAP.msgNum;         //通讯编号
    res->MBAP.deviceCode	= swap_endian(0x0001);	    //设备编码
    res->MBAP.Datalen       = swap_endian(0x0003);      //字节据实填充
	res->MBAP.deviceAddr	= ADU->MBAP.deviceAddr;	    //设备地址
	res->PDU_Func			= ADU->PDU_Func;			//功能码
    res->respond            = 0x00;                     //数据区-响应码

    firmware_transmission_status = ADU->transmission_status;//传输状态  

    g_SendDataLen = 6 + swap_endian(res->MBAP.Datalen);
    log_d("\r\ng_SendDataLen：%d\r\n",g_SendDataLen);
    memcpy(g_SendData,(char*)res,g_SendDataLen); 
    fibo_free(res);
    return 0;
}

//数采器固件升级MD5校验
static INT32 MD5Verify_Func (UINT8 bHash[], UINT32 Verify_Len, UINT32 DataBlock_Len)//生成MD5值
{
    HASH_MD5_CTX  MD5;
    HASH_MD5Init (&MD5);
    static UINT8 s_pTempMd5[512] = {0};
    UINT32       dwOff           = 0;
    memset(s_pTempMd5, 0, sizeof(s_pTempMd5));

    log_d("\r\nVerify_Len is %d\r\n",Verify_Len);

    while (dwOff < Verify_Len)
    {
        fibo_file_open(g_recName, FS_O_RDONLY);
        fibo_file_seek(g_iFd, dwOff, 0);
        fibo_file_read(g_iFd, s_pTempMd5, DataBlock_Len);
        if ((Verify_Len - dwOff) < DataBlock_Len) 
        {
            DataBlock_Len = Verify_Len - dwOff;
        }
        HASH_MD5Update(&MD5, s_pTempMd5, DataBlock_Len);
        dwOff += DataBlock_Len;
        fibo_file_close(g_iFd);
    }
    HASH_MD5Final (bHash, &MD5);
    return 0;
}

//设备固件升级MD5校验
static INT32 device_MD5Verify_Func (UINT8 bHash[], UINT32 Verify_Len, UINT32 DataBlock_Len)//生成MD5值
{
    HASH_MD5_CTX  MD5;
    HASH_MD5Init (&MD5);
    static UINT8 s_pTempMd5[512] = {0};
    UINT32       dwOff           = 0;
    memset(s_pTempMd5, 0, sizeof(s_pTempMd5));

    log_d("\r\nVerify_Len is %d\r\n",Verify_Len);

    while (dwOff < Verify_Len)
    {
        fibo_file_open(g_device_recName, FS_O_RDONLY);
        fibo_file_seek(g_device_iFd, dwOff, 0);
        fibo_file_read(g_device_iFd, s_pTempMd5, DataBlock_Len);
        if ((Verify_Len - dwOff) < DataBlock_Len) 
        {
            DataBlock_Len = Verify_Len - dwOff;
        }
        HASH_MD5Update(&MD5, s_pTempMd5, DataBlock_Len);
        dwOff += DataBlock_Len;
        fibo_file_close(g_device_iFd);
    }
    HASH_MD5Final (bHash, &MD5);
    return 0;
}

//校时
static void correction_time(void)
{
    UINT16 len = 0;
    char current_char[30]={0};
	hal_rtc_time_t current = {
		.year 		= g_RecvData[8]+2000,
		.month 		= g_RecvData[9],
		.day 		= g_RecvData[10],
		.hour		= g_RecvData[11]+8,
		.min	    = g_RecvData[12],
		.sec		= g_RecvData[13],
	};

    hal_rtc_time_t local_time;

	INT32 current_flag = fibo_setRTC(&current);             //设置本地时间 参数：time 时间结构体
    log_d("current_flag is %ld\r\n",current_flag);

    INT32 local_time_flag = fibo_getRTC(&local_time);       //获取本地时间
    log_d("local_time_flag is %ld\r\n",local_time_flag);

    sprintf(current_char, "%04d-%02d-%02d %02d:%02d:%02d",current.year,current.month,current.day,current.hour,current.min,current.sec);
                                                            //yyyy-MM-dd HH:mm:ss

    for (int j = 0; j < number_of_array_elements; j++)
    {
        if(26 == PDT[j].num)                                //系统时间
        {
            memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j],current_char, &len);
        }
    }

	log_d("current is :%04d-%02d-%02d %02d:%02d:%02d\r\n",current.year,current.month,current.day,current.hour,current.min,current.sec);
    log_d("local_time is :%04d-%02d-%02d %02d:%02d:%02d\r\n",2000+local_time.year,local_time.month,local_time.day,local_time.hour,local_time.min,local_time.sec);
	log_d("correction_time ok\r\n");
}