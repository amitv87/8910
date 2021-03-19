/***************************Copyright eybond 2015-2100*************************
  *@brief   : CRC
  *@notes   : 2016.10.28 CGQ
*******************************************************************************/
#ifndef  __EYBLIB_CRC_H_
#define  __EYBLIB_CRC_H_

typedef enum
{
    CRC_USA     = 0x8005,
    CRC_CCITI   = 0x1021,
    CRC_RTU     = 0xA001,     
}CRCStandard_e;

unsigned short crc16_standard(CRCStandard_e std, const unsigned char *data, int len);

#endif  //__EYBLIB_CRC_H_
/******************************************************************************/

