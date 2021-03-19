/**********************    COPYRIGHT 2014-2100, QIYI    ************************ 
 * @File    : FlashEquilibria.c
 * @Author  : Unarty
 * @Date    : 2017-04-23
 * @Brief   : 
 ******************************************************************************/
#include "FlashEquilibria.h"
#include "FlashHard.h"
#include "Debug.h"

typedef struct 
{
	u32_t 	allotAddr;	//allot address
	u32_t   allotSize;	//allot size
	u16_t 	size;		//struct size
	u16_t	addr;    //next Write addr
}FlashEquilibriaOpt_t;  //

#define ADDR_VAL(addr)				(*(u16_t*)&(addr))

static void userValueFind(FlashEquilibria_t *head, void *val);


/*******************************************************************************
 Brief    : flash epuilibraia init
 Parameter: head:	brief head
 			areaAddr:	area address
 			areaSize:	area size
 			userSize:	single struct data sizeof
 return   : val:	current save value
*******************************************************************************/
void FlashEquilibria_init(FlashEquilibria_t *head, mcu_t areaAddr, 
									mcu_t areaSize, mcu_t userSize, void *val)
{
	FlashEquilibriaOpt_t *opt = (FlashEquilibriaOpt_t*)head;

	opt->allotAddr = areaAddr;
	opt->allotSize = areaSize;
	opt->size = userSize;	
	opt->addr = 0;
	
    userValueFind(head, val);
}

/*******************************************************************************
 Brief    : flash epuilibraia clear
 Parameter: head:	brief head
 return   : 
*******************************************************************************/
void FlashEquilibria_clear(FlashEquilibria_t *head)
{
	mcu_t i = head->allotAddr&(Flash_pageSize()-1);
	ADDR_VAL(head->addr) = 0;
	
	if (i != 0)
	{
		 u8_t *buf = Flash_ramMalloc(i);
		 u32_t addr = head->allotAddr&(~(Flash_pageSize()-1));
		 Flash_read(addr, i, buf);
		 Flash_earseArea(addr, head->allotSize);
		 Flash_write(addr, i, buf);
         Flash_ramFree(buf);
	}
	else
	{
		Flash_earseArea(head->allotAddr, head->allotSize);
	}
}


/*******************************************************************************
 Brief    : Flash epuilibria write
 Parameter: head: brief head
 			val:  user value
 return   : null
*******************************************************************************/
void FlashEquilibria_write(FlashEquilibria_t *head, void const * const val)
{
	if (head->addr >= (head->allotSize)) //记录区已越界
	{
		FlashEquilibria_clear(head);
	}

	Flash_write(head->addr + head->allotAddr, head->size, (u8_t*)val);

    ADDR_VAL(head->addr) += head->size;
}  


/*******************************************************************************
 Brief    : Flash epuilibria reliable write
 Parameter: head: brief head
 			val:  user value
 return   : null
*******************************************************************************/
void FlashEquilibria_reliableWrite(FlashEquilibria_t *head, void const * const val)
{
	//u8_t *buffer = FLASH_RAM_MALLOC(head->size);  //注意分配
	u8_t buffer[8];
    
	FlashEquilibria_read(head, buffer); 
	
	while (0 != Flash_ramcmp(buffer, val, head->size))
	{	
		if (head->addr >= (head->allotSize)) //记录区已越界
		{
  			FlashEquilibria_clear(head);
		}

		Flash_write(head->addr + head->allotAddr, head->size, (u8_t*)val);
		userValueFind(head, buffer);
        if ((0 != Flash_ramcmp(buffer, val, head->size)))  //SPI接口会出错，解决这个问题。
        {
            ADDR_VAL(head->addr) = 0;
            userValueFind(head, buffer);
        }
	}	
} 

/*******************************************************************************
 Brief    : Flash epuilibria write
 Parameter: head: brief head
 			val:  user value
 return   : null
*******************************************************************************/
void FlashEquilibria_read(FlashEquilibria_t *head, void *val)
{
    if (head->addr == 0)
    {
        Flash_ramset(val, 0, head->size);
    }
    else
    {
        Flash_read(head->addr + head->allotAddr - head->size, head->size, val);
    }
}

/*******************************************************************************
 Brief    : user value find in the head falsh area
 Parameter: head: brief head
 			val:	user value
 return   : null
*******************************************************************************/
static void userValueFind(FlashEquilibria_t *head, void *val)
{	
   	mcu_t i;
	u8_t *buffer = Flash_ramMalloc(head->allotSize);

	Flash_read(head->allotAddr, head->allotSize, buffer);
	Flash_ramset(val, Flash_defaultVal(), head->size);
	for (i = head->addr; i < head->allotSize; i += head->size)
	{
		if (0 == Flash_ramcmp(&buffer[i], val, head->size))
		{
			if (0 == i) 
			{
				Flash_ramset(val, 0, head->size);
				goto SET_ADDR;
			}
			break;
		}
		
	}

	Flash_ramcpy(val, &buffer[i - head->size], head->size);
	
SET_ADDR:
	ADDR_VAL(head->addr) = i;
	Flash_ramFree(buffer);
}

/******************************************************************************/
