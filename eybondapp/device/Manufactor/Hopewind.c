/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : Sofar.c
  *@notes   : 2018.01.08 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Modbus.h"
#include "Device.h"

static u8_t protocolCheck(void *load, void *optPoint);

static const ModbusGetCmd_t _0237[] = {
	{03,   40500,  40570},
	{03,   40571,  40599},
	{03,   40600,  40650},
};

static const ModbusGetCmd_t _0231[] = {
	{03,   40500,  40570},
	{03,   40561,  40568},
	{03,   40571,  40599},
	{03,   40600,  40650},
};

static const ModbusGetCmd_t _0288[] = {
    {0x03,  40500,  40531},
	{0x03,  40532,  40570},
	{0x03,  40561,  40568},
	{0x03,  40571,  40575},
	{0x03,  40600,  40644},
};



static const ModbusGetCmd_t proCmdTab[] = {
    {0x03,  40641, 40642},
};

static const ModbusGetCmdTab_t Protocol  = PROTOCOL_TABER(proCmdTab, 0, 0);
static const ModbusGetCmdTab_t _0231Protocol = PROTOCOL_TABER(_0231, 1, 0x0231);
static const ModbusGetCmdTab_t _0237Protocol = PROTOCOL_TABER(_0237, 1, 0x0237);
static const ModbusGetCmdTab_t _0288Protocol = PROTOCOL_TABER(_0288, 0, 0x0288);
static const ModbusGetCmdTab_t _023AProtocol = PROTOCOL_TABER(_0237, 1, 0x023A);
static const ModbusGetCmdTab_t _023BProtocol = PROTOCOL_TABER(_0237, 1, 0x023B);
static const ModbusGetCmdTab_t _023CProtocol = PROTOCOL_TABER(_0237, 1, 0x023C);
static const ModbusGetCmdTab_t _023DProtocol = PROTOCOL_TABER(_0237, 1, 0x023D);
static const ModbusGetCmdTab_t _023EProtocol = PROTOCOL_TABER(_0237, 1, 0x023E);
static const ModbusGetCmdTab_t _023FProtocol = PROTOCOL_TABER(_0237, 1, 0x023F);
static const ModbusGetCmdTab_t _0239Protocol = PROTOCOL_TABER(_0237, 1, 0x0239);
static const ModbusGetCmdTab_t _0246Protocol = PROTOCOL_TABER(_0237, 1, 0x0246);
static const ModbusGetCmdTab_t _0247Protocol = PROTOCOL_TABER(_0237, 1, 0x0247);

const ModbusDeviceHead_t HopewindDevice = {
    &UART_9600_N1,
    &proCmdTab[0],
    &Protocol,
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
		u32_t ver = (cmd->ack.payload[3]<<8)|(cmd->ack.payload[4])|(cmd->ack.payload[5]<<24)|(cmd->ack.payload[6]<<16);
		if (ver == 0x00100000)
		{
			*((CONVERT_TYPE)optPoint) = &_0237Protocol;
		}
        else if (ver == 0x00200000)
        {
           *((CONVERT_TYPE)optPoint) = &_0239Protocol;
        }
        else if (ver == 0x00300000)
        {
            *((CONVERT_TYPE)optPoint) = &_023AProtocol;
        }
        else if (ver == 0x00400000)
        {
            *((CONVERT_TYPE)optPoint) = &_023BProtocol;
        }
        else if (ver == 0x00500000)
        {
            *((CONVERT_TYPE)optPoint) = &_023CProtocol;
        }
        else if (ver == 0x00600000)
        {
            *((CONVERT_TYPE)optPoint) = &_023DProtocol;
        }
        else if (ver == 0x00700000)
        {
            *((CONVERT_TYPE)optPoint) = &_023EProtocol;
        }
        else if (ver == 0x00800000)
        {
            *((CONVERT_TYPE)optPoint) = &_023FProtocol;
        }
        else if (ver == 0x00900000)
        {
            *((CONVERT_TYPE)optPoint) = &_0246Protocol;
        }
        else if (ver == 0x01000000)
        {
            *((CONVERT_TYPE)optPoint) = &_0247Protocol;
        }

        else if (ver == 0)
        {
            *((CONVERT_TYPE)optPoint) = &_0288Protocol;
        }
		else
		{
			*((CONVERT_TYPE)optPoint) = &_0231Protocol;
		}
				
		return 0;
	}

	return 1;
}



static const ModbusGetCmd_t _0257[] = {
    {0x03,  1,  2},
	{0x03,  8,  27},
	{0x03,  29,  39},
	{0x03,  205,  249},
	{0x03,  251,  263},
    {0x03,  270,  272},
};
static const ModbusGetCmdTab_t _0257Protocol = PROTOCOL_TABER(_0257, 1, 0x0257);
const ModbusDeviceHead_t _0257Device = {
    &UART_9600_N1,
    &_0257[0],
    &_0257Protocol,
    null,
    null,
};

/******************************************************************************/

