/**************Copyright(C)��2015-2026��Shenzhen Eybond Co., Ltd.***************
  *@brief   : Hanergy.c 
  *@notes   : 2017.06.03 CGQ ���� 
*******************************************************************************/
#include "Hanergy.h"
#include "typedef.h"
#include "Debug.h"
#include "memory.h"
#include "SysPara.h"
#include "list.h"
#include "device.h"
#include "modbus.h"
#include "Algorithm.h"
#include "ModbusDevice.h"
#include "Modbus.h"
#include "debug.h"
#include "r_stdlib.h"
#include "Clock.h"
#include "Swap.h"
#include "SysPara.h"
#include "FlashFIFO.h"
#include "FlashEquilibria.h"
#include "crc.h"
#include "FlashHard.h"
#include "log.h"

static ListHandler_t rcveList;
static u16_t serialNumber; 
static u16_t reportDataTime;
static u16_t heartbeatTime;
static int historySaveTime;

static FlashFIFOHead_t historyHead;


static const char hanergyServerAddr[] = "iot.hanergy.com:8081";
static void Hanergy_init(void);
static void Hanergy_run(u8_t status);
static void HSP_process(void);
static u8_t HSP_cmd(Buffer_t *buf, DataAck ch);
static ServerAddr_t *hanergy_Addr(void);
static void hanergy_close(void);
static void HanergyHistoryDataClean(void);
const CommonServer_t HanergyServer = {
    "Hanergy",
    60,
    Hanergy_init,
    null,
    Hanergy_run,
    HSP_process,
    HSP_cmd,
    hanergy_Addr,
    hanergy_close,
    HanergyHistoryDataClean
};


static void heartbeatSend(void);
static u8_t heartbeatAck(HSP_t *hsp);
static u8_t deviceDataSend(void);
static u8_t deviceDataAck(HSP_t *hsp);
static u8_t deviceHistoryDataSend(void);
static u8_t deviceHistoryDataAck(HSP_t *hsp);
static u8_t deviceParaGet(HSP_t *hsp);
static u8_t deviceSet(HSP_t *hsp);
static void historySave(void);

static const HanergyTab_t funTab[] = {
  {HANERGY_HEARTBEA, heartbeatAck},  
  {HANERGY_DEVICE_PARA, deviceParaGet},
  {HANERGY_DEVICE_DATA, deviceDataAck},
  {HANERGY_DEVICE_HISTORY_DATA, deviceHistoryDataAck}, 
  {HANERGY_DEVICE_SET, deviceSet}
};

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
int Hanergy_check(void)
{
    Buffer_t buf;
    int i = -1;

    SysPara_Get(3, &buf);

    if ((buf.payload != null) && (buf.lenght > 1))
    {
        i = r_strfind("87", (char*)buf.payload);
    }

    memory_release(buf.payload);
    
    return i;
}

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void Hanergy_init(void)
{   
    serialNumber = 0;
	reportDataTime = 60*5 - 15;
	heartbeatTime = 25;
	historySaveTime = 60*5;

	list_init(&rcveList);

	FlashFIFO_init(&historyHead, FLASH_HANERGY_HISTORY_ADDR, FLASH_HANERGY_HISTORY_SIZE);
}

static ServerAddr_t * hanergy_Addr(void)
{
    ServerAddr_t *hanergyServer = ServerAdrrGet(HANERGY_SERVER_ADDR);   

    if (hanergyServer == null)
    {
        Buffer_t buf;
        buf.lenght = sizeof(hanergyServerAddr);
        buf.payload = hanergyServerAddr;
        SysPara_Set(HANERGY_SERVER_ADDR, &buf);
    }
    
    return hanergyServer;    
}

