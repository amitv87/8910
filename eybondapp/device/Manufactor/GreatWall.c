/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : GreatWall.c
  *@notes   : 2018.03.07 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Device.h"



static const ModbusGetCmd_t greatwall[] = {
	{0x04, 30010, 30024},
    {0x04, 30033, 30050},
    {0x04, 30051, 30090},
    {0x04, 30091, 30130},
    {0x04, 30131, 30139},
    {0x04, 30281, 30292},
    {0x04, 30302, 30305},
};
static const ModbusGetCmdTab_t greatwallProtocol = PROTOCOL_TABER(greatwall, 0, 0x02AE);
const ModbusDeviceHead_t GreatWallDevice = {
  &UART_9600_N1,
  &greatwall[0],
  &greatwallProtocol,
  null,
  null,
};

static const ModbusGetCmd_t greatwall_old[] = {
	{0x03,  9, 23},
    {0x03, 32, 49},
    {0x03, 50, 89},
    {0x03, 90, 129},
    {0x03, 130, 138},
    {0x03, 280, 291},
    {0x03, 301, 304},
};
static const ModbusGetCmdTab_t greatWallOldProtocol = PROTOCOL_TABER(greatwall_old, (1<<0), 0x02B0);
const ModbusDeviceHead_t GreatWallOldDevice = {
  &UART_9600_N1,
  &greatwall_old[0],
  &greatWallOldProtocol,
  null,
  null,
};

static const ModbusGetCmd_t greatwall_EQMX1[] = {
	{0x04, 1008, 1063},
    {0x04, 1070, 1078},
};
static const ModbusGetCmdTab_t greatWallEQMX1Protocol = PROTOCOL_TABER(greatwall_EQMX1, 0, 0x02CD);
const ModbusDeviceHead_t GreatWallEQMX1Device = {
  &UART_9600_N1,
  &greatwall_EQMX1[0],
  &greatWallEQMX1Protocol,
  null,
  null,
};

static const ModbusGetCmd_t greatwall_EQMX2[] = {
	{0x04, 1008, 1016},
    {0x04, 1020, 1022},
	{0x04, 1032, 1036},
	{0x04, 1039, 1056},
	{0x04, 1061, 1063},
	{0x04, 1070, 1078},
};
static const ModbusGetCmdTab_t greatWallEQMX2Protocol = PROTOCOL_TABER(greatwall_EQMX2, 0, 0x02CE);
const ModbusDeviceHead_t GreatWallEQMX2Device = {
  &UART_9600_N1,
  &greatwall_EQMX2[0],
  &greatWallEQMX2Protocol,
  null,
  null,
};

static const ModbusGetCmd_t greatwall_EQMX3[] = {
	{0x04, 1000, 1111},
};
static const ModbusGetCmdTab_t greatWallEQMX3Protocol = PROTOCOL_TABER(greatwall_EQMX3, 0, 0x02D2);
const ModbusDeviceHead_t GreatWallEQMX3Device = {
  &UART_9600_N1,
  &greatwall_EQMX3[0],
  &greatWallEQMX3Protocol,
  null,
  null,
};

static const ModbusGetCmd_t greatwall2[] = {
	{0x03, 30010, 30012},
    {0x03, 30020, 30021},
    {0x03, 30052, 30058},
    {0x03, 30070, 30075},
    {0x03, 36001, 36012},
    {0x03, 36061, 36072},
};
static const ModbusGetCmdTab_t greatwall2Protocol = PROTOCOL_TABER(greatwall2, 0, 0x0508);
const ModbusDeviceHead_t GreatWall2Device = {
  &UART_9600_N1,
  &greatwall2[0],
  &greatwall2Protocol,
  null,
  null,
};

static const ModbusGetCmd_t greatwall3[] = {
	{0x04,    1,    9},
    {0x04,   11,   69},
    {0x04, 2101, 2123},
    {0x04, 2221, 2235},
};
static const ModbusGetCmdTab_t greatwall3Protocol = PROTOCOL_TABER(greatwall3, 0, 0x0702);
const ModbusDeviceHead_t GreatWall3Device = {
  &UART_9600_N1,
  &greatwall3[0],
  &greatwall3Protocol,
  null,
  null,
};




/******************************************************************************/

