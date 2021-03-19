#include "common_data.h"

/*----------------------------------------------------------------------------*
**                             Global Vars                                    *
**----------------------------------------------------------------------------*/
UINT32 g_EventFlag = 0;

/*****************************************************************************
 函数名  : COMMON_DataInit
 函数功能: 对公共数据进行初始化
 输入    : NULL
 输出    : NULL
 返回    : NULL
 时间    : 2017-11-22
*****************************************************************************/
void COMMON_DataInit(void)
{
    g_EventFlag = 0;
}

void COMMON_SetEvent(UINT32 CommonEvent)
{
    g_EventFlag |= CommonEvent;
}

void COMMON_ResetEvent(UINT32 CommonEvent)
{
    g_EventFlag &= (~CommonEvent);
}