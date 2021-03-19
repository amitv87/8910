/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : Huawei.c
  *@notes   : 2018.01.08 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Modbus.h"
#include "Device.h"

typedef enum {
	HUAWEI_20KTL = 0,
	HUAWEI_17KTL,
	HUAWEI_15KTL,
	HUAWEI_12KTL,
	HUAWEI_10KTL,
	HUAWEI_8KTL,
	HUAWEI_500KTL = 9,
	HUAWEI_24_5KTL = 16,
	HUAWEI_23KTL,
	HUAWEI_28KTL,
	HUAWEI_33KTL,
	HUAWEI_40KTL,
	HUAWEI_25KTL,
	HUAWEI_30KTL,
	HUAWEI_30KTL_A,
	HUAWEI_50KTL_C1,
	HUAWEI_42KTL = 26,
	HUAWEI_36KTL,
	HUAWEI_30KTL_JP,
	HUAWEI_40KTL_JP,
	HUAWEI_50KTL,
	HUAWEI_43KTL_IN_C1,

}HuaweiType_e;


static const ModbusGetCmd_t _0284[] = {
    {0x03, 32001 ,32012},
    {0x03, 32262 ,32307},
    {0x03, 32320 ,32328},
    {0x03, 33022 ,33027},
    {0x03, 40000 ,40001},
    {0x03, 50000 ,50009},    
};

static const ModbusGetCmd_t _0299[] = {
    {0x03, 40000 ,40023},
	{0x03, 40032, 40041},
	{0x03, 40051, 40071},
	{0x03, 40076, 40076},
    {0x03, 40117 ,40124},
    {0x03, 40710 ,40722},
    {0x03, 40500 ,40511},
    {0x03, 40523 ,40533},
    {0x03, 40544 ,40546},
    {0x03, 40560 ,40563},
    {0x03, 40572 ,40579},
    {0x03, 40613 ,40616},
    {0x03, 40685 ,40699},
    {0x03, 40931 ,40939},
    {0x03, 50000 ,50011},             
};

static const ModbusGetCmd_t _02A7[] = {
    {0x03, 40000 ,40023},
    {0x03, 40117 ,40124},
    {0x03, 40710 ,40722},
    {0x03, 40500 ,40511},
    {0x03, 40523 ,40533},
    {0x03, 40544 ,40546},
    {0x03, 40560 ,40563},
    {0x03, 40572 ,40579},
    {0x03, 40613 ,40616},
    {0x03, 40685 ,40699},
    {0x03, 40931 ,40939},
    {0x03, 50000 ,50011},             
};

static const ModbusGetCmd_t _02A8[] = {
	{0x03, 32001 ,32012},
	{0x03, 32200 ,32203},
	{0x03, 32262 ,32307},
	{0x03, 32320 ,32328},
	{0x03, 32343 ,32358},
	{0x03, 33022 ,33027},
	{0x03, 50000 ,50016},              
};

static const ModbusGetCmd_t _02A9[] = {
   {0x03, 32001 ,32012},
   {0x03, 32200 ,32203},
   {0x03, 32262 ,32307},
   {0x03, 32314 ,32328},
   {0x03, 32343 ,32358},
   {0x03, 33022 ,33027},
   {0x03, 33070 ,33071},
   {0x03, 50000 ,50016},             
};

static const ModbusGetCmd_t _02C6[] = {
	{0x03, 40000 ,40015},
	{0x03, 40022 ,40023},
    {0x03, 40117 ,40124},
    {0x03, 40710 ,40710},
	{0x03, 40713 ,40722},
    {0x03, 40500 ,40511},
    {0x03, 40523 ,40529},
	{0x03, 40532 ,40533},
    {0x03, 40544 ,40546},
    {0x03, 40560 ,40563},
    {0x03, 40572 ,40574},
	{0x03, 40577 ,40579},
    {0x03, 40613 ,40616},
    {0x03, 40685 ,40691},
	{0x03, 40696 ,40699},
    {0x03, 40931 ,40931},
	{0x03, 40939 ,40939},
    {0x03, 50000 ,50009},              
};

static const ModbusGetCmd_t _0219[] = {
	{0x03, 32001 ,32012},
	{0x03, 32200 ,32203},
	{0x03, 32262 ,32307},
	{0x03, 32320 ,32328},
	{0x03, 33022 ,33027},          
	{0x03, 40000 ,40001},
	{0x03, 50000 ,50009},    
};

