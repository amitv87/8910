/**********************    COPYRIGHT 2014-2015, EYBOND    ************************ 
 * @File    : FlashFIFO.c
 * @Author  : Unarty
 * @Date    : 2017-04-08
 * @Brief   : 
 **********************************************************************************/
#include "FlashFIFO.h"
#include "FlashHard.h"

#define BLOCK_MAX_SIZE              (0xFFE)             //最大块大小        


#define NEXT_EDGE(val)              ((val + (Flash_pageSize()))&(~(Flash_pageSize()-1)))
#define CURRENT_EDGE(val)           (val&(~(Flash_pageSize()-1)))
#define CHECK_ESGE(val)             (val&(Flash_pageSize()-1))

typedef struct
{
    u16_t len;
    u16_t crc;
}Block_t;

static u32_t userSize(FlashFIFOHead_t *head);
static u32_t spaceSize(FlashFIFOHead_t *head);
static void frontOffset(FlashFIFOHead_t *head);
static void read(FlashFIFOHead_t *head, u8_t *data, u16_t len);
static void write(FlashFIFOHead_t *head, u8_t *data, u16_t len);
static void edge(FlashFIFOHead_t *head, u16_t len);
static u16_t getNextBlockSize(FlashFIFOHead_t *head);


/*******************************************************************************
 Brief    : 队列指针初始化
 Parameter: null
 return   : null
 Note     : 改变静态全局变量值
*******************************************************************************/
void FlashFIFO_init(FlashFIFOHead_t *head, u32_t startAddr, u32_t areaSize)
{
    FlashEquilibria_init(&head->frontAddr, startAddr, POINT_SAVE_AREA_SIZE, sizeof(head->front), &head->front);
    FlashEquilibria_init(&head->rearAddr, startAddr + POINT_SAVE_AREA_SIZE, POINT_SAVE_AREA_SIZE, sizeof(head->rear), &head->rear);
	head->addr = startAddr + POINT_SAVE_AREA_SIZE + POINT_SAVE_AREA_SIZE; 
	head->size = areaSize - POINT_SAVE_AREA_SIZE - POINT_SAVE_AREA_SIZE;
	
    if ((head->front > head->size)
        || (head->rear > head->size)
        )
    {
        FlashFIFO_clear(head);
    }
}

/*******************************************************************************
 Brief    : 初始化队列数据存储区
 Parameter: 
 return   : null
*******************************************************************************/
void FlashFIFO_clear(FlashFIFOHead_t *head)
{
	FlashEquilibria_clear(&head->frontAddr);
    FlashEquilibria_clear(&head->rearAddr);
	head->front = 0;
	head->rear = 0;	
}

/*******************************************************************************
 Brief    : Flash队列数据存储
 Parameter: data:	目标存储数据
 			len:	目标数据长度
 return   : null
*******************************************************************************/
void FlashFIFO_put(FlashFIFOHead_t *head,  Buffer_t *buf)
{	
    Block_t block;
    u16_t writeLen;
     
    if (buf == null || buf->lenght  > BLOCK_MAX_SIZE)
    {
        return ;
    }
    writeLen = (buf->lenght + sizeof(block));
    if (writeLen > spaceSize(head))
    {
        frontOffset(head);
    }
    block.len = buf->lenght;
    block.crc = ~buf->lenght;
    edge(head, writeLen);
	write(head, (u8_t*)&block, sizeof(block));
    write(head, buf->payload, buf->lenght);
    //FlashEquilibria_reliableWrite(&head->rearAddr, &head->rear);	
	FlashEquilibria_write(&head->rearAddr, &head->rear);
}

/*******************************************************************************
 Brief    : Flash队列数据获取 
 Parameter: data:	获取数据存储地址
 return   : 获取数据长度
*******************************************************************************/
u16_t FlashFIFO_get(FlashFIFOHead_t *head, Buffer_t *buf)
{	
    u16_t size = FlashFIFO_see(head, buf);
    
    FlashEquilibria_write(&head->frontAddr, &head->front);
 
    return size;
}

