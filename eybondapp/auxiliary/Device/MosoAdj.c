/**************Copyright(C)��2015-2026��Shenzhen Eybond Co., Ltd.***************
  *@brief   : anti-reflux.c 
  *@notes   : 2018.11.06 CGQ 
*******************************************************************************/
#include "antiReflux.h"
#include "typedef.h"
#include "Debug.h"
#include "device.h"
#include "modbus.h"
#include "ModbusDevice.h"
#include "r_stdlib.h"
#include "Swap.h"
#include "Protocol.h"
#include "log.h"

#define ADJ_ADDR_0212    (112)
#define ADJ_ADDR_0211    (0xC019)

typedef struct
{
    int cnt;
}MosoDeviceInfo_t;

static void invert_0212(u16_t deviceCount, Device_t *dev, MeterValue_t *meter);
static void invert_0211(u16_t deviceCount, Device_t *dev, MeterValue_t *meter);
static void setingAck(Device_t *dev);

static const ModbusGetCmd_t _0212[] = {
    {0x04, 0, 0},
    {0x03, 28, 28},
    {0x03, ADJ_ADDR_0212, ADJ_ADDR_0212},
    {0x03, 05, 06}
};

static const ModbusGetCmd_t _0211[] = {
    {0x03, 0xc010, 0xc012},
    {0x03, 0xC018, ADJ_ADDR_0211},
};


const AntiReflux_t MOSO_0212 = {
    PROTOCOL_TABER(_0212, 0, 0x0212), invert_0212
};

const AntiReflux_t MOSO_0211 = {
    PROTOCOL_TABER(_0211, 0, 0x0211), invert_0211
};


/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void invert_0212(u16_t deviceCount, Device_t *dev, MeterValue_t *meter)
{
    int cmpValue = 0;
    u32_t poewrRating = 0;
    u32_t val = 0;
    u16_t enable = 0;
    u16_t percentage = 0;
    u16_t regaddr = 0;
    u16_t status = 0;

    AntiRefluxDeviceInfo_t *info = dev->explain;
    DeviceCmd_t *cmd = list_nextData(&dev->cmdList, null);
    
    while (cmd != null)
    {
        regaddr = cmd->cmd.payload[2]<<8 | cmd->cmd.payload[3];

        switch (regaddr)
        {
            case 0:
                status = cmd->ack.payload[3]<<8 | cmd->ack.payload[4];
                break;
            case 28:
                enable = cmd->ack.payload[3]<<8 | cmd->ack.payload[4];
                break;
            case ADJ_ADDR_0212:
                percentage = cmd->ack.payload[3]<<8 | cmd->ack.payload[4];
                percentage /= 10;
                break;
            case 05:
                poewrRating =  cmd->ack.payload[3]<<8 | cmd->ack.payload[4] | cmd->ack.payload[5]<<24 | cmd->ack.payload[6]<<16;
                poewrRating /= 1000;
                cmpValue = poewrRating>>1;
                break;
        }   
        cmd = list_nextData(&dev->cmdList, cmd);
    }

    APP_DEBUG("Moso addr %d.status: %d.percentage: %d.poewrRating: %d.\r\n ", info->addr, status, percentage, poewrRating);
    
    if (status != 0x0002)
    {}
    else if (meter->power < (-cmpValue))
    {
        int i = ((-meter->power) + cmpValue) / poewrRating;

        if (i > deviceCount)
        {
             i /= deviceCount;
        }
       
        APP_DEBUG("%d  power down %d.\r\n", info->addr, i);
        if (i != 0 &&  percentage > 1)
        {
            if (percentage > i)
            {
                val = percentage - i;          
            }
            else
            {
                val = 1;
            }
        }
    }
    else if (meter->power > cmpValue)
    {
        int i = (meter->power + cmpValue) / poewrRating;

        if (i > deviceCount)
        {
             i /= deviceCount;
        }
        
        APP_DEBUG("%d power up %d.\r\n",  info->addr, i);
        if (i != 0 && percentage < 100)
        {
            if ((percentage  + i) > 100)
            {
                val = 100;
            }
            else
            {
                val = percentage + i; 
            }
        }
    }

    if (val != 0)
    {
        Device_t *dev = list_nodeApply(sizeof(Device_t));
        MosoDeviceInfo_t *setInfo = memory_apply(sizeof(MosoDeviceInfo_t));
        log_saveAbnormal("Moso adj ", info->addr);
        log_saveAbnormal(" val: ", val);
        meter->state = 0;

        setInfo->cnt = 0;
        dev->cfg = null;
        dev->callBack = setingAck;
        dev->explain = setInfo;
        dev->type = DEVICE_TRANS;
        list_init(&dev->cmdList);        
        
        if (enable == 0)
        {
            Modbus_06SetCmd(&dev->cmdList, info->addr, 28, 0x01);
        }

        Modbus_06SetCmd(&dev->cmdList, info->addr, ADJ_ADDR_0212, val * 10);

        Device_inset(dev);
    }
}

