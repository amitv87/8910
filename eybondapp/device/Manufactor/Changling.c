/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : moso.c
  *@notes   : 2017.12.26 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "eyblib_memory.h"
#include "eyblib_r_stdlib.h"
#include "Device.h"

static const ModbusGetCmd_t _0228[] = {
    {03,  0x1002,  0x102B},
    {03,  0x1030,  0x1031},
    {03,  0x10A0,  0x10BF},
    {03,  0x1102,  0x1103},
    {03,  0x1107,  0x1108},
    {03,  0x110C,  0x110D},
    {03,  0x1114,  0x1114},
};

static const ModbusGetCmd_t _0229[] = {
    {03,  0x1616,  0x1617},
    {03,  0x1652,  0x1653},
    {03,  0x1678,  0x167D},
    {04,  0x168E,  0x16AE},
};



static const ModbusGetCmd_t proCmdTab[] = {
    {03,  0x1030,  0x1031},
    {03,  0x1616,  0x1617},
};

static const ModbusGetCmdTab_t protocolCmd = PROTOCOL_TABER(proCmdTab, 0, 0);
static const ModbusGetCmdTab_t _0228Protocol = PROTOCOL_TABER(_0228, 0, 0x0228);
static const ModbusGetCmdTab_t _0229Protocol = PROTOCOL_TABER(_0229, 0, 0x0225);

static u8_t protocolCheck(void *load, void *optPoint);


const ModbusDeviceHead_t ChangLingDevice = {
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
//        Buffer_t buf;     // mike 20200914
//        u16_t val;
		switch (cmd->cmd.payload[3])
		{
			case 0x30:
                *((CONVERT_TYPE)optPoint) = &_0228Protocol;

				return 0;
			case 0x16:
                 *((CONVERT_TYPE)optPoint) = &_0229Protocol;
                return 0;
			default:
				break;
		}
	}

	return 1;
}


/******************************************************************************/