static void hanergy_close(void)
{
    list_delete(&rcveList);
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void Hanergy_run(u8_t status)
{
    if (status == 0)
    {
        if (((reportDataTime++) > (60 *5))//* 6))
            &&  (deviceDataSend() == 1)
            )
        {
            reportDataTime = 60*5 - 8; 
        }
        else if (deviceHistoryDataSend() == 1)
        {
        }
        else if ((heartbeatTime++) > (30))
        {
            heartbeatTime = 22;
            heartbeatSend();
        }  
    }
    else if ((runTimeCheck(4, 19) == 0 )
        &&(0 >= (historySaveTime--))
        )
    {
        historySave();
    }
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
*******************************************************************************/
static u8_t HSP_cmd(Buffer_t *buf, DataAck ch)
{
    u8_t e;
    
    if (buf == null || buf->payload == null || ch == null)
    {
        e = 1;
    }
    else if (buf->lenght < sizeof(HanergyHeader_t))
    {
        e = 2;
    }
    else
    {
        HSP_t *esp = list_nodeApply(buf->lenght + sizeof(HSP_t) - sizeof(HanergyHeader_t));
        
        if (esp == null)
        {
            e = 3;
			log_save("HSP memory apply fail!");
		}
        else
        {	
            esp->PDULen = buf->lenght;
            esp->waitCnt = 0;
            esp->ack = ch;
            r_memcpy(&esp->head, buf->payload, buf->lenght);
            e = 0;
            list_topInsert(&rcveList, esp);
        }
    }
    
    return e;
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
*******************************************************************************/
static void HSP_process(void)
{
    HSP_t *hsp = (HSP_t *)list_nextData(&rcveList, null);
    
    if (null == hsp)
    {
        return;
    }
    
    while (hsp != null && hsp->waitCnt != 0)
    {
		if (++hsp->waitCnt > HSP_WAIT_CNT)  //wait prcesso overtime
        {
			list_nodeDelete(&rcveList, hsp);
            return;
        }
		
		hsp = (HSP_t *)list_nextData(&rcveList, hsp);
    }
    
    if (hsp != null)
    {		
        HanergyTab_t *exe = (HanergyTab_t*)ALG_binaryFind(hsp->head.func, (void*)funTab, (void*)(&funTab[sizeof(funTab)/sizeof(funTab[0])]), sizeof(funTab[0]));
        if (exe != null)
        {
            if (0 != exe->fun(hsp))
            {
                hsp->waitCnt++;
                return;
            }
        }
    }    
    list_nodeDelete(&rcveList, hsp);
}

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void heartbeatSend(void)
{
    typedef struct
    {
        u8_t pn[14];
    }send_t;
	
    Buffer_t buf;
    HanergyHeader_t *pSend = memory_apply(sizeof(HanergyHeader_t) + sizeof(send_t));
	send_t *send = (send_t*)(pSend+1);
    
    pSend->serial = ++serialNumber;
    pSend->code = HANERGY_DEVICE_CODE;
    pSend->msgLen = sizeof(send_t) + HANERGY_DEFAULT_LENGHT;
    pSend->addr = 0xFF;
    pSend->func = HANERGY_HEARTBEA;

	SysPara_Get(2, &buf);
	if (buf.payload == null || buf.lenght != 14)
	{ 
		APP_DEBUG("PN ERR: %d, %s\r\n", buf.lenght, buf.payload);
		buf.lenght = 14;
		r_memcpy(send, "EYBONDERR00000", 14);
		SysPara_auth();
		
	}
	else
	{
    	r_memcpy(send, buf.payload,  sizeof(send_t));
	}
    memory_release(buf.payload);
    Swap_bigSmallShort(&pSend->msgLen);
    Swap_bigSmallShort(&pSend->serial);
    
	buf.lenght = sizeof(HanergyHeader_t) + sizeof(send_t);
	buf.payload = pSend;
	CommonServerDataSend(&buf);
	memory_release(buf.payload);
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t heartbeatAck(HSP_t *hsp)
{
    typedef struct{
        u8_t year;
        u8_t month;
        u8_t day;
        u8_t hour;
        u8_t minute;
        u8_t second;
    }BeatAck_t;
    
    heartbeatTime = 0;

    return 0;
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t deviceHistoryDataSend(void)
{
    typedef struct
    {
        u8_t pn[14];
        u8_t ccid[20];
        u8_t state;
        u8_t regType;
    }send_t;
    
    typedef struct 
    {
        u16_t serial;   //Serial number
        u16_t code;     //device code  
        u16_t msgLen;    //meseger lenght
        u8_t  addr;      //device addresss
        u8_t  func;      //funcation code
        u8_t  state;
        u8_t  time[6];
        u8_t  regType;
    }historySave_t;
    
    
    u8_t result  = 0;
   
    HanergyHeader_t *pSend = memory_apply(sizeof(HanergyHeader_t) + sizeof(send_t) + 1024);
    historySave_t *historyData = (historySave_t*)(((u8_t*)pSend) 
                                                    + (sizeof(HanergyHeader_t) 
                                                       + sizeof(send_t) 
                                                       - sizeof(historySave_t)));
    Buffer_t getBuf;
    getBuf.size = 1024;
    getBuf.payload = (u8_t*)historyData;
    if (FlashFIFO_get(&historyHead, &getBuf) != 0)
    {
        Buffer_t buf;
        send_t *sendPara = (send_t*)(pSend+1);
        u16_t msgLen = historyData->msgLen;
        Swap_bigSmallShort(&msgLen);
        msgLen = msgLen + sizeof(send_t) - sizeof(historySave_t);
        
        pSend->serial = ++serialNumber;
        pSend->code = HANERGY_DEVICE_CODE;
        pSend->msgLen = msgLen + HANERGY_DEFAULT_LENGHT;
        pSend->addr = historyData->addr;
        pSend->func = HANERGY_DEVICE_HISTORY_DATA;
		
        SysPara_Get(2, &buf);
		r_memcpy(sendPara->pn, buf.payload, 14);
		memory_release(buf.payload);
		SysPara_Get(56, &buf);
		r_memcpy(sendPara->ccid, buf.payload, buf.lenght);
		memory_release(buf.payload);
        sendPara->state = 0x01;
        sendPara->regType = historyData->regType;
        Swap_bigSmallShort(&pSend->msgLen);
        Swap_bigSmallShort(&pSend->serial);
		buf.lenght = msgLen + 6;
		buf.payload = (u8_t*)pSend;
        CommonServerDataSend(&buf);
        result = 1;
	}

    memory_release(pSend);
    
    return result;
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t deviceHistoryDataAck(HSP_t *hsp)
{
    // typedef struct{
    //     u8_t state;
    // }Ack_t;
    
    // Ack_t *pAck = (Ack_t*)hsp->PDU;
    

    return 0;
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t deviceDataSend(void)
{
    typedef struct
    {
        u8_t pn[14];
        u8_t ccid[20];
        u8_t state;
        u8_t regType;
    }send_t;
	
	HanergyHeader_t *sendHead;
	send_t *sendPara;
	u8_t *ackBuf;
	Node_t *tail;
	Buffer_t buf;
	u8_t result = 0;
	
	buf.size = 1024;
	buf.payload = memory_apply(buf.size);
	sendHead = (HanergyHeader_t*)buf.payload;
	sendPara = (send_t *)(sendHead+1);
	ackBuf = (u8_t*)(sendPara+1);
	tail = onlineDeviceList.node;

	if (tail != null && buf.payload != null)
	{
		DeviceOnlineHead_t *head;
		Node_t *node;
		
		do {
			head = (DeviceOnlineHead_t*)tail->payload;
			node = head->cmdList.node;
			tail = tail->next;
			
			if (node != null)
			{
				CmdBuf_t *cmd;
				Buffer_t paraBuf;
				u8_t cmdLen = 0;
				u8_t *endAddrH = null;
				u8_t *endAddrL = null;
                u8_t code = 0;

				do {
                    cmd = (CmdBuf_t*)node->payload;
                    node = node->next;
					if (code != cmd->fun)
                    {
                        if (code == 0)
                        {
                            sendPara->state = 0x01;
				            sendPara->regType = 0x00;
                        }
                        else
                        {
                            sendPara->state = 0x01;
				            sendPara->regType = 0x02;
                        }
                        code = cmd->fun;
                    }
                    if (((head->flag&0x01) == 0 || ((cmdLen>>4) == (head->flag>>1)))
							&& endAddrH != null && ((((*endAddrH<<8) | *endAddrL) + 1) == cmd->startAddr))
                    {
                        *endAddrH = cmd->endAddr>>8;
						*endAddrL = cmd->endAddr;
                    }
                    else
                    {
						u16_t regAddr;
						
						regAddr = ENDIAN_BIG_LITTLE_16(cmd->startAddr);
                        r_memcpy(ackBuf, &regAddr, 2);
						ackBuf += 2;
						regAddr = ENDIAN_BIG_LITTLE_16(cmd->endAddr);
						r_memcpy(ackBuf, &regAddr, 2);
                        endAddrH = ackBuf++;
						endAddrL = ackBuf++;
                    }
					cmdLen = (cmd->endAddr - cmd->startAddr + 1)<<1; // User modbus 01 02 code
                    r_memcpy(ackBuf, cmd->buf.payload, cmd->buf.lenght);
					r_memset(&ackBuf[cmd->buf.lenght], 0, cmdLen - cmd->buf.lenght);
                    ackBuf += cmdLen;
				} while (node != head->cmdList.node && head->cmdList.node != null);
				
				sendHead->serial = ENDIAN_BIG_LITTLE_16(++serialNumber);
				sendHead->code = HANERGY_DEVICE_CODE;
				sendHead->msgLen = ENDIAN_BIG_LITTLE_16((int)ackBuf - (int)sendPara + 2);
				sendHead->addr = head->addr;
				sendHead->func = HANERGY_DEVICE_DATA;
				SysPara_Get(2, &paraBuf);
				r_memcpy(sendPara->pn, paraBuf.payload, paraBuf.lenght);
				memory_release(paraBuf.payload);
				SysPara_Get(56, &paraBuf);
				r_memcpy(sendPara->ccid, paraBuf.payload, paraBuf.lenght);
				memory_release(paraBuf.payload);
                // buf.payload[48] = 0x05;
                // buf.payload[49] = 0x30;
				buf.lenght = ackBuf - buf.payload;
                APP_DEBUG("hanergy update data: %d", buf.lenght);
				CommonServerDataSend(&buf); 
			}
            result = 1;
		}while (tail != onlineDeviceList.node && onlineDeviceList.node != null);
		
	}
	
    memory_release(buf.payload);
    
    return result;
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t deviceDataAck(HSP_t *hsp)
{
    typedef struct{
        u8_t state;
    }ack_t;
    

    reportDataTime = 0;
    historySaveTime = 60*5 + 15;
    /*if (pAck->state == 0x01)
    {
         log_save("Hanergy data parsing error!!!");
    }//*/

    return 0;
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t deviceParaGet(HSP_t *hsp)
{
    u8_t i;
    u16_t len;
	Buffer_t *paraBufTab;
    Buffer_t *ackBufTab;
    Buffer_t buf;
    u8_t *ackBuf;
    HanergyHeader_t *ackHead;
    
    u8_t *para = hsp->PDU;
    
    i = hsp->PDULen - sizeof(HanergyHeader_t);
    
    ackBufTab = memory_apply(i * sizeof(Buffer_t) + 8);
    ackBuf = (u8_t*)ackBufTab;
	paraBufTab = ackBufTab;
    len = 0;
    
    while (i--)
    {
		buf.lenght = 0;
		buf.payload = null;
		
		SysPara_Get(*para, &buf);
        len += buf.lenght;
        r_memcpy(ackBuf, &buf, sizeof(Buffer_t));
        ackBuf += sizeof(Buffer_t);
		para++;
    }
    
    i = hsp->PDULen - sizeof(HanergyHeader_t);
	
    buf.lenght = i*2 + len + (i*sizeof(HanergyHeader_t));
    buf.payload = memory_apply(buf.lenght + 20);
    ackBuf = buf.payload;
    para = hsp->PDU;
    
    
    while (i--)
    {  
        ackHead = (HanergyHeader_t*)ackBuf;
        r_memcpy(ackHead, &hsp->head, sizeof(HanergyHeader_t));
        ackBuf += sizeof(HanergyHeader_t);
        if (ackBufTab->lenght > 0 && ackBufTab->payload != null)
        {
            *ackBuf++ = 0;
            *ackBuf++ = *para++;
			
            r_memcpy(ackBuf, ackBufTab->payload, ackBufTab->lenght);
            ackBuf += ackBufTab->lenght;
        }
        else
        {
            *ackBuf++ = 1;
            *ackBuf++ = *para++;
			ackBufTab->lenght = 0;
        }
		//byte alignment/*
		*(((u8*)ackHead) + 4) = (ackBufTab->lenght + 4) >> 8;
		*(((u8*)ackHead) + 5) = (ackBufTab->lenght + 4); //*/

		//ackHead->msgLen = ENDIAN_BIG_LITTLE_16(ackBufTab->lenght + 4);
        memory_release(ackBufTab->payload);
        ackBufTab++; 
    }
    hsp->ack(&buf); 
	memory_release(paraBufTab);
    memory_release(buf.payload);
    
    return 0;
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t hspCmp(void *src, void *dest)
{
	return r_memcmp(src, dest, sizeof(HSP_t));
}

static u8_t deviceSetAck(Device_t *dev)
{
	typedef struct{
        u16_t start;
        u16_t end;
        u8_t data[];
    }rcve_t;
	
	typedef struct{
        u16_t start;
        u16_t end;
        u8_t state;
    }ack_t;

	HSP_t *hsp;
	Buffer_t buf;
	HanergyHeader_t *ackHead;
	DeviceCmd_t *cmd;
	ack_t *ack;
	rcve_t *rcve;
	
	hsp = list_find(&rcveList, hspCmp, dev->explain);
	
	if (hsp != null)
	{
		cmd = (DeviceCmd_t*)dev->cmdList.node->payload;
		
		buf.lenght = sizeof(ack_t) + sizeof(HanergyHeader_t);
		buf.payload = memory_apply(buf.lenght);
		rcve = (rcve_t*)hsp->PDU;
		ackHead = (HanergyHeader_t*)buf.payload;
		ack = (ack_t*)(ackHead+1);
		Ql_memcpy(ackHead, &hsp->head, sizeof(HanergyHeader_t));
		Ql_memcpy(ack, rcve, 4);
		ack->state = Modbus_CmdCheck(&cmd->cmd, &cmd->ack);
		ackHead->msgLen = ENDIAN_BIG_LITTLE_16(sizeof(ack_t) + 2);
		hsp->ack(&buf);
		memory_release(buf.payload);
		list_nodeDelete(&rcveList, hsp);
	}
	Device_remove(dev);

    return 0;
}

static u8_t deviceSet(HSP_t *hsp)
{
    typedef struct{
        u16_t start;
        u16_t end;
        u8_t data[];
    }rcve_t;
    
	Device_t *dev;
    DeviceCmd_t *cmd;
    rcve_t *rcve = hsp->PDU;

	dev = list_nodeApply(sizeof(Device_t));
	
	dev->cfg = null;
    dev->callBack = deviceSetAck;
    dev->explain = hsp;
	dev->type = DEVICE_MOBUS;

	list_init(&dev->cmdList);
	if (rcve->start == rcve->end)
	{
		Modbus_06SetCmd(&dev->cmdList, hsp->head.addr, rcve->start, *(u16_t*)rcve->data);
	}
	else
	{
		Modbus_16SetCmd(&dev->cmdList, hsp->head.addr, rcve->start, rcve->end, rcve->data);
	}
    Device_inset(dev);
	
    return 1;
}


/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
*******************************************************************************/
static void historySave(void)
{
#pragma pack(1)
    typedef struct
    {
		HanergyHeader_t head;
		u8_t ret;
        u8_t year;
        u8_t month;
        u8_t day;
        u8_t hour;
        u8_t min;
        u8_t secs;
    }save_t;
#pragma pack() 

    Buffer_t buf;
	u16_t regAddr;
    u8_t *endAddrH = null;
	u8_t *endAddrL = null;
    save_t *save = null;
	Node_t *tail;
    DeviceOnlineHead_t *head;
	Clock_t *time;

	if (onlineDeviceList.node == null)
	{
		return;
	}
	log_save("Hanergy history save!");
	buf.lenght = 0;
    buf.size = 2048;
    buf.payload = memory_apply(buf.size);
    tail = onlineDeviceList.node;
    save = (save_t*)buf.payload;
	
    if (buf.payload != null)
    {
		historySaveTime = 60*5;
		time = Clock_get();
		save->year = time->year - 2000;
		save->month = time->month;
		save->day = time->day;
		save->hour = time->hour;
		save->min = time->min;
		save->secs = time->secs;
        do {
            Node_t *node;
            
            head = (DeviceOnlineHead_t*)tail->payload;
            node = head->cmdList.node;
            tail = tail->next;
            if (node != null)
            {
                CmdBuf_t *cmd;
                u8_t *para;
				u8_t cmdLen;
                u8_t code = 0;
				u8_t *pCode = null;
                
                para = (u8*)(save+1);
                do {
                    cmd = (CmdBuf_t*)node->payload;
                    node = node->next;
                    if (code != cmd->fun)
                    {   
						code = cmd->fun;
                        if (pCode == null)
                        {
							pCode = para;
                            *para++ = 0x02;
                        }
						else
						{
							*pCode = 0x00;
						}
                    }
                    if (((head->flag&0x01) == 0 || ((cmdLen>>4) == (head->flag>1)))
							&& endAddrH != null && ((((*endAddrH<<8) | *endAddrL) + 1) == cmd->startAddr))
                    {
                        *endAddrH = cmd->endAddr>>8;
						*endAddrL = cmd->endAddr;
                    }
                    else
                    {
						regAddr = ENDIAN_BIG_LITTLE_16(cmd->startAddr);
                        r_memcpy(para, &regAddr, 2);
						para += 2;
						regAddr = ENDIAN_BIG_LITTLE_16(cmd->endAddr);
						r_memcpy(para, &regAddr, 2);
                        endAddrH = para++;
						endAddrL = para++;
                    }
					cmdLen = (cmd->endAddr - cmd->startAddr + 1)<<1; // User modbus 01 02 code
                    r_memcpy(para, cmd->buf.payload, cmd->buf.lenght);
					r_memset(&para[cmd->buf.lenght], 0, cmdLen - cmd->buf.lenght);
                    para += cmdLen;
                } while (node != head->cmdList.node && head->cmdList.node != null);
				
				buf.lenght = para - buf.payload;
                save->head.addr = head->addr;
				save->head.code = ENDIAN_BIG_LITTLE_16(head->code);
                save->head.msgLen = ENDIAN_BIG_LITTLE_16(buf.lenght - sizeof(HanergyHeader_t) + 2);
				regAddr = crc16_standard(CRC_RTU, buf.payload, buf.lenght);
				*para++ = regAddr;
				*para++ = regAddr>>8;
				buf.lenght += 2;
                FlashFIFO_put(&historyHead, &buf);
            } 
        }while (tail != onlineDeviceList.node && onlineDeviceList.node != null);
	}

    memory_release(buf.payload);
}
/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
*******************************************************************************/
static void HanergyHistoryDataClean(void)
{
	FlashFIFO_clear(&historyHead);
}

/******************************************************************************/

