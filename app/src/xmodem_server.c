/*
 * Copyright 2001-2010 Georges Menie (www.menie.org)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* this code needs standard functions memcpy() and memset()
   and input/output functions _inbyte() and _outbyte().

   the prototypes of the input/output functions are:
     int _inbyte(unsigned short timeout); // msec timeout
     void _outbyte(int c);
 */
#include "xmodem_server.h"
#include "big_little_endian_swap.h"
#include "modbus_crc.h"
#include "string.h"
#include "uart_operate.h"
#include "elog.h"
#include "common_data.h"

int g_device_update_flag;

extern UINT8    g_device_FW_type;               //设备固件类型
extern UINT16   g_device_baudrate;              //设备升级波特率
extern UINT8    g_device_AddressField;          //设备地址
extern INT32    g_device_iFd;                   //文件描述符
extern const INT8 *g_device_recName;           //文件名
extern UINT32 g_device_firmware_real_size;      //设备固件实际大小
extern UINT32 g_EventFlag;

#define XMODEM_NO_EXIT      (0)
#define XMODEM_EXIT         (1)
#define XMODEM_TIMEOUT      (500)      

// 传输过程的校验模式
#define XMODEM_CHECK_CRC    1    //  CRC校验
#define XMODEM_CHECK_SUM    0    //  累加校验

#define SOH     0x01
#define STX     0x02    //Xmodem-1K数据头
#define ETX     0x03    //中断操作(CTRL+C)
#define EOT     0x04    //文件传输结束
#define ACK     0x06    //认可响应
#define NAK     0x15    //不认可响应
#define CAN     0x18    //撤销传送
#define CTRLZ   0x1A    //填充字符(CTRL+Z)

#define DLY_1S 1000
#define MAXRETRANS 10

/* 设备正常响应升级的数据包 */
#pragma pack(1)
typedef  struct
{
    UINT8 AddressField; //地址域
    UINT8 PDU_Func;     //功能码
    UINT8 DataBuff[4];
    UINT16 Crc16;       //校验码
}DEVICE_UPDATE_RESPOND;
#pragma pack()

static INT8 xmodem_1K_server(UINT32 dwSize);
static void xmodem_XmodemInit(void);

// 定义Xmodem当前工作模式
static volatile UINT8 bXmodemExit = XMODEM_NO_EXIT;

// 定义Xmodem缓存，1024 for XModem 1k + 3 head chars + 2 crc + nul
static UINT8 bXmodemBuf[1030] = {0};

// 定义传输过程的校验模式
static UINT8 bXmodemCheckMode;

// 定义信息包序号
static UINT8 bPktNum = 1;

 UINT8 UpgradeProgress = 0;//升级进度

static UINT16 EachPackageSize = 0;

DEVICE_UPDATE_START DeviceUpdateStart;              //设备开始更新命令包
static DEVICE_UPDATE_RESPOND DeviceUpdateRespond;   //设备响应包

//发送数据
static void xmodem_lowlevelSendByte(UINT8 bData)
{
    log_d("\r\n%s()\r\n", __func__);
    UINT8 write_buf[1]={0};
    write_buf[0] = bData;
    uart_write(UART1,write_buf, strlen((char *)write_buf));
}

//接收数据
static UINT8 xmodem_lowLevelRecvByte(UINT8* bData)
{
    log_d("\r\n%s()\r\n", __func__);
    log_d("\r\nuart1_recv_len is %d\r\n",uart1_recv_len); 
    log_hex((UINT8 *)uart1_recv_data, uart1_recv_len);
    memcpy(bData,uart1_recv_data,uart1_recv_len); 
    if(uart1_recv_len > 0)
    {
        return TRUE;
    }
    return FALSE;
}

// 接收到返回TRUE
static UINT8 xmodem_inbyte(UINT16 wMs, UINT8* pbData)
{
    log_d("\r\n%s()\r\n", __func__);
    fibo_taskSleep(wMs);
    if(xmodem_lowLevelRecvByte(pbData) == TRUE)
    {
        return TRUE;
    }
        
    return FALSE;
}

//发送数据
static void xmodem_outbyte(UINT8 bData)
{
    log_d("\r\n%s()\r\n", __func__);
    xmodem_lowlevelSendByte(bData);
}

