/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : litto.c
  *@notes   : 2017.12.29 CGQ   
*******************************************************************************/
#include "eyblib_typedef.h"
#include "eyblib_r_stdlib.h"
#include "Protocol.h"
#include "Modbus.h"
#include "Device.h"

static u8_t commandCheck(void *load, void *optPoint);

const ModbusGetCmd_t litto[] = {
    {0x03,     0,    36},              
    {0x03, 10000, 10007},       
    {0x03, 15000, 15112},        
    {0x03, 17000, 17049},        
};
const ModbusGetCmdTab_t LittoProtocol = PROTOCOL_TABER(litto, 0, 0x0260);
const ModbusDeviceHead_t LittoDevice = {
    &UART_9600_N1,
    &litto[0],
    &LittoProtocol,
    null,
    commandCheck,
};

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static u8_t commandCheck(void *load, void *optPoint)
{
	int ret;
	DeviceCmd_t *cmd = (DeviceCmd_t*)load;


	ret = Modbus_CmdCheck(&cmd->cmd, &cmd->ack);

	if (cmd->state == DEVICE_ACK_OVERTIME 
		&& (((cmd->cmd.payload[2]<<8)|(cmd->cmd.payload[3])) == litto[3].start)
		)
	{
		cmd->ack.lenght = cmd->ack.size;
		r_memset(cmd->ack.payload, 0, cmd->ack.lenght);
	}
	else if (ret >  0)
	{
		*((u8_t*)optPoint) |= ret;
	}
	else if (ret < 0)
	{
		goto ERR;
	}

	
	return 1;
ERR:
	*((u8_t*)optPoint) |= 0x10;
	return 0;

}


static const ModbusGetCmd_t litto2[] = {
	{0x03, 0x4E20, 0x4E31},
};
static const ModbusGetCmdTab_t litto2Protocol = PROTOCOL_TABER(litto2, 0, 0x0501);
const ModbusDeviceHead_t Litto2Device = {
	&UART_9600_N1,
	&litto2[0],
	&litto2Protocol,
	null,
	null,
};


/******************************************************************************/

