/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : Suntrio.c
  *@notes   : 2018.06.04 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Modbus.h"
#include "Device.h"


static const ModbusGetCmd_t _0c01[] = {
    {0x03, 0, 31},
    {0x03, 32, 63},
    {0x03, 64, 70},
};

static const ModbusGetCmd_t _0c02[] = {
    {0x03, 0, 20},
    {0x03, 71, 100},
    {0x03, 101, 120},
};

static const ModbusGetCmd_t _0c03[] = {
    {0x03, 0, 20},
    {0x03, 121, 150},
    {0x03, 151, 180},
    {0x03, 181, 210},
    {0x03, 211, 250},
};
 
static const ModbusGetCmd_t _0c04[] = {
    {0x03, 0, 20},
    {0x03, 251, 280},
    {0x03, 281, 300},
};

static const ModbusGetCmd_t proCmdTab[] = {
    {03,       0,   0},
};

static u8_t protocolCheck(void *load, void *optPoint);

static const ModbusGetCmdTab_t protocolCmd = PROTOCOL_TABER(proCmdTab, 0, 0);
static const ModbusGetCmdTab_t _0c01Protocol = PROTOCOL_TABER(_0c01, 0, 0x0c01);
static const ModbusGetCmdTab_t _0c02Protocol = PROTOCOL_TABER(_0c02, 0, 0x0c02);
static const ModbusGetCmdTab_t _0c03Protocol = PROTOCOL_TABER(_0c03, 0, 0x0c03);
static const ModbusGetCmdTab_t _0c04Protocol = PROTOCOL_TABER(_0c04, 0, 0x0c04);
const ModbusDeviceHead_t EybondSmartMeter = {
    &UART_9600_N1,
    &proCmdTab[0],
    &protocolCmd,
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
        u16_t val = cmd->ack.payload[3]<<8 | cmd->ack.payload[4];
		switch (val)
		{
			case 0x0401:
                *((CONVERT_TYPE)optPoint) = &_0c01Protocol;
				return 0;
			case 0x0402:
                *((CONVERT_TYPE)optPoint) = &_0c02Protocol;
				return 0;
            case 0x0403:
                *((CONVERT_TYPE)optPoint) = &_0c03Protocol;
				return 0;
            case 0x0404:
                *((CONVERT_TYPE)optPoint) = &_0c04Protocol;
				return 0;
			default:
                
				break;
		}
	}

	return 1;
}

/******************************************************************************/

