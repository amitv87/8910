/**************Copyright(C)��2015-2026��Shenzhen Eybond Co., Ltd.***************
  *@brief   : anti-reflux.c 
  *@notes   : 2018.11.06 CGQ 
*******************************************************************************/
#include "antiReflux.h"
#include "ql_system.h"
#include "typedef.h"
#include "Debug.h"
#include "appTask.h"
#include "memory.h"
#include "SysPara.h"
#include "device.h"
#include "ModbusDevice.h"
#include "Modbus.h"
#include "r_stdlib.h"
#include "Swap.h"
#include "Protocol.h"
#include "MosoAdj.h"
#include "_0409.h"
#include "_040B.h"

typedef struct
{
    Device_t *dev;
}InvertList_t;

static MeterValue_t meterPower;
static ListHandler_t invertDevice;

static void Anti_reflux_init(void);
static u8_t checkDeviceOnline(void);
static void ArtiReflux_InvDataAck(Device_t *dev);
static u8_t invterLockStatus(void *load, void *val);
static u8_t invterUnlock(void *load, void *val);
static void ArtiReflux_meterDataAck(Device_t *dev);
static u8_t invterAdd(void *load, void *val);
static u8_t invterRemove(void *load, void *val);
static void deviceDel(Device_t *dev);

static const AntiReflux_t *deviceTab[] = {
     &MOSO_0211,
     &MOSO_0212,
     &Device_0409,
     &Device_040B
};

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void proc_antiReflux_task(s32_t taskId)
{
    ST_MSG msg;

    APP_DEBUG("anti-reflux task run!!\r\n");
    Anti_reflux_init();

    while (TRUE)
    {
        Ql_OS_GetMessage(&msg);
        switch(msg.message)
        {
            case SYS_PARA_CHANGE:
            case USER_TIMER_ID:
                checkDeviceOnline();
                break;
            defualt :
                break;
        }
    }
}

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
int float2int(u32_t data)
{
#if 0  //方法 1
    s8_t S;     //符号
    s8_t  E;    //指数
    u32_t F;    //小数据位
    u32_t carry;
    int val;

    S = data&0x80000000 ?  -1 : 1;
    E = ((data>>23) & 0xFF) - 127;
    F = data &0x7FFFFF;
    
   
    for (val = F, carry = 1; val > 0; val /= 10 )
    {
        carry *= 10;
    }

    if (E >= 0 && E < 32)
    {
        val = (F + carry)/((carry/10)>>E);
        val = (val+5)/10;
    }
    else if (E == -1)
    {
        val = (F + carry)/(carry/5);
        val = (val+5)/10;
    }
    else
    {
        val = 0;
    }

    return val*S;
#elif  1 //方法 二

    int val;
    float f;

    r_memcpy(&f, &data, sizeof(f));

    val = f + 0.5;

    return val;
#else]
    int val;
    float *pF;

    pF = (float *)&data;

    val = *pF + 0.5;

    return val;
#endif
}

