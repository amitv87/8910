/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : InnovPower.c
  *@notes   : 2018.03.17 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Device.h"

static const ModbusGetCmd_t innovpower[] = {
	{0x03, 0x1000, 0x100D},
    {0x03, 0x006A, 0x0071},
};

static const ModbusGetCmd_t innovpower250K[] = {
	{0x03, 0xA000, 0xA00D},
    {0x03, 0x0000, 0x0021},
};

static const ModbusGetCmd_t protocolCmd[] = {
	{0x03, 0x1000, 0x100D},
	{0x03, 0x0000, 0x0021},
};

static u8_t protocolCheck(void *load, void *optPoint);
static const ModbusGetCmdTab_t protocol = PROTOCOL_TABER(protocolCmd, 0, 0);
static const ModbusGetCmdTab_t innovpowerProtocol = PROTOCOL_TABER(innovpower, 0, 0x02AF);
static const ModbusGetCmdTab_t innovpower250KProtocol = PROTOCOL_TABER(innovpower250K, 0, 0x029D);
const ModbusDeviceHead_t InnovpowerAFDevice = {
  &UART_9600_N1,
  &innovpower[0],
  &innovpowerProtocol,
  null,
  null,
};

const ModbusDeviceHead_t Innovpower9DDevice = {
  &UART_9600_N1,
  &innovpower250K[0],
  &innovpower250KProtocol,
  null,
  null,
};


const ModbusDeviceHead_t InnovpowerDevice = {
  &UART_9600_N1,
  &innovpower250K[0],
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
		switch (cmd->cmd.payload[2])
		{
			case 0x00:
				*((CONVERT_TYPE)optPoint) = &innovpower250KProtocol;
				return 0;
			case 0x10:
				*((CONVERT_TYPE)optPoint) = &innovpowerProtocol;
				return 0;
			default:
				break;
		}
	}

	return 1;
}



/******************************************************************************/

