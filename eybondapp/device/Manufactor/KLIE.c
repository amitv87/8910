/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : KLNE.c
  *@notes   : 2019.04.30 CGQ   北京昆兰新能源技术有限公司
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Modbus.h"
#include "Device.h"

static u8_t protocolCheck(void *load, void *optPoint);

static const ModbusGetCmd_t _02CA_CC[] = {
    {0x03, 0, 11},
    {0x04, 0, 31},              
};



static const ModbusGetCmd_t proCmdTab[] = {
    {0x03,  0, 0},
};

static const ModbusGetCmdTab_t protocol  = PROTOCOL_TABER(proCmdTab, 0, 0);
static const ModbusGetCmdTab_t _02CA = PROTOCOL_TABER(_02CA_CC, 0, 0x02CA);
static const ModbusGetCmdTab_t _02CB = PROTOCOL_TABER(_02CA_CC, 0, 0x02CB);
static const ModbusGetCmdTab_t _02CC = PROTOCOL_TABER(_02CA_CC, 0, 0x02CC);

const ModbusDeviceHead_t KLNEDevice = {
    &UART_9600_N1,
    &proCmdTab[0],
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

        switch(cmd->ack.payload[4]&0x0F)
        {
            case 1:
                *((CONVERT_TYPE)optPoint) = &_02CA;
                break;
            case 2:
                *((CONVERT_TYPE)optPoint) = &_02CB;
                break;
            case 3:
                *((CONVERT_TYPE)optPoint) = &_02CC;
                break;
            default:
                break;
        }
        return 0;
	}
	

	return 1;
}


/******************************************************************************/