/*******************************************************************************
  * @note   data check
  * @param  None
  * @retval None
*******************************************************************************/
u8_t modbusDataCheck(void *load, void *changeData)
{
    int ret;
    DeviceCmd_t *cmd= (DeviceCmd_t*)load;

    if (0 == cmd->state)
    {
		ret = Modbus_CmdCheck(&cmd->cmd, &cmd->ack);
		cmd->state = (u8_t)ret;
		if (ret != 0)
		{
            *((u8_t*)changeData) = 1;
        }
    }
    else
    {
        *((u8_t*)changeData) = 1;
    }
 
	return 1;
}

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void Anti_reflux_init(void)
{
    meterPower.state = 0;
    list_init(&invertDevice);
}

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t checkDeviceOnline(void)
{
    static int onlineDeviceCount = 0;

    if (onlineDeviceList.count !=  onlineDeviceCount 
        && (MeterDevice.head != null)
    )
    {
        onlineDeviceCount = 0;

        if (onlineDeviceList.count > 1)
        {
            int i;
            
            AntiRefluxDeviceInfo_t *info;
            Device_t *meterDev = null;
            DeviceOnlineHead_t *online = null;

            online = list_nextData(&onlineDeviceList, null);
            Device_removeType(DEVICE_ARTI);
            list_delete(&invertDevice);

            while (online != null)
            {
                for (i = 0; i < sizeof(deviceTab)/sizeof(deviceTab[0]); i++)
                {
                    if (deviceTab[i]->tab.code == online->code)
                    {
                        if (0x0200 == (online->code & 0x0200))
                        {
                            InvertList_t *deviceList;
                            Device_t *invsterDev = null;

                            invsterDev = list_nodeApply(sizeof(Device_t));
                            info = memory_apply(sizeof(AntiRefluxDeviceInfo_t));

                            info->addr = online->addr;
                            info->aux = &deviceTab[i]->tab;
                            invsterDev->cfg = null;
                            invsterDev->callBack = ArtiReflux_InvDataAck;
                            invsterDev->explain = info;
                            invsterDev->type = DEVICE_ARTI;
                            list_init(&invsterDev->cmdList);
                            deviceList = list_nodeApply(sizeof(InvertList_t));
                            deviceList->dev = invsterDev;
                            list_bottomInsert(&invertDevice, deviceList);

                            Modbus_GetCmdAdd(&invsterDev->cmdList, online->addr, deviceTab[i]->tab.cmdTab, deviceTab[i]->tab.count*sizeof(ModbusGetCmd_t));
                        }
                        else if (0x0400 == (online->code & 0x0400))
                        {
                            if (meterDev == null)
                            {
                                meterDev = list_nodeApply(sizeof(Device_t));
                                info = memory_apply(sizeof(AntiRefluxDeviceInfo_t));

                                info->addr = online->addr;
                                info->aux = &deviceTab[i]->tab;
                                meterDev->cfg = null;
                                meterDev->callBack = ArtiReflux_meterDataAck;
                                meterDev->explain = info;
                                meterDev->type = DEVICE_ARTI;
                                list_init(&meterDev->cmdList);
                            }

                            Modbus_GetCmdAdd(&meterDev->cmdList, online->addr, deviceTab[i]->tab.cmdTab, deviceTab[i]->tab.count*sizeof(ModbusGetCmd_t));
                        }
                    }
                }
                online = list_nextData(&onlineDeviceList, online);
            }

            if ((meterDev != null)
                && (invertDevice.count > 0)
            )
            {
                onlineDeviceCount = onlineDeviceList.count;
                Device_add(meterDev);
                list_trans(&invertDevice, invterAdd, null);
            }
            else
            {
                deviceDel(meterDev);
                list_trans(&invertDevice, invterRemove, null);
                list_delete(&invertDevice);
            }   
        }
        return 1;
    }

    return 0;
}
/*******************************************************************************
  * @note   add invter
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t invterAdd(void *load, void *val)
{
    InvertList_t *deviceList = load;

    Device_add(deviceList->dev);

    return 1;
}

static u8_t invterRemove(void *load, void *val)
{
    InvertList_t *deviceList = load;

    deviceDel(deviceList->dev);

    return 1;
}

static void deviceDel(Device_t *dev)
{
    if (dev != null)
    {
        DeviceCmd_clear(dev);
        list_nodeDelete(null, dev);
    }
}

/*******************************************************************************
  * @note   inv Data Collect
  * @param  None
  * @retval None
*******************************************************************************/
static void ArtiReflux_InvDataAck(Device_t *dev)
{
    AntiRefluxDeviceInfo_t *info = (AntiRefluxDeviceInfo_t*)dev->explain;
    u8_t result = 0;

    list_trans(&dev->cmdList, modbusDataCheck, &result);

    if (result == 0)
    {
        if (meterPower.state == 1)
        {
            info->aux->func(invertDevice.count, dev, &meterPower);
            if (meterPower.state != 1)
            {
                return;
            }
        }
    }

    dev->lock = 0;
}

static u8_t invterLockStatus(void *load, void *val)
{
    InvertList_t *invert = load;
    u8_t *pVal = val;

    if (invert->dev->lock == 0)
    {
        *pVal = 1;

        return 0;
    }

    return 1;
}

static u8_t invterUnlock(void *load, void *val)
{
    InvertList_t *invert = load;

    invert->dev->lock = 0;

    return 1;
}

static void ArtiReflux_meterDataAck(Device_t *dev)
{
    AntiRefluxDeviceInfo_t *info = (AntiRefluxDeviceInfo_t*)dev->explain;
    u8_t result = 0;

    dev->lock = 0;
    list_trans(&dev->cmdList, modbusDataCheck, &result);

    if (result == 0)
    {
        u8_t flag = 0;

        info->aux->func(0, dev, &meterPower);
        list_trans(&invertDevice, invterLockStatus, &flag);
        if (flag == 0)
        {
            APP_DEBUG("Invter unlock!!\r\n");
            list_trans(&invertDevice, invterUnlock, null);
        }
    }
}

/******************************************************************************/

