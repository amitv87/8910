#ifndef __EYBOND_MODBUS_TCP_PROTOCOL_H__
#define __EYBOND_MODBUS_TCP_PROTOCOL_H__
#include "net_task.h"

extern char g_RecvData[2*1024];
extern char g_SendData[2*1024];

extern UINT16 g_RecvDataLen;
extern UINT16 g_SendDataLen;

void receiving_processing(UINT8 function_code);
BOOL  data_frame_legal_checking(UINT8 a[]);

#endif // __EYBOND_MODBUS_TCP_PROTOCOL_H__