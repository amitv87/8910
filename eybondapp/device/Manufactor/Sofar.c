/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : Sofar.c
  *@notes   : 2018.01.08 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Modbus.h"
#include "Device.h"

static u8_t protocolCheck(void *load, void *optPoint);

static const ModbusGetCmd_t sofar1[] = {
    {0x04, 0x2000, 0x200F},              
    {0x03, 0x0000, 0x0027},               
};

static const ModbusGetCmd_t sofar2[] = {
    {0x04, 0x2000, 0x200F},              
    {0x03, 0x0000, 0x002E},               
};

static const ModbusGetCmd_t proCmdTab[] = {
    {0x04,  0x2000, 0x200F},
};

static const ModbusGetCmdTab_t sofarProtocol  = PROTOCOL_TABER(proCmdTab, 0, 0);
static const ModbusGetCmdTab_t sofarProtocol1 = PROTOCOL_TABER(sofar1, 0, 0x02A0);
static const ModbusGetCmdTab_t sofarProtocol2 = PROTOCOL_TABER(sofar2, 0, 0x029F);

const ModbusDeviceHead_t SofarDevice = {
    &UART_9600_N1,
    &proCmdTab[0],
    &sofarProtocol,
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
		u8_t type = cmd->ack.payload[6];
		if (type == 'I' || type == 'J')
		{
			*((CONVERT_TYPE)optPoint) = &sofarProtocol2;
		}
		else
		{
			*((CONVERT_TYPE)optPoint) = &sofarProtocol1;
		}
				
		return 0;
	}

	return 1;
}


/******************************************************************************/

