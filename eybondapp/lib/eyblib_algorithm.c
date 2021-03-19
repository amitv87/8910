/***************************Copyright eybond 2015-06-18*************************
文	件：	eyblib_aLgorithm.c
说	明：	算法实现函数
修　改：	2015.06.23 Unarty establish
        2015.06.27 Unarty Add 分差查找子函数 BinaryFind
*******************************************************************************/
#include "eyblib_algorithm.h"

static mcu_t BinaryFind(mcu_t start, mcu_t end, u16_t size);

/*******************************************************************************
  函数名:  	ALG_binaryFind
功能说明:  	二叉分查找算法
    参数:  	arm:		查找的目标
    		srcStart:	查找源起始地址
    		srcEnd:		查找源的结束地址
    		onceSize:	单个源的数据空间大小	
  返回值:  	匹配到的数据存储地址
*******************************************************************************/
void *ALG_binaryFind(u16_t arm, void *srcStart, void *srcEnd, u16_t onceSize)
{
    u8_t *cmp = (u8_t*)(BinaryFind((mcu_t)srcStart, (mcu_t)srcEnd, onceSize));

	while ((void*)cmp < srcEnd)
	{
        if (arm > *(u16_t*)cmp)
        {
            srcStart = cmp + onceSize;
            cmp = (u8_t*)(BinaryFind((mcu_t)srcStart, (mcu_t)srcEnd, onceSize));;
        }
        else if (arm < *(u16_t*)cmp)
        {
            srcEnd = cmp;
            cmp = (u8_t*)(BinaryFind((mcu_t)srcStart, (mcu_t)srcEnd, onceSize));;
        }
        else 
        {
            return cmp;
        }
	}

    return null;
}


/*******************************************************************************
  函数名:  	BinaryFind
功能说明:  	二叉分定位
    参数:  	start:	查找源起始地址
    		end:	查找源的结束地址
    		size:	单个源的数据空间大小	
  返回值:  	偏移地址值; 单位(byte)
*******************************************************************************/
static mcu_t BinaryFind(mcu_t start, mcu_t end, u16_t size)
{
    mcu_t i = (end - start)>>1;     // n/2

	i = i - i%size + start;

    return i;
}

/*******************************Copyright QYKJ*********************************/