static UINT16 xmodem_crc16_ccitt(const UINT8 *pbBuf, UINT32 dwLen)
{
    UINT16 wCRC = 0;
    UINT8  bCycle;

    while (dwLen-- > 0)
    {
        wCRC ^= (unsigned short) * pbBuf++ << 8;
        for (bCycle = 0; bCycle < 8; bCycle++)
        {
            if (wCRC & 0x8000)
            {
                wCRC = wCRC << 1 ^ 0x1021;
            }
            else
            {
                wCRC <<= 1;
            }
        }
    }
    return wCRC;
}

//static UINT8 xmodem_check(UINT8 bCheckMode, const UINT8 *pbBuf, UINT32 dwLen)
//{
//    if (bCheckMode)
//    {
//        UINT16 wCRC    = xmodem_crc16_ccitt(pbBuf, dwLen);
//        UINT16 wCRCRef = (pbBuf[dwLen] << 8) + pbBuf[dwLen + 1];
//        if (wCRC == wCRCRef)
//        {
//            return TRUE;
//        }
//    }
//    else
//    {
//        UINT32 dwCycle;
//        UINT8  bSum = 0;
//        for (dwCycle = 0; dwCycle < dwLen; ++dwCycle)
//        {
//            bSum += pbBuf[dwCycle];
//        }
//        if (bSum == pbBuf[dwLen])
//        {
//            return TRUE;
//        }
//    }

//    return FALSE;
//}

//初始化
INT32 xmodem_transmitInit(void)
{
    log_d("\r\n%s()\r\n", __func__);
    UINT8 bData = 0;
    UINT8 bRetry = 0;

    bPktNum     = 1;
    bXmodemExit = XMODEM_NO_EXIT;

    for(bRetry = 0; bRetry < MAXRETRANS; bRetry++)
    {
        if(xmodem_inbyte((DLY_1S), &bData) == TRUE)
        {
            switch (bData)
            {
                case 'C'://启动传输
                    log_d("\r\n\r\nC:Start transfer\r\n\r\n");
                    UpgradeProgress = 0;
                    bXmodemCheckMode = XMODEM_CHECK_CRC;//  CRC校验
                    return 0;
                case NAK://请求重发同一数据包
                    log_d("\r\n\r\nNAK:Request to resend the same packet\r\n\r\n");
                    UpgradeProgress = 0;
                    bXmodemCheckMode = XMODEM_CHECK_SUM;//  累加校验
                    return 0;
                case CAN://请求无条件停止传输过程
                    log_d("\r\n\r\nCAN:Request to stop the transfer process unconditionally\r\n\r\n");
                    return -1; /* canceled by remote 被远程取消 */
                    break;
                default:
                    break;
            }
        }
        if(bXmodemExit == XMODEM_EXIT)
        {
            return -1;
        }
    }
    return -2; /* no sync */
}

