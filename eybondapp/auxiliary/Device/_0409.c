/**************Copyright(C)��2015-2026��Shenzhen Eybond Co., Ltd.***************
  *@brief   : anti-reflux.c 
  *@notes   : 2018.11.06 CGQ 
*******************************************************************************/
#include "antiReflux.h"
#include "Debug.h"
#include "device.h"
#include "modbus.h"
#include "r_stdlib.h"
#include "Swap.h"
#include "Protocol.h"
#include "log.h"

static void  meter_0409Get(u16_t deviceCount, Device_t *dev, MeterValue_t *meter);

static const ModbusGetCmd_t _0409[] = {
    {0x03, 47, 48},
};

const AntiReflux_t Device_0409 = {
    PROTOCOL_TABER(_0409, 0, 0x0409), meter_0409Get
};


static void  meter_0409Get(u16_t deviceCount, Device_t *dev, MeterValue_t *meter)
{
    static s32_t val = 0;
    DeviceCmd_t *cmd = list_nextData(&dev->cmdList, null);


    val += float2int((u32_t)((cmd->ack.payload[3]<<24) | (cmd->ack.payload[4]<<16) |(cmd->ack.payload[5]<<8) | (cmd->ack.payload[6])));
    val >>= 1;
    meter->state = 1;
    meter->power = val;
     

    APP_DEBUG("Meter power %d w\r\n", meter->power);
}

// void test(void)
// {
//      u32_t buf[] = {0x43588990, 0x4358D298, 0x4358C418, 0x43BBA681,
//                     0x43BBBFD8, 0x43BBA039, 0x4006391C,  0x40046C00,
//                     0x400E9E00, 0x43CAF4FD, 0x43C941AE,  0x43E3476E, 0x449DE178, 0x43373851,  0x433144BE,
//                     0x4303B8BD, 0x43F61321, 0x43DEACAC, 0x43DBE8DC,  0x43ECA117, 0x44A9715E, 0x3F6957CC,  0x3F6A4BA4,  0x3F75E9EC, 0x3F6E9092, 0x4247FE80, 0x3F970A3E};

//     int i = 0;
//     float fo;
//     for (i = 0; i < sizeof(buf)/sizeof(buf[0]); i++)
//     {
//         APP_DEBUG("Meter test %d w\r\n", float2int(buf[i]));
//     }

//     r_memcpy(&fo, &buf[0], sizeof(fo));
//     APP_DEBUG("foalt : %f", fo);

//     fo = (float)(buf[0]);
//     APP_DEBUG("foalt : %f", fo);
//     fo = 1.0010;
//     APP_DEBUG("foalt : %f", fo);
// }

/******************************************************************************/