static int powerRatingGet(u16_t val)
{
    typedef struct {
        u16_t enumVal;
        u16_t powerVal;
    }PowerTab;

    const static PowerTab powerTabel[] = {
        {101, 10000}, {102, 12000}, {103, 15000}, {104, 17000}, {105, 33000},
        {106,  5000}, {107,  6000}, {108,  7000}, {109,  8000}, {110, 36000},
        {111, 20000}, {112, 25000}, {113, 30000}, {114, 28000}, {115, 40000},
        {116, 33000}, {120, 50000}, 
    };

    int i; 
    int power = 10000;
     
    for (i = 0; i < sizeof(powerTabel)/sizeof(powerTabel[0]); i++)
    {
        if (powerTabel[i].enumVal == val)
        {
            power =  powerTabel[i].powerVal;
            break;
        }
    }

    APP_DEBUG("Device enum val %d, power %d. \r\n", val, power);
    return power;
}
/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void invert_0211(u16_t deviceCount, Device_t *dev, MeterValue_t *meter)
{
    int cmpValue = 0;
    u32_t poewrRating = 0;
    u32_t val = 0;
    u16_t enable = 0;
    u16_t percentage = 0;
    u16_t regaddr = 0;
    u16_t status = 0;
    AntiRefluxDeviceInfo_t *info = dev->explain;
    DeviceCmd_t *cmd = list_nextData(&dev->cmdList, null);
    
    while (cmd != null)
    {
        regaddr = cmd->cmd.payload[2]<<8 | cmd->cmd.payload[3];

        switch (regaddr)
        {
            case 0xc010:
                status = cmd->ack.payload[3]<<8 | cmd->ack.payload[4];
                poewrRating = powerRatingGet(cmd->ack.payload[7]<<8 | cmd->ack.payload[8]);
                poewrRating /= 100;
                cmpValue = poewrRating>>1;
                break;
            case 0xC018:
                enable = cmd->ack.payload[3]<<8 | cmd->ack.payload[4];
                percentage = cmd->ack.payload[5]<<8 | cmd->ack.payload[6];
                break;
        }   
        cmd = list_nextData(&dev->cmdList, cmd);
    }

    APP_DEBUG("Moso addr %d. status %d,percentage: %d.poewrRating: %d.\r\n ", info->addr, status, percentage, poewrRating);
    
    if (status != 0x0002)
    {}
    else if (meter->power < (-cmpValue))
    {
        int i = ((-meter->power) + cmpValue) / poewrRating;

        if (i > deviceCount)
        {
             i /= deviceCount;
        }
        APP_DEBUG("%d  power down %d.\r\n", info->addr, i);
        if (i != 0 &&  percentage > 1)
        {
            if (percentage > i)
            {
                val = percentage - i;          
            }
            else
            {
                val = 1;
            }
        }
    }
    else if (meter->power > cmpValue)
    {
        int i = (meter->power + cmpValue) / poewrRating;

        if (i > deviceCount)
        {
             i /= deviceCount;
        }
        
        APP_DEBUG("%d power up %d.\r\n",  info->addr, i);
        if (i != 0 && percentage < 100)
        {
            if ((percentage  + i) > 100)
            {
                val = 100;
            }
            else
            {
                val = percentage + i; 
            }
        }
    }

    if (val != 0)
    {
        Device_t *dev = list_nodeApply(sizeof(Device_t));
        MosoDeviceInfo_t *setInfo = memory_apply(sizeof(MosoDeviceInfo_t));
        log_saveAbnormal("Moso adj ", info->addr);
        log_saveAbnormal(" val: ", val);
        meter->state = 0;

        setInfo->cnt = 0;
        dev->cfg = null;
        dev->callBack = setingAck;
        dev->explain = setInfo;
        dev->type = DEVICE_TRANS;
        list_init(&dev->cmdList);        
        
        if (enable == 0)
        {
            Modbus_06SetCmd(&dev->cmdList, info->addr, 0xc012, 0x01);
        }

        Modbus_06SetCmd(&dev->cmdList, info->addr, ADJ_ADDR_0211, val);

        Device_inset(dev);
    }
}

static void setingAck(Device_t *dev)
{
    MosoDeviceInfo_t *info = (MosoDeviceInfo_t*)dev->explain;
    u8_t result = 0;

    list_trans(&dev->cmdList, modbusDataCheck, &result);

    if (result == 0)
    {
        log_save("Moso device power adj Ok");
        Device_remove(dev);
    }
    else if (info->cnt++ > 1)
    {
        Device_remove(dev);
    }
    else
    {
        log_save("Moso device power adj fail!!");
        dev->lock = 0;
    }
}

/******************************************************************************/