/*******************************************************************************
 Brief    : Flash队列数据获取 
 Parameter: data:	获取数据存储地址
 return   : 获取数据长度
*******************************************************************************/
u16_t FlashFIFO_see(FlashFIFOHead_t *head, Buffer_t *buf)
{
    u16_t size = getNextBlockSize(head);

    if ((size == 0) 
        || (buf == null) 
        || (buf->payload == null )
        || (size > buf->size))
    {
        goto END;
    }
    else if (size <= userSize(head))
    {
        buf->lenght = size;
        read(head, buf->payload, size);
    }
    else
    {
        head->front = head->rear;
        size = 0;
    }
END:    
    return size;
}

/*******************************************************************************
 Brief    : FlashFIFO 已用空间
 Parameter: null
 return   : null
*******************************************************************************/
static u32_t userSize(FlashFIFOHead_t *head)
{
    return ((head->front > head->rear)
            ? (head->size - head->front + head->rear)
            : (head->rear - head->front));         
}

/*******************************************************************************
 Brief    : FlashFIFO 空闲空间
 Parameter: null
 return   : null
*******************************************************************************/
static u32_t spaceSize(FlashFIFOHead_t *head)
{
    return ((head->front > head->rear)
            ? (head->front - head->rear - 1)
            : (head->size - head->rear + head->front - 1));           
}
/*******************************************************************************
 Brief    : Flash存储边界判断
 Parameter: null
 return   : null
*******************************************************************************/
static void edge(FlashFIFOHead_t *head, u16_t len)
{
    u32_t offset = (head->rear + len);
    
    if (offset > head->size)
    {
        offset = 0;
    }

    if ((CHECK_ESGE(head->rear) == 0) || (CURRENT_EDGE(offset) != CURRENT_EDGE(head->rear)))
    {
        Flash_earsePage(CURRENT_EDGE(offset + head->addr));
    }
}

/*******************************************************************************
 Brief    : Flash前指针偏移到下一页
 Parameter: null
 return   : null
*******************************************************************************/
static void frontOffset(FlashFIFOHead_t *head)
{
    head->front = NEXT_EDGE(head->front);
    
    if (head->front >= head->size)
    {
        head->front = 0;
    }
    FlashEquilibria_reliableWrite(&head->frontAddr, &head->front);
}

/*******************************************************************************
 Brief    : Flash存储边界判断
 Parameter: null
 return   : null
*******************************************************************************/
static void read(FlashFIFOHead_t *head, u8_t *data, u16_t len)
{
    u32_t size = head->size - head->front; 
    
    if (size <= len)
    {
        Flash_read(head->addr + head->front, size, data);
        head->front = len - size;
        Flash_read(head->addr , head->front, data + size);
    }
    else
    {
        Flash_read(head->addr + head->front, len, data);
        head->front += len;
    }    
}

/*******************************************************************************
 Brief    : Flash存储边界判断
 Parameter: null
 return   : null
*******************************************************************************/
static void write(FlashFIFOHead_t *head, u8_t *data, u16_t len)
{
   u32_t size = head->size - head->rear; 
    
    if (size <= len)
    {
        Flash_write(head->addr + head->rear, size, data);
        head->rear = len - size;
        Flash_write(head->addr , head->rear, data + size);
    }
    else
    {
        Flash_write(head->addr + head->rear, len, data);
        head->rear += len;
    }    
}

/*******************************************************************************
 Brief    : Flash存储边界判断
 Parameter: null
 return   : null
*******************************************************************************/
static u16_t getNextBlockSize(FlashFIFOHead_t *head)
{
    Block_t block;
    u32_t cnt = Flash_pageSize();
    u8_t *p = (u8_t*)&block;
    u8_t readSize = sizeof(block);
    
    while (cnt--)
    {
        if (userSize(head) > readSize)
        {
            read(head, p, readSize);
			
            if (0xFFFF == (block.len^block.crc))
            {
                goto END;
            }
            else if (readSize != 1)
            {
                p += 3;
                readSize = 1;
            }
            *(u32_t*)&block >>= 8;
        }
        else
        {
            break;
        }
    }
    head->front = head->rear;
    block.len = 0;
END:
    return block.len;
}

/******************************************************************************/

