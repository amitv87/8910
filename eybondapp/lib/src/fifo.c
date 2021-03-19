/***************************Copyright QYKJ   2015-06-18*************************
文	件：	fifo.c
说	明：	队列操作函数
修　改：	2015.06.18 Unarty establish
*******************************************************************************/
//#include "fifo.h"					//mike 20200805
//#include "stdlib/r_stdlib.h"		//mike 20200805
#include "typedef.h"				//mike 20200805

/*******************************************************************************
函 数 名：  fifo_init
功能说明：  队列初始化
参	  数：  fifo:  	队列头
            buf:  	数据存储首地址
            size:   数据存储区域长度
返 回 值：  初始化结果 0/1
*******************************************************************************/
void fifo_init(FIFO_t *fifo, void *buf, mcu_t size)
{
    ERRR((null == fifo)||(null == buf)||(0 == size), return);
    fifo->buf = buf;
    fifo->size = size;
    fifo->front = fifo->rear;
}

/*******************************************************************************
函 数 名：  fifo_reset
功能说明：  队列清空
参	  数：  fifo:  队列头
返 回 值：  无
*******************************************************************************/
void fifo_reset(FIFO_t *fifo)
{
    ERRR(fifo == null, return);
    fifo->front = fifo->rear;
}

/*******************************************************************************
函 数 名：  fifo_clearn
功能说明：  队列清理
参	  数：  fifo:  队列头
            len:   清理长度
返 回 值：  无
*******************************************************************************/
void fifo_clearn(FIFO_t *fifo, mcu_t len)
{
    ERRR(fifo == null, return);

    if (len >= fifo_userSpace(fifo))
    {
        fifo_reset(fifo);
    }
    else
    {
        fifo->front = (fifo->front + len) % fifo->size;
    }
}

/*******************************************************************************
函 数 名：  fifo_offset
功能说明：  队列头指针偏移
参	  数：  fifo:  队列头
            offset:   偏移位置
返 回 值：  无
*******************************************************************************/
void fifo_offset(FIFO_t *fifo, mcu_t offset)
{
    ERRR(fifo == null, return);

    fifo->front = offset % fifo->size;

}


/*******************************************************************************
函 数 名：  fifo_empty
功能说明：  判断队列是否为空
参	  数：  *fifo:  队列头
返 回 值：  0(队列为空)/1
*******************************************************************************/
mcu_t fifo_empty(FIFO_t *fifo)
{  
    return ((fifo->front == fifo->rear) ? 0 : 1);
}

/*******************************************************************************
函 数 名：  fifo_full
功能说明：  判断队列是否已满
参	  数：  *fifo:  队列头
返 回 值：  0(队列已满)/1
*******************************************************************************/
mcu_t fifo_full(FIFO_t *fifo)
{  
    return ((fifo->front == ((fifo->rear+1)%fifo->size)) ? 0 : 1);
}

/*******************************************************************************
函 数 名：  fifo_freeSpace
功能说明：  队列可用空间
参	  数：  *fifo:  队列头
返 回 值：  剩余空间大小(字符为单位)
*******************************************************************************/
mcu_t fifo_freeSpace(FIFO_t *fifo)
{
    return ((fifo->front + fifo->size  - fifo->rear - 1)%fifo->size);
}

/*******************************************************************************
函 数 名：  fifo_userSpace
功能说明：  队列已用空间
参	  数：  *fifo:  队列头
返 回 值：  已用空间大小(字符为单位)
*******************************************************************************/
mcu_t fifo_userSpace(FIFO_t *fifo)
{

	return ((fifo->rear + fifo->size - fifo->front)%fifo->size);
}