//传输数据
INT8 xmodem_transmitData(UINT8 *pbData, UINT32 dwDataSize)
{
    log_d("\r\n%s()\r\n", __func__);
    UINT32 dwPktDataLen;        // 信息帧中数据区长度
    UINT32 dwCycle;
    UINT32 dwOffset = 0;        // 与原始数据长度对应的偏移量
    UINT32 dwLen;               // 当前数据长度
    UINT8  bData = 0;           // 接收到的数据
    UINT8  bRetry;              // 重复次数

    for(;;)
    {
    start_trans:
        log_d("\r\nstart_trans\r\n");
        dwPktDataLen = 1024;    //包数据长度

        pbData[0] = STX;        //数据头
        log_d("\r\npbData[0] is %d\r\n",pbData[0]);
        pbData[1] = bPktNum;    //包序号
        log_d("\r\npbData[1] is %d\r\n",pbData[1]);
        pbData[2] = ~bPktNum;   //包序号的补码 
        log_d("\r\npbData[2] is %d\r\n",pbData[2]);

        if(dwOffset >= dwDataSize)
        {
            // 原始数据发送完毕
            log_d("\r\ndwOffset is %d\r\n",dwOffset);
            log_d("\r\ndwDataSize is %d\r\n",dwDataSize);
            log_d("\r\n End of raw data sending\r\n");
            goto done_trans;
        }
        else
        {
            // 原始数据未发送完，再取数据
            dwLen = dwDataSize - dwOffset;
            log_d("\r\ndwDataSize is %d\r\n",dwDataSize);
            log_d("\r\ndwOffset is %d\r\n",dwOffset);
            log_d("\r\ndwLen = dwDataSize - dwOffset = %d\r\n",dwLen);
        }

        if(dwLen > dwPktDataLen)
        {
            dwLen = dwPktDataLen;// 信息帧中数据区长度
            log_d("\r\ndwPktDataLen is %d\r\n",dwPktDataLen);
            log_d("\r\ndwLen is %d\r\n",dwLen);
        }

        if(dwLen >= 0)
        {
            //  memset (&bXmodemBuf[3], 0, dwPktDataLen);
            if(dwLen == 0)
            {
                pbData[3] = CTRLZ;
            }
            else
            {
                //  memcpy (&bXmodemBuf[3], &pbData[dwOffset], dwLen);
                if(dwLen < dwPktDataLen)
                {
                    pbData[3 + dwLen] = CTRLZ;
                }
            }

            if(bXmodemCheckMode)//1 
            {
                UINT16 wCRC = xmodem_crc16_ccitt(&pbData[3], dwPktDataLen);
                log_d("\r\nxmodem_crc16_ccitt wCRC is %x\r\n",wCRC);

                pbData[dwPktDataLen + 3] = (wCRC >> 8) & 0xFF;
                log_d("\r\npbData[%d] is %x\r\n",dwPktDataLen + 3,pbData[dwPktDataLen + 3]);

                pbData[dwPktDataLen + 4] = wCRC & 0xFF;
                log_d("\r\npbData[%d] is %x\r\n",dwPktDataLen + 4,pbData[dwPktDataLen + 4]);
            }
            else
            {
                UINT8 bSum = 0;
                for (dwCycle = 3; dwCycle < dwPktDataLen + 3; ++dwCycle)
                {
                    bSum += pbData[dwCycle];
                }
                pbData[dwPktDataLen + 3] = bSum;
            }

            //for(bRetry = 0; bRetry < 10; ++bRetry)
            for(bRetry = 0; bRetry < MAXRETRANS; ++bRetry)
            {
                fibo_taskSleep(1);//延时

                // for(dwCycle = 0; dwCycle < dwPktDataLen + 3 + (bXmodemCheckMode ? 2 : 1); ++dwCycle)
                // {                       //信息帧中数据区长度   //定义传输过程的校验模式
                //     xmodem_outbyte(pbData[dwCycle]);
                //     log_d("\r\npbData[%d]=%d\r\n",dwCycle,pbData[dwCycle]);
                // }
                
                uart_write(UART1,pbData, 1029);
                log_hex(pbData, 1029);
                if(xmodem_inbyte(DLY_1S, &bData) == TRUE)
                {
                    switch (bData)
                    {
                        case ACK://认可响应
                            log_d("\r\nACK:Acknowledgement response。\r\n");
                            ++bPktNum;
                            dwOffset += dwPktDataLen;
                            goto start_trans;
                        case CAN://撤销传送
                            log_d("\r\nCAN:Undo delivery。\r\n");
                            return -1; /* canceled by remote */
                            break;
                        case NAK://不认可响应
                            log_d("\r\nNAK:Disapproval response。\r\n");
                            continue;
                        default:
                            continue;
                    }
                }
            }
            return -4; /* xmit error */
        }
        else
        {
        done_trans:
            return 0;//srcsz;
        }
    }
}

//传输完成
INT32 xmodem_transmitFinish(void)
{
    UINT8 bRetry;
    UINT8 bData = 0;

    for(bRetry = 0; bRetry < MAXRETRANS; ++bRetry)
    {
        xmodem_outbyte(EOT);
        if((xmodem_inbyte((DLY_1S), &bData) == TRUE) && (bData == ACK))
        {
            break;
        }
        if(bXmodemExit == XMODEM_EXIT)
        {
            return -1;
        }
    }

    return (bData == ACK) ? 0 : -5;
}

