/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : Suntrio.c
  *@notes   : 2018.06.04 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Modbus.h"


static const ModbusGetCmd_t Suntrio02A2[] = {
    {0x04, 0x0000, 57}
};
static const ModbusGetCmdTab_t Suntrio02A2Protocol = PROTOCOL_TABER(Suntrio02A2, 0, 0x02A2);
const ModbusDeviceHead_t Suntrio02A2Device = {
    &UART_9600_N1,
    &Suntrio02A2[0],
    &Suntrio02A2Protocol,
    null,
    null,
};
static const ModbusGetCmd_t Suntrio02A3[] = {
    {0x03, 0x8F00, 0x8F1C},
    {0x03, 0x0100, 0x013A}
};
static const ModbusGetCmdTab_t Suntrio02A3Protocol = PROTOCOL_TABER(Suntrio02A3, 0, 0x02A3);
const ModbusDeviceHead_t Suntrio02A3Device = {
	&UART_115200_N1,
	&Suntrio02A3[0],
	&Suntrio02A3Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t suntrio0292[] = {
    {0x03,  0x8F00,  0x8F1C},
	{0x03,  0x0100,  0x014A},
};
static const ModbusGetCmdTab_t suntrioProtocol = PROTOCOL_TABER(suntrio0292, 0, 0x0292);
const ModbusDeviceHead_t Suntrio0292Device = {
    &UART_9600_N1,
    &suntrio0292[0],
    &suntrioProtocol,
    null,
    null,
};

/******************************************************************************/

