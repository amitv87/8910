/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : ATOAI.c
  *@notes   : 2018.07.04 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Modbus.h"
#include "Device.h"


static const ModbusGetCmd_t atoai[] = {
    {0x03, 0, 35}
};

static const ModbusGetCmd_t protocol[] = {
    {0x03, 32, 32}
};

static u8_t protocolCheck(void *load, void *optPoint);

static const ModbusGetCmdTab_t atoaiProtocol = PROTOCOL_TABER(protocol, 0, 0);
static const ModbusGetCmdTab_t atoai02c7Protocol = PROTOCOL_TABER(atoai, 0, 0x02C7);
static const ModbusGetCmdTab_t atoai02c8Protocol = PROTOCOL_TABER(atoai, 0, 0x02C8);
static const ModbusGetCmdTab_t atoai02c9Protocol = PROTOCOL_TABER(atoai, 0, 0x02C9);

const ModbusDeviceHead_t AtoaiDevice = {
    &UART_9600_N1,
    &protocol[0],
    &atoaiProtocol,
    protocolCheck,
    null,
};

static u8_t protocolCheck(void *load, void *optPoint)
{
	int ret;
	DeviceCmd_t *cmd;

	cmd = (DeviceCmd_t*)load;

	ret = Modbus_CmdCheck(&cmd->cmd, &cmd->ack);
	if (ret == 0)
	{
		u16_t reg = (cmd->cmd.payload[2]<<8)|(cmd->cmd.payload[3]);

        if ((reg > 12000 && reg <= 13000)
                || (reg > 14000 && reg <= 15000)
            )
        {
            *((CONVERT_TYPE)optPoint) = &atoai02c9Protocol;
        }
        else if ((reg > 15000 && reg <= 16000)
                )
        {
            *((CONVERT_TYPE)optPoint) = &atoai02c8Protocol;
        }
        else 
        {
            *((CONVERT_TYPE)optPoint) = &atoai02c7Protocol;
        }
        return 0;
    }

	return 1;
}


/******************************************************************************/

