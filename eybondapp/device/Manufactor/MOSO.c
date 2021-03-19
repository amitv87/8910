/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : moso.c
  *@notes   : 2017.12.26 CGQ   
*******************************************************************************/
#include "eyblib_typedef.h"
#include "eyblib_memory.h"
#include "eyblib_r_stdlib.h"
#include "eybpub_SysPara_File.h"

#include "Protocol.h"
#include "Device.h"

static const ModbusGetCmd_t moso50k_1[] = {
    {03,   0,  39},
    {03,  80,  93},
    {03, 101, 116},
    {04,   0,  25},
    {04,  46,  59},
    {04,  60,  99},
};

static const ModbusGetCmd_t moso50k[] = {
    {03,   0,  33},
    {03,  80,  93},
    {03, 101, 116},
    {04,   0,  25},
    {04,  46,  59},
    {04,  60,  99},
};

static const ModbusGetCmd_t moso1k[] = {
    {03,  0xC000,  0xC005},
    {03,  0xC008,  0xC019},
    {03,  0xC100,  0xC10D},
};

static const ModbusGetCmd_t moso3k[] = {
    {03,  0xC000,  0xC002},
    {03,  0xC008,  0xC022},
    {03,  0xC100,  0xC122}, 
    {03,  0xC200,  0xC222},
};

//static const ModbusGetCmd_t _0214[] = {
//    {04,   30000,  30040},
//    {04,   30042,  30071},
//    {03,   40000,  40013},
//};
//static const ModbusGetCmdTab_t _0214Protocol = PROTOCOL_TABER(_0214, 0, 0x0214);
//const ModbusDeviceHead_t _0214Device = {
//    &UART_9600_N1,
//    &_0214[0],
//    &_0214Protocol,
//    null,
//    null,
//};

static const ModbusGetCmd_t mosoBatter[] = {
    {03,  0x0000,  0x000D},
    {03,  0x0029,  0x0029},
    {03,  0x002A,  0x002E}, 
};

static const ModbusGetCmd_t proCmdTab[] = {
    {04,       0,   4},
    {03,  0xC012,  0xC012},
    {03,  0x00,  4},
};

static const ModbusGetCmdTab_t protocolCmd = PROTOCOL_TABER(proCmdTab, 0, 0);
static const ModbusGetCmdTab_t moso50kProtocol = PROTOCOL_TABER(moso50k, 0, 0x0212);
static const ModbusGetCmdTab_t moso1kProtocol = PROTOCOL_TABER(moso1k, 0, 0x0210);
static const ModbusGetCmdTab_t moso3kProtocol = PROTOCOL_TABER(moso3k, 0, 0x0211);
static const ModbusGetCmdTab_t moso50k_1Protocol = PROTOCOL_TABER(moso50k_1, 0, 0x0212);
static const ModbusGetCmdTab_t mosoBatterProtocol = PROTOCOL_TABER(mosoBatter, 1, 0x0803);

static u8_t protocolCheck(void *load, void *optPoint);

const ModbusDeviceHead_t MOSODevice = {
    &UART_9600_N1,
    &moso50k[0],
    &protocolCmd,
    protocolCheck,
    null,
};

const ModbusDeviceHead_t MOSOBatter = {
    &UART_9600_N1,
    &mosoBatter[0],
    &mosoBatterProtocol,
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
        Buffer_t buf;
        u16_t val;
		switch (cmd->cmd.payload[3])
		{
			case 0:
                if (cmd->cmd.payload[1] == 4)
                {
//                    SysPara_Get(3, &buf);
                    parametr_get(3, &buf);
//                    if (r_strfind("87", (char*)buf.payload) >= 0) // mike 20200828
                    if (r_strstr((char*)buf.payload, "87") != NULL)
                    {
                        *((CONVERT_TYPE)optPoint) = &moso50k_1Protocol;//&moso3kProtocol;//
                    }
                    else
                    {
                        *((CONVERT_TYPE)optPoint) = &moso50kProtocol;
                    }
                    memory_release(buf.payload);
                    buf.size = 0;
                    buf.lenght = 0;
                }
				return 0;
			case 0x12:
                val = ((cmd->ack.payload[3]<<8)|(cmd->ack.payload[4]));

                if (val >= 100)
                {
				    *((CONVERT_TYPE)optPoint) = &moso3kProtocol;
                }
                else
                {
				    *((CONVERT_TYPE)optPoint) = &moso1kProtocol;
				}
                return 0;
			default:
				break;
		}
	}

	return 1;
}


/******************************************************************************/

