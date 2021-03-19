/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : ModbusDevice.c  MOSOEC Modbus
  *@notes   : 2017.08.04 CGQ
*******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_stdlib.h"
#include "ql_memory.h"
#endif
#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#endif

#include "eyblib_list.h"
#include "eyblib_swap.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_memory.h"

#include "eybpub_Debug.h"
#include "eybpub_run_log.h"
#include "eybpub_utility.h"

#include "ModbusDevice.h"
#include "Modbus.h"
#include "Protocol.h"
#include "Device.h"

#include "eybond.h"

ListHandler_t onlineDeviceList;      //sucess online device tab

static void addrFind(void);
static u8_t addrFindProcess(Device_t *dev);
static void protocolFind(Device_t *dev);
static u8_t protocolFindProcess(Device_t *dev);
static u8_t ackProcess(Device_t *dev);
static u8_t cmdTabCheck(void *load, void *changeData);
static void onlineDeviceAddr(Device_t *dev);
static u8_t onlineDeviceRemove(void *dev, void *point);
static u8_t onlineDeviceCmdAdd(void *load, void *changePoint);
static u8_t onlineDeviceCmdRemove(void *load, void *changePoint);
static u8_t onlineDeviceCmp(void *src, void *dest);
static u8_t onlineDeviceCmdCmp(void *src, void *dest);
// static void onlineDevicePrintf(DeviceOnlineHead_t *head);    // mike 20200828
static u8_t onlineCmdTabPrintf(void *load, void *changeData);    // mike 20200828

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void ModbusDevice_init(void) {
  list_init(&onlineDeviceList);
  if (ModbusDevice.head != null || MeterDevice.head != null) {
    addrFind();
  }
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void ModbusDevice_clear(void) {
  APP_DEBUG("ModbusDevice_clear\r\n");
  list_trans(&onlineDeviceList, onlineDeviceRemove, null);
  list_delete(&onlineDeviceList);
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t addrCmp(void *load, void *val) {
  Device_t *dev = load;

  if (dev->type == DEVICE_MOBUS) {
    int i;
    DeviceExplain_t *exp = dev->explain;
    u8_t *addrTab = val;
    for (i = 0; *addrTab != 0; i++, addrTab++) {
      if (exp->addr == *addrTab) {
        *addrTab = 0xFF;
        break;
      }
    }
  }
  return 1;
}

static void addrFind(void) {
  int i;
  Device_t *dev;
  DeviceExplain_t *exp;
  u8_t addrTab[64];

  r_memset(addrTab, 0, sizeof(addrTab));
  r_memcpy(addrTab, ModbusDevice.addrTab, sizeof(addrTab));

  /* 如果查找到指定地址，本地址对应数组数据为0xFF */
  list_trans(&DeviceList, addrCmp, addrTab);

  for (i = 0; addrTab[i] != 0; i++) {
    APP_DEBUG("addrTab[%d]: %04X \r\n", i, addrTab[i]);
    if (addrTab[i] != 0xFF) {
      ModbusGetCmd_t *cmd = null;
      exp = memory_apply(sizeof(DeviceExplain_t));
      if (MeterDevice.head != null && addrTab[i] >= MeterDevice.startAddr && addrTab[i] <= MeterDevice.endAddr) {
        dev = list_nodeApply(sizeof(Device_t));
        cmd = (ModbusGetCmd_t *)MeterDevice.head->findCmd;
        exp->head = (ModbusDeviceHead_t *)MeterDevice.head;
        if (MeterDevice.cfg != null) {
          dev->cfg = MeterDevice.cfg;
        } else {
          dev->cfg = (ST_UARTDCB *)MeterDevice.head->hardCfg;
        }
        APP_DEBUG("add meter \r\n");
      } else if (ModbusDevice.head != null) {
        dev = list_nodeApply(sizeof(Device_t));
        cmd = (ModbusGetCmd_t *)ModbusDevice.head->findCmd;
        exp->head = (ModbusDeviceHead_t *)ModbusDevice.head;
        if (ModbusDevice.cfg == null) {
          dev->cfg = (ST_UARTDCB *)ModbusDevice.head->hardCfg;
        } else {
          dev->cfg = ModbusDevice.cfg;
        }
      }

      if (cmd != null) {
        exp->addr = addrTab[i];
        dev->type = DEVICE_MOBUS;
        dev->callBack = addrFindProcess;
        dev->explain = exp;
        list_init(&dev->cmdList);

        APP_DEBUG("add cmd %02X, %04X, %04X \r\n", cmd->fun, cmd->start, cmd->end);
        /* 为底层命令对列只添加了对应的一条命令，原因是长度为sizeof(ModbusGetCMD_t)这里的限制 */
        Modbus_GetCmdAdd(&dev->cmdList, addrTab[i], cmd, sizeof(ModbusGetCmd_t));  // mike 添加第一条查询指令到 dev cmd list中
        Device_add(dev);
      }
    }
  }
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t addrFindProcess(Device_t *dev) {
  APP_DEBUG("addrFindProcess!!\r\n");
  u8_t ret = 0;
  DeviceCmd_t *cmd = (DeviceCmd_t *)(dev->cmdList.node->payload);
  dev->lock = 0;

  if (ModbusDevice.monitorCount <= onlineDeviceList.count) {
    APP_DEBUG("Monitor num over!!\r\n");
    log_save("Monitor num over!!");
    Device_remove(dev);
  } else if (0 <= Modbus_CmdCheck(&cmd->cmd, &cmd->ack)) {
    protocolFind(dev);
    ret = 1;
  }

  return ret;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static void protocolFind(Device_t *dev) {
  APP_DEBUG("protocolFind!!\r\n");
  DeviceExplain_t *exp = (DeviceExplain_t *)(dev->explain);

  if (exp->head->procmd != null && exp->head->procmd->code == 0) {
    APP_DEBUG("Add the protocol adjust!!!!!!!!!!!!!!!!!!\r\n");
    dev->explain = null;
    DeviceCmd_clear(dev);

    dev->callBack = protocolFindProcess;
    dev->explain = exp;

    Modbus_GetCmdAdd(&dev->cmdList, exp->addr, exp->head->procmd->cmdTab, \
                     sizeof(exp->head->procmd->cmdTab[0])* exp->head->procmd->count);
  } else {
    protocolFindProcess(dev);
  }
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t protocolFindProcess(Device_t *dev) {
  APP_DEBUG("protocolFindProcess!!\r\n");
  u8_t ret = 0;
  ModbusGetCmdTab_t *cmdTab = null;
  DeviceExplain_t *exp = (DeviceExplain_t *)(dev->explain);

  dev->lock = 0;
  if (exp->head->devicePro != null) {
    list_trans(&dev->cmdList, exp->head->devicePro, &cmdTab);
  } else {
    cmdTab = (ModbusGetCmdTab_t *)exp->head->procmd;
  }

  APP_DEBUG("cmdTab %d %02X %04X!!\r\n", cmdTab->count, cmdTab->flag, cmdTab->code);
  APP_DEBUG("ModbusDevice count %d onlineDeviceList count %d!!\r\n", ModbusDevice.monitorCount, onlineDeviceList.count);
  
  if (cmdTab == null) {
    Device_remove(dev);
    addrFind();
  } else if (ModbusDevice.monitorCount > onlineDeviceList.count) {
    ret = 1;
    dev->explain = null;
    DeviceCmd_clear(dev);
    dev->callBack = ackProcess;
    dev->explain = exp;
    exp->code = cmdTab->code;
    exp->flag = cmdTab->flag & (~(1 << 7));

    if (cmdTab->flag & (1 << 7)) { // user to must protocol
      int i = 0;
      APP_DEBUG("flag debug1 %02X!!\r\n", cmdTab->flag);

      for (i = cmdTab->count, cmdTab = (ModbusGetCmdTab_t *)(cmdTab->cmdTab); \
           i > 0 && exp->code == cmdTab->code; i--, cmdTab++) {
        if (cmdTab->flag == 0) {
          Modbus_GetCmdAdd(&dev->cmdList, exp->addr, cmdTab->cmdTab, \
                           sizeof(cmdTab->cmdTab[0]) * cmdTab->count);
        } else {
          Modbus_GetCmdAdd(&dev->cmdList, cmdTab->flag, cmdTab->cmdTab, \
                           sizeof(cmdTab->cmdTab[0]) * cmdTab->count);
        }
      }
    } else {
      APP_DEBUG("flag debug2 %02X!!\r\n", cmdTab->flag);
      exp->flag = cmdTab->flag;
      Modbus_GetCmdAdd(&dev->cmdList, exp->addr, cmdTab->cmdTab, \
                       sizeof(cmdTab->cmdTab[0]) * cmdTab->count);
    }
  } else {
    APP_DEBUG("Monitor num over!!\r\n");
    log_save("Monitor num over!!");
    Device_remove(dev);
  }

  return ret;
}

// static Buffer_t memStateBuf;
// static void rcveMemStatic(u8 *data, u16 len)
// {
//     if (memStateBuf.size > memStateBuf.lenght && memStateBuf.payload != null)
//     {
//         len = memStateBuf.size - memStateBuf.lenght > len ? len : memStateBuf.lenght;

//         r_memcpy(&memStateBuf.payload[memStateBuf.lenght], data, len);
//         memStateBuf.lenght += len;
//     }
// }

// static void memoryLog(void)
// {
//     memStateBuf.size = 1024;
//     memStateBuf.lenght = 0;
//     memStateBuf.payload = memory_apply(memStateBuf.size);

//     memory_trans(rcveMemStatic);
//     memStateBuf.payload[memStateBuf.lenght] = '\0';
//     log_save( memStateBuf.payload);
//     memory_release(memStateBuf.payload);
// }
/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t ackProcess(Device_t *dev) {
  APP_DEBUG("ackProcess!!\r\n");
  u8_t checkResult = 0;

  DeviceExplain_t *exp = (DeviceExplain_t *)dev->explain;

  dev->lock = 0;

  if (exp->head->ackDataPro != null) {
    list_trans(&dev->cmdList, exp->head->ackDataPro, &checkResult);
  } else {
    list_trans(&dev->cmdList, cmdTabCheck, &checkResult);
  }

  if (checkResult == 0) { // Device ack Data OK
    exp->tryCnt = 0;
    onlineDeviceAddr(dev);
  } else if (exp->tryCnt++ > 10) {
    DeviceOnlineHead_t *head = list_find(&onlineDeviceList, onlineDeviceCmp, dev);
    if (head != null) {
      APP_DEBUG("Device unline: %d\r\n", exp->addr);
      log_save("Device unline: %d", exp->addr);
      onlineDeviceRemove(head, null);
      list_nodeDelete(&onlineDeviceList, head);
    }
    APP_DEBUG("Device remove: %d\r\n", exp->addr);
    log_save("Device remove: %d", exp->addr);
    Device_remove(dev);
    addrFind();
    // memoryLog();
  }

  return 0;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t cmdTabCheck(void *load, void *changeData) {
  DeviceCmd_t *cmd = (DeviceCmd_t *)load;
  int ret = 0;

  if (0 == cmd->state) {
    ret = Modbus_CmdCheck(&cmd->cmd, &cmd->ack);
    cmd->state = (u8_t)ret;
//    APP_DEBUG("cmdTabCheck cmd ack len:%d !!\r\n", cmd->ack.lenght);
    if (ret == 0) {
    }
    else if (ret >  0) {
      *((u8_t *)changeData) |= (u8_t)ret;
    } else if (ret < 0) {
      *((u8_t *)changeData) |= 0x10;
      return 0;
    }
  } else {
    *((u8_t *)changeData) |= 0x10;
    return 0;
  }

  return 1;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static void onlineDeviceAddr(Device_t *dev) {
  APP_DEBUG("onlineDeviceAddr!!\r\n");
  DeviceOnlineHead_t *head = list_find(&onlineDeviceList, onlineDeviceCmp, dev);

  // memoryLog();
  if (head == null) {
    if (ModbusDevice.monitorCount > onlineDeviceList.count) {
      DeviceExplain_t *exp = dev->explain;
      head = list_nodeApply(sizeof(DeviceOnlineHead_t));
      list_bottomInsert(&onlineDeviceList, head);
      head->addr = exp->addr;
      head->flag = exp->flag;
      head->code = exp->code;
      list_init(&head->cmdList);
      APP_DEBUG("Device online: %d\r\n", exp->addr);
  //    log_save("Device online: %d", exp->addr);
#ifdef _PLATFORM_BC25_
      Ql_OS_SendMessage(EYBOND_TASK, EYBOND_CMD_REPORT, 0, 0);  // mike Report first message when device online
#endif
#ifdef _PLATFORM_L610_
      Eybpub_UT_SendMessage(EYBOND_TASK, EYBOND_CMD_REPORT, 0, 0);
#endif
    } else {
      APP_DEBUG("Monitor num over!!\r\n");
      log_save("Monitor num over!!");
      Device_remove(dev);
      return;
    }
  }

  list_trans(&dev->cmdList, onlineDeviceCmdAdd, &head->cmdList);
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t onlineDeviceRemove(void *dev, void *point) {
  APP_DEBUG("onlineDeviceRemove!!\r\n");
  DeviceOnlineHead_t *head = dev;

  list_trans(&head->cmdList, onlineDeviceCmdRemove, null);
  list_delete(&head->cmdList);

  return 1;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t onlineDeviceCmdAdd(void *load, void *changePoint) {
  APP_DEBUG("onlineDeviceCmdAdd!!\r\n");
  u16_t crc;
  ListHandler_t *head = changePoint;
  DeviceCmd_t *cmd = (DeviceCmd_t *)load;

  CmdBuf_t *cmdBuf = list_find(head, onlineDeviceCmdCmp, cmd->cmd.payload);
  r_memcpy(&crc, &cmd->ack.payload[cmd->ack.lenght - 2], 2);

  if (cmdBuf == null) {
    cmdBuf = list_nodeApply(sizeof(CmdBuf_t));
    list_bottomInsert(head, cmdBuf);
    cmdBuf->state = 0;
    cmdBuf->fun = cmd->cmd.payload[1];
    cmdBuf->startAddr = (cmd->cmd.payload[2] << 8) | cmd->cmd.payload[3];
    cmdBuf->endAddr = cmdBuf->startAddr + cmd->cmd.payload[5] - 1;
    cmdBuf->crc = crc;
    cmdBuf->buf.size = cmd->ack.lenght > 5 ? (cmd->ack.lenght - 5) : 0;
    cmdBuf->buf.lenght = cmdBuf->buf.size;
    cmdBuf->buf.payload = memory_apply(cmdBuf->buf.size);
    r_memcpy(cmdBuf->buf.payload, &cmd->ack.payload[3], cmdBuf->buf.size);
  }

  if (crc != cmdBuf->crc && cmd->ack.lenght > 5) {
    int dataLenght = cmd->ack.lenght - 5;

    if (dataLenght > cmdBuf->buf.size) {
      memory_release(cmdBuf->buf.payload);
      cmdBuf->buf.size = dataLenght;
      cmdBuf->buf.lenght = cmdBuf->buf.size;
      cmdBuf->buf.payload = memory_apply(cmdBuf->buf.size);
    }
    cmdBuf->buf.lenght = dataLenght;
    r_memcpy(cmdBuf->buf.payload, &cmd->ack.payload[3], dataLenght);
    cmdBuf->state = 0;
    cmdBuf->crc = crc;
  }

  // cmdBuf->buf.lenght = cmd->ack.lenght - 5;
  // cmdBuf->buf.payload =  &cmd->ack.payload[3];
  return 1;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t onlineDeviceCmdRemove(void *load, void *changePoint) {
  CmdBuf_t *cmdBuf = load;
  if (cmdBuf->buf.payload != NULL) {
    memory_release(cmdBuf->buf.payload);
  }
  return 1;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t onlineDeviceCmp(void *src, void *dest) {
  Device_t *dev = dest;
  DeviceOnlineHead_t *head = src;
  DeviceExplain_t *exp = dev->explain;

  if (exp->addr == head->addr && exp->code == head->code) {
    return 0;
  }

  return 1;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t onlineDeviceCmdCmp(void *src, void *dest) {
  u8_t *para = (u8_t *)dest;
  u16_t srcAddr = ((para[2] << 8) | para[3]);
  CmdBuf_t *cmd = (CmdBuf_t *)src;

  if (cmd->startAddr == srcAddr && para[1] == cmd->fun) {
    return 0;
  }

  return 1;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t onlineCmdTabPrintf(void *load, void *changeData) {
  DeviceCmd_t *cmd;

  cmd = (DeviceCmd_t *)load;

  if (cmd->ack.lenght > 0) {
    char *buf = memory_apply(cmd->ack.lenght * 2 + 1);
//    Swap_hexChar(buf, cmd->ack.payload, cmd->ack.lenght, ' ');    // mike 20200904
    hextostr(cmd->ack.payload, buf, cmd->ack.lenght);   // 和Swap_hexChar是反的
    APP_DEBUG("%s\r\n", buf);
    memory_release(buf);
  }

  return 1;
}

/******************************************************************************/