//整个过程
INT8 xmodem_transmit(UINT8* pbData, UINT32 dwDataSize)
{
    log_d("\r\n%s()\r\n", __func__);
    INT8   bErr     = 0;
    UINT32 dwLen    = dwDataSize;
    UINT32 dwOffset = 0;//偏移量
    EachPackageSize = dwDataSize / 100;   //将固件包分成100份

    bErr = xmodem_transmitInit();
    if(bErr != 0)
    {
        log_d("xmodem_transmitInit fail\r\n");
        return bErr;
    }

    while(dwLen)
    {
        if(dwLen >= 1024)
        {
            log_d("\r\ndwLen = %d\r\n",dwLen);
            memset (&pbData[3], 0, 1024);
            fibo_file_open(g_device_recName, FS_O_RDONLY);
            log_d("\r\ncm_file_open\r\n");
            fibo_file_seek(g_device_iFd, dwOffset, 0);
            log_d("\r\ncm_file_seek dwOffset is %d\r\n",dwOffset);
            fibo_file_read(g_device_iFd, &pbData[3], 1024);
            log_d("\r\ncm_file_read\r\n");

            bErr = xmodem_transmitData(pbData, 1024);

            if(bErr == 0)
            {
                //传输成功
                log_d("\r\nTransfer succeeded\r\n");
            }
            else
            {
                //传输失败，或被主动取消
                log_d("\r\nTransmission failed or was cancelled\r\n");
                return -1;
            }
            dwLen -= 1024;
            dwOffset += 1024;
            UpgradeProgress = dwOffset / EachPackageSize;
            fibo_file_close(g_device_iFd);
            log_d("\r\ncm_file_close\r\n");
        }
        else
        {
            memset (&pbData[3], 0, dwLen);
            fibo_file_open(g_device_recName, FS_O_RDONLY);
            fibo_file_seek(g_device_iFd, dwOffset, 0);
            fibo_file_read(g_device_iFd, &pbData[3], dwLen);
            bErr = xmodem_transmitData(pbData, dwLen);
            if(bErr == 0)
            {
                // 传输成功
                log_d("\r\nTransfer succeeded\r\n");
            }
            else
            {
                // 传输失败，或被主动取消
                log_d("\r\nTransmission failed or was cancelled\r\n");
                return -1;
            }
            dwOffset += dwLen;
            dwLen = 0;
            UpgradeProgress = 100;
            fibo_file_close(g_device_iFd);
        }
        log_d("UpgradeProgress = %d\r\n", UpgradeProgress);

        if(XMODEM_EXIT == bXmodemExit)
        {
            bErr = -1;
            return bErr;
        }
    }
    bErr = xmodem_transmitFinish();
    if(bErr != 0)
    {
        // 正文结束错误
    }

    return bErr;
}

//服务
INT8 xmodem_1K_server(UINT32 dwSize)
{
    log_d("\r\n%s()\r\n", __func__);
    log_d("\r\ndwSize is %d\r\n", dwSize);
    INT8 st = 0;

    st = xmodem_transmit(bXmodemBuf, dwSize);

    if (st < 0)
    {
        // Xmodem transmit error
        xmodem_outbyte(ETX);//中断操作
        xmodem_outbyte(ETX);
        xmodem_outbyte(ETX);
        UpgradeProgress = 0;
        return st;
    }

    return 0;
}

UINT8 xmodem_TransProgress(void)
{
    return UpgradeProgress;
}

void xmodem_ExcelXmodemUpdateState(void)
{
    bXmodemExit = XMODEM_EXIT;
}