static const ModbusGetCmd_t _02D6[] = {
	{0x03, 30000 ,30034},
	{0x03, 30070 ,30082},
	{0x03, 32000 ,32039},
	{0x03, 32064 ,32094},
	{0x03, 32106 ,32107},          
};

static const ModbusGetCmd_t _02D3[] = {
	{0x03, 30000 ,30034},
	{0x03, 30070 ,30082},
	{0x03, 32000 ,32023},
	{0x03, 32064 ,32094},
	{0x03, 32106 ,32107},          
};

static const ModbusGetCmd_t _02DD[] = {
	{0x03, 30000 ,30034},
	{0x03, 30070 ,30082},
	{0x03, 32000 ,32063},
	{0x03, 32064 ,32094},
	{0x03, 32106 ,32107},  
	{0x03, 32114 ,32119},
	{0x03, 40000 ,40001},
	{0x03, 40120 ,40125},
	{0x03, 40200 ,40201},
};

static const ModbusGetCmd_t _02DE[] = {
    {0x03, 30000 ,30034},
    {0x03, 30070 ,30082},
    {0x03, 32000 ,32023},
    {0x03, 32064 ,32094},
    {0x03, 32106 ,32107},
};

static const ModbusGetCmd_t _0926[] = {
    {0x03, 40000 ,40010},
	{0x03, 40500, 40500},
	{0x03, 40521, 40526},
	{0x03, 40532, 40532},
    {0x03, 40544 ,40545},
    {0x03, 40550 ,40555},
    {0x03, 40560 ,40563},
    {0x03, 40568 ,40574},
    {0x03, 40685 ,40685},
    {0x03, 40693 ,40700},
    {0x03, 40713 ,40722},
    {0x03, 40736 ,40743},
    {0x03, 40802 ,40803},
    {0x03, 41124 ,41124},
    {0x03, 41934 ,41942},             
};


static const ModbusGetCmd_t _025E[] = {
	{0x03, 30000 ,30034},
	{0x03, 30070 ,30082},
	{0x03, 32000 ,32039},
	{0x03, 32064 ,32119},
};

static const ModbusGetCmd_t _0269[] = {
    {0x03, 30000 ,30034},
    {0x03, 30070 ,30082},
    {0x03, 32000 ,32063},
    {0x03, 32064 ,32094},
    {0x03, 32106 ,32107},
    {0x03, 32114 ,32119},
};

static const ModbusGetCmd_t proCmdTab[] = {
    {0x03, 32001, 32001},
	{0x03, 32001, 32001},
    {0x03, 30070, 30070},
    {0x03, 40710, 40710},
    {0x03, 40000 ,40023},
};

static const ModbusGetCmdTab_t protocol   = PROTOCOL_TABER(proCmdTab, 	0, 		0);
static const ModbusGetCmdTab_t huawei0219 = PROTOCOL_TABER(_0219, 		0, 		0x0219);
static const ModbusGetCmdTab_t huawei0299 = PROTOCOL_TABER(_0299, 		0, 		0x0299);
static const ModbusGetCmdTab_t huawei0284 = PROTOCOL_TABER(_0284, 		0, 		0x0284);
static const ModbusGetCmdTab_t huawei02A7 = PROTOCOL_TABER(_02A7, 		0, 		0x02A7);
static const ModbusGetCmdTab_t huawei02A8 = PROTOCOL_TABER(_02A8, 		0, 		0x02A8);
static const ModbusGetCmdTab_t huawei02A9 = PROTOCOL_TABER(_02A9, 		0, 		0x02A9);
static const ModbusGetCmdTab_t huawei02C6 = PROTOCOL_TABER(_02C6, 		0, 		0x02C6);
static const ModbusGetCmdTab_t huawei02D6 = PROTOCOL_TABER(_02D6, 		0, 		0x02D6);
static const ModbusGetCmdTab_t huawei02D3 = PROTOCOL_TABER(_02D3, 		0, 		0x02D3);
static const ModbusGetCmdTab_t huawei02DE = PROTOCOL_TABER(_02DE, 		0, 		0x02DE);
static const ModbusGetCmdTab_t huawei02DD = PROTOCOL_TABER(_02DD, 		0x01, 	0x02DD);
static const ModbusGetCmdTab_t huawei0926 = PROTOCOL_TABER(_0926, 		0x00, 	0x0926);
static const ModbusGetCmdTab_t huawei025E = PROTOCOL_TABER(_025E, 		0x00, 	0x025E);
static const ModbusGetCmdTab_t huawei0269 = PROTOCOL_TABER(_0269, 		0x01, 	0x0269);

static u8_t protocolCheck(void *load, void *optPoint);
static u8_t cmdCheck(void *load, void *changeData);

const ModbusDeviceHead_t HuaweiDevice = {
    &UART_9600_N1,
    &proCmdTab[0],
    &protocol,
    protocolCheck,
    cmdCheck,   // 2018.12.05 华为机器02A9 发现有逆变器命令间歇性不能回应，显示逆变器掉线，固增加特定数据核对方法，排除些类情况。CGQ
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
		u16_t reg= (cmd->cmd.payload[2]<<8)|(cmd->cmd.payload[3]);
		if ((reg == proCmdTab[0].start)
			|| (reg == proCmdTab[1].start)
		)
		{
			switch(cmd->ack.payload[4])
			{
				// case HUAWEI_28KTL:
				case HUAWEI_33KTL:
				case HUAWEI_40KTL:
				case HUAWEI_30KTL:
				case HUAWEI_30KTL_A:
					*((CONVERT_TYPE)optPoint) = &huawei0219;//&huawei02A8;//
					break;
				case HUAWEI_50KTL_C1:
				case HUAWEI_42KTL:
				case HUAWEI_36KTL:
				case HUAWEI_30KTL_JP:
				case HUAWEI_40KTL_JP:
				case HUAWEI_50KTL:
				case HUAWEI_43KTL_IN_C1:
					*((CONVERT_TYPE)optPoint) = &huawei02A9;
					break;
				case HUAWEI_28KTL:
                    *((CONVERT_TYPE)optPoint) = &huawei0284;
                    break;
				default:
					*((CONVERT_TYPE)optPoint) = &huawei0299;//&huawei02DD;//&huawei02A7;//
					break;
			}
			return 0;
		}
		else if (reg == proCmdTab[2].start)
		{
			u16_t type = (cmd->ack.payload[3]<<8)|(cmd->ack.payload[4]);

			if (type >= 400){
				*((CONVERT_TYPE)optPoint) = &huawei02DE;
			}
			else if (type >= 300)
			{
				*((CONVERT_TYPE)optPoint) = &huawei02D3;
			}
			else if (type >= 141)
			{
				*((CONVERT_TYPE)optPoint) = &huawei0269;
			}
			else if (type == 55)
			{
				*((CONVERT_TYPE)optPoint) = &huawei025E;
			}
			else if (type > 55)
			{
				*((CONVERT_TYPE)optPoint) = &huawei02DD;
			}
            else if (type >= 46)
            {
                *((CONVERT_TYPE)optPoint) = &huawei02D6;
            }
			else
			{
				*((CONVERT_TYPE)optPoint) = &huawei02DD;
			}
			
			return 0;
		}
		else if (reg == proCmdTab[3].start)
		{
			*((CONVERT_TYPE)optPoint) = &huawei02A7;//&huawei02C6;

		}
        else if (reg == proCmdTab[4].start)
        {
            if (*((CONVERT_TYPE)optPoint) == &huawei02C6)
            {
                *((CONVERT_TYPE)optPoint) = &huawei02A7;
            }
            else
            {
                *((CONVERT_TYPE)optPoint) = &huawei0926;
            }
			return 0;
        }
	}

	return 1;
}

static u8_t cmdCheck(void *load, void *changeData)
{
	static u8_t flag = 1;
	DeviceCmd_t *cmd= (DeviceCmd_t*)load;
	int ret;
    
    
    if (0 == cmd->state)
    {
		ret = Modbus_CmdCheck(&cmd->cmd, &cmd->ack);
		cmd->state = (u8_t)ret;
		if (ret == 0)
		{
			flag = 0;
		}
		else if (ret >  0)
		{
			*((u8_t*)changeData) |= (u8_t)ret;
		}
		else if (ret < 0)
		{
			*((u8_t*)changeData) |= 0x10;
		}
    }
    else
    {
		u16_t startAddr = (cmd->cmd.payload[2]<<8) | (cmd->cmd.payload[3]);
		cmd->state = 0;
		
		if (flag++ > 2
			|| (startAddr == 30000  //非逆变器序列号同步命令不可应答
				|| startAddr == 32001 
				|| startAddr == 40713
				|| startAddr == 40710
				)
		)
		{
			*((u8_t*)changeData) |= 0x10;
		}
    }
    
    return 1;
}

/******************************************************************************/

