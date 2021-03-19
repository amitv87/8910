/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : ChunHui.h
  *@notes   : 2018.10.13 CQG 
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Modbus.h"
#include "Device.h"

static u8_t protocolCheck(void *load, void *optPoint);

static const ModbusGetCmd_t chunhui0215[] = {
	{0x03,  0xC100,  0xC11A},
	{0x03,  0xC11F,  0xC136},
	{0x03,  0xC200,  0xC203},
	{0x03,  0xC21C,  0xC224},
	{0x03,  0xC000,  0xC002},
	{0x03,  0xC00A,  0xC01E},
	{0x03,  0xC020,  0xC023},
};

static const ModbusGetCmd_t chunhui0293[] = {
	{0x03,  0xC100,  0xC11A},
	{0x03,  0xC200,  0xC203},
	{0x03,  0xC21C,  0xC224},
	{0x03,  0xC000,  0xC002},
	{0x03,  0xC00A,  0xC01E},
	{0x03,  0xC020,  0xC023},
};

static const ModbusGetCmd_t chunhui0294[] = {
    {0x03,  0xC100,  0xC111},
	{0x03,  0xC001,  0xC002},
	{0x03,  0xC008,  0xC008},
	{0x03,  0xC00D,  0xC018},
};

static const ModbusGetCmd_t _0281[] = {
    {0x03, 0xC00D, 0xC00F},
    {0x03, 0xC100, 0xC10C},              
};

static const ModbusGetCmd_t _0283[] = {
    {0x03, 0xC00A ,0xC014},
    {0x03, 0xC100 ,0xC11A},
    {0x03, 0xC200 ,0xC21B},
};

static const ModbusGetCmd_t proCmdTab[] = {
    {0x03,  0xC012, 0xC012},
};

static const ModbusGetCmdTab_t protocol  = PROTOCOL_TABER(proCmdTab, 0, 0);
static const ModbusGetCmdTab_t cordic0281 = PROTOCOL_TABER(_0281, 0, 0x0281);
static const ModbusGetCmdTab_t chunhui0215Protocol = PROTOCOL_TABER(chunhui0215, 0, 0x0215);
static const ModbusGetCmdTab_t chunhui0293Protocol = PROTOCOL_TABER(chunhui0293, 0, 0x0293);
static const ModbusGetCmdTab_t chunhui0294Protocol = PROTOCOL_TABER(chunhui0294, 0, 0x0294);
static const ModbusGetCmdTab_t _0283Protocol = PROTOCOL_TABER(_0283, 0, 0x0283);
const ModbusDeviceHead_t ChunHuiDevice = {
    &UART_9600_N1,
    &proCmdTab[0],
    &protocol,
    protocolCheck,
    null,
};

const ModbusDeviceHead_t _0283Device = {
    &UART_9600_N1,
    &_0283[0],
    &_0283Protocol,
    null,
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
		u16_t type =  (cmd->ack.payload[3]<<8)|(cmd->ack.payload[4]);

		if (type <= 9 )
		{
			*((CONVERT_TYPE)optPoint) = &cordic0281;
		}
		else if (type < 100 )
		{
			*((CONVERT_TYPE)optPoint) = &chunhui0294Protocol;//chunhui0215Protocol;//chunhui0293Protocol;//
		}
		else if (type > 112)
		{
			*((CONVERT_TYPE)optPoint) = &chunhui0215Protocol;
		}
		else 
		{
			*((CONVERT_TYPE)optPoint) = &chunhui0293Protocol;
		}
	}

	return 0;
}

/******************************************************************************/

