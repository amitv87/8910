/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : Cordic.c
  *@notes   : 2018.01.09 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Modbus.h"
#include "Device.h"

static u8_t protocolCheck(void *load, void *optPoint);

static const ModbusGetCmd_t _0281[] = {
    {0x03, 0xC00D, 0xC00F},
    {0x03, 0xC100, 0xC10C},              
};

static const ModbusGetCmd_t _0283[] = {
    {0x03, 0xC00A, 0xC014},
    {0x03, 0xC100, 0xC11A},
    {0x03, 0xC200, 0xC21B},              
};

static const ModbusGetCmd_t proCmdTab[] = {
    {0x03,  0xC104, 0xC117},
};

static const ModbusGetCmdTab_t protocol  = PROTOCOL_TABER(proCmdTab, 0, 0);
static const ModbusGetCmdTab_t cordic1 = PROTOCOL_TABER(_0281, 0, 0x0281);
static const ModbusGetCmdTab_t cordic2 = PROTOCOL_TABER(_0283, 0, 0x0283);

const ModbusDeviceHead_t CordicDevice = {
    &UART_9600_N1,
    &proCmdTab[0],
    &protocol,
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
		*((CONVERT_TYPE)optPoint) = &cordic2;
	}
	else if (ret > 0)
	{
		*((CONVERT_TYPE)optPoint) = &cordic1;
	}
	else 
	{
		return 1;
	}

	return 0;
}


/******************************************************************************/