/*******************************************************************************
函 数 名：  fifo_puts
功能说明：  入队
参	  数：  *fifo:  队列头
            *data:  入队数据
            len:    数据长度
返 回 值：  入队结果(0失败)/(入队长度)
*******************************************************************************/
mcu_t fifo_puts(FIFO_t *fifo, void *data, mcu_t len)
{  
	mcu_t size;
    int i;
    
    i = len - fifo_freeSpace(fifo);
    
    if (i > 0)
    {
        fifo->front = (fifo->front + i)%fifo->size;
    }
    
	size = MIN(len, fifo->size - fifo->rear);
	r_memcpy(fifo->buf + fifo->rear, data, size);
	r_memcpy(fifo->buf, (u8_t*)data + size, len - size);

	fifo->rear = (fifo->rear + len)%fifo->size;

    return len;   
}

/*******************************************************************************
函 数 名：  fifo_gets
功能说明：  出队
参	  数：  *fifo:  队列头
            *data:  出队数据
            len:    出队数据长度
返 回 值：  出队结果 0/1
*******************************************************************************/
mcu_t fifo_gets(FIFO_t *fifo, void *data, mcu_t len)
{
	mcu_t size;

	len = MIN(len, fifo_userSpace(fifo));
	size = MIN(len, fifo->size - fifo->front);
	r_memcpy(data, fifo->buf + fifo->front, size);
	r_memcpy((u8_t*)data + size, fifo->buf, len - size);

	fifo->front = (fifo->front + len)%fifo->size;
 
    return len;   
}

/*******************************************************************************
函 数 名：  fifo_putc
功能说明：  入队一个字符
参	  数：  *fifo:  队列头
            data:   要入队的数据
返 回 值：  入队结果 0/1
*******************************************************************************/
mcu_t fifo_putc(FIFO_t *fifo, u8_t data)
{
    if (0 == fifo_full(fifo)) //判断队列中是否还有存储空间
    {
        fifo->front = (fifo->front + 1)%fifo->size;
    }
    
    fifo->buf[fifo->rear++] = data;

    fifo->rear %= fifo->size;

    return 0;   
}

/*******************************************************************************
函 数 名：  fifo_getc
功能说明：  出队一个字符
参	  数：  *fifo:  队列头
返 回 值：  出队内容
*******************************************************************************/
mcu_t fifo_getc(FIFO_t *fifo, u8_t *data)
{
    ERRR(0 == fifo_empty(fifo), return 1); //队列为空

    *data = fifo->buf[fifo->front++];
    fifo->front %= fifo->size;

    return 0;   
}

/*******************************************************************************
函 数 名：  fifo_dataGet
功能说明：  队列数据获取
参	  数：  *fifo:  队列头
            offset: 队列偏移位置， 
            len:    目标长度
返 回 值：  出队内容
*******************************************************************************/
void fifo_dataGet(FIFO_t *fifo, u8_t *trg, mcu_t offset, mcu_t len)
{  
    mcu_t size;
    ERRR(0 == fifo_empty(fifo), return); //队列为空   
    
    offset %= fifo->size;
	size = MIN(len, fifo->size - offset);
    
	r_memcpy(trg, fifo->buf + offset, size);
	r_memcpy(trg + size, fifo->buf, len - size);
}


/*******************************************************************************
函 数 名：  fifo_Find
功能说明：  队列数据查找
参	  数：  fifo:  		队列头
			findStart:	查找头
			findStop:	查找尾
            *data:  数据匹配内容
            len:    匹配数据长度
返 回 值：  队列匹配指针，查找到的位置 
*******************************************************************************/
mcu_t fifo_find(FIFO_t *fifo, mcu_t findStart, mcu_t findStop, const u8_t *cmpData, mcu_t cmpSize)
{
	mcu_t i = 0;
	
    findStart %= fifo->size;
	findStop %= fifo->size;
    
	while (findStart != findStop)
	{
		if (fifo->buf[findStart] == cmpData[i])	//数据内容相等
		{
			if (++i == cmpSize)	//匹配长度相等
			{
                i -= 1;
				return ((findStart + fifo->size - i)%fifo->size);
			}
		}
		else
		{
			i = 0;
		}
        
        findStart++;
		findStart %= fifo->size;
	}
	
	return (findStop);
}

/*******************************Copyright QYKJ*********************************/

