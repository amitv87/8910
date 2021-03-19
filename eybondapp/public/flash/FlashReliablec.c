/**********************    COPYRIGHT 2014-2015,QIYI TEAM*********************** 
 * @File    : FlashReliable.c
 * @Author  : Unarty
 * @Date    : 2017-04-15
 * @Brief   : 
 ******************************************************************************/
#include "FlashReliablec.h"
#include "FlashHard.h"
#include "Debug.h"

#define HEAD_USER_SIZE          (sizeof(FlashRecrode_t) + 2)

static void getUserAera(FlashReliable_t *head);
static u8_t checkArea(FlashReliable_t *head);



/*******************************************************************************
 Brief    : 队列指针初始化
 Parameter: null
 return   : 0 OK  1, 分配大小有问题
 Note     : 改变静态全局变量值
*******************************************************************************/
u8_t FlashReliable_init(FlashReliable_t *head, u32_t addr, u32_t size)
{
    if ((addr & (Flash_pageSize() - 1)) || (size & (Flash_pageSize()*2 - 1)))
    {
        return 1;
		
    }

	*(u32_t*)&head->startAddr = addr;
	*(u32_t*)&head->areaSize = size>>1;

	getUserAera(head);
	
    return 0;
}

/*******************************************************************************
 Brief    : 队列指针初始化
 Parameter: null
return   : result , 0: success 1: save area init... 2: over area rang
 Note     : 改变静态全局变量值
*******************************************************************************/
int FlashReliable_read(FlashReliable_t *head, u16_t offset, Buffer_t *buf)
{
	if (0 != head->userAddr)
	{
        if (head->recrode.userSize >= (HEAD_USER_SIZE  + offset + buf->lenght))
        {
            Flash_read(head->userAddr + offset + sizeof(FlashRecrode_t), buf->lenght, buf->payload);
            return buf->lenght;
        }
        else 
        {
            return 0;
        }
	}
    else
    {
        return -1;
    }
}

/*******************************************************************************
 Brief    : 队列指针初始化
 Parameter: null
 return   : resutl : 0 : OK , or err
 Note     : 改变静态全局变量值
*******************************************************************************/
int FlashReliable_write(FlashReliable_t *head, u16_t offset, Buffer_t *buf)
{
	FlashRecrode_t *pR;
	u8_t *cache;
    u8_t *pData;
    u32_t writeAddr;
    u32_t userBufSize;
    int err = 0;
	u16_t crc;

    userBufSize = offset + buf->lenght + HEAD_USER_SIZE;
    
    if (userBufSize > head->areaSize)
    {
        err = -0x11;  // Data over boundary
        goto END;
    }
    else if (userBufSize < head->recrode.userSize)
    {
        userBufSize = head->recrode.userSize;
    }
	cache = (u8_t*)Flash_ramMalloc(userBufSize);
    if (cache == null)
    {
        err = 0x20; //memory apply fail
        goto END;
    }
    pR = (FlashRecrode_t*)cache;
    pData = (u8_t*)(pR+1);
    
//	Flash_ramset(cache, Flash_defaultVal(), userBufSize);
	if (0 != head->userAddr)
	{
		Flash_read(head->userAddr, head->recrode.userSize, cache);
		if (0 != Flash_ramCRC(cache, head->recrode.userSize))
		{
			err = -2; 	//Data read fail
			goto END;
		}		
		else if ( 0 == Flash_ramcmp(&pData[offset], buf->payload, buf->lenght)) //dest change data sampe
		{
			goto END;
		}
	}

	Flash_ramcpy(&pData[offset], buf->payload, buf->lenght);
    pR->userSize = userBufSize;
    pR->updateTime = head->recrode.updateTime + 1;	
    crc = Flash_ramCRC(cache, pR->userSize - 2); //CRC Add
    Flash_ramcpy(&pData[userBufSize - HEAD_USER_SIZE], &crc, sizeof(crc));
    writeAddr = (head->userAddr == head->startAddr) ? head->startAddr + head->areaSize : head->startAddr;
    Flash_earseArea(writeAddr, userBufSize);
	Flash_write(writeAddr, userBufSize, cache);
	Flash_read(writeAddr, pR->userSize, cache);
    
	if (0 != Flash_ramCRC(cache, userBufSize))
	{
		err = -1; //data Write err
		goto END;
	}
	
    Flash_ramcpy(&head->recrode, pR, sizeof(FlashRecrode_t));
	head->userAddr = writeAddr;

END:
	Flash_ramFree(cache);

	return err;
}

/*******************************************************************************
 Brief    : 获取有效数据区
 Parameter: null
 return   : null
 Note     : 改变静态全局变量值
*******************************************************************************/
static void getUserAera(FlashReliable_t *head)
{
    FlashRecrode_t recode;
    
	Flash_read(head->startAddr, sizeof(FlashRecrode_t), (u8_t*)&head->recrode);
    Flash_read(head->startAddr + head->areaSize, sizeof(FlashRecrode_t), (u8_t*)&recode);
	if ((((recode.updateTime - head->recrode.updateTime) > 2)
            ||((head->recrode.updateTime > recode.updateTime)&& ((head->recrode.updateTime - recode.updateTime) < 2)))
         && (head->recrode.userSize <= head->areaSize)
        )
	{
        head->userAddr = head->startAddr;
        if (0 == checkArea(head))
        {
           return;
        }
	}
    
    if (recode.userSize <= head->areaSize)
    {
        head->userAddr = head->startAddr + head->areaSize;
        Flash_ramcpy(&head->recrode, &recode, sizeof(FlashRecrode_t));
        if (0 == checkArea(head))
        {
           return;
        }
        Flash_read(head->startAddr, sizeof(FlashRecrode_t), (u8_t*)&head->recrode);
        if (head->recrode.userSize <= head->areaSize)
        {
            head->userAddr = head->startAddr;
            if (0 == checkArea(head))
            {
               return;
            }
        }
    }  
    
    head->userAddr = 0;     //No Fine user Data
    head->recrode.updateTime = 0;
    head->recrode.userSize = 0;
}

/*******************************************************************************
Brief    : Flash 记录区数据校验
 Parameter: null
 return   : null
 Note     : 
*******************************************************************************/
static u8_t checkArea(FlashReliable_t *head)
{
    u8_t err = 0;
    u8_t *cache;
    
    cache = (u8_t*)Flash_ramMalloc(head->recrode.userSize);
    Flash_read(head->userAddr, head->recrode.userSize, cache);
    if (0 != Flash_ramCRC(cache, head->recrode.userSize))
    {
        err = 1;
    }
    Flash_ramFree(cache);
    return err;
}
/******************************************************************************/

