/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : East.c
  *@notes   : 2018.04.12 CGQ   
*******************************************************************************/
#include "eyblib_typedef.h"
#include "eyblib_swap.h"
#include "eyblib_r_stdlib.h"

#include "Protocol.h"
#include "Modbus.h"
#include "Device.h"

static u8_t protocolCheck(void *load, void *optPoint);

static const ModbusGetCmd_t east35K[] = {
    {0x03, 0x6000, 0x601C},              
    {0x04, 0x0000, 0x0031},               
};

static const ModbusGetCmd_t east5k[] = {      
    {0x04, 0x0000, 0x0018},     
	{0x04, 0x0060, 0x0077}, 
	{0x04, 0x8000, 0x801F}, 
	{0x02, 0x0000, 0x0003},          
};

static const ModbusGetCmd_t east3_5k[] = {      
    {0x04, 0x0000, 0x001F},     
	{0x04, 0x0060, 0x0077}, 
	{0x04, 0x8000, 0x801F}, 
	{0x04, 0x8020, 0x8020}, 
	{0x02, 0x0000, 0x0003},          
};

static const ModbusGetCmd_t eastNew[] = {
    {0x03, 8194, 8195},              
    {0x03, 8448, 8449},     
	{0x04,    0,   77}, 
	{0x04,  256,  308},     
	{0x04,  512,  537},      
};

static const ModbusGetCmd_t proCmdTab[] = {	
	{0x04,  0x800C, 0x8013},
	{0x04,  512,  537},
};

static const ModbusGetCmdTab_t eastProtocol = PROTOCOL_TABER(proCmdTab, 0, 0);
static const ModbusGetCmdTab_t east35KProtocol  = PROTOCOL_TABER(east35K, 0, 0x0250);
static const ModbusGetCmdTab_t east5kProtocol = PROTOCOL_TABER(east5k, 0, 0x0251);
static const ModbusGetCmdTab_t eastNewProtocol = PROTOCOL_TABER(eastNew, 0, 0x0252);
static const ModbusGetCmdTab_t east3_5KProtocol = PROTOCOL_TABER(east3_5k, 0, 0x0253);

const ModbusDeviceHead_t EastDevice = {
    &UART_9600_N1,
    &proCmdTab[0],
    &eastProtocol,
    protocolCheck,
    null,
};

const ModbusDeviceHead_t _0253Device = {
    &UART_4800_N1,
    &east3_5k[0],
    &east3_5KProtocol,
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
		int type;

		switch (cmd->cmd.payload[2])
		{
			case 0x02:
				*((CONVERT_TYPE)optPoint) = &eastNewProtocol;
				return 0;
			case 0x80:
				type =  Swap_charNum((char*)&cmd->ack.payload[7]);

				if (type > 10)
				{ 
					*((CONVERT_TYPE)optPoint) = &east35KProtocol;
				}
				else
				{
                    if (r_strstr((char*)&cmd->ack.payload[3], "LPV") != NULL)
					{
						*((CONVERT_TYPE)optPoint) = &east5kProtocol;
					}
					else
					{
						*((CONVERT_TYPE)optPoint) = &east3_5KProtocol;
					}
					
					
				}
				return 0;
			default:
				break;
		}
	}

	return 1;
}


/******************************************************************************/

