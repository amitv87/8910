#ifndef XMODEM_H
#define XMODEM_H

#include "fibo_opencpu_comm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 设置开始更新数据包 */
#pragma pack(1)
typedef  struct
{
    UINT8   AddressField;   //地址域
    UINT8   PDU_Func;       //功能码
    UINT16  StartRegAdd;    //起始寄存器地址
    UINT16  RegNum;         //寄存器数量
    UINT8   ByteCounter;    //字节计数
    UINT16  FW_Type;        //固件类型
    UINT16  Baudrate;       //波特率
    UINT16  Crc16;          //校验码
}DEVICE_UPDATE_START;
#pragma pack()

extern DEVICE_UPDATE_START DeviceUpdateStart; //设备开始更新命令格式

UINT8 xmodem_TransProgress(void);
void  xmodem_ExcelXmodemUpdateState(void);
void  DeviceUpdateTask(void);
void device_update_task(void *param);
#ifdef __cplusplus
}
#endif

#endif

/******************************************************************************
 End of the File (EOF). Do not put anything after this part!
******************************************************************************/