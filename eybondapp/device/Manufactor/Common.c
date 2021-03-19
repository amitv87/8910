/**************Copyright(C)锛�2015-2026锛孲henzhen Eybond Co., Ltd.***************
  *@brief   : Common.c
  *@notes   : 2017.12.26 CGQ   
*******************************************************************************/
#include "Protocol.h"
#include "eyblib_typedef.h"
#include "Device.h"

static const ModbusGetCmd_t eybond[] = {
    {03,   0,  19},
    {03,  20,  58},
    {03,  59, 124},
};
static const ModbusGetCmdTab_t eybondProtocol = PROTOCOL_TABER(eybond, (1<<0), 0x0200);
const ModbusDeviceHead_t EybondDevice = {
    &UART_9600_N1,
    &eybond[0],
    &eybondProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _0201[] = {
    {0x04, 0x00 ,0x19},
};
static const ModbusGetCmdTab_t _0201Protocol = PROTOCOL_TABER(_0201, 0, 0x0201);
const ModbusDeviceHead_t _0201Device = {
    &UART_9600_N1,
    &_0201[0],
    &_0201Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0202[] = {
    {0x04, 2999 ,3077},
    {0x04, 3079 ,3088},
    {0x04, 3298 ,3339},
};
static const ModbusGetCmdTab_t _0202Protocol = PROTOCOL_TABER(_0202, 0, 0x0202);
const ModbusDeviceHead_t _0202Device = {
    &UART_9600_N1,
    &_0202[0],
    &_0202Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0205[] = {
    {03,   1999, 2017},
};
static const ModbusGetCmdTab_t _0205Protocol = PROTOCOL_TABER(_0205, 0, 0x0205);
const ModbusDeviceHead_t _0205Device = {
    &UART_9600_N1,
    &_0205[0],
    &_0205Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0206[] = {
    {04,   2999, 3077},
};
static const ModbusGetCmdTab_t _0206Protocol = PROTOCOL_TABER(_0206, 0, 0x0206);
const ModbusDeviceHead_t _0206Device = {
    &UART_9600_N1,
    &_0206[0],
    &_0206Protocol,
    null,
    null,
};

/* static const ModbusGetCmd_t _0206[] = {
    {04,   2999, 3012},
    {04,   3013, 3025},
    {04,   3026, 3038},
    {04,   3039, 3051},
    {04,   3052, 3064},
    {04,   3065, 3077},
};

static const ModbusGetCmdTab_t _0206Protocol = PROTOCOL_TABER(_0206, 0, 0x0206);
const ModbusDeviceHead_t _0206Device = {
    &UART_9600_N1,
    &_0206[0],
    &_0206Protocol,
    null,
    null,
}; */

static const ModbusGetCmd_t _0214[] = {
    {04,   30000,  30040},
    {04,   30042,  30071},
    {03,   40000,  40013},
};
static const ModbusGetCmdTab_t _0214Protocol = PROTOCOL_TABER(_0214, 0, 0x0214);
const ModbusDeviceHead_t _0214Device = {
    &UART_9600_N1,
    &_0214[0],
    &_0214Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0209[] = {
    {03,   1,  12},
    {03,   16,  21},
    {03,   24,  25},
	{03,   26,  26},
    {03,   31,  41},
    {03,   51,  58},
	{03,   61,  68},
    {03,   71,  78},
    {03,   81,  88},
	{03,   91,  98},
	{03,   101,  108},
    {03,   111,  118},
    {03,   121,  128},
	{03,   131,  138},
    {03,   141,  148},
};
static const ModbusGetCmdTab_t _0209Protocol = PROTOCOL_TABER(_0209, 1, 0x0209);
const ModbusDeviceHead_t _0209Device = {
    &UART_9600_E1,
    &_0209[0],
    &_0209Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _020E[] = {
    {0x03, 0 ,65},
};
static const ModbusGetCmdTab_t _020EProtocol = PROTOCOL_TABER(_020E, 0, 0x020E);
const ModbusDeviceHead_t _020EDevice = {
    &UART_9600_N1,
    &_020E[0],
    &_020EProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _0216[] = {
    {04,   0x1001,  0x1028},
    {04,   0x1037,  0x1042},
    {03,   0x3000,  0x3003},
	{03,   0x5000,  0x5011},
	{03,   0x5031,  0x5031},
	{03,   0x5101,  0x5104},
	{03,   0x510E,  0x5118},
	{03,   0x6001,  0x6001},
};
static const ModbusGetCmdTab_t _0216Protocol = PROTOCOL_TABER(_0216, 0, 0x0216);
const ModbusDeviceHead_t _0216Device = {
    &UART_9600_N1,
    &_0216[0],
    &_0216Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0217[] = {
    {03,   0x0100,  0x013B},
};
static const ModbusGetCmdTab_t _0217Protocol = PROTOCOL_TABER(_0217, 0, 0x0217);
const ModbusDeviceHead_t _0217Device = {
    &UART_9600_N1,
    &_0217[0],
    &_0217Protocol,
    null,
    null,
};
static const ModbusGetCmd_t _0218[] = {
    {04,   0x1000,  0x102A},
	{04,   0x1033,  0x1041},
	{04,   0x1047,  0x1066},
	{04,   0x1067,  0x107A},
};
static const ModbusGetCmdTab_t _0218Protocol = PROTOCOL_TABER(_0218, 0, 0x0218);
const ModbusDeviceHead_t _0218Device = {
    &UART_9600_N1,
    &_0218[0],
    &_0218Protocol,
    null,
    null,
};



static const ModbusGetCmd_t _021A[] = {
    {03,   1999,  2018},
};
static const ModbusGetCmdTab_t _021AProtocol = PROTOCOL_TABER(_021A, 0, 0x021A);
const ModbusDeviceHead_t _021ADevice = {
    &UART_9600_N1,
    &_021A[0],
    &_021AProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _021B[] = {
    {03,   0x8F00,  0x8F1C},
	{03,   0x0100,  0x0136},
};
static const ModbusGetCmdTab_t _021BProtocol = PROTOCOL_TABER(_021B, 0, 0x021B);
const ModbusDeviceHead_t _021BDevice = {
    &UART_9600_N1,
    &_021B[0],
    &_021BProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _021C[] = {
    {0x03, 0x0000 ,0x0021},
    {0x03, 0x0050 ,0x005D},
    {0x03, 0x0065 ,0x0074},
    {0x04, 0x0000 ,0x0019},
    {0x04, 0x002E ,0x0063},
};
static const ModbusGetCmdTab_t _021CProtocol = PROTOCOL_TABER(_021C, 0, 0x021C);
const ModbusDeviceHead_t _021CDevice = {
    &UART_9600_N1,
    &_021C[0],
    &_021CProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _021E[] = {
    {04,   1032,  1065},
	{04,   1068,  1103},
	{04,   1131,  1151},
	{04,   3072,  3074},
	{04,   3088,  3090},
	{04,   3104,  3106},
};
static const ModbusGetCmdTab_t _021EProtocol = PROTOCOL_TABER(_021E, 0, 0x021E);
const ModbusDeviceHead_t _021EDevice = {
    &UART_9600_N1,
    &_021E[0],
    &_021EProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _021F[] = {
	{03,   30001,  30007},
	{03,   30012,  30023},
	{03,   30029,  30035},
	{03,   30038,  30044},
	{03,   30047,  30059},
	{03,   30101,  30101},
	{03,   30311,  30318},
};
static const ModbusGetCmdTab_t _021FProtocol = PROTOCOL_TABER(_021F, 0, 0x021F);
const ModbusDeviceHead_t _021FDevice = {
    &UART_9600_N1,
    &_021F[0],
    &_021FProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _021D[] = {
    {03,   10101,  10112},
	{03,   15206,  15209},
	{03,   16222,  16231},
	{03,   20001,  20008},
	{03,   20106,  20106},
	{03,   20108,  20108},
	{03,   20113,  20114},
	{03,   20133,  20140},
	{03,   20161,  20161},
	{03,   20185,  20193},
	{03,   25201,  25201},
	{03,   25206,  25207},
	{03,   25209,  25210},
	{03,   25212,  25212},
	{03,   25214,  25215},
	{03,   25245,  25248},
	{03,   25257,  25257},
	{03,   25261,  25263},
	{03,   25265,  25266},
	{03,   25330,  25330},
};
static const ModbusGetCmdTab_t _021DProtocol = PROTOCOL_TABER(_021D, 0, 0x021D);
const ModbusDeviceHead_t _021DDevice = {
    &UART_9600_N1,
    &_021D[0],
    &_021DProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _0224[] = {
    {03,   0x2000,  0x2027},
	{03,   0x202D,  0x2039},
	{03,   0x2040,  0x2046},
};
static const ModbusGetCmdTab_t _0224Protocol = PROTOCOL_TABER(_0224, 0, 0x0224);
const ModbusDeviceHead_t _0224Device = {
    &UART_9600_N1,
    &_0224[0],
    &_0224Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0226[] = {
    {03,   0x0000,  0x002B},
//	{03,   0x002C,  0x0076},
};
static const ModbusGetCmdTab_t _0226Protocol = PROTOCOL_TABER(_0226, 1, 0x0226);
const ModbusDeviceHead_t _0226Device = {
    &UART_9600_N1,
    &_0226[0],
    &_0226Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0229[] = {
    {02,   2501,  2564},
	{04,   4501,  4543},
	{04,   4700,  4711},
	{04,   4800,  4839},
};
static const ModbusGetCmdTab_t _0229Protocol = PROTOCOL_TABER(_0229, 0, 0x0229);
const ModbusDeviceHead_t _0229Device = {
    &UART_9600_N1,
    &_0229[0],
    &_0229Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _022B[] = {
    {03,   130,  143},
	{03,   150,  170},
	{03,   176,  202},
	{03,   353,  353},
};
static const ModbusGetCmdTab_t _022BProtocol = PROTOCOL_TABER(_022B, 0, 0x022B);
const ModbusDeviceHead_t _022BDevice = {
    &UART_9600_E1,
    &_022B[0],
    &_022BProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _022D[] = {
    {03,   0x0300,  0x033C},
	{04,   0x0400,  0x043B},
};
static const ModbusGetCmdTab_t _022DProtocol = PROTOCOL_TABER(_022D, 0, 0x022D);
const ModbusDeviceHead_t _022DDevice = {
    &UART_9600_N1,
    &_022D[0],
    &_022DProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _0232[] = {
    {0x04, 0x0000 ,0x003A},
};
static const ModbusGetCmdTab_t _0232Protocol = PROTOCOL_TABER(_0232, 0, 0x0232);
const ModbusDeviceHead_t _0232Device = {
    &UART_9600_N1,
    &_0232[0],
    &_0232Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0233[] = {
	{03,   0x2632,  0x2632},
    {03,   0x2640,  0x2659},
	{03,   0x2664,  0x2666},
	{03,   0x2668,  0x2669},
	{03,   0x266D,  0x266D},
	{03,   0x2670,  0x2670},
	{03,   0x2673,  0x2674},
	{03,   0x2677,  0x267B},
	{03,   0x267D,  0x267F},
};
static const ModbusGetCmdTab_t _0233Protocol = PROTOCOL_TABER(_0233, 1, 0x0233);
const ModbusDeviceHead_t _0233Device = {
    &UART_9600_N1,
    &_0233[0],
    &_0233Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0234[] = {
    {03,   1,  3},
	{03,   56,  119},
};
static const ModbusGetCmdTab_t _0234Protocol = PROTOCOL_TABER(_0234, 0, 0x0234);
const ModbusDeviceHead_t _0234Device = {
    &UART_9600_N1,
    &_0234[0],
    &_0234Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0235[] = {
    {03,   1,  55},
};
static const ModbusGetCmdTab_t _0235Protocol = PROTOCOL_TABER(_0235, 0, 0x0235);
const ModbusDeviceHead_t _0235Device = {
    &UART_9600_N1,
    &_0235[0],
    &_0235Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0236[] = {
    {03,   25000,  25040},
};
static const ModbusGetCmdTab_t _0236Protocol = PROTOCOL_TABER(_0236, 0, 0x0236);
const ModbusDeviceHead_t _0236Device = {
    &UART_9600_N1,
    &_0236[0],
    &_0236Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0238[] = {
    {03,   00,  85},
};
static const ModbusGetCmdTab_t _0238Protocol = PROTOCOL_TABER(_0238, 0, 0x0238);
const ModbusDeviceHead_t _0238Device = {
    &UART_9600_N1,
    &_0238[0],
    &_0238Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0248[] = {
    {04,   1024,  1036},
    {04,   1040,  1041},
    {04,   1056,  1084},
    {04,   1104,  1129},
    {04,   16641,  16649},
};
static const ModbusGetCmdTab_t _0248Protocol = PROTOCOL_TABER(_0248, 0, 0x0248);
const ModbusDeviceHead_t _0248Device = {
    &UART_9600_N1,
    &_0248[0],
    &_0248Protocol,
    null,
    null,
};



static const ModbusGetCmd_t _0249[] = {
    {03,   32001,  32042},
    {03,   32051,  32071},
    {03,   32081,  32089},
    {03,   50001,  50008},
};

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static u8_t _0249commandCheck(void *load, void *optPoint)
{
	int ret;
	DeviceCmd_t *cmd = (DeviceCmd_t*)load;


	ret = Modbus_CmdCheck(&cmd->cmd, &cmd->ack);

	if (ret < 0)
	{
        if (((cmd->cmd.payload[2]<<8)|(cmd->cmd.payload[3])) == _0249[1].start)
        {
            cmd->ack.lenght = cmd->ack.size;
        }
		else
        {
            goto ERR;
        }
	}
	else if (ret >  0)
	{
		*((u8_t*)optPoint) |= ret;
	}
	
	return 1;
ERR:
	*((u8_t*)optPoint) |= 0x10;
	return 0;

}
static const ModbusGetCmdTab_t _0249Protocol = PROTOCOL_TABER(_0249, 0, 0x0249);
const ModbusDeviceHead_t _0249Device = {
    &UART_9600_E1,
    &_0249[0],
    &_0249Protocol,
    null,
    _0249commandCheck,
};

static const ModbusGetCmd_t _0242[] = {
    {04,   0,  79},
};
static const ModbusGetCmdTab_t _0242Protocol = PROTOCOL_TABER(_0242, 0, 0x0242);
const ModbusDeviceHead_t _0242Device = {
    &UART_9600_N1,
    &_0242[0],
    &_0242Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _024A[] = {
    {03,   3000,  3057},
    {03,   3060,  3083},
    {04,   4000,  4042},
};
static const ModbusGetCmdTab_t _024AProtocol = PROTOCOL_TABER(_024A, 0, 0x024A);
const ModbusDeviceHead_t _024ADevice = {
    &UART_9600_N1,
    &_024A[0],
    &_024AProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _024B[] = {
    {03,   3001,  3032},
};
static const ModbusGetCmdTab_t _024BProtocol = PROTOCOL_TABER(_024B, 0, 0x024B);
const ModbusDeviceHead_t _024BDevice = {
    &UART_9600_N1,
    &_024B[0],
    &_024BProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _024D[] = {
    {03,   3200,  3216},
    {04,   3000,  3063},
    {04,   3064,  3127},
    {04,   3128,  3162},
};
static const ModbusGetCmdTab_t _024DProtocol = PROTOCOL_TABER(_024D, 0, 0x024D);
const ModbusDeviceHead_t _024DDevice = {
    &UART_9600_N1,
    &_024D[0],
    &_024DProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _024E[] = {
    {04,   0000,  0062},
};
static const ModbusGetCmdTab_t _024EProtocol = PROTOCOL_TABER(_024E, 0, 0x024E);
const ModbusDeviceHead_t _024EDevice = {
    &UART_9600_N1,
    &_024E[0],
    &_024EProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _024F[] = {
    {04,   0x00,  0x25},
};
static const ModbusGetCmdTab_t _024FProtocol = PROTOCOL_TABER(_024F, 0, 0x024F);
const ModbusDeviceHead_t _024FDevice = {
    &UART_9600_N1,
    &_024F[0],
    &_024FProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _0254[] = {
    {02,   0x0000,  0x0005},
	{04,   0x0000,  59},
};
static const ModbusGetCmdTab_t _0254Protocol = PROTOCOL_TABER(_0254, 0, 0x0254);
const ModbusDeviceHead_t _0254Device = {
    &UART_9600_N1,
    &_0254[0],
    &_0254Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0255[] = {
    {03,   0x0100,  0x013A},
};
static const ModbusGetCmdTab_t _0255Protocol = PROTOCOL_TABER(_0255, 0, 0x0255);
const ModbusDeviceHead_t _0255Device = {
    &UART_115200_N1,
    &_0255[0],
    &_0255Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0256[] = {
    {0x04, 15149 ,15260},
};

static const ModbusGetCmdTab_t _0256Protocol = PROTOCOL_TABER(_0256, 0, 0x0256);
const ModbusDeviceHead_t _0256Device = {
    &UART_9600_N1,
    &_0256[0],
    &_0256Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0258[] = {
    {03,  1000,  1009},
    {03,  1020,  1025},
    {03,  1030,  1062},
    {03,  1070,  1085},
    {03,  1100,  1110},
};

static const ModbusGetCmdTab_t _0258Protocol = PROTOCOL_TABER(_0258, 0, 0x0258);
const ModbusDeviceHead_t _0258Device = {
    &UART_9600_N1,
    &_0258[0],
    &_0258Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0259[] = {
    {03,  100,  138},
};

static const ModbusGetCmdTab_t _0259Protocol = PROTOCOL_TABER(_0259, 0, 0x0259);
const ModbusDeviceHead_t _0259Device = {
    &UART_9600_N1,
    &_0259[0],
    &_0259Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _025A[] = {
    {04,  4989,  5048},
    {04,  5070,  5103},
    {04,  5112,  5140},
    {04,  5145,  5151},
    {04,  7012,  7035},
};

static const ModbusGetCmdTab_t _025AProtocol = PROTOCOL_TABER(_025A, 0, 0x025A);
const ModbusDeviceHead_t _025ADevice = {
    &UART_9600_N1,
    &_025A[0],
    &_025AProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _025B[] = {
    {03,  0,  24},
};

static const ModbusGetCmdTab_t _025BProtocol = PROTOCOL_TABER(_025B, 0, 0x025B);
const ModbusDeviceHead_t _025BDevice = {
    &UART_9600_N1,
    &_025B[0],
    &_025BProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _025D[] = {
    {0x03, 0x1000 ,0x1032},
    {0x03, 0x0000 ,0x0009},
};

static const ModbusGetCmdTab_t _025DProtocol = PROTOCOL_TABER(_025D, 0, 0x025D);
const ModbusDeviceHead_t _025DDevice = {
    &UART_9600_N1,
    &_025D[0],
    &_025DProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _025F[] = {
    {0x04, 0 ,5},
    {0x03, 0 ,32},
};

static const ModbusGetCmdTab_t _025FProtocol = PROTOCOL_TABER(_025F, 0, 0x025F);
const ModbusDeviceHead_t _025FDevice = {
    &UART_9600_N1,
    &_025F[0],
    &_025FProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _0262[] = {
    {03, 1000 ,1040},
    {03, 1061 ,1067},
    {03, 1071 ,1074},
    {03, 1110 ,1111},
    {03, 1151 ,1156},
    {03, 1159 ,1159},
    {03, 1165 ,1165},
    {03, 1171 ,1173},
    {03, 1273 ,1286},
};

static const ModbusGetCmdTab_t _0262Protocol = PROTOCOL_TABER(_0262, 0, 0x0262);
const ModbusDeviceHead_t _0262Device = {
    &UART_9600_N1,
    &_0262[0],
    &_0262Protocol,
    null,
    null,
};


static const ModbusGetCmd_t _0263[] = {
    {03, 9000 ,9044},
    {03, 9150 ,9212},
    {03, 9300 ,9304},
    {03, 9650 ,9651},
    {03, 9700 ,9717},
};

static const ModbusGetCmdTab_t _0263Protocol = PROTOCOL_TABER(_0263, 0, 0x0263);
const ModbusDeviceHead_t _0263Device = {
    &UART_9600_N1,
    &_0263[0],
    &_0263Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0264[] = {
    {0x03, 8000 ,8045},
    {0x03, 1050 ,1082},
    {0x03, 1100 ,1114},
    {0x03, 1480 ,1485},
    {0x03, 1451 ,1453},
    {0x03, 1800 ,1806},
    {0x03, 2030 ,2032},
    {0x03, 2010 ,2018},
};

static const ModbusGetCmdTab_t _0264Protocol = PROTOCOL_TABER(_0264, 0, 0x0264);
const ModbusDeviceHead_t _0264Device = {
    &UART_9600_N1,
    &_0264[0],
    &_0264Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0265[] = {
    {03, 1 ,41},
};

static const ModbusGetCmdTab_t _0265Protocol = PROTOCOL_TABER(_0265, 0, 0x0265);
const ModbusDeviceHead_t _0265Device = {
    &UART_9600_N1,
    &_0265[0],
    &_0265Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0266[] = {
    {04, 0 ,26},
};

static const ModbusGetCmdTab_t _0266Protocol = PROTOCOL_TABER(_0266, 0, 0x0266);
const ModbusDeviceHead_t _0266Device = {
    &UART_9600_N1,
    &_0266[0],
    &_0266Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _026A[] = {
    {03, 3200 ,3216},
    {04, 2964 ,3063},
};

static const ModbusGetCmdTab_t _026AProtocol = PROTOCOL_TABER(_026A, 0, 0x026A);
const ModbusDeviceHead_t _026ADevice = {
    &UART_9600_N1,
    &_026A[0],
    &_026AProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _026B[] = {
    {0x02, 2501 ,2510},
    {0x02, 2518 ,2526},
    {0x02, 2534 ,2544},
    {0x02, 2550 ,2551},
    {0x02, 2556 ,2558},
    {0x02, 2566 ,2573},
    {0x04, 2999 ,3043},
    {0x04, 3060 ,3071},
    {0x04, 4999 ,4999},
};

static const ModbusGetCmdTab_t _026BProtocol = PROTOCOL_TABER(_026B, 1, 0x026B);
const ModbusDeviceHead_t _026BDevice = {
    &UART_9600_N1,
    &_026B[0],
    &_026BProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _026C[] = {
    {03, 1007 ,1011},
    {03, 1018 ,1023},
    {03, 1016 ,1017},
    {03, 1012 ,1012},
};

static const ModbusGetCmdTab_t _026CProtocol = PROTOCOL_TABER(_026C, 0, 0x026C);
const ModbusDeviceHead_t _026CDevice = {
    &UART_9600_N1,
    &_026C[0],
    &_026CProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _026F[] = {
    {3, 4096 ,4106},
    {3, 8448 ,8451},
    // {3, 12288 ,12288},
    // {3, 16384 ,16388},
};

static const ModbusGetCmdTab_t _026FProtocol = PROTOCOL_TABER(_026F, 0, 0x026F);
const ModbusDeviceHead_t _026FDevice = {
    &UART_9600_N1,
    &_026F[0],
    &_026FProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _0B01[] = {
    {03,   0,  63},
	{03,   64,  124},
};
static const ModbusGetCmdTab_t _0B01Protocol = PROTOCOL_TABER(_0B01, 0, 0x0B01);
const ModbusDeviceHead_t _0B01Device = {
    &UART_9600_N1,
    &_0B01[0],
    &_0B01Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0407[] = {         // mike 鏁版嵁瓒婄晫浜�
    {04,   40021,  40022},
    {04,   40029,  40030},
//	{04,   463745,  463746},    // mike 修正越界问题 20201222
//  {04,   463761,  463762},
//	{04,   463777,  463778},
//  {04,   463793,  463794},
    {04,   4993,   4994},
    {04,   5009,   5010},
    {04,   5025,   5026},
    {04,   5041,   5042},
    {03,   30001,  30007},
	{03,   30013,  30019},
	{03,   30025,  30031},
	{03,   30071,  30076},
	{03,   30343,  30344},
};
static const ModbusGetCmdTab_t _0407Protocol = PROTOCOL_TABER(_0407, 0, 0x0407);
const ModbusDeviceHead_t _0407Device = {
    &UART_9600_N1,
    &_0407[0],
    &_0407Protocol,
    null,
    null,
};

static const ModbusGetCmd_t clou[] = {
    {03,   2,  10},
    {03,  25,  35},
    {03,  47,  52},
	{03,  61,  68},
	{03,  92,  101},
	{04,   2,  45},
	{04,  47,  85},
	{04,  92,  116},
};
static const ModbusGetCmdTab_t clouProtocol = PROTOCOL_TABER(clou, 0, 0x0222);
const ModbusDeviceHead_t ClouDevice = {
    &UART_9600_N1,
    &clou[0],
    &clouProtocol,
    null,
    null,
};

static const ModbusGetCmd_t invt[] = {
    {0x03, 0x1041 ,0x1042},
    {0x03, 0x1215 ,0x1218},
    {0x03, 0x1234 ,0x1243},
    {0x03, 0x1600 ,0x1601},
    {0x03, 0x160C ,0x160D},
    {0x03, 0x1612 ,0x161F},
    {0x03, 0x1652 ,0x165B},
    {0x03, 0x1670 ,0x167C},
    {0x03, 0x1690 ,0x169D},
    {0x03, 0x169E ,0x16AE},
};
static const ModbusGetCmdTab_t invtProtocol = PROTOCOL_TABER(invt, 1, 0x0230);
const ModbusDeviceHead_t InvtDevice = {
    &UART_9600_N1,
    &invt[0],
    &invtProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _091A[] = {
    {0x03, 0x9600 ,0x9624},
    {0x04, 0x3500 ,0x355B},
};
static const ModbusGetCmdTab_t _091AProtocol = PROTOCOL_TABER(_091A, 0, 0x091A);
const ModbusDeviceHead_t _091ADevice = {
    &UART_9600_N1,
    &_091A[0],
    &_091AProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _091C[] = {
    {0x03, 0 ,0x26},
};
static const ModbusGetCmdTab_t _091CProtocol = PROTOCOL_TABER(_091C, 0, 0x091C);
const ModbusDeviceHead_t _091CDevice = {
    &UART_9600_N1,
    &_091C[0],
    &_091CProtocol,
    null,
    null,
};


static const ModbusGetCmd_t _091D[] = {
    {0x03, 0x9000 ,0x900E},
    {0x03, 0x9067 ,0x9067},
    {0x03, 0x906B ,0x9070},
    {0x04, 0x3000 ,0x3007},
    {0x04, 0x3100 ,0x3111},
    {0x04, 0x3200 ,0x3202},
    {0x04, 0x3302 ,0x3313},
    {0x04, 0x331A ,0x331C},
};
static const ModbusGetCmdTab_t _091DProtocol = PROTOCOL_TABER(_091D, 0, 0x091D);
const ModbusDeviceHead_t _091DDevice = {
    &UART_115200_N1,
    &_091D[0],
    &_091DProtocol,
    null,
    null,
};

static const ModbusGetCmd_t invt0923[] = {
    {0x03, 0x1612 ,0x1625},
	{0x03, 0x162A ,0x162B},
    {0x03, 0x1670 ,0x1680},
    {0x03, 0x168E ,0x16A0},
	{0x03, 0x16A1 ,0x16B2},
    {0x03, 0x224E ,0x2250},
	{0x03, 0x2260 ,0x227C},
	{0x03, 0x227E ,0x229C},
    {0x03, 0x1041 ,0x1041},
    {0x03, 0x121C ,0x122F},
    {0x03, 0x1234 ,0x1243},
};
static const ModbusGetCmdTab_t invt0923Protocol = PROTOCOL_TABER(invt0923, 0, 0x0923);
const ModbusDeviceHead_t Invt0923Device = {
    &UART_9600_N1,
    &invt0923[0],
    &invt0923Protocol,
    null,
    null,
};

static const ModbusGetCmd_t mixed[] = {
    {0x03, 0x000A, 0x001A},
};
static const ModbusGetCmdTab_t mixedProtocol = PROTOCOL_TABER(mixed, 0, 0x0298);
const ModbusDeviceHead_t MixedDevice = {
    &UART_9600_N1,
    &mixed[0],
    &mixedProtocol,
    null,
    null,
};

static const ModbusGetCmd_t crrc[] = {
    {0x03, 0x0000, 0x0052},
	{0x03, 0x0057, 0x0064},
	{0x03, 0x006B, 0x0082},
	{0x03, 0x0087, 0x008A},
	{0x03, 0x0096, 0x0096},
	{0x03, 0x00A5, 0x00AA},
	{0x03, 0x00C0, 0x00C1},
	{0x04, 0x1000, 0x1041},
    {0x04, 0x1046, 0x1059},
	{0x04, 0x1073, 0x1081},
	{0x04, 0x1087, 0x1091},
};
static const ModbusGetCmdTab_t crrcProtocol = PROTOCOL_TABER(crrc, 0, 0x029C);
const ModbusDeviceHead_t CRRCDevice = {
    &UART_9600_N1,
    &crrc[0],
    &crrcProtocol,
    null,
    null,
};



static const ModbusGetCmd_t mustsolar[] = {
    {0x03, 20001 ,20016},
    {0x03, 20099 ,20132},
    {0x03, 25201 ,25274},
    {0x03, 15001 ,15008},
    {0x03, 15111 ,15122},
    {0x03, 15201 ,15221},
};
static const ModbusGetCmdTab_t mustsolarProtocol = PROTOCOL_TABER(mustsolar, 0, 0x02A1);
const ModbusDeviceHead_t MustsolarDevice = {
    &UART_9600_N1,
    &mustsolar[0],
    &mustsolarProtocol,
    null,
    null,
};

static const ModbusGetCmd_t poweroak1[] = {
    {0x03, 20001 ,20016},
    {0x03, 20099 ,20132},
    {0x03, 25201 ,25274},
    {0x03, 15001 ,15008},
    {0x03, 15111 ,15122},
    {0x03, 15201 ,15221},
};
static const ModbusGetCmdTab_t poweroak1Protocol = PROTOCOL_TABER(poweroak1, 0, 0x02A4);
const ModbusDeviceHead_t Poweroak1Device = {
    &UART_9600_N1,
    &poweroak1[0],
    &poweroak1Protocol,
    null,
    null,
};
static const ModbusGetCmd_t poweroak2[] = {
    {0x03,  1,  35},
    {0x03, 42,  45},
    {0x03, 54, 119},
};
static const ModbusGetCmdTab_t poweroak2Protocol = PROTOCOL_TABER(poweroak2, 0, 0x02AB);
const ModbusDeviceHead_t Poweroak2Device = {
    &UART_9600_N1,
    &poweroak2[0],
    &poweroak2Protocol,
    null,
    null,
};

static const ModbusGetCmd_t tsol[] = {
    {0x03, 0x0000 ,0x0035},
};
static const ModbusGetCmdTab_t tsolProtocol = PROTOCOL_TABER(tsol, 0, 0x02AA);
const ModbusDeviceHead_t TsolDevice = {
    &UART_9600_N1,
    &tsol[0],
    &tsolProtocol,
    null,
    null,
};

static const ModbusGetCmd_t keHua[] = {
    {0x02,  2501,  2549},
	{0x04,  4501,  4543},
	{0x04,  4700,  4711},
	{0x04,  4800,  4839,}
};
static const ModbusGetCmdTab_t keHuaProtocol = PROTOCOL_TABER(keHua, 0, 0x0291);
const ModbusDeviceHead_t KeHuaDevice = {
    &UART_9600_N1,
    &keHua[0],
    &keHuaProtocol,
    null,
    null,
};


static const ModbusGetCmd_t yanxu[] = {
    {0x04,  1000,  1023},
};
static const ModbusGetCmdTab_t yanxuProtocol = PROTOCOL_TABER(yanxu, 0, 0x0295);
const ModbusDeviceHead_t YanXuDevice = {
    &UART_9600_N1,
    &yanxu[0],
    &yanxuProtocol,
    null,
    null,
};


static const ModbusGetCmd_t _0289[] = {
    {0x03,  0,  43},
	{0x03,  6000,  6006},
	{0x03,  12000,  12031},
	{0x03,  12032,  12052},
};
static const ModbusGetCmdTab_t _0289Protocol = PROTOCOL_TABER(_0289, 0, 0x0289);
const ModbusDeviceHead_t InovanceDevice = {
    &UART_9600_N1,
    &_0289[0],
    &_0289Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _028C[] = {
    {0x03,  0x0300,  0x0320},
	{0x03,  0x033B,  0x0340},
	{0x03,  0x0354,  0x036F},
};
static const ModbusGetCmdTab_t _028CProtocol = PROTOCOL_TABER(_028C, 0, 0x028C);
const ModbusDeviceHead_t _028CDevice = {
    &UART_9600_N1,
    &_028C[0],
    &_028CProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _028F[] = {
    {0x03,  0xC218,  0xC219},
	{0x03,  0xC100,  0xC111},
	{0x03,  0xC001,  0xC002},
    {0x03,  0xC008,  0xC008},
	{0x03,  0xC00D,  0xC018},
};
static const ModbusGetCmdTab_t _028FProtocol = PROTOCOL_TABER(_028F, 0, 0x028F);
const ModbusDeviceHead_t _028FDevice = {
    &UART_9600_N1,
    &_028F[0],
    &_028FProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _02E1[] = {
    {0x02,  2501,  2532},
	{0x02,  2565,  2573},
	{0x02,  2597,  2599},
	{0x04,  4501,  4501},
	{0x04,  4506,  4531},
	{0x04,  4551,  4561},
	{0x04,  4601,  4625},
	{0x04,  4651,  4654},
	{0x04,  00,  01},
	{0x04,  5500,  5512},
};
static const ModbusGetCmdTab_t _02E1Protocol = PROTOCOL_TABER(_02E1, 0, 0x02E1);
const ModbusDeviceHead_t _02E1Device = {
    &UART_9600_N1,
    &_02E1[0],
    &_02E1Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _02DC[] = {
    {0x04,0 ,45},
    {0x04,60,67},
};
static const ModbusGetCmdTab_t _02DCProtocol = PROTOCOL_TABER(_02DC, 0, 0x02DC);
const ModbusDeviceHead_t _02DCDevice = {
    &UART_9600_N1,
    &_02DC[0],
    &_02DCProtocol,
    null,
    null,
};

static const ModbusGetCmd_t delta[] = {
    {0x04,  1031,  1037},
	{0x04,  40962,  40967},
	{0x04,  53255,  53257},
	{0x04,  1049,  1049},
	{0x04,  49151,  49163},
	{0x04,  45055,  45061},
	{0x04,  49247,  49249},
	{0x04,  1071,  1097},
	{0x04,  1135,  1142},
	{0x04,  3071,  3073},
	{0x04,  3087,  3088},
	{0x04,  3103,  3107},
	{0x03,  797,  800},
	{0x03,  33615,  33616},
	{0x03,  33623,  33623}
};
static const ModbusGetCmdTab_t deltaProtocol = PROTOCOL_TABER(delta, 0, 0x0296);
const ModbusDeviceHead_t DeltaDevice = {
    &UART_9600_N1,
    &delta[0],
    &deltaProtocol,
    null,
    null,
};

static const ModbusGetCmd_t MYSolar[] = {
    {0x03,  0x0000,  0x001F},
	{0x03,  0x0040,  0x0049},
	{0x03,  0x0070,  0x0079},
	{0x03,  0x0090,  0x0095},
	{0x03,  0x00A0,  0x00A4},
	{0x03,  0x00B0,  0x00B3}
};
static const ModbusGetCmdTab_t MYProtocol = PROTOCOL_TABER(MYSolar, 0, 0x0297);
const ModbusDeviceHead_t MYSolarDevice = {
    &UART_9600_N1,
    &MYSolar[0],
    &MYProtocol,
    null,
    null,
};



static const ModbusGetCmd_t tsinergy[] = {
    {0x04,  0,  82},
};
static const ModbusGetCmdTab_t tsinergyProtocol = PROTOCOL_TABER(tsinergy, 0, 0x02AC);
const ModbusDeviceHead_t TsinergyDevice = {
    &UART_9600_N1,
    &tsinergy[0],
    &tsinergyProtocol,
    null,
    null,
};

static const ModbusGetCmd_t solarlake[] = {
	{0x04, 0x00, 0x15},
	{0x04, 0x16, 0x21},
	{0x04, 0x22, 0x25},
};
static const ModbusGetCmdTab_t solarlakeProtocol = PROTOCOL_TABER(solarlake, 0, 0x02B1);
const ModbusDeviceHead_t SolarlakeDevice = {
  &UART_9600_N1,
  &solarlake[0],
  &solarlakeProtocol,
  null,
  null,
};  

static const ModbusGetCmd_t tbea1[] = {
	{0x03, 0x101A, 0x1029},
	{0x03, 0x10A0, 0x10B6},
	{0x03, 0x10B7, 0x10BB},
	{0x03, 0x10BC, 0x10CB},
	{0x03, 0x12F3, 0x1343},
};
static const ModbusGetCmdTab_t tbea1Protocol = PROTOCOL_TABER(tbea1, 0, 0x02B2);
const ModbusDeviceHead_t Tbea1Device = {
	&UART_9600_N1,
	&tbea1[0],
	&tbea1Protocol,
	null,
	null,
}; 
static const ModbusGetCmd_t tbea2[] = {
	{0x03, 0x101A, 0x1029},
	{0x03, 0x10A0, 0x10BB},
	{0x03, 0x12F0, 0x133F},
};
static const ModbusGetCmdTab_t tbea2Protocol = PROTOCOL_TABER(tbea2, 0, 0x02B4);
const ModbusDeviceHead_t Tbea2Device = {
	&UART_9600_N1,
	&tbea2[0],
	&tbea2Protocol,
	null,
	null,
}; 
static const ModbusGetCmd_t tbea3[] = {
	{0x03, 0x101A, 0x1029},
	{0x03, 0x10A0, 0x10BF},
	{0x03, 0x10CC, 0x10CF},
};
static const ModbusGetCmdTab_t tbea3Protocol = PROTOCOL_TABER(tbea3, 0, 0x02DB);
const ModbusDeviceHead_t Tbea3Device = {
	&UART_9600_N1,
	&tbea3[0],
	&tbea3Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t tbea4[] = {
	{0x03, 0x2000, 0x2011},
	{0x03, 0x2020, 0x2027},
	{0x03, 0x2040, 0x2041},
	{0x03, 0x2050, 0x2052},
	{0x03, 0x2100, 0x211B},
	{0x03, 0x2200, 0x222F},
};
static const ModbusGetCmdTab_t tbea4Protocol = PROTOCOL_TABER(tbea4, 0, 0x0285);
const ModbusDeviceHead_t Tbea4Device = {
	&UART_9600_N1,
	&tbea4[0],
	&tbea4Protocol,
	null,
	null, 
}; 

static const ModbusGetCmd_t tbea022C[] = {
	{0x03, 0x2000, 0x2026},
	{0x03, 0x2040, 0x2046},
	{0x03, 0x2200, 0x223C},
	{0x03, 0x2300, 0x231D},
};
static const ModbusGetCmdTab_t tbea022CProtocol = PROTOCOL_TABER(tbea022C, 0, 0x022C);
const ModbusDeviceHead_t Tbea022CDevice = {
	&UART_9600_N1,
	&tbea022C[0],
	&tbea022CProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t NOGO[] = {
	{0x03, 0, 40},
};
static const ModbusGetCmdTab_t NOGOProtocol = PROTOCOL_TABER(NOGO, 0, 0x02DA);
const ModbusDeviceHead_t NOGODevice = {
	&UART_9600_N1,
	&NOGO[0],
	&NOGOProtocol,
	null,
	null,
}; 


static const ModbusGetCmd_t jxznet1[] = {
	{0x03,   0,  18},
	{0x03,  67, 111},
	{0x03, 151, 153},
};
static const ModbusGetCmdTab_t jxznet1Protocol = PROTOCOL_TABER(jxznet1, 0, 0x02B3);
const ModbusDeviceHead_t Jxznet1Device = {
	&UART_9600_N1,
	&jxznet1[0],
	&jxznet1Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t jxznet2[] = {
	{0x03, 0x0110, 0x0115},
    {0x03, 0x0210, 0x0210},
    {0x03, 0x0303, 0x0305},
    {0x03, 0x0323, 0x0325},
    {0x03, 0x0340, 0x0343},
    {0x03, 0x0401, 0x0403},
    {0x03, 0x0501, 0x0501},
    {0x03, 0x0D17, 0x0D17},
    {0x03, 0x1200, 0x1207},
};
static const ModbusGetCmdTab_t jxznet2Protocol = PROTOCOL_TABER(jxznet2, 0, 0x02B6);
const ModbusDeviceHead_t Jxznet2Device = {
	&UART_9600_N1,
	&jxznet2[0],
	&jxznet2Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _02B7[] = {
	{0x04, 0, 44},
	{0x04, 45, 90},
	{0x03, 4096, 4132},
	{0x03, 4133, 4160},
};
static const ModbusGetCmdTab_t _02B7Protocol = PROTOCOL_TABER(_02B7, 0, 0x02B7);
const ModbusDeviceHead_t AthertonDevice = {
	&UART_9600_N1,
	&_02B7[0],
	&_02B7Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t kstar1[] = {
	{0x03, 3200, 3205},
	{0x03, 3000, 3063},
};
static const ModbusGetCmdTab_t kstar1Protocol = PROTOCOL_TABER(kstar1, 0, 0x02B5);
const ModbusDeviceHead_t Kstar1Device = {
	&UART_9600_N1,
	&kstar1[0],
	&kstar1Protocol,
	null,
	null,
}; 
static const ModbusGetCmd_t kstar2[] = {
    {0x03, 3200 ,3205},
    {0x04, 3000 ,3048},
};
static const ModbusGetCmdTab_t kstar2Protocol = PROTOCOL_TABER(kstar2, 0, 0x0270);
const ModbusDeviceHead_t Kstar2Device = {
    &UART_9600_N1,
    &kstar2[0],
    &kstar2Protocol,
    null,
    null,
};



static const ModbusGetCmd_t kstar3[] = {
    {0x03, 1029, 1032},
    {0x03, 1050, 1072},
    {0x03, 1080, 1082},
};
static const ModbusGetCmdTab_t kstar3Protocol = PROTOCOL_TABER(kstar3, 0, 0x0502);
const ModbusDeviceHead_t Kstar3Device = {
    &UART_9600_N1,
    &kstar3[0],
    &kstar3Protocol,
    null,
    null,
};
static const ModbusGetCmd_t _0908[] = {
    {0x03, 3206 ,3213},
    {0x03, 3000 ,3055},
    {0x03, 3100 ,3145},
};
static const ModbusGetCmdTab_t _0908Protocol = PROTOCOL_TABER(_0908, 0, 0x0908);
const ModbusDeviceHead_t Kstar4Device = {
    &UART_9600_N1,
    &_0908[0],
    &_0908Protocol,
    null,
    null,
};


static const ModbusGetCmd_t deli[] = {
	{0x03,  0, 35},
    {0x03, 51, 74},
};
static const ModbusGetCmdTab_t deliProtocol = PROTOCOL_TABER(deli, 0, 0x02B8);
const ModbusDeviceHead_t DeliDevice = {
	&UART_9600_N1,
	&deli[0],
	&deliProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t deli_02BE[] = {
	{0x03,  0, 42},
    {0x03, 51, 81},
};
static const ModbusGetCmdTab_t deli02BEProtocol = PROTOCOL_TABER(deli_02BE, 0, 0x02BE);
const ModbusDeviceHead_t Deli02BEDevice = {
	&UART_9600_N1,
	&deli_02BE[0],
	&deli02BEProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t ginlong[] = {
	{0x04, 2999, 3077},
};
static const ModbusGetCmdTab_t ginlongProtocol = PROTOCOL_TABER(ginlong, 0, 0x02BA);
const ModbusDeviceHead_t GinlongDevice = {
	&UART_9600_N1,
	&ginlong[0],
	&ginlongProtocol,
	null,
	null,
}; 


static const ModbusGetCmd_t zeversolar[] = {
	{0x04,   0,  46},
};
static const ModbusGetCmdTab_t zeversolarProtocol = PROTOCOL_TABER(zeversolar, 0, 0x02BC);
const ModbusDeviceHead_t ZeversolarDevice = {
	&UART_9600_N1,
	&zeversolar[0],
	&zeversolarProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t goodwe[] = {
	{0x03, 0x0000, 0x0005},
    {0x03, 0x0010, 0x0012},
    {0x03, 0x0200, 0x0207},
    {0x03, 0x0210, 0x0214},
    {0x03, 0x0220, 0x0236},
};
static const ModbusGetCmdTab_t goodweProtocol = PROTOCOL_TABER(goodwe, 0, 0x02BD);
const ModbusDeviceHead_t GoodweDevice = {
	&UART_9600_N1,
	&goodwe[0],
	&goodweProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t zealink[] = {
	{0x03, 0x2000, 0x201F},
    {0x03, 0x2020, 0x203F},
    {0x03, 0x2100, 0x2147},
};
static const ModbusGetCmdTab_t zealinkProtocol = PROTOCOL_TABER(zealink, 0, 0x02BF);
const ModbusDeviceHead_t ZealinkDevice = {
	&UART_9600_N1,
	&zealink[0],
	&zealinkProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t chint[] = {
	{0x04, 0x0000, 0x003B},
};
static const ModbusGetCmdTab_t chintProtocol = PROTOCOL_TABER(chint, 0, 0x02C0);
const ModbusDeviceHead_t ChintDevice = {
	&UART_9600_N1,
	&chint[0],
	&chintProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t kostal[] = {
	{0x03, 30000, 30106},
	{0x03, 30300, 30300},
};
static const ModbusGetCmdTab_t kostalProtocol = PROTOCOL_TABER(kostal, 0, 0x02C4);
const ModbusDeviceHead_t KostalDevice = {
	&UART_9600_E1,
	&kostal[0],
	&kostalProtocol,
	null,
	null,
}; 


static const ModbusGetCmd_t greatwall1[] = {
	{0x04, 1009, 1064},
    {0x04, 1071, 1079},
};
static const ModbusGetCmdTab_t greatwall1Protocol = PROTOCOL_TABER(greatwall1, 0, 0x02C6);
const ModbusDeviceHead_t Greatwall1Device = {
	&UART_9600_N1,
	&greatwall1[0],
	&greatwall1Protocol,
	null,
	null,
}; 
static const ModbusGetCmd_t goldwind[] = {
	{0x01, 1280, 1343},
    {0x04, 4096, 4132},
};
static const ModbusGetCmdTab_t goldwindProtocol = PROTOCOL_TABER(goldwind, 0, 0x02CF);
const ModbusDeviceHead_t GoldwindDevice = {
	&UART_9600_N1,
	&goldwind[0],
	&goldwindProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t xjgc[] = {
	{0x03, 0x1000, 0x102B},
    {0x03, 0x6000, 0x600A},
};
static const ModbusGetCmdTab_t xjgcProtocol = PROTOCOL_TABER(xjgc, 0, 0x02D0);
const ModbusDeviceHead_t XjgcDevice = {
	&UART_9600_N1,
	&xjgc[0],
	&xjgcProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t senErgy[] = {
	{0x03, 0x1001, 0x1026},
    {0x03, 0x1037, 0x1041},
	{0x03, 0x1A10, 0x1A17},
    {0x03, 0x1A44, 0x1A48},
};
static const ModbusGetCmdTab_t senErgyProtocol = PROTOCOL_TABER(senErgy, 0, 0x02D7);
const ModbusDeviceHead_t SenErgyDevice = {
	&UART_9600_N1,
	&senErgy[0],
	&senErgyProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t clenErgy[] = {
	{0x03,   39,  53},
    {0x03,  110,  129},
	{0x03,  130,  143},
    {0x03,  150,  171},
	{0x03,  174,  192},
	{0x03,  353,  353},
};
static const ModbusGetCmdTab_t clenErgyProtocol = PROTOCOL_TABER(clenErgy, 0, 0x02D8);
const ModbusDeviceHead_t ClenErgyDevice = {
	&UART_9600_E1,
	&clenErgy[0],
	&clenErgyProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t eybondH[] = {
	{0x03, 0x0000, 0x0013},
};
static const ModbusGetCmdTab_t eybondHProtocol = PROTOCOL_TABER(eybondH, 0, 0x0300);
const ModbusDeviceHead_t EybondHDevice = {
	&UART_9600_N1,
	&eybondH[0],
	&eybondHProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t whxph1[] = {
	{0x03, 0x0000, 0x000F},
};
static const ModbusGetCmdTab_t whxph1Protocol = PROTOCOL_TABER(whxph1, 0, 0x0301);
const ModbusDeviceHead_t Whxph1Device = {
	&UART_9600_N1,
	&whxph1[0],
	&whxph1Protocol,
	null,
	null,
}; 


static const ModbusGetCmdTab_t whxph3Protocol = PROTOCOL_TABER(whxph1, 0, 0x0306);
const ModbusDeviceHead_t Whxph3Device = {
	&UART_9600_N1,
	&whxph1[0],
	&whxph3Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t whxph2[] = {
	{0x03, 0x0000, 0x0000},
};
static const ModbusGetCmdTab_t whxph2Protocol = PROTOCOL_TABER(whxph2, 0, 0x0303);
const ModbusDeviceHead_t Whxph2Device = {
	&UART_9600_N1,
	&whxph2[0],
	&whxph2Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0308[] = {
	{0x03, 0x0000, 0x000F},
};
static const ModbusGetCmdTab_t _0308Protocol = PROTOCOL_TABER(_0308, 0, 0x0308);
const ModbusDeviceHead_t _0308Device = {
	&UART_9600_N1,
	&_0308[0],
	&_0308Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0309[] = {
	{0x03, 0x0000, 0x000F},
};
static const ModbusGetCmdTab_t _0309Protocol = PROTOCOL_TABER(_0309, 0, 0x0309);
const ModbusDeviceHead_t _0309Device = {
	&UART_9600_N1,
	&_0309[0],
	&_0309Protocol,
	null,
	null,
};

static const ModbusGetCmd_t _030A[] = {
	{0x03, 0x0000, 0x000F},
};
static const ModbusGetCmdTab_t _030AProtocol = PROTOCOL_TABER(_030A, 0, 0x030A);
const ModbusDeviceHead_t _030ADevice = {
	&UART_9600_N1,
	&_030A[0],
	&_030AProtocol,
	null,
	null,
};

static const ModbusGetCmd_t _030B[] = {
	{0x03, 0x0000, 0x000F},
};
static const ModbusGetCmdTab_t _030BProtocol = PROTOCOL_TABER(_030B, 0, 0x030B);
const ModbusDeviceHead_t _030BDevice = {
	&UART_9600_N1,
	&_030B[0],
	&_030BProtocol,
	null,
	null,
};

static const ModbusGetCmd_t _030C[] = {
	{0x03, 0x0000, 0x000F},
};
static const ModbusGetCmdTab_t _030CProtocol = PROTOCOL_TABER(_030C, 0, 0x030C);
const ModbusDeviceHead_t _030CDevice = {
	&UART_9600_N1,
	&_030C[0],
	&_030CProtocol,
	null,
	null,
};

static const ModbusGetCmd_t _030D[] = {
    {0x03, 0x0000 ,0x0003},
};
static const ModbusGetCmdTab_t _030DProtocol = PROTOCOL_TABER(_030D, 0, 0x030D);
const ModbusDeviceHead_t _030DDevice = {
	&UART_9600_N1,
	&_030D[0],
	&_030DProtocol,
	null,
	null,
};

static const ModbusGetCmd_t _030E[] = {
    {0x03, 0x0000 ,0x000F},
};
static const ModbusGetCmdTab_t _030EProtocol = PROTOCOL_TABER(_030E, 0, 0x030E);
const ModbusDeviceHead_t _030EDevice = {
	&UART_9600_N1,
	&_030E[0],
	&_030EProtocol,
	null,
	null,
};

static const ModbusGetCmd_t jinZhou[] = {
	{0x03, 0x0000, 0x002A},
};
static const ModbusGetCmdTab_t jinZhouProtocol = PROTOCOL_TABER(jinZhou, 0, 0x0305);
const ModbusDeviceHead_t JinZhouDevice = {
	&UART_9600_N1,
	&jinZhou[0],
	&jinZhouProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0307[] = {
	{0x04, 0x0000, 0x0008},
};
static const ModbusGetCmdTab_t _0307Protocol = PROTOCOL_TABER(_0307, 0, 0x0307);
const ModbusDeviceHead_t BVGDevice = {
	&UART_9600_N1,
	&_0307[0],
	&_0307Protocol,
	null,
	null,
};

static const ModbusGetCmd_t eybond4[] = {
	{0x03, 0x0000, 0x0019},
};
static const ModbusGetCmdTab_t eybond4Protocol = PROTOCOL_TABER(eybond4, 0, 0x0400);
const ModbusDeviceHead_t Eybond4Device = {
	&UART_9600_N1,
	&eybond4[0],
	&eybond4Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t chlykj[] = {
	{0x03, 0x0014, 0x001B},
};
static const ModbusGetCmdTab_t chlykjProtocol = PROTOCOL_TABER(chlykj, 0, 0x0401);
const ModbusDeviceHead_t ChlykjDevice = {
	&UART_9600_N1,
	&chlykj[0],
	&chlykjProtocol,
	null,
	null,
}; 
static const ModbusGetCmd_t ha0402[] = {
	{0x03,  0,  6},
    {0x03, 20, 58},
};
static const ModbusGetCmdTab_t ha0402Protocol = PROTOCOL_TABER(ha0402, 0, 0x0402);
const ModbusDeviceHead_t Ha0402Device = {
	&UART_9600_N1,
	&ha0402[0],
	&ha0402Protocol,
	null,
	null,
}; 


static const ModbusGetCmd_t ha0403[] = {
	{0x03,  0,  6},
    {0x03, 20, 58},
};
static const ModbusGetCmdTab_t ha0403Protocol = PROTOCOL_TABER(ha0403, 0, 0x0403);
const ModbusDeviceHead_t Ha0403Device = {
	&UART_9600_N1,
	&ha0403[0],
	&ha0403Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t yg[] = {
	{0x03, 0x000A, 0x0038},
    {0x03, 0x0046, 0x0075},
};
static const ModbusGetCmdTab_t ygProtocol = PROTOCOL_TABER(yg, 0, 0x0404);
const ModbusDeviceHead_t YgDevice = {
	&UART_9600_N1,
	&yg[0],
	&ygProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t acrel[] = {
	{0x03, 243, 280},
    {0x03, 301, 302},
    {0x03, 373, 552},
    {0x03, 553, 558},
};
static const ModbusGetCmdTab_t acrelProtocol = PROTOCOL_TABER(acrel, 0, 0x0405);
const ModbusDeviceHead_t AcrelDevice = {
	&UART_9600_N1,
	&acrel[0],
	&acrelProtocol,
	null,
	null,
}; 
 
static const ModbusGetCmd_t chintMetet[] = {
	{0x03, 0x0000, 0x000C},
	{0x03, 0x0029, 0x002E},
	{0x03, 0x2000, 0x2031},
	{0x03, 0x2044, 0x2045},
	{0x03, 0x401E, 0x401F},
	{0x03, 0x4028, 0x4029},
	{0x03, 0x4032, 0x4033},
	{0x03, 0x403C, 0x403D},
	{0x03, 0x4046, 0x4047},
	{0x03, 0x4050, 0x4051},
};
static const ModbusGetCmdTab_t chintMetetProtocol = PROTOCOL_TABER(chintMetet, 0, 0x0406);
const ModbusDeviceHead_t ChintMetetDevice = {
	&UART_9600_N1,
	&chintMetet[0],
	&chintMetetProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0408[] = {
    {03,   2000,  2023},
    {03,   2139,  2178},
	{03,   4000,  4023},
    {03,   4024,  4047},
	{03,   4048,  4071},
    {03,   4072,  4079},
    {03,   5000,  5055},
};
static const ModbusGetCmdTab_t _0408Protocol = PROTOCOL_TABER(_0408, 1, 0x0408);
const ModbusDeviceHead_t _0408Device = {
    &UART_9600_N1,
    &_0408[0],
    &_0408Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0409[] = {
    {03,   0,  4},
    {03,   22,  22},
	{03,   23,  82},
};
static const ModbusGetCmdTab_t _0409Protocol = PROTOCOL_TABER(_0409, 0, 0x0409);
const ModbusDeviceHead_t _0409Device = {
    &UART_9600_N1,
    &_0409[0],
    &_0409Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _040A[] = {
    {03,   1,  19},
};
static const ModbusGetCmdTab_t _040AProtocol = PROTOCOL_TABER(_040A, 0, 0x040A);
const ModbusDeviceHead_t _040ADevice = {
    &UART_9600_N1,
    &_040A[0],
    &_040AProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _040B[] = {
    {03,   0,  5},
	{03,   0x1900,  0x1931},
};
static const ModbusGetCmdTab_t _040BProtocol = PROTOCOL_TABER(_040B, 0, 0x040B);
const ModbusDeviceHead_t _040BDevice = {
    &UART_9600_N1,
    &_040B[0],
    &_040BProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _040C[] = {
    {03,   40016,  40089},
	{03,   40200,  40207},
    {03,   40250,  40281},
	{03,   41000,  41017},
    {03,   41500,  41583},
	{03,   41901,  41903},
    {03,   52000,  52011},
	{03,   60000,  60003},
    {03,   60200,  60221},
};
static const ModbusGetCmdTab_t _040CProtocol = PROTOCOL_TABER(_040C, 0, 0x040C);
const ModbusDeviceHead_t _040CDevice = {
    &UART_9600_N1,
    &_040C[0],
    &_040CProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _040D[] = {
    {03,   40016,  40029},
	{03,   40200,  40203},
    {03,   41000,  41002},
};
static const ModbusGetCmdTab_t _040DProtocol = PROTOCOL_TABER(_040D, 0, 0x040D);
const ModbusDeviceHead_t _040DDevice = {
    &UART_9600_E1,
    &_040D[0],
    &_040DProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _0412[] = {
    {3, 4096 ,4099},
    {3, 4104 ,4107},
    {3, 4109 ,4112},
    {3, 8192 ,8195},
    {3, 8448 ,8451},
    {3, 8450 ,8451},
};
static const ModbusGetCmdTab_t _0412Protocol = PROTOCOL_TABER(_0412, 0, 0x0412);
const ModbusDeviceHead_t _0412Device = {
    &UART_9600_N1,
    &_0412[0],
    &_0412Protocol,
    null,
    null,
};

static const ModbusGetCmd_t teihe[] = {
	{0x02, 0, 16},
	{0x04, 0x0000, 0x003B},
};
static const ModbusGetCmdTab_t teiheProtocol = PROTOCOL_TABER(teihe, 0, 0x0505);
const ModbusDeviceHead_t TeiHeDevice = {
	&UART_9600_N1,
	&teihe[0],
	&teiheProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t fyg[] = {
	{0x03, 0, 67},
};
static const ModbusGetCmdTab_t fygProtocol = PROTOCOL_TABER(fyg, 0, 0x0509);
const ModbusDeviceHead_t FygDevice = {
	&UART_9600_N1,
	&fyg[0],
	&fygProtocol,
	null,
	null,
}; 


static const ModbusGetCmd_t _0510[] = {
	{0x02, 0x0000, 0x0007},
	{0x04, 0x0000, 0x000C},
	{0x04, 0x0022, 0x003B},
	{0x04, 0x00F7, 0x00F8},
	{0x04, 0xFF0A, 0xFF12},
	{0x04, 0xFF8D, 0xFFA6},
	{0x04, 0xFFA7, 0xFFC6},
};
static const ModbusGetCmdTab_t _0510Protocol = PROTOCOL_TABER(_0510, 0, 0x0510);
const ModbusDeviceHead_t XintronDevice = {
	&UART_9600_N1,
	&_0510[0],
	&_0510Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _050B[] = {
	{0x03, 1999, 2048},
};
static const ModbusGetCmdTab_t _050BProtocol = PROTOCOL_TABER(_050B, 0, 0x050B);
const ModbusDeviceHead_t _050BDevice = {
	&UART_4800_N1,
	&_050B[0],
	&_050BProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _050C[] = {
    {0x03, 0 ,49},
    {0x03, 69 ,70},
    {0x03, 80 ,150},
};
static const ModbusGetCmdTab_t _050CProtocol = PROTOCOL_TABER(_050C, 0, 0x050C);
const ModbusDeviceHead_t _050CDevice = {
	&UART_9600_N1,
	&_050C[0],
	&_050CProtocol,
	null,
	null,
};

static const ModbusGetCmd_t _051B[] = {
    {0x02, 00  ,00},
    {0x02, 01  ,01},
    {0x02, 02  ,02},
    {0x02, 03  ,03},
    {0x02, 04  ,04},
    {0x02, 05  ,05},
    {0x02, 06  ,06},
    {0x02, 07  ,07},
    {0x04, 8   ,69},
    {0x04, 100 ,115},
};
static const ModbusGetCmdTab_t _051BProtocol = PROTOCOL_TABER(_051B, 1, 0x051B);
const ModbusDeviceHead_t _051BDevice = {
	&UART_9600_N1,
	&_051B[0],
	&_051BProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0707[] = {
	{0x03, 200, 220},
};
static const ModbusGetCmdTab_t _0707Protocol = PROTOCOL_TABER(_0707, 0, 0x0707);
const ModbusDeviceHead_t _0707Device = {
	&UART_9600_N1,
	&_0707[0],
	&_0707Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0708[] = {
	{0x03, 296, 326},
	{0x03, 400, 426},
	{0x03, 500, 526},
	{0x03, 600, 626},
	{0x03, 700, 726},
	{0x03, 800, 826},
	{0x03, 900, 926},
	{0x03, 1000, 1026},
};
static const ModbusGetCmdTab_t _0708Protocol = PROTOCOL_TABER(_0708, 0, 0x0708);
const ModbusDeviceHead_t _0708Device = {
	&UART_9600_N1,
	&_0708[0],
	&_0708Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0804[] = {
	{0x03, 0x0000, 0x0005},
};
static const ModbusGetCmdTab_t _0804Protocol = PROTOCOL_TABER(_0804, 0, 0x0804);
const ModbusDeviceHead_t _0804Device = {
	&UART_9600_N1,
	&_0804[0],
	&_0804Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t poweroak3[] = {
	{0x03, 200, 224},
};
static const ModbusGetCmdTab_t poweroak3Protocol = PROTOCOL_TABER(poweroak3, 0, 0x0701);
const ModbusDeviceHead_t PoweroakDevice3 = {
	&UART_9600_N1,
	&poweroak3[0],
	&poweroak3Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t poweroak4[] = {
	{0x03, 120, 161},
};
static const ModbusGetCmdTab_t poweroak4Protocol = PROTOCOL_TABER(poweroak4, 0, 0x0801);
const ModbusDeviceHead_t PoweroakDevice4 = {
	&UART_9600_N1,
	&poweroak4[0],
	&poweroak4Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0904[] = {
	{0x03, 0x0003 ,0x0005},
    {0x03, 0x00D0 ,0x00ED},
    {0x03, 0x0127 ,0x012B},
    {0x03, 0x015E ,0x0183},
};
static const ModbusGetCmdTab_t _0904Protocol = PROTOCOL_TABER(_0904, 0, 0x0904);
const ModbusDeviceHead_t VoltronicDevice = {
	&UART_9600_N1,
	&_0904[0],
	&_0904Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0906[] = {
	{0x03, 0x0003 ,0x0005},
    {0x03, 0x00D0 ,0x00EE},
    {0x03, 0x0197 ,0x01A0},
    {0x03, 0x03EB ,0x03F1},
};
static const ModbusGetCmdTab_t _0906Protocol = PROTOCOL_TABER(_0906, 0, 0x0906);
const ModbusDeviceHead_t VoltronicDevice2 = {
	&UART_9600_N1,
	&_0906[0],
	&_0906Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0907[] = {
	{0x03, 0x0000 ,0x0001},
    {0x03, 0x00BC ,0x00BC},
    {0x03, 0x00D8 ,0x00F8},
    {0x03, 0x0197 ,0x01A0},
    {0x03, 0x01C0 ,0x01CD},
    {0x03, 0x030D ,0x030E},
    {0x03, 0x031E ,0x0321},
    {0x03, 0x0360 ,0x0360},
    {0x03, 0x037E ,0x037F},
    {0x03, 0x03D8 ,0x03D9},
    {0x03, 0x03EB ,0x03FC},
    {0x03, 0x04B3 ,0x04B7},
    {0x03, 0x04D9 ,0x04DA},
};
static const ModbusGetCmdTab_t _0907Protocol = PROTOCOL_TABER(_0907, 0, 0x0907);
const ModbusDeviceHead_t VoltronicDevice3 = {
	&UART_9600_N1,
	&_0907[0],
	&_0907Protocol,
	null,
	null,
}; 


static const ModbusGetCmd_t _0919[] = {
    {0x03, 0x00D0 ,0x00D0},
    {0x03, 0x01FE ,0x0219},
    {0x03, 0x0197 ,0x0198},
    {0x03, 0x019B ,0x01A0},
    {0x03, 0x021B ,0x021B},
    {0x03, 0x021C ,0x021C},
    {0x03, 0x024A ,0x024A},
    {0x03, 0x0223 ,0x0223},
    {0x03, 0x03E0 ,0x03E1},
    {0x03, 0x03EB ,0x03F5},
    {0x03, 0x01E7 ,0x01FD},
};
static const ModbusGetCmdTab_t _0919Protocol = PROTOCOL_TABER(_0919, 1, 0x0919);
const ModbusDeviceHead_t _0919Device = {
	&UART_9600_N1,
	&_0919[0],
	&_0919Protocol,
	null,
	null,
};

static const ModbusGetCmd_t _0905[] = {
	{0x04,    0,   23},
    {0x04,   35,   43},
    {0x04,   50,   91},
    {0x04,   95,  127},
    {0x04,  981,  988},
    {0x04, 3000, 3024},
};
static const ModbusGetCmdTab_t _0905Protocol = PROTOCOL_TABER(_0905, 0, 0x0905);
const ModbusDeviceHead_t NicestDevice = {
	&UART_9600_N1,
	&_0905[0],
	&_0905Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0909[] = {
	{0x02, 2501 ,2548},
	{0x04, 4501 ,4605}
};
static const ModbusGetCmdTab_t _0909Protocol = PROTOCOL_TABER(_0909, 0, 0x0909);
const ModbusDeviceHead_t KelongDevice = {
	&UART_9600_N1,
	&_0909[0],
	&_0909Protocol,
	null,
	null,
}; 
//锟斤拷锟绞碉拷锟斤拷-MODBU通讯锟斤拷协锟斤拷
static const ModbusGetCmd_t _090F[] = {
	{0x04,   1,   69},
    {0x04,  1001,  1036},
    {0x04,  1043,  1059},
    {0x04,  2101,  2123},
    {0x04,  2151,  2178},
    {0x04,  2221,  2235},
	{0x04,  3008,  3015},
    {0x04,  3066,  3101},
};
static const ModbusGetCmdTab_t _090FProtocol = PROTOCOL_TABER(_090F, 0, 0x090F);
const ModbusDeviceHead_t _090FDevice = {
	&UART_9600_N1,
	&_090F[0],
	&_090FProtocol,
	null,
	null,
}; 
static const ModbusGetCmd_t _0910[] = {
	{0x04,    0,   23},
    {0x04,   35,   43},
    {0x04,   50,   91},
    {0x04,   95,  127},
    {0x04,  981,  988},
    {0x04, 3000, 3024},
};
static const ModbusGetCmdTab_t _0910Protocol = PROTOCOL_TABER(_0910, 0, 0x0910);
const ModbusDeviceHead_t CanroonDevice = {
	&UART_9600_N1,
	&_0910[0],
	&_0910Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0921[] = {
	{0x03, 0x0001, 0x0055},
	{0x03, 0x0068, 0x006A},
	{0x03, 0x0070, 0x0072},
};
static const ModbusGetCmdTab_t _0921Protocol = PROTOCOL_TABER(_0921, 0, 0x0921);
const ModbusDeviceHead_t ChuangChiDevice = {
	&UART_9600_N1,
	&_0921[0],
	&_0921Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0924[] = {
	{0x03, 0x0000, 0x0010},
};
static const ModbusGetCmdTab_t _0924Protocol = PROTOCOL_TABER(_0924, 0, 0x0924);
const ModbusDeviceHead_t _0924Device = {
	&UART_9600_N1,
	&_0924[0],
	&_0924Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _092A[] = {
	{0x03, 30001, 30019},
	{0x03, 30021, 30053},
};
static const ModbusGetCmdTab_t _092AProtocol = PROTOCOL_TABER(_092A, 0, 0x092A);
const ModbusDeviceHead_t _092ADevice = {
	&UART_9600_N1,
	&_092A[0],
	&_092AProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _092D[] = {
    {0x03, 3200 ,3216},
    {0x04, 3000 ,3001},
    {0x04, 3012 ,3013},
    {0x04, 3024 ,3025},
    {0x04, 3036 ,3056},
    {0x04, 3061 ,3075},
    {0x04, 3080 ,3138},
    {0x04, 3159 ,3159},
    {0x04, 3162 ,3167},
    {0x04, 3174 ,3182},
    {0x04, 3189 ,3207},
};
static const ModbusGetCmdTab_t _092DProtocol = PROTOCOL_TABER(_092D, 0, 0x092D);
const ModbusDeviceHead_t _092DDevice = {
	&UART_9600_N1,
	&_092D[0],
	&_092DProtocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0912[] = {
	{0x04,    0,   23},
    {0x04,   35,   43},
    {0x04,   50,   91},
    {0x04,   95,  127},
    {0x04,  981,  988},
    {0x04, 3000, 3073},
};
static const ModbusGetCmdTab_t _0912Protocol = PROTOCOL_TABER(_0912, 0, 0x0912);
const ModbusDeviceHead_t Nicest0912Device = {
	&UART_38400_N1,
	&_0912[0],
	&_0912Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0915[] = {
    {0x02, 0x0001 ,0x0001},
    {0x02, 0x0002 ,0x0002},
    {0x02, 0x0003 ,0x0003},
    {0x02, 0x0004 ,0x0004},
    {0x02, 0x0005 ,0x0005},
    {0x02, 0x0006 ,0x0006},
    {0x02, 0x0007 ,0x0007},
    {0x02, 0x0008 ,0x0008},
    {0x02, 0x0009 ,0x0009},
    {0x02, 0x000A ,0x000A},
    {0x02, 0x000B ,0x000B},
    {0x02, 0x000C ,0x000C},
    {0x02, 0x000D ,0x000D},
	{0x04, 0x1000 ,0x1009},
};
static const ModbusGetCmdTab_t _0915Protocol = PROTOCOL_TABER(_0915, 1, 0x0915);
const ModbusDeviceHead_t _0915Device = {
	&UART_9600_N1,
	&_0915[0],
	&_0915Protocol,
	null,
	null,
};

static const ModbusGetCmd_t _0925[] = {
	{0x03, 4501, 4531},
	{0x03, 4532, 4561},
};
static const ModbusGetCmdTab_t _0925Protocol = PROTOCOL_TABER(_0925, 0, 0x0925);
const ModbusDeviceHead_t Nicest0925Device = {
	&UART_2400_N1,
	&_0925[0],
	&_0925Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0927[] = {
	{0x03, 00, 19},
	{0x03, 59, 64},
	{0x03, 86, 116},
	{0x03, 150, 191},
	{0x03, 200, 249},
	{0x03, 283, 326},
};
static const ModbusGetCmdTab_t _0927Protocol = PROTOCOL_TABER(_0927, 0, 0x0927);
const ModbusDeviceHead_t DEYE0927Device = {
	&UART_9600_N1,
	&_0927[0],
	&_0927Protocol,
	null,
	null,
}; 
static const ModbusGetCmd_t _0928[] = {
	{0x04, 0x0001, 0x0029},
	{0x04, 0x0074, 0x008C},
	{0x04, 0x03AE, 0x03CC},
	{0x04, 0x03D0, 0x03D3},
	{0x04, 0x0A00, 0x0A14},
};

static const ModbusGetCmdTab_t _0928Protocol = PROTOCOL_TABER(_0928, 0, 0x0928);
const ModbusDeviceHead_t BOS0928Device = {
	&UART_19200_N1,
	&_0928[0],
	&_0928Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0935[] = {
	{0x03, 33000, 33040},
	{0x03, 33049, 33058},
	{0x03, 33071, 33084},
	{0x03, 33091, 33106},
	{0x03, 33115, 33150},
	{0x03, 33161, 33180},
	{0x03, 33250, 33286},
};
static const ModbusGetCmdTab_t _0935Protocol = PROTOCOL_TABER(_0935, 0, 0x0935);
const ModbusDeviceHead_t _0935Device = {
	&UART_9600_N1,
	&_0935[0],
	&_0935Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0936[] = {
	{0x04, 3000, 3031},
	{0x04, 3036, 3053},
	{0x04, 3079, 3144},
};
static const ModbusGetCmdTab_t _0936Protocol = PROTOCOL_TABER(_0936, 0, 0x0936);
const ModbusDeviceHead_t _0936Device = {
	&UART_9600_N1,
	&_0936[0],
	&_0936Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0937[] = {
	{0x02, 0x2000, 0x2000},
	{0x02, 0x200C, 0x200C},
	{0x04, 0x3100, 0x3103},
	{0x04, 0x310C, 0x3111},
	{0x04, 0x3200, 0x3202},
	{0x04, 0x3302, 0x3313},
	{0x04, 0x331A, 0x331C},
	{0x04, 0x3000, 0x3007},
};
static const ModbusGetCmdTab_t _0937Protocol = PROTOCOL_TABER(_0937, 0, 0x0937);
const ModbusDeviceHead_t _0937Device = {
	&UART_115200_N1,
	&_0937[0],
	&_0937Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _0938[] = {
	{0x03, 00, 19},
	{0x03, 59, 64},
	{0x03, 70, 83},
	{0x03, 86, 116},
	{0x03, 150, 191},
	{0x03, 195, 195},
	{0x03, 200, 249},
	{0x03, 283, 326},
};
static const ModbusGetCmdTab_t _0938Protocol = PROTOCOL_TABER(_0938, 0, 0x0938);
const ModbusDeviceHead_t _0938Device = {
	&UART_115200_N1,
	&_0938[0],
	&_0938Protocol,
	null,
	null,
};

static const ModbusGetCmd_t _0939[] = {
    {0x03, 0x000B ,0x000B },
    {0x03, 0x0100 ,0x0102 },
    {0x03, 0x0107 ,0x010F },
    {0x03, 0x0204 ,0x0211 },
    {0x03, 0x0212 ,0x0225 },
    {0x03, 0xE004 ,0xE004 },
    {0x03, 0xE116 ,0xE116 },
    {0x03, 0xF000 ,0xF006 },
    {0x03, 0xF007 ,0xF00D },
    {0x03, 0xF00E ,0xF014 },
    {0x03, 0xF015 ,0xF01B },
    {0x03, 0xF01C ,0xF022 },
    {0x03, 0xF023 ,0xF029 },
    {0x03, 0xF02D ,0xF031 },
    {0x03, 0xF034 ,0xF03D },
    {0x03, 0xF040 ,0xF04B },
};
static const ModbusGetCmdTab_t _0939Protocol = PROTOCOL_TABER(_0939, 1, 0x0939);
const ModbusDeviceHead_t _0939Device = {
	&UART_9600_N1,
	&_0939[0],
	&_0939Protocol,
	null,
	null,
}; 

static const ModbusGetCmd_t _093A[] = {
	{0x03, 20001, 20016},
	{0x03, 20099, 20132},
	{0x03, 25201, 25274},
	{0x03, 15001, 15008},
	{0x03, 15111, 15122},
	{0x03, 15201, 15221},
};
static const ModbusGetCmdTab_t _093AProtocol = PROTOCOL_TABER(_093A, 0, 0x093A);
const ModbusDeviceHead_t _093ADevice = {
	&UART_115200_N1,
	&_093A[0],
	&_093AProtocol,
	null,
	null,
};

static const ModbusGetCmd_t _093B[] = {
	{0x03, 0x1041, 0x1041},
	{0x03, 0x1215, 0x121B},
	{0x03, 0x1233, 0x1243},
	{0x03, 0x1612, 0x167C},
	{0x03, 0x1690, 0x1693},
	{0x03, 0x1698, 0x169B},
	{0x03, 0x16A0, 0x16A7},
	{0x03, 0x223E, 0x223F},
	{0x03, 0x224E, 0x224E},
	{0x03, 0x2250, 0x2250},
	{0x03, 0x2260, 0x228B},
	{0x03, 0x2297, 0x229B},
};
static const ModbusGetCmdTab_t _093BProtocol = PROTOCOL_TABER(_093B, 0, 0x093B);
const ModbusDeviceHead_t _093BDevice = {
	&UART_9600_N1,
	&_093B[0],
	&_093BProtocol,
	null,
	null,
};

static const ModbusGetCmd_t _093C[] = {
	{0x03, 1000, 1020},
	{0x03, 2000, 2041},
};
static const ModbusGetCmdTab_t _093CProtocol = PROTOCOL_TABER(_093C, 0, 0x093C);
const ModbusDeviceHead_t _093CDevice = {
	&UART_9600_N1,
	&_093C[0],
	&_093CProtocol,
	null,
	null,
};

static const ModbusGetCmd_t _093D[] = {
	{0x03, 1000, 1020},
	{0x03, 2000, 2057},
};
static const ModbusGetCmdTab_t _093DProtocol = PROTOCOL_TABER(_093D, 0, 0x093D);
const ModbusDeviceHead_t _093DDevice = {
	&UART_9600_N1,
	&_093D[0],
	&_093DProtocol,
	null,
	null,
};


static const ModbusGetCmd_t _0940[] = {
	{0x03, 2, 2},
	{0x03, 3, 3},
	{0x03, 208, 208},
	{0x03, 275, 281},
	{0x03, 407, 408},
	{0x03, 411, 416},
	{0x03, 448, 461},
	{0x03, 487, 568},
	{0x03, 944, 972},
};
static const ModbusGetCmdTab_t _0940Protocol = PROTOCOL_TABER(_0940, 0, 0x0940);
const ModbusDeviceHead_t _0940Device = {
	&UART_115200_N1,
	&_0940[0],
	&_0940Protocol,
	null,
	null,
};

static const ModbusGetCmd_t _0941[] = {
    {03,   25000,  25040},
};
static const ModbusGetCmdTab_t _0941Protocol = PROTOCOL_TABER(_0941, 0, 0x0941);
const ModbusDeviceHead_t _0941Device = {
    &UART_9600_N1,
    &_0941[0],
    &_0941Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0942[] = {
    {03,   0x2632,  0x2632},
    {03,   0x2640,  0x2659},
    {03,   0x2664,  0x2666},
    {03,   0x2668,  0x2669},
    {03,   0x266D,  0x266D},
    {03,   0x2670,  0x2670},
    {03,   0x2673,  0x2674},
    {03,   0x2677,  0x267B},
    {03,   0x267D,  0x267F},
};
static const ModbusGetCmdTab_t _0942Protocol = PROTOCOL_TABER(_0942, 0, 0x0942);
const ModbusDeviceHead_t _0942Device = {
    &UART_9600_N1,
    &_0942[0],
    &_0942Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0943[] = {
    {0x03, 53000 ,53043},
    {0x03, 53100 ,53106},
    {0x03, 53200 ,53227},
    {0x03, 53239 ,53248},
    {0x03, 53414 ,53415},
    {0x03, 53579 ,53589},
    {0x03, 53590 ,53593},
    {0x03, 53622 ,53623},
    {0x03, 53663 ,53664},
};
static const ModbusGetCmdTab_t _0943Protocol = PROTOCOL_TABER(_0943, 1, 0x0943);
const ModbusDeviceHead_t _0943Device = {
	&UART_9600_N1,
	&_0943[0],
	&_0943Protocol,
	null,
	null,
};


static const ModbusGetCmd_t _0944[] = {
    {03,   10000,  10010},
    {03,   10101,  10126},
    {03,   16001,  16002},
    {03,   15201,  15224},
};
static const ModbusGetCmdTab_t _0944Protocol = PROTOCOL_TABER(_0944, 0, 0x0944);
const ModbusDeviceHead_t _0944Device = {
    &UART_9600_N1,
    &_0944[0],
    &_0944Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0945[] = {
    {03,   3200,  3216},
    {04,   3000,  3167},
};
static const ModbusGetCmdTab_t _0945Protocol = PROTOCOL_TABER(_0945, 0, 0x0945);
const ModbusDeviceHead_t _0945Device = {
    &UART_9600_N1,
    &_0945[0],
    &_0945Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _094B[] = {
    {0x02, 0x5000 ,0x5000},
    {0x02, 0x5001 ,0x5001},
    {0x02, 0x5002 ,0x5002},
    {0x02, 0x5003 ,0x5003},
    {0x02, 0x5004 ,0x5004},
    {0x02, 0x5005 ,0x5005},
    {0x02, 0x5006 ,0x5006},
    {0x02, 0x5007 ,0x5007},
    {0x02, 0x5008 ,0x5008},
    {0x02, 0x5009 ,0x5009},
    {0x02, 0x500A ,0x500A},
    {0x02, 0x500C ,0x500C},
    {0x02, 0x500D ,0x500D},
    {0x02, 0x500E ,0x500E},
    {0x02, 0x500F ,0x500F},
    {0x02, 0x5010 ,0x5010},
    {0x02, 0x5011 ,0x5011},
    {0x02, 0x5012 ,0x5012},
    {0x02, 0x5013 ,0x5013},
    {0x02, 0x5014 ,0x5014},
    {0x02, 0x5015 ,0x5015},
    {0x02, 0x5016 ,0x5016},
    {0x02, 0x5017 ,0x5017},
    {0x02, 0x5018 ,0x5018},
    {0x02, 0x5019 ,0x5019},
    {0x02, 0x501A ,0x501A},
    {0x02, 0x501B ,0x501B},
    {0x02, 0x501C ,0x501C},
    {0x04, 0x3000 ,0x3015},
};
static const ModbusGetCmdTab_t _094BProtocol = PROTOCOL_TABER(_094B, 1, 0x094B);
const ModbusDeviceHead_t _094BDevice = {
    &UART_9600_N1,
    &_094B[0],
    &_094BProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _094C[] = {
    {0x03, 32 ,62},
    {0x03, 14 ,17},
    {0x03, 128 ,139},
};
static const ModbusGetCmdTab_t _094CProtocol = PROTOCOL_TABER(_094C, 0, 0x094C);
const ModbusDeviceHead_t _094CDevice = {
	&UART_9600_N1,
	&_094C[0],
	&_094CProtocol,
	null,
	null,
};

static const ModbusGetCmd_t _094E[] = {
    {0x03,0x0000,0x0032},
    {0x03,0x004C,0x0071},
    {0x03,0x03E9,0x0417},
    {0x03,0x0418,0x0443},
};
static const ModbusGetCmdTab_t _094EProtocol = PROTOCOL_TABER(_094E, 1, 0x094E);
const ModbusDeviceHead_t _094EDevice = {
	&UART_9600_N1,
	&_094E[0],
	&_094EProtocol,
	null,
	null,
};

static const ModbusGetCmd_t _094F[] = {
    {0x03, 32  ,67},
    {0x03, 80  ,88},
    {0x03, 12  ,12},
    {0x03, 14  ,17},
    {0x03, 128 ,139},
};
static const ModbusGetCmdTab_t _094FProtocol = PROTOCOL_TABER(_094F, 0, 0x094F);
const ModbusDeviceHead_t _094FDevice = {
	&UART_9600_N1,
	&_094F[0],
	&_094FProtocol,
	null,
	null,
};

static const ModbusGetCmd_t _0965[] = {
    {0x02, 2501 ,2502},
    {0x02, 2505 ,2506},
    {0x02, 2507 ,2508},
    {0x02, 2509 ,2516},
    {0x04, 4501 ,4531},
    {0x04, 4551 ,4561},
    {0x04, 4601 ,4625},
    {0x04, 4651 ,4654},
    {0x04, 4661 ,4676},
    {0x04, 4701 ,4796},
    {0x04, 4800 ,4820},
    {0x04, 4825 ,4872},
};
static const ModbusGetCmdTab_t _0965Protocol = PROTOCOL_TABER(_0965, 1, 0x0965);
const ModbusDeviceHead_t _0965Device = {
	&UART_9600_N1,
	&_0965[0],
	&_0965Protocol,
	null,
	null,
};

static const ModbusGetCmd_t _0971[] = {
    {0x04, 0x2000 ,0x200F},
    {0x03, 0x0200 ,0x0255},
};
static const ModbusGetCmdTab_t _0971Protocol = PROTOCOL_TABER(_0971, 0, 0x0971);
const ModbusDeviceHead_t _0971Device = {
	&UART_9600_N1,
	&_0971[0],
	&_0971Protocol,
	null,
	null,
};

static const ModbusGetCmd_t _0C05[] = {
    {0x03, 0    ,   11},
    {0x03, 100  ,   145},
    {0x03, 3504 ,   3504},
    {0x03, 3520 ,   3521},
};
static const ModbusGetCmdTab_t _0C05Protocol = PROTOCOL_TABER(_0C05, 0, 0x0C05);
const ModbusDeviceHead_t _0C05Device = {
    &UART_9600_N1,
    &_0C05[0],
    &_0C05Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0C0E[] = {
    {0x03,   0x0000,  0x0001},
    {0x03,   0x0008,  0x000E},
    {0x03,   0x0010,  0x0011},
    {0x03,   0x0015,  0x0015},
};
static const ModbusGetCmdTab_t _0C0E1Protocol = PROTOCOL_TABER(_0C0E, 0, 0x0C0E);
const ModbusDeviceHead_t _0C0EDevice = {
    &UART_9600_N1,
    &_0C0E[0],
    &_0C0E1Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0E01[] = {
    {04,   25000,  25045},
};
static const ModbusGetCmdTab_t _0E01Protocol = PROTOCOL_TABER(_0E01, 0, 0x0E01);
const ModbusDeviceHead_t _0E01Device = {
    &UART_9600_N1,
    &_0E01[0],
    &_0E01Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0E0E[] = {
    {04,   25000,  25045},
};
static const ModbusGetCmdTab_t _0E0EProtocol = PROTOCOL_TABER(_0E0E, 0, 0x0E0E);
const ModbusDeviceHead_t _0E0EDevice = {
    &UART_9600_N1,
    &_0E0E[0],
    &_0E0EProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _0272[] = {
    {03, 0x0000 ,0x0063},
};
static const ModbusGetCmdTab_t _0272Protocol = PROTOCOL_TABER(_0272, 0, 0x0272);
const ModbusDeviceHead_t _0272Device = {
    &UART_9600_N1,
    &_0272[0],
    &_0272Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0273[] = {
    {03, 0x1000 ,0x1060},
    {03, 0x2000 ,0x2000},
    {03, 0x2027 ,0x2027},
    {03, 0x204E ,0x204E},
    {03, 0x2075 ,0x2075},
    {03, 0x209C ,0x209C},
    {03, 0x20C3 ,0x20C3},
    {03, 0x20EA ,0x20EA},
    {03, 0x2111 ,0x2111},
    {03, 0x2138 ,0x2138},
    {03, 0x215F ,0x215F},
    {03, 0x2186 ,0x2186},
    {03, 0x21AD ,0x21AD},
    {03, 0x21D4 ,0x21D4},
    {03, 0x21FB ,0x21FB},
};
static const ModbusGetCmdTab_t _0273Protocol = PROTOCOL_TABER(_0273, 0, 0x0273);
const ModbusDeviceHead_t _0273Device = {
    &UART_9600_N1,
    &_0273[0],
    &_0273Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0274[] = {
  {0x04, 15069 ,15132},
};
static const ModbusGetCmdTab_t _0274Protocol = PROTOCOL_TABER(_0274, 0, 0x0274);
const ModbusDeviceHead_t _0274Device = {
    &UART_9600_N1,
    &_0274[0],
    &_0274Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0275[] = {
    {0x03, 0x1000 ,0x102E},
    {0x03, 0x1200 ,0x1205},
    {0x03, 0x6000 ,0x6007},
};
static const ModbusGetCmdTab_t _0275Protocol = PROTOCOL_TABER(_0275, 0, 0x0275);
const ModbusDeviceHead_t _0275Device = {
    &UART_9600_N1,
    &_0275[0],
    &_0275Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0276[] = {
    {0x03, 0x1612 ,0x1617},
    {0x03, 0x167C ,0x167D},
    {0x03, 0x168E ,0x16AE},
    {0x03, 0x1800 ,0x1800},
};
static const ModbusGetCmdTab_t _0276Protocol = PROTOCOL_TABER(_0276, 0, 0x0276);
const ModbusDeviceHead_t _0276Device = {
    &UART_9600_N1,
    &_0276[0],
    &_0276Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0278[] = {
    {3, 30000 ,30006},
    {3, 30010 ,30047},
    {3, 30100 ,30124},
    {3, 30131 ,30147},
};
static const ModbusGetCmdTab_t _0278Protocol = PROTOCOL_TABER(_0278, 0, 0x0278);
const ModbusDeviceHead_t _0278Device = {
    &UART_9600_N1,
    &_0278[0],
    &_0278Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _0279[] = {
    {0x04, 4989 ,5048},
    {0x04, 5070 ,5103},
    {0x04, 5112 ,5140},
    {0x04, 5145 ,5151},
    {0x04, 7012 ,7025},
};
static const ModbusGetCmdTab_t _0279Protocol = PROTOCOL_TABER(_0279, 0, 0x0279);
const ModbusDeviceHead_t _0279Device = {
    &UART_9600_N1,
    &_0279[0],
    &_0279Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _027A[] = {
    {0x02, 0x0001,0x0004},
    {0x04, 0x0001,0x0029},
};

static const ModbusGetCmdTab_t _027AProtocol = PROTOCOL_TABER(_027A, 1, 0x027A);
const ModbusDeviceHead_t _027ADevice = {
    &UART_9600_N1,
    &_027A[0],
    &_027AProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _027B[] = {
    {0x02, 2501 ,2502},
    {0x02, 2505 ,2506},
    {0x02, 2507 ,2508},
    {0x02, 2509 ,2516},
    {0x04, 4501 ,4531},
    {0x04, 4551 ,4561},
    {0x04, 4601 ,4625},
    {0x04, 4651 ,4654},
    {0x04, 4661 ,4676},
    {0x04, 4701 ,4796},
    {0x04, 4800 ,4820},
    {0x04, 4825 ,4872},
};
static const ModbusGetCmdTab_t _027BProtocol = PROTOCOL_TABER(_027B, 1, 0x027B);
const ModbusDeviceHead_t _027BDevice = {
    &UART_9600_N1,
    &_027B[0],
    &_027BProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _027C[] = {
    {0x03, 0x0300 ,0x0332},
    {0x04, 0x0400 ,0x042A},
};
static const ModbusGetCmdTab_t _027CProtocol = PROTOCOL_TABER(_027C, 0, 0x027C);
const ModbusDeviceHead_t _027CDevice = {
    &UART_9600_N1,
    &_027C[0],
    &_027CProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _027D[] = {
    {0x04, 0 ,18},
};
static const ModbusGetCmdTab_t _027DProtocol = PROTOCOL_TABER(_027D, 0, 0x027D);
const ModbusDeviceHead_t _027DDevice = {
    &UART_9600_N1,
    &_027D[0],
    &_027DProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _027E[] = {
    {0x04, 2999 ,3077},
    {0x04, 3229 ,3248},
};
static const ModbusGetCmdTab_t _027EProtocol = PROTOCOL_TABER(_027E, 0, 0x027E);
const ModbusDeviceHead_t _027EDevice = {
    &UART_9600_N1,
    &_027E[0],
    &_027EProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _027F[] = {
    {0x03, 0x1001 ,0x1026},
    {0x03, 0x1037 ,0x103D},
    {0x03, 0x1A10 ,0x1A17},
    {0x03, 0x1A44 ,0x1A48},
};
static const ModbusGetCmdTab_t _027FProtocol = PROTOCOL_TABER(_027F, 0, 0x027F);
const ModbusDeviceHead_t _027FDevice = {
    &UART_9600_N1,
    &_027F[0],
    &_027FProtocol,
    null,
    null,
};

static const ModbusGetCmd_t _091B[] = {
    {04, 0 ,140},
};
static const ModbusGetCmdTab_t _091BProtocol = PROTOCOL_TABER(_091B, 0, 0x091B);
const ModbusDeviceHead_t _091BDevice = {
    &UART_115200_N1,
    &_091B[0],
    &_091BProtocol,
    null,
    null,
};

static const ModbusGetCmdTab_t _0204Protocol = PROTOCOL_TABER(_0E0E, 0, 0x0204);
const ModbusDeviceHead_t _0204Device = {
    &UART_9600_N1,
    &_0E0E[0],
    &_0204Protocol,
    null,
    null,
};

static const ModbusGetCmd_t _1001[] = {
    {0x03, 0x000A ,0x0026},
    {0x03, 0x00FD ,0x012D},
    {0x03, 0xE000 ,0xE021},
    {0x03, 0xE080 ,0xE0B7},
};
static const ModbusGetCmdTab_t _1001Protocol = PROTOCOL_TABER(_1001, 0, 0x1001);
const ModbusDeviceHead_t _1001Device = {
    &UART_9600_N1,
    &_1001[0],
    &_1001Protocol,
    null,
    null,
};

/******************************************************************************/