void xmodem_XmodemInit(void)
{
    log_d("\r\n%s()\r\n", __func__);
    memset(bXmodemBuf, 0, sizeof(bXmodemBuf));
    UpgradeProgress = 0;
    bXmodemExit = XMODEM_NO_EXIT;
}
/*******************************************************************************
* Function    : DeviceUpdateTask
* Description : 用Xmodem协议升级设备
* Input       : void
* Output      : void
* Return      : void
* Data        : 2017年12月20日
* Others      : 
*******************************************************************************/
void DeviceUpdateTask()
{
    log_d("\r\n%s()\r\n", __func__);
    UINT8   i   = 0;
    uint16_t write_len = 0;
    char upp_c = '0';
    COMMON_DataInit();

    COMMON_SetEvent(DEF_EVENT_DEVICE_START_UPDATE);

    while(!(100==UpgradeProgress))
    {
        fibo_taskSleep(500);

        if(g_EventFlag & DEF_EVENT_DEVICE_START_UPDATE)//按位与
        {
            COMMON_SetEvent(DEF_EVENT_DEVICE_UPDATING);
      
            xmodem_XmodemInit(); 

            //地址域
            DeviceUpdateStart.AddressField  = g_device_AddressField;
            //功能码
            DeviceUpdateStart.PDU_Func      = 0x10;
            //起始寄存器地址
            DeviceUpdateStart.StartRegAdd   = swap_endian(0x0438);
            //寄存器数量
            DeviceUpdateStart.RegNum        = swap_endian(0x0002);
            //字节计数
            DeviceUpdateStart.ByteCounter   = 0x04;
            //固件类型
            DeviceUpdateStart.FW_Type       = g_device_FW_type;
            //波特率
            DeviceUpdateStart.Baudrate      = g_device_baudrate;
            //校验码
            DeviceUpdateStart.Crc16         = crc16rtu((UINT8 *)&DeviceUpdateStart, sizeof(DeviceUpdateStart) - sizeof(DeviceUpdateStart.Crc16));

            i = 0;
            while(i++<10)
            {
                fibo_taskSleep(10);
                memset((UINT8 *)&DeviceUpdateRespond, 0, sizeof(DeviceUpdateRespond));
                write_len = uart_write(UART1,(uint8_t*)&DeviceUpdateStart, sizeof(DeviceUpdateStart));
                log_d("\r\nwrite_len is %d.\r\n",write_len);
                log_hex((uint8_t*)&DeviceUpdateStart, sizeof(DeviceUpdateStart));
               	fibo_taskSleep(1500);
	            log_d("\r\nuart1_recv_len is %d\r\n",uart1_recv_len); 
                log_hex((UINT8 *)uart1_recv_data, uart1_recv_len);
                upp_c = uart1_recv_data[0];
                memcpy(&DeviceUpdateRespond,uart1_recv_data,uart1_recv_len); 
                //保证升级失败后能重新升级
                log_hex((UINT8 *)uart1_recv_data, uart1_recv_len);
                if((0x10 == DeviceUpdateRespond.PDU_Func)||('C' == upp_c))
                {
                    COMMON_SetEvent(DEF_EVENT_RUN_XMODEM);//正常响应
                    log_d("\r\nDEF_EVENT_RUN_XMODEM.\r\n");
                    break;
                }
            }

            memset(uart1_recv_data, 0, sizeof(uart1_recv_data));              //接收清零
            uart1_recv_len = 0;
            COMMON_ResetEvent(DEF_EVENT_DEVICE_START_UPDATE);

            if(!(g_EventFlag & DEF_EVENT_RUN_XMODEM))
            {
                log_d("\r\n xmodem_outbyte(ETX).\r\n");
                xmodem_outbyte(ETX);//中断操作
                xmodem_outbyte(ETX);
                xmodem_outbyte(ETX);
                COMMON_ResetEvent(DEF_EVENT_DEVICE_UPDATING);
            }
        }
        
        if(g_EventFlag & DEF_EVENT_RUN_XMODEM)
        {
            log_d("Xmodem begin\r\n");
            log_d("g_device_firmware_real_size is %d\r\n",g_device_firmware_real_size);
            if(xmodem_1K_server(g_device_firmware_real_size) < 0)
            {
                log_d("Xmodem fail\r\n");
            }
            else
            {
                log_d("Xmodem success\r\n");
            }
            COMMON_ResetEvent(DEF_EVENT_DEVICE_UPDATING);
            COMMON_ResetEvent(DEF_EVENT_DEVICE_START_UPDATE);
            COMMON_ResetEvent(DEF_EVENT_RUN_XMODEM);
            memset(uart1_recv_data, 0, sizeof(uart1_recv_data));              //接收清零
            uart1_recv_len = 0;
            //串口波特率初始化
            break;
        }
    }
}

void device_update_task(void *param)
{	
	while(1)
	{
        log_d("\r\n%s()\r\n",__func__);
	    fibo_taskSleep(1000);//不能删除、给其他任务运行时间
		if(1 == g_device_update_flag)
		{
			DeviceUpdateTask();
		}
	}
}