/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : SunGrow.c
  *@notes   : 2018.04.24 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Modbus.h"


static const ModbusGetCmd_t sungrow029E[] = {
    {0x04, 4999 ,5071},
};
static const ModbusGetCmdTab_t sungrow029EProtocol = PROTOCOL_TABER(sungrow029E, 0, 0x029E);
const ModbusDeviceHead_t Sungrow029EDevice = {
    &UART_9600_N1,
    &sungrow029E[0],
    &sungrow029EProtocol,
    null,
    null,
};

static const ModbusGetCmd_t sungrow02AD[] = {
	{0x04, 4989, 5048},
	{0x04, 5080, 5081},
	{0x04, 5112, 5115},
};
static const ModbusGetCmdTab_t sungrow02ADProtocol = PROTOCOL_TABER(sungrow02AD, 0, 0x02AD);
const ModbusDeviceHead_t Sungrow02ADDevice = {
  &UART_9600_N1,
  &sungrow02AD[0],
  &sungrow02ADProtocol,
  null,
  null,
};

static const ModbusGetCmd_t sungrow02D5[] = {
	{0x04, 15069, 15118},
};
static const ModbusGetCmdTab_t sungrow02D5Protocol1 = PROTOCOL_TABER(sungrow02D5, 0, 0x02D5);

const ModbusDeviceHead_t Sungrow02D5Device = {
  &UART_9600_N1,
  &sungrow02D5[0],
  &sungrow02D5Protocol1,
  null,
  null,
};

static const ModbusGetCmd_t sungrow0287[] = {
  {0x04, 4949, 4982},
	{0x04, 4989, 5048},
	{0x04, 5080, 5081},
	{0x04, 5112, 5117},
  {0x04, 5145, 5147},
	{0x04, 7012, 7029},
};
static const ModbusGetCmdTab_t sungrow0287Protocol = PROTOCOL_TABER(sungrow0287, 0, 0x0287);
const ModbusDeviceHead_t Sungrow0287Device = {
  &UART_9600_N1,
  &sungrow0287[0],
  &sungrow0287Protocol,
  null,
  null,
};

static const ModbusGetCmd_t sungrow022A[] = {
  {0x04, 4949, 4982},
	{0x04, 4989, 5048},
	{0x04, 5080, 5081},
	{0x04, 5112, 5117},
  {0x04, 5145, 5147},
	{0x04, 7012, 7029},
};
static const ModbusGetCmdTab_t sungrow022AProtocol = PROTOCOL_TABER(sungrow022A, 0, 0x022A);
const ModbusDeviceHead_t Sungrow022ADevice = {
  &UART_9600_N1,
  &sungrow022A[0],
  &sungrow022AProtocol,
  null,
  null,
};


/******************************************************************************/

