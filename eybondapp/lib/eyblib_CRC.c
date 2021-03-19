/***************************Copyright QYKJ   2015-06-18*************************
  *@brief   : CRC校验函数
  *@notes   : 2016.10.28 CGQ 创建   
*******************************************************************************/
#include "eyblib_CRC.h"

/*******************************************************************************
  * @brief  IEEE 802.15.4 FCS field 16-bit ITU-T CRC
  * @note   CITT CRC16 polynomial ^16 + ^12 + ^5 + 1
  * @param  data: traget buffer data addr
            len: traget data lenght
  * @retval cal result 
*******************************************************************************/
unsigned short crc16_ITU_T(const unsigned char *data, int len)
{
    unsigned short acc = 0;
    
    while (len-- > 0)
    {
        acc ^= *data++;
        acc  = (acc >> 8) | (acc << 8);
        acc ^= (acc & 0xff00) << 4;
        acc ^= (acc >> 8) >> 4;
        acc ^= (acc & 0xff00) >> 5;

        /*
        acc  = (unsigned char)(acc >> 8) | (acc << 8);
        acc ^= *data++;
        acc ^= (unsigned char)(acc & 0xff) >> 4;
        acc ^= (acc << 8) << 4;
        acc ^= ((acc & 0xff) << 4) << 1;
        */
    }
    
    return acc;
}

/*******************************************************************************
  * @brief  16位CRC标准校验
  * @note   0x31国际标准校验
  * @param  sta: 校验标准
            data: 校验内容
            len:  校验长度
  * @retval 校验值
*******************************************************************************/
unsigned short crc16_standard(CRCStandard_e std, const unsigned char *data, int len)
{    
    unsigned short i, uwCrcReg = 0xFFFF;

    while (len--)
    {
        uwCrcReg ^= *data++;
        for (i = 0; i < 8; i++)
        {
            uwCrcReg = (uwCrcReg &0x01) == 0 ? (uwCrcReg >> 1) : ((uwCrcReg >> 1) ^ std);
        }
    }
    return (uwCrcReg);
}	

/******************************************************************************/

