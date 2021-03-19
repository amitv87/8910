/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
 *@brief   : Modbus.c
 *@notes   : 2017.04.20 CGQ 创建
*******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_stdlib.h"
#include "ql_memory.h"
#endif

#include "eyblib_typedef.h"
#include "eyblib_list.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_memory.h"
#include "eyblib_CRC.h"

#include "eybpub_Debug.h"

#include "Modbus.h"
#include "Device.h"

// #include "eyblib_r_stdlib.h"

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void Modbus_GetCmdAdd(ListHandler_t *list, u8_t addr, void *tab, u16_t tabSize) {
  ModbusGetCmd_t *cmdTab = (ModbusGetCmd_t *)tab;
  u8_t *tabEndAddr = &(((u8_t *)tab)[tabSize]);

  while ((int)cmdTab < (int)tabEndAddr) {
    switch (cmdTab->fun) {
      case 0x01:
      case 0x02:
        ModbusCmd_0102(list, addr, cmdTab->fun, cmdTab->start, cmdTab->end);
        break;
      case 0x03:
      case 0x04:
        ModbusCmd_0304(list, addr, cmdTab->fun, cmdTab->start, cmdTab->end);
        break;
      default:
        break;
    }
    cmdTab++;
  }
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
int Modbus_CmdCheck(Buffer_t *send, Buffer_t *ack) {
  int ret = 0;

  if (send == null || send->lenght == 0 || send->payload == null
      || ack == null || ack->lenght == 0 || ack->payload == null
     ) {
    ret = -1;
  } else if (0 != crc16_standard(CRC_RTU, ack->payload, ack->lenght)) {
    ret = -2;
  } else if (send->payload[0] != ack->payload[0]) {
    ret = -3; //addr err
  } else if (send->payload[1] != ack->payload[1]) {
    if ((ack->payload[1] & 0x80) == 0x80) {
      ret =  ack->payload[2];
    } else {
      ret = -4;
    }
  }
  APP_DEBUG("Modbus_CmdCheck ret :%d\r\n", ret);
  return ret;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void ModbusCmd_0102(ListHandler_t *list, u8_t addr, u8_t fun, u16_t start, u16_t end) {
  u8_t *ackBuf;
  ModbusFC0102_t *pLoad;
  DeviceCmd_t *cmd;
  u8_t num = ABS(start, end) + 1;
  u16_t ackSize = (num >> 3) + (num & 0x07 ? 1 : 0) ; //
  u16_t startReg = MIN(start, end);

  APP_DEBUG("num:%d ackSize:%d\r\n", num, ackSize);
  cmd = list_nodeApply(sizeof(DeviceCmd_t));
  pLoad = memory_apply(sizeof(ModbusFC0102_t));
  ackBuf = memory_apply(ackSize + 5);
  if (cmd != null && pLoad != null && ackBuf != null) {
    list_bottomInsert(list, cmd);
    cmd->waitTime = 2000;
    cmd->state = 0;
    cmd->ack.size = ackSize + 5;
    cmd->ack.payload = ackBuf;
    cmd->ack.lenght = 0;
//    cmd->cmd.size = MODEBUS_MAX_LOAD;
    cmd->cmd.lenght = sizeof(ModbusFC0304_t);
    cmd->cmd.payload = (u8_t *)pLoad;

    pLoad->addr = addr;
    pLoad->fun = fun;
    pLoad->reg = ENDIAN_BIG_LITTLE_16(startReg);
    pLoad->num = ENDIAN_BIG_LITTLE_16(num);
    pLoad->crc = crc16_standard(CRC_RTU, (u8_t *)pLoad, sizeof(ModbusFC0304_t) - sizeof(pLoad->crc));
  } else {
    list_nodeDelete(list, cmd);
    memory_release(pLoad);
    memory_release(ackBuf);
  }
}


/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void ModbusCmd_0304(ListHandler_t *list, u8_t addr, u8_t fun, u16_t start, u16_t end) {
  u16_t len;
  u8_t *ackBuf;
  ModbusFC0304_t *pLoad;
  DeviceCmd_t *cmd;
  u16_t cmdSize = (ABS(start, end) + 1) << 1;  // n*2
  u16_t startReg = MIN(start, end);

  APP_DEBUG("cmdSize:%d\r\n", cmdSize);
  while (cmdSize > 0) {
    len = cmdSize > MODEBUS_MAX_LOAD ? MODEBUS_MAX_LOAD : cmdSize;
    cmd = list_nodeApply(sizeof(DeviceCmd_t));
    pLoad = memory_apply(sizeof(ModbusFC0304_t));
    ackBuf = memory_apply(len + 5);
    if (cmd != null && pLoad != null && ackBuf != null) {
      list_bottomInsert(list, cmd);
      cmd->waitTime = 2000;
      cmd->state = 0;
      cmd->ack.size = len + 5;
      cmd->ack.payload = ackBuf;
      cmd->ack.lenght = 0;
//    cmd->cmd.size = MODEBUS_MAX_LOAD;
      cmd->cmd.lenght = sizeof(ModbusFC0304_t);
      cmd->cmd.payload = (u8_t *)pLoad;

      pLoad->addr = addr;
      pLoad->fun = fun;
      pLoad->reg = ENDIAN_BIG_LITTLE_16(startReg);
      pLoad->num = ENDIAN_BIG_LITTLE_16(len >> 1);
      pLoad->crc = crc16_standard(CRC_RTU, (u8_t *)pLoad, sizeof(ModbusFC0304_t) - sizeof(pLoad->crc));
    } else {
      list_nodeDelete(list, cmd);
      memory_release(pLoad);
      memory_release(ackBuf);
    }
    startReg += len >> 1;
    cmdSize -= len;
  }
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void Modbus_06SetCmd(ListHandler_t *list, u8_t addr, u16_t reg, u16_t val) {
  u8_t *ackBuf;
  ModbusFC06_t *pLoad;
  DeviceCmd_t *cmd;

  cmd = list_nodeApply(sizeof(DeviceCmd_t));
  pLoad = memory_apply(sizeof(ModbusFC06_t));
  ackBuf = memory_apply(sizeof(ModbusFC06_t));
  if (cmd != null && pLoad != null && ackBuf != null) {
    list_bottomInsert(list, cmd);
    cmd->waitTime = 2000;
    cmd->state = 0;
    cmd->ack.size = 8;
    cmd->ack.payload = ackBuf;
    cmd->ack.lenght = 0;
    cmd->cmd.lenght = sizeof(ModbusFC06_t);
    cmd->cmd.payload = (u8_t *)pLoad;

    pLoad->addr = addr;
    pLoad->code = 0x06;
    pLoad->reg = ENDIAN_BIG_LITTLE_16(reg);
    pLoad->val = ENDIAN_BIG_LITTLE_16(val);
    pLoad->crc = crc16_standard(CRC_RTU, (u8_t *)pLoad, sizeof(ModbusFC06_t) - MODEBUS_CRC_SIZE);
  } else {
    list_nodeDelete(list, cmd);
    memory_release(pLoad);
    memory_release(ackBuf);
  }
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void Modbus_16SetCmd(ListHandler_t *list, u8_t addr, u16_t start, u16_t end, u8_t *data) {
  u8_t *ackBuf;
  ModbusFC16_t *pLoad;
  DeviceCmd_t *cmd;
  u16_t crc;
  u16_t cmdSize = (ABS(start, end) + 1) << 1;  //n*2
  u16_t startReg = MIN(start, end);

  cmd = list_nodeApply(sizeof(DeviceCmd_t));
  pLoad = memory_apply(sizeof(ModbusFC16_t) + MODEBUS_CRC_SIZE + cmdSize);
  ackBuf = memory_apply(8);
  if (cmd != null && pLoad != null && ackBuf != null) {
    list_bottomInsert(list, cmd);
    cmd->waitTime = 2000;
    cmd->state = 0;
    cmd->ack.size = 8;
    cmd->ack.payload = ackBuf;
    cmd->ack.lenght = 0;
    cmd->cmd.lenght = sizeof(ModbusFC16_t) + MODEBUS_CRC_SIZE + cmdSize;
    cmd->cmd.payload = (u8_t *)pLoad;

    pLoad->addr = addr;
    pLoad->code = 0x10;
    pLoad->reg = ENDIAN_BIG_LITTLE_16(startReg);
    pLoad->num = ENDIAN_BIG_LITTLE_16(cmdSize >> 1);
    pLoad->count = cmdSize;
    r_memcpy(pLoad->data, data, cmdSize);
    crc = crc16_standard(CRC_RTU, (u8_t *)pLoad, sizeof(ModbusFC16_t) + cmdSize);
    ackBuf = ((u8_t *)pLoad->data) + cmdSize;
    *ackBuf++ = crc;
    *ackBuf++ = crc >> 8;
  } else {
    list_nodeDelete(list, cmd);
    memory_release(pLoad);
    memory_release(ackBuf);
  }
}
/******************************************************************************/

