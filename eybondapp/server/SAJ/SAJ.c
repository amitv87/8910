/**************Copyright(C)��2015-2026��Shenzhen Eybond Co., Ltd.***************
  *@brief   : SAJ Server connect.c 
  *@notes   : 2018.09.10 CGQ
*******************************************************************************/
#include "SAJ.h"
#include "typedef.h"
#include "Debug.h"
#include "memory.h"
#include "SysPara.h"
#include "list.h"
#include "device.h"
#include "r_stdlib.h"
#include "swap.h"
#include "log.h"


static const char sajServerAddr[] = "iot.suntech-power.net:5735:UDP";
static int heartbeatTime;
static ListHandler_t rcveList;

static void SAJ_init(void);
static void SAJ_run(u8_t status);
static void SAJ_process(void);
static u8_t SAJ_cmd(Buffer_t *buf, DataAck ch);
static ServerAddr_t * SAJ_Addr(void);
static void SAJ_close(void);

const CommonServer_t SAJ_API = {
    "SAJ",
    60,
    SAJ_init,
    null,
    SAJ_run,
    SAJ_process,
    SAJ_cmd,
    SAJ_Addr,
    SAJ_close,
    null
};

static void heartbeatSend(void);
static u8_t transAck(Device_t *dev);
static u8_t trans(SAJ_t *asp);

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
int SAJ_check(void)
{
    Buffer_t buf;
    int i = -1;

    SysPara_Get(3, &buf);

    if ((buf.payload != null) && (buf.lenght > 1))
    {
        i = r_strfind("ABC", (char*)buf.payload);
    }

    memory_release(buf.payload);

    return i;
}

static void SAJ_init(void)
{
    heartbeatTime = 50;
    list_init(&rcveList);
}

static void SAJ_run(u8_t status)
{
    if (status == 0 && ++heartbeatTime > 60)
    {
        heartbeatTime = 0;
        heartbeatSend();
    }
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
*******************************************************************************/
static void  SAJ_process(void)
{
    SAJ_t *asp = (SAJ_t *)list_nextData(&rcveList, null);
    
    if (null == asp)
    {
        return;
    }
    
    while (asp != null && asp->waitCnt != 0)
    {
		if (++asp->waitCnt > SAJ_WAIT_CNT)  //wait prcesso overtime
        {
			list_nodeDelete(&rcveList, asp);
            return;
        }
		
		asp = (SAJ_t *)list_nextData(&rcveList, asp);
    }
    
    if (asp != null)
    {		
        trans(asp);
        asp->waitCnt++;
        return;
    }    
    list_nodeDelete(&rcveList, asp);
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
*******************************************************************************/
static u8_t SAJ_cmd(Buffer_t *buf, DataAck ch)
{
    u8_t e;
    
    if (buf == null || buf->payload == null || buf->lenght <= 0 || ch == null)
    {
        e = 1;
    }
    else
    {
        SAJ_t *asp = list_nodeApply(buf->lenght + sizeof(SAJ_t));
        
        if (asp == null)
        {
            e = 3;
			log_save("SAJ memory apply fail!");
		}
        else
        {	
            asp->PDULen = buf->lenght;
            asp->waitCnt = 0;
            asp->ack = ch;
            r_memcpy(&asp->PDU, buf->payload, buf->lenght);
            e = 0;
            list_topInsert(&rcveList, asp);
        }
    }
    
    return e;
}

static ServerAddr_t * SAJ_Addr(void)
{
    ServerAddr_t *sajServer = ServerAdrrGet(SAJ_SERVER_ADDR);   
    if (sajServer == null)
    {
        Buffer_t buf;
        buf.lenght = sizeof(sajServerAddr);
        buf.payload = sajServerAddr;
        SysPara_Set(SAJ_SERVER_ADDR, &buf);
    }
    
    return sajServer;    
}

static void SAJ_close(void)
{
    list_delete(&rcveList);
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
*******************************************************************************/
static u8_t aspCmp(void *src, void *dest)
{
    return r_memcmp(src, dest, sizeof(SAJ_t));
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
*******************************************************************************/
static u8_t trans(SAJ_t *asp)
{
    Device_t *dev;
    DeviceCmd_t *cmd;
    
    dev = list_nodeApply(sizeof(Device_t));
    cmd = list_nodeApply(sizeof(DeviceCmd_t));
    
    cmd->waitTime = 1500;
    cmd->state = 0;
    cmd->ack.size = 0;
    cmd->ack.payload = null;
    cmd->cmd.size = asp->PDULen;
    cmd->cmd.lenght = asp->PDULen;
    cmd->cmd.payload = asp->PDU;
    
    dev->cfg = null;
    dev->callBack = transAck;
    dev->explain = asp;
	dev->type = DEVICE_TRANS;
    
    list_init(&dev->cmdList);
    list_bottomInsert(&dev->cmdList, cmd);
    Device_inset(dev);

    return 1;
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
*******************************************************************************/
static u8_t transAck(Device_t *dev)
{
    SAJ_t *asp;
    DeviceCmd_t *cmd;
    Buffer_t buf;
    
    asp = list_find(&rcveList, aspCmp, dev->explain);
    
    if (asp != null)
    {
        cmd = (DeviceCmd_t*)dev->cmdList.node->payload;
        
        buf.lenght = cmd->ack.lenght;
        buf.payload = memory_apply(buf.lenght);

        r_memcpy(buf.payload, cmd->ack.payload, cmd->ack.lenght);
        asp->ack(&buf);
        memory_release(buf.payload);
        list_nodeDelete(&rcveList, asp);
    }
    Device_remove(dev);

    return 0;
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void heartbeatSend(void)
{
    Buffer_t buf;
    
	SysPara_Get(58, &buf);
	if (buf.payload == null)
	{ 
        log_save("ASP get cpuID fail!");
	}
	else
	{
    	u8_t *pSend;

        buf.lenght = (buf.lenght + 1)>> 1;
        pSend = memory_apply(buf.lenght);
        Swap_charHex(pSend, (char*)buf.payload);
        memory_release(buf.payload);
        
        buf.payload = pSend;
        CommonServerDataSend(&buf);
        memory_release(buf.payload);
    }
}
/******************************************************************************/

