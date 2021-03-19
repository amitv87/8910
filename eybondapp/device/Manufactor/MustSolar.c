/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : MustSolar.c
  *@notes   : 2018.01.09 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Modbus.h"
#include "Device.h"

static const ModbusGetCmd_t must02BB_02C1[] = {
   {0x03, 20001 ,20008},
   {0x03, 20106 ,20114},
   {0x03, 20133 ,20141},
   {0x03, 20161 ,20214},
   {0x03, 25201 ,25215},
   {0x03, 25226 ,25233},
   {0x03, 25245 ,25266},
   {0x03, 25267 ,25269},
   {0x03, 25329 ,25332},
   {0x03, 10101 ,10112},
   {0x03, 15205 ,15234},             
};

static const ModbusGetCmd_t _02BB_02C1Pro[] = {
	{0x03, 25267, 25267},
};

static u8_t protocolCheck(void *load, void *optPoint);
static const ModbusGetCmdTab_t _02BB_02C1ProFind = PROTOCOL_TABER(_02BB_02C1Pro, 0, 0);
static const ModbusGetCmdTab_t mustProtocol1 = PROTOCOL_TABER(must02BB_02C1, 0, 0x02BB);
static const ModbusGetCmdTab_t mustProtocol2 = PROTOCOL_TABER(must02BB_02C1, 0, 0x02C1);
const ModbusDeviceHead_t MustSolarDevice1 = {
    &UART_19200_N1,
    &_02BB_02C1Pro[0],
    &_02BB_02C1ProFind,
    protocolCheck,
    null,
};

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static u8_t protocolCheck(void *load, void *optPoint)
{
	int ret;
	DeviceCmd_t *cmd;

	cmd = (DeviceCmd_t*)load;

	ret = Modbus_CmdCheck(&cmd->cmd, &cmd->ack);
	if (ret == 0)
	{
		u16_t type = (cmd->ack.payload[3]<<8)|(cmd->ack.payload[4]);
		type /= 1000;
		type %= 10;
		if (type == 6)
		{
			*((CONVERT_TYPE)optPoint) = &mustProtocol1;
		}
		else
		{
			*((CONVERT_TYPE)optPoint) = &mustProtocol2;
		}
				
		return 0;
	}

	return 1;
}


static const ModbusGetCmd_t must02B9[] = {
    {0x03, 20001, 20016},
    {0x03, 20101, 20132},
    {0x03, 20142, 20143},
    {0x03, 20213, 20214},
    {0x03, 25201, 25274},
    {0x03, 10001, 10008},
    {0x03, 10101, 10112},
    {0x03, 15201, 15221},              
};

static const ModbusGetCmd_t _02B9Pro[] = {
	{0x03, 20005, 20005},
};

static u8_t protocol3Check(void *load, void *optPoint);
static const ModbusGetCmdTab_t _02B9ProFind = PROTOCOL_TABER(_02B9Pro, 0, 0);
static const ModbusGetCmdTab_t mustProtocol3 = PROTOCOL_TABER(must02B9, 0, 0x02B9);
static const ModbusGetCmdTab_t musttab[] = {
    {3, 1, 0x02B9, (ModbusGetCmd_t*)must02B9},
    {5, 0, 0x02B9, (ModbusGetCmd_t*)&must02B9[3]}
};
static const ModbusGetCmdTab_t mustProtocol4 = {
	sizeof(musttab)/sizeof(musttab[0]), (1<<7), 0x02B9, (ModbusGetCmd_t*)musttab
};
const ModbusDeviceHead_t MustSolarDevice2 = {
    &UART_19200_N1,
    &must02B9[0],
    &_02B9ProFind,
    protocol3Check,
    null,
};

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static u8_t protocol3Check(void *load, void *optPoint)
{
	int ret;
	DeviceCmd_t *cmd;

	cmd = (DeviceCmd_t*)load;

	ret = Modbus_CmdCheck(&cmd->cmd, &cmd->ack);
	if (ret == 0)
	{
		u16_t type = (cmd->ack.payload[3]<<8)|(cmd->ack.payload[4]);
		type /= 1000;
		type %= 10;
		if (type < 5)
		{
			*((CONVERT_TYPE)optPoint) = &mustProtocol4;
		}
		else
		{
			*((CONVERT_TYPE)optPoint) = &mustProtocol3;
		}
				
		return 0;
	}

	return 1;
}


static const ModbusGetCmd_t must0802[] = {
	{0x03, 15001 ,15008},
	{0x03, 15111 ,15122},
	{0x03, 15201 ,15221},            
};
static const ModbusGetCmdTab_t MustPro0802 = PROTOCOL_TABER(must0802, 0, 0x0802);
const ModbusDeviceHead_t MustSolarDevice3 = {
    &UART_19200_N1,
    &must0802[0],
    &MustPro0802,
    null,
    null,
};

static const ModbusGetCmd_t must0902[] = {
	{0x03, 20001 ,20016},
    {0x03, 20099 ,20132},
    {0x03, 25201 ,25274},
    {0x03, 15001 ,15008},
    {0x03, 15111 ,15122},
    {0x03, 15201 ,15221},       
};
static const ModbusGetCmdTab_t MustPro0902 = PROTOCOL_TABER(must0902, 0, 0x0902);
const ModbusDeviceHead_t MustSolarDevice4 = {
    &UART_19200_N1,
    &must0902[0],
    &MustPro0902,
    null,
    null,
};

static const ModbusGetCmd_t must0903[] = {
	{0x03, 20001 ,20008},
    {0x03, 20106 ,20114},
    {0x03, 20133 ,20140},
    {0x03, 20161 ,20214},
    {0x03, 25201 ,25215},
    {0x03, 25217 ,25217},
    {0x03, 25221 ,25221},
    {0x03, 25226 ,25233},
    {0x03, 25245 ,25266},
    {0x03, 25329 ,25332},
    {0x03, 10101 ,10112},
    {0x03, 15206 ,15209},
    {0x03, 16222 ,16231},     
};
static const ModbusGetCmdTab_t MustPro0903 = PROTOCOL_TABER(must0903, 0, 0x0903);
const ModbusDeviceHead_t MustSolarDevice5 = {
    &UART_19200_N1,
    &must0903[0],
    &MustPro0903,
    null,
    null,
};


/******************************************************************************/

