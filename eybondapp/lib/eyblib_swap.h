/**********************    COPYRIGHT 2014-2100,  Eybond ************************ 
 * @File    : eyblib_swap.h
 * @Author  : CGQ
 * @Date    : 2015-06-28
 * @Brief   : 
 ******************************************************************************/
#ifndef __EYBLIB_SWAP_H_
#define __EYBLIB_SWAP_H_

#include "eyblib_typedef.h"
#include "eyblib_list.h"

void  Swap_bigSmallShort(u16_t *pShort);	  	//0xaa55 -> 0x55aa
void  Swap_bigSmallLong(u32_t *pLong);   		//0x12345678 -> 0x87654321
void  Swap_headTail(u8_t *head, u16_t len);	//0xaabbccddeeffgg -> 0xggffeeddccbbaa
u16_t Swap_hexChar(char *buf, u8_t *hex, u16_t len, char fill);	//0x12,0x34,0x56,0x78,0xab,0x0c -> "12 34 56 78 ab 0c"
u16_t Swap_charHex(u8_t *hex, char *buf);		//"12 34 56 87 ab 0c" -> 0x12,0x34,0x56,0x78,0xab,0x0c
u8_t  Swap_numChar(char *buf, u32_t num);		//0x0400 -> "1024"
u32_t Swap_charNum(char *buf);           		//"1024" -> 0x0400;
u16_t Swap_BCDChar(char *buf, u8_t *bcd, u16_t len);		//0x123456 -> "123456"
u16_t Swap_CharBCD(u8_t *bcd, char *buf);		//"98765" -> 0x098765
u16_t Swap_numBCD(u8_t *bcd, u64_t num);		//0x0100 -> 0x0256
u64_t Swap_BCDNum(u8_t *bcd, u16_t len);		//0x1024 -> 0x0400
u8_t  Swap_numHexChar(char *buf, u16_t num);
u32_t Swap_HexCharNum(char *buf);
s32_t Swap_stringSigned(char *buf);
u8_t  Swap_signedString(char *buf, s32_t num);
void  Swap_numCharFixed(char *buf, u8_t bufLen, u64_t num);
u8_t  Swap_CharString(char *buf, s8_t num) ;
int r_strsplit(ListHandler_t *subStr , char *str, char ch);
void hextostr(void *hexbuff, void * strtarget, u32_t len);

#endif //__EYBLIB_SWAP_H_
/*******************************Copyright Eybond*********************************/
							
