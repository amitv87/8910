 /**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : Growatt.c
  *@notes   : 2017.12.26 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Device.h"


const ModbusGetCmd_t cp[] = {
    {04,   0,  45},
    {04,  60,  67},
};

const ModbusGetCmd_t _0213[] = {
    {03,   2,  44},
    {04,   0,  58},
};

const ModbusGetCmd_t ssb[] = {
    {03,  0xC008,  0xC01A},
    {03,  0xC100,  0xC10D},
};

const ModbusGetCmd_t _0268[] = {
    {0x03, 0 ,44},
    {0x04, 0 ,44},
    {0x04, 45 ,65},
    {0x04, 70 ,89},
    {0x04, 115 ,129},
};

static const ModbusGetCmd_t growatt[] = {
    {0x03,  0, 44},
    {0x04,  0, 44},
    {0x04, 45, 65},
};

static const ModbusGetCmd_t proCmdTab[] = {
    {04,       0,   44},
    {03,  0xC008,  0xC01A},
    {03,  	   2,   45},
};

static const ModbusGetCmdTab_t protocolCmd = PROTOCOL_TABER(proCmdTab, 0, 0);
static const ModbusGetCmdTab_t growattProtocol = PROTOCOL_TABER(growatt, 0|(1<<0), 0x0290);
static const ModbusGetCmdTab_t ssbProtocol = PROTOCOL_TABER(ssb, 0, 0x02D1);
static const ModbusGetCmdTab_t cpProtocol = PROTOCOL_TABER(cp, 0, 0x02D9);
static const ModbusGetCmdTab_t _0213Protocol = PROTOCOL_TABER(_0213, 0, 0x0213);
static const ModbusGetCmdTab_t _0268Protocol = PROTOCOL_TABER(_0268, 1, 0x0268);

static u8_t protocolCheck(void *load, void *optPoint);

const ModbusDeviceHead_t GrowattDevice = {
    &UART_9600_N1,
    &growatt[0],
    &growattProtocol,
//    &protocolCmd,
//    protocolCheck,
    null,
    null,
};

const ModbusDeviceHead_t _02D9Device = {
    &UART_9600_N1,
    &cp[0],
    &cpProtocol,
    null,
    null,
};

const ModbusDeviceHead_t _02D1Device = {
    &UART_9600_N1,
    &ssb[0],
    &ssbProtocol,
    null,
    null,
};

const ModbusDeviceHead_t _0213Device = {
    &UART_9600_N1,
    &_0213[0],
    &_0213Protocol,
    null,
    null,
};

const ModbusDeviceHead_t _0268Device = {
    &UART_9600_N1,
    &_0268[0],
    &_0268Protocol,
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
		switch (cmd->cmd.payload[3])
		{
			case 0:
				*((CONVERT_TYPE)optPoint) = &growattProtocol;
				return 0;
			case 8:
				*((CONVERT_TYPE)optPoint) = &ssbProtocol;
				return 0;
			case 2:
				*((CONVERT_TYPE)optPoint) = &cpProtocol;
				return 0;
			default:
				break;
		}
	}

	return 1;
}


/******************************************************************************/

