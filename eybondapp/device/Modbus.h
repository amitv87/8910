/**********************    COPYRIGHT 2014-2015, EYBOND    ************************ 
  * @File	 : Modbus.h
  * @Author  : ChenGQ
  * @Date	 : 2017-04-05
  * @Brief	 : 
**********************************************************************************/
#ifndef __MODBUS_H
#define __MODBUS_H


#include "eyblib_typedef.h"
#include "eyblib_list.h"

#define MODEBUS_MAX_LOAD        (240)
#define MODEBUS_CRC_SIZE		(2)

#define ENDIAN_BIG_LITTLE_16(val)		((((val)&0x00FF)<<8)|(((val)&0xFF00)>>8)) //?????
#define ENDIAN_BIG_LITTLE_32(val)		((((val)&0x000000FF)<<24)\
										|(((val)&0x0000FF00)<<8)\
										|(((val)&0x00FF0000)>>8)\
										|(((val)&0xFF000000)>>24))

#pragma pack(1)

typedef struct
{
    u8_t    addr;
    u8_t    fun;
	u16_t   reg;
	u16_t   num;
    u16_t   crc;
}ModbusFC0102_t;

typedef struct
{
    u8_t    addr;
    u8_t    fun;
	u16_t   reg;
	u16_t   num;
    u16_t   crc;
}ModbusFC0304_t;

typedef struct
{
	u8_t    addr;
	u8_t    code;
	u16_t 	reg;	//reg start addr
	u16_t   val;	//reg num
	u16_t   crc;//data
}ModbusFC06_t;

typedef struct
{
	u8_t    addr;
	u8_t    code;
	u16_t 	reg;	//reg start addr
	u16_t   num;	//reg num
	u8_t    count;	//data count
	u16_t   data[];//data
}ModbusFC16_t;

#pragma pack()

typedef struct
{
	u8_t fun;   	//function code
    u16_t start;   // start register address
    u16_t end;      // end register address
}ModbusGetCmd_t;

typedef struct
{
	u8_t fun;   //function code
    u16_t start;   // start register address
    u16_t end;      // end register address
    u16_t *data; 	// set date;
}ModbusSetCmd_t;

void Modbus_GetCmdAdd(ListHandler_t *list, u8_t addr, void *tab, u16_t tabSize);
int Modbus_CmdCheck(Buffer_t *send, Buffer_t *ack);
void ModbusCmd_0102(ListHandler_t *list, u8_t addr, u8_t fun, u16_t start, u16_t end);
void ModbusCmd_0304(ListHandler_t *list, u8_t addr, u8_t fun, u16_t start, u16_t end);
void Modbus_06SetCmd(ListHandler_t *list, u8_t addr, u16_t reg, u16_t val);
void Modbus_16SetCmd(ListHandler_t *list, u8_t addr, u16_t start, u16_t end, u8_t *data);

#endif //__MODBUS_H end
/******************************************************************************/

