/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
  *@brief   : state grid modbus device data get
  *@notes   : 2018.10.23 CGQ
*******************************************************************************/
#include "eyblib_swap.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_memory.h"

#include "eybpub_run_log.h"
#include "eybpub_SysPara_File.h"
#include "eybpub_Debug.h"

#include "Device.h"
#include "Modbus.h"
#include "StateGridModbus.h"

static ModbusCollectTab_t *modbusCollecTab = null;
static int dataState = 0;

static ListHandler_t *pointTabCmomandGet(void);
static u8_t pointTabFree(void *load, void *changeData);
static void pointDataSys(DeviceCmd_t *cmd);
static void UpdateCommunicationState(u8_t addr, u8_t result);
static u8_t dataCheck(void *load, void *changeData);

static StateGridPointTab_t *modbusCheck(void *pointFile, int fileSize);
static void modbusCollectCmd(void);
static int modbusStatus(void);
static void modbusRelease(void);

const StateGridDevice_t StateGridModbusDevice = {
  modbusCheck,        // 点表文件数据转换为ModbusCollectPoint_t结构体
  modbusCollectCmd,
  modbusStatus,
  modbusRelease
};

static void chunhuiProcess(DeviceCmd_t *cmd) {
  u16_t activePowerA = 0;
  u16_t activePowerB = 0;
  u16_t activePowerC = 0;
  u16_t apparentPowerA = 0;
  u16_t apparentPowerB = 0;
  u16_t apparentPowerC = 0;
  u16_t activePowerTotal = 0;
  u16_t reactivePowerTotal = 0;
  u16_t apparentPowerTotal = 0;

  activePowerA = cmd->ack.payload[3 + 20] << 8 | cmd->ack.payload[3 + 21];
  activePowerB = cmd->ack.payload[3 + 22] << 8 | cmd->ack.payload[3 + 23];
  activePowerC = cmd->ack.payload[3 + 24] << 8 | cmd->ack.payload[3 + 25];

  apparentPowerA = cmd->ack.payload[3 + 26] << 8 | cmd->ack.payload[3 + 27];
  apparentPowerB = cmd->ack.payload[3 + 28] << 8 | cmd->ack.payload[3 + 29];
  apparentPowerC = cmd->ack.payload[3 + 30] << 8 | cmd->ack.payload[3 + 31];

  activePowerTotal = activePowerA + activePowerB + activePowerC;
  apparentPowerTotal = apparentPowerA + apparentPowerB + apparentPowerC;
  reactivePowerTotal = apparentPowerTotal - activePowerTotal;

  cmd->ack.payload[3 + 38] = (activePowerTotal >> 8) & 0xFF;
  cmd->ack.payload[3 + 39] = activePowerTotal & 0xFF;

  cmd->ack.payload[3 + 40] = (reactivePowerTotal >> 8) & 0xFF;
  cmd->ack.payload[3 + 41] = reactivePowerTotal & 0xFF;

}

static void chunhuiMaiGeProcess(DeviceCmd_t *cmd, u16_t *temp) {
  u16_t activePowerA = 0;
  u16_t activePowerB = 0;
  u16_t activePowerC = 0;
//    u16_t totalActivePower = 0;

  activePowerA = cmd->ack.payload[3 + 20] << 8 | cmd->ack.payload[3 + 21];
  activePowerB = cmd->ack.payload[3 + 22] << 8 | cmd->ack.payload[3 + 23];
  activePowerC = cmd->ack.payload[3 + 24] << 8 | cmd->ack.payload[3 + 25];

  *temp = activePowerA + activePowerB + activePowerC;

//    totalActivePower = activePowerA + activePowerB + activePowerC;

//    APP_DEBUG("activePower = %04X %04X %04X \r\n", activePowerA,activePowerB,activePowerC);
//    APP_DEBUG("apparentPower = %04X %04X %04X \r\n", apparentPowerA,apparentPowerA,apparentPowerA);
//    APP_DEBUG("activePowerTotal = %04X %04X %04X \r\n", activePowerTotal,apparentPowerTotal,reactivePowerTotal);


//    cmd->ack.payload[3 + 32] = (totalActivePower >> 8) & 0xFF;
//    cmd->ack.payload[3 + 33] = totalActivePower & 0xFF;


//    APP_DEBUG("activePowerTotal = %04X %04X %04X %04X\r\n", cmd->ack.payload[3 + 38],
//            cmd->ack.payload[3 + 39],cmd->ack.payload[3 + 40],cmd->ack.payload[3 + 41]);

}

static void setChunhuiMaigeValue(DeviceCmd_t *cmd, u16_t *temp) {
  cmd->ack.payload[3 + 0] = (*temp >> 8) & 0xFF;
  cmd->ack.payload[3 + 1] = *temp & 0xFF;
}

// 转换效率问题
static void taidaProcess(DeviceCmd_t *cmd) {
  cmd->ack.payload[7] = ((cmd->ack.payload[7]) << 2) ;
}

/**
 * @brief 设备回复数据的特殊处理
 * @param cmd
 */
static void SpecialProcess(DeviceCmd_t *cmd) {
  static u16_t temp;
  Buffer_t Protocol;
  Buffer_t CompanyID;

//  SysPara_Get(14, &Protocol);
//  SysPara_Get(8, &CompanyID);
  parametr_get(14, &Protocol);
  parametr_get(8, &CompanyID);

  // 艾伏1BE7 E8 E9数据处理
  if (!r_memcmp("0281", Protocol.payload, strlen("0281")) &&
      cmd->cmd.payload[2] == 0xC1 &&
      cmd->cmd.payload[3] == 0x0B &&
      cmd->cmd.payload[5] == 0x15) {
    chunhuiProcess(cmd);
  }

  if (!r_memcmp("0281", Protocol.payload, strlen("0281")) &&
      cmd->cmd.payload[2] == 0xC1 &&
      cmd->cmd.payload[3] == 0x0B &&
      cmd->cmd.payload[5] == 0x0D) {
    chunhuiMaiGeProcess(cmd, &temp);
  }

  if (!r_memcmp("0281", Protocol.payload, strlen("0281")) &&
      cmd->cmd.payload[2] == 0xC1 &&
      cmd->cmd.payload[3] == 0x1B &&
      cmd->cmd.payload[5] == 0x01) {
    setChunhuiMaigeValue(cmd, &temp);
  }

  // 春晖、麦格米特 发电功率=交流侧ABC三相有功功率之和
  // if(!memcmp("0293", Protocol.payload, strlen("0293")) &&
  //         cmd->cmd.payload[2] == 0x03 &&
  //         cmd->cmd.payload[3] == 0x0B &&
  //         cmd->cmd.payload[5] == 0x15
  //         ){
  //     chunhuiProcess(cmd);
  // }

  // 台达 转换效率问题
  if (!r_memcmp("0296", Protocol.payload, strlen("0296")) &&
      cmd->cmd.payload[2] == 0xC0 &&
      cmd->cmd.payload[3] == 0x5F &&
      cmd->cmd.payload[5] == 0x03) {
    taidaProcess(cmd);
  }

  memory_release(Protocol.payload);
  memory_release(CompanyID.payload);
}

static void outputMemory(char *pMemory, u32_t lenght) {
  u32_t displayNum = 0;

  if (lenght < 1024) {
    u8_t *str = memory_apply(lenght * 3 + 8);
    if (str != null) {
      int l = Swap_hexChar((char *)str, (u8_t *)pMemory, lenght, ' ');

      while (l) {
        if (l >= 16 * 3) {
          Debug_output(str + displayNum, 16 * 3);
          l -= 16 * 3;
          displayNum += 16 * 3;
        } else {
          Debug_output(str + displayNum, l);
          l = 0;
        }
        APP_DEBUG("\r\n");
      }

      memory_release(str);
    } else {
      APP_DEBUG("outputMemory malloc failed\r\n");
    }
  }
}
/*******************************************************************************
  * @note   state grid point taber
  * @param  None
  * @retval None
*******************************************************************************/
static StateGridPointTab_t *modbusCheck(void *pointFile, int fileSize) {
  StateGridPointTab_t *tab = null;
  ModbusPointFile_t *point = pointFile;  // 此内容是flash中的国网点表格式

  if ((point->count > 0)
      && ((point->count * sizeof(FileModbusPoint_t)) + sizeof(point->count)) == fileSize) {
    if (modbusCollecTab != null) {
      memory_release(modbusCollecTab);
      modbusCollecTab = null;
    }

    tab = memory_apply(point->count * sizeof(StateGridPoint_t) + 4);
    modbusCollecTab = memory_apply(point->count * sizeof(ModbusCollectPoint_t) + 4);

    if (tab != null && modbusCollecTab != null) {
      int i;

      r_memset(tab, 0, (point->count * sizeof(StateGridPoint_t) + 4));
      r_memset(modbusCollecTab, 0, (point->count * sizeof(ModbusCollectPoint_t) + 4));
      tab->count = point->count;
      modbusCollecTab->count = point->count;
      for (i = 0; i < point->count; i++) {
        tab->tab[i].id = point->point[i].id;
        tab->tab[i].type = point->point[i].type;
        tab->tab[i].lenght = point->point[i].lenght;
        tab->tab[i].rsv  = point->point[i].rsv;
        tab->tab[i].addr = point->point[i].addr;

        modbusCollecTab->tab[i].point.addr = point->point[i].addr;
        modbusCollecTab->tab[i].point.reg = point->point[i].reg;
        modbusCollecTab->tab[i].point.regAddr = point->point[i].regAddr;
        modbusCollecTab->tab[i].src = &tab->tab[i];
      }
      APP_DEBUG("Point table load OK : %d\r\n", point->count);
    } else {
      memory_release(tab);
      memory_release(modbusCollecTab);
      tab = null;
      modbusCollecTab = null;

      log_save("State grid point buffer apply fail!!");
    }

  }

  return tab;
}

/*******************************************************************************
  * @note   state grid point new resource destroy
  * @param  None
  * @retval dataState > 0 data is valict
*******************************************************************************/
static void modbusRelease(void) {
  Device_removeType(DEVICE_GRID);
  memory_release(modbusCollecTab);
  modbusCollecTab = null;
  dataState = 0;
}

/*******************************************************************************
  * @note   state grid point Data Command get
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t command_cmp(void *src, void *dest) {
  int i = 1;

  ModbusGetCmd_t *pSrc = (ModbusGetCmd_t *)src;
  ModbusGetCmd_t *pDest = (ModbusGetCmd_t *)dest;

  if ((pSrc->fun == pDest->fun)
      && (pSrc->start > pDest->start)) {
    i = 0;
  }

  return  i;
}

/**
 *****************************************************************************************
 * @name    pointTabCmomandGet
 * @brief   Generator the Modbus commands via state grid point datas
 * @note    When two register addresses are adjacent to each other,
 *          the collection command is merged into one, and no more collection command is
 *          added to the new address
 * @return
 *****************************************************************************************
 */
static ListHandler_t *pointTabCmomandGet(void) {
  int i;
  PointTabCmd_t *tabCmd;
  ModbusGetCmd_t *cmd;
  ListHandler_t *list = memory_apply(sizeof(ListHandler_t));

  list_init(list);
  APP_DEBUG("ADD %d\r\n", modbusCollecTab->count);

  /* 整个点表轮询 */
  for (i = 0; i < modbusCollecTab->count; i++) {
    tabCmd = list_nextData(list, null);
    while (tabCmd != null) {
      if (tabCmd->addr == modbusCollecTab->tab[i].point.addr) {
        cmd = list_nextData(&tabCmd->cmdList, null);

        while (cmd != null) {
          if (cmd->fun == modbusCollecTab->tab[i].point.reg) {
            int lenght;
            int start;
            int end;
            int pointLen = StateGrid_pointLenght(modbusCollecTab->tab[i].src->type, modbusCollecTab->tab[i].src->lenght,
                                                 modbusCollecTab->tab[i].src->rsv);

            if (pointLen <= 0) {
              goto OK;
            }
            pointLen -= 1;
            //TODO 当现有命令的结束地址比下一条点表的寄存器地址大的时候的处理逻辑
            if (cmd->end >  modbusCollecTab->tab[i].point.regAddr) {
              lenght = cmd->end - modbusCollecTab->tab[i].point.regAddr ;

              end = pointLen > lenght ? modbusCollecTab->tab[i].point.regAddr + pointLen : cmd->end;
              start = cmd->start > modbusCollecTab->tab[i].point.regAddr ? modbusCollecTab->tab[i].point.regAddr : cmd->start;
            }
            /* 处理寄存器地址连续的情况*/
            else {
              start = cmd->start;
              end = modbusCollecTab->tab[i].point.regAddr  + pointLen ;
            }

            /* 处理寄存器不连续的问题*/
            if (ABS(cmd->start, start + pointLen) > 1 || ABS(cmd->end, modbusCollecTab->tab[i].point.regAddr) > 1) {
              goto NEXT;
            }

            if (((end - start) >= 0) && (end - start) < 0x20) { //TODO 此处0x20为何意义
              cmd->start = start;
              cmd->end = end;
              goto OK;
            }
          }
        NEXT:
          cmd = list_nextData(&tabCmd->cmdList, cmd);
        }

        goto ADDCMD;
      }
      tabCmd = list_nextData(list, tabCmd);
    }
    tabCmd = list_nodeApply(sizeof(PointTabCmd_t));
    tabCmd->addr = modbusCollecTab->tab[i].point.addr;
    list_init(&tabCmd->cmdList);
    list_bottomInsert(list, tabCmd);
    if (StateGrid_pointLenght(modbusCollecTab->tab[i].src->type, modbusCollecTab->tab[i].src->lenght,
                              modbusCollecTab->tab[i].src->rsv) <= 0) {
      goto OK;
    }
  ADDCMD:
    cmd = list_nodeApply(sizeof(ModbusGetCmd_t));
    cmd->fun = modbusCollecTab->tab[i].point.reg;
    cmd->start = modbusCollecTab->tab[i].point.regAddr;
    cmd->end = cmd->start + StateGrid_pointLenght(modbusCollecTab->tab[i].src->type, modbusCollecTab->tab[i].src->lenght,
               modbusCollecTab->tab[i].src->rsv) - 1;
    list_ConfInsert(&tabCmd->cmdList, command_cmp, cmd);
  OK:
    continue;
  }

  return list;
}

static u8_t pointTabFree(void *load, void *changeData) {
  PointTabCmd_t *point = (PointTabCmd_t *) load;

  list_delete(&point->cmdList);

  return 1;
}

/**
 * @brief   Convert modbus data to national network data frame format
 * @param[in] cmd
 */
static void pointDataSys(DeviceCmd_t *cmd) {
  int i;
  int startAddr;
  int endAddr;

  SpecialProcess(cmd);
  startAddr = cmd->cmd.payload[2] << 8 | cmd->cmd.payload[3];
  endAddr = cmd->cmd.payload[5] + startAddr - 1;
  for (i = 0; i < modbusCollecTab->count; i++) {
    if ((modbusCollecTab->tab[i].point.addr == cmd->ack.payload[0])
        && (modbusCollecTab->tab[i].point.reg == cmd->ack.payload[1])
        && (modbusCollecTab->tab[i].point.regAddr >= startAddr)
        && (modbusCollecTab->tab[i].point.regAddr <= endAddr)) {
      int offset = (modbusCollecTab->tab[i].point.regAddr - startAddr) * 2 + 3;
      modbusCollecTab->tab[i].src->value.lenght = StateGrid_pointValueLenght(modbusCollecTab->tab[i].src->type,
          modbusCollecTab->tab[i].src->lenght, modbusCollecTab->tab[i].src->rsv);
      if (modbusCollecTab->tab[i].src->value.size == 0) {
        modbusCollecTab->tab[i].src->value.size = modbusCollecTab->tab[i].src->value.lenght;
        modbusCollecTab->tab[i].src->value.payload = memory_apply(modbusCollecTab->tab[i].src->value.size);
      }
      if (modbusCollecTab->tab[i].src->value.lenght == 1) {
        //TODO 寄存器类型为何含义
        if (modbusCollecTab->tab[i].point.reg == 02) {
          offset = modbusCollecTab->tab[i].point.regAddr - startAddr;
          modbusCollecTab->tab[i].src->value.payload[0] = (cmd->ack.payload[(offset >> 3) + 3] >> (offset & 0x07) & 0x01);
        } else if (modbusCollecTab->tab[i].src->type == 'b') {
          u16_t value = ((cmd->ack.payload[offset] << 8) | (cmd->ack.payload[offset + 1]));
          modbusCollecTab->tab[i].src->value.payload[0] = (value >> modbusCollecTab->tab[i].src->lenght) & 0x01;
        } else if (modbusCollecTab->tab[i].src->type == 'B') {
          modbusCollecTab->tab[i].src->value.payload[0] = cmd->ack.payload[offset + modbusCollecTab->tab[i].src->lenght];
        } else if (modbusCollecTab->tab[i].src->type == 'e') {
          u16_t val;
          modbusCollecTab->tab[i].src->value.payload[0] = 0;

          if (modbusCollecTab->tab[i].src->lenght == 0) { // big
            val = (u16_t)((cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
          } else {
            val = (u16_t)((cmd->ack.payload[offset]) | (cmd->ack.payload[offset + 1] << 8));
          }

          if (val == modbusCollecTab->tab[i].src->rsv) {
            modbusCollecTab->tab[i].src->value.payload[0] = 1;
          }
        }

      } else if (((modbusCollecTab->tab[i].src->lenght & 0x80) == 0x80)
                 && ((modbusCollecTab->tab[i].src->type == 'S')
                     || (modbusCollecTab->tab[i].src->type == 'I')
                     || (modbusCollecTab->tab[i].src->type == 'G'))) {
        int k;
        float *pF = (float *)modbusCollecTab->tab[i].src->value.payload;
        float m = 1.00;
        float real = 0.00;

        if (modbusCollecTab->tab[i].src->type == 'S') {
          if ((modbusCollecTab->tab[i].src->lenght & 0x40) == 0x40) {  // 有符号
            s16_t val = (s16_t)((cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
            if (val == 0xFFFF) {
              val = 0;
            }
            real = val;
          } else {
            u16_t val = (u16_t)((cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
            if (val == 0xFFFF) {
              val = 0;
            }
            real = val;
          }

        } else if (modbusCollecTab->tab[i].src->type == 'I') {
          if ((modbusCollecTab->tab[i].src->lenght & 0x40) == 0x40) {  // 有符号
            s32_t val;
            if ((modbusCollecTab->tab[i].src->lenght & 0x30) == 0x10) {  // 低位寄存器在前
              val = (s32_t)((cmd->ack.payload[offset + 2] << 24) | (cmd->ack.payload[offset + 3] << 16) |
                            (cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
            } else {
              val = (s32_t)((cmd->ack.payload[offset] << 24) | (cmd->ack.payload[offset + 1] << 16) |
                            (cmd->ack.payload[offset + 2] << 8) | cmd->ack.payload[offset + 3]);
            }

            real = val;
          } else {
            u32_t val;
            if ((modbusCollecTab->tab[i].src->lenght & 0x30) == 0x10) {  // 低位寄存器在前
              val = (u32_t)((cmd->ack.payload[offset + 2] << 24) | (cmd->ack.payload[offset + 3] << 16) |
                            (cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
            } else {
              val = (u32_t)((cmd->ack.payload[offset] << 24) | (cmd->ack.payload[offset + 1] << 16) |
                            (cmd->ack.payload[offset + 2] << 8) | cmd->ack.payload[offset + 3]);
            }

            real = val;
          }
        } else {
          if ((modbusCollecTab->tab[i].src->lenght & 0x40) == 0x40) {  // 有符号
            s32_t val;
            if ((modbusCollecTab->tab[i].src->lenght & 0x30) == 0x10) {  // 低位寄存器在前
              val = (s32_t)((cmd->ack.payload[offset + 2] << 24) | (cmd->ack.payload[offset + 3] << 16) |
                            (cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
            } else {
              val = (s32_t)((cmd->ack.payload[offset] << 24) | (cmd->ack.payload[offset + 1] << 16) |
                            (cmd->ack.payload[offset + 2] << 8) | cmd->ack.payload[offset + 3]);
            }

            real = val;
          } else {
            u32_t val;
            if ((modbusCollecTab->tab[i].src->lenght & 0x30) == 0x10) {  // 低位寄存器在前
              val = (u32_t)((cmd->ack.payload[offset + 2] << 24) | (cmd->ack.payload[offset + 3] << 16) |
                            (cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
            } else {
              val = (u32_t)((cmd->ack.payload[offset] << 24) | (cmd->ack.payload[offset + 1] << 16) |
                            (cmd->ack.payload[offset + 2] << 8) | cmd->ack.payload[offset + 3]);
            }

            real = ~val;
          }

        }

        for (k = modbusCollecTab->tab[i].src->lenght & 0x07; k > 0; k--) {
          if ((modbusCollecTab->tab[i].src->lenght & 0x08) == 0x08) {  // 放大
            m *= 10;
          } else {  // 缩小
            m *= 0.1;
          }
        }

        // 自定义功能
        if ((0x4000 == ((modbusCollecTab->tab[i].src->rsv) & 0x4000))) {
          real = ((modbusCollecTab->tab[i].src->rsv) & (~0x4000));
        }

        *pF = real * m;

        Swap_headTail(modbusCollecTab->tab[i].src->value.payload, 4);  // 转换成大端模式
      } else if (modbusCollecTab->tab[i].src->type == 'd') {
        double real = 0;
        double *pF = (double *)modbusCollecTab->tab[i].src->value.payload;

        if (modbusCollecTab->tab[i].src->lenght & 0x80) {
          APP_DEBUG("high bit formwark\r\n");
        } else {
          for (int k = ((modbusCollecTab->tab[i].src->lenght) & 0x3F) - 1; k >= 0; k--) {
            real += (u16_t)(cmd->ack.payload[offset + k * 2] << 8 | cmd->ack.payload[offset + k * 2 + 1]);
            if (k) {
              real *= 1000;
            }
          }
        }

        *pF = real;
        Swap_headTail(modbusCollecTab->tab[i].src->value.payload, 8);  // 转换成大端模式
      } else if (modbusCollecTab->tab[i].src->type == 'M') {
        char *strValue = NULL;
        float m = 1.00;
        float real = 0.00;
        float *pF = (float *)modbusCollecTab->tab[i].src->value.payload;

        s16_t val = (s16_t)((cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
        real = val - 1000;

        for (int k = modbusCollecTab->tab[i].src->lenght & 0x07; k > 0; k--) {
          if ((modbusCollecTab->tab[i].src->lenght & 0x08) == 0x08) {  // 放大
            m *= 10;
          } else {  // 缩小
            m *= 0.1;
          }
        }

        *pF = real * m;
        Swap_headTail(modbusCollecTab->tab[i].src->value.payload, 4);  // 转换成大端模式

      } else if (modbusCollecTab->tab[i].src->type == 'V') {
        char *strValue = NULL;
        float m = 1.00;
        float real = 0.00;
        float *pF = (float *)modbusCollecTab->tab[i].src->value.payload;

        s16_t val = (s16_t)((cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
        real = (val * 1.000) / 1.732;

        for (int k = modbusCollecTab->tab[i].src->lenght & 0x07; k > 0; k--) {
          if ((modbusCollecTab->tab[i].src->lenght & 0x08) == 0x08) {  // 放大
            m *= 10;
          } else {  // 缩小
            m *= 0.1;
          }
        }

        *pF = real * m;
        Swap_headTail(modbusCollecTab->tab[i].src->value.payload, 4);  // 转换成大端模式

      } else {
        r_memcpy(modbusCollecTab->tab[i].src->value.payload, &cmd->ack.payload[offset],
                 modbusCollecTab->tab[i].src->value.lenght);
        if ((modbusCollecTab->tab[i].src->type == 'I') && (modbusCollecTab->tab[i].src->lenght == 1)) {
          int *pi = (int *)modbusCollecTab->tab[i].src->value.payload;
          *pi = (*pi >> 16) | (*pi << 16);
        }
      }
    }
  }
}

static void UpdateCommunicationState(u8_t addr, u8_t result) {
  int i;

  for (i = 0; i < modbusCollecTab->count; i++) {
    if (modbusCollecTab->tab[i].point.addr == addr) {
      if ((modbusCollecTab->tab[i].src->type == 0)
          && (modbusCollecTab->tab[i].src->lenght == 0)) {
        modbusCollecTab->tab[i].src->value.lenght = 1;
        if (modbusCollecTab->tab[i].src->value.size == 0) {
          modbusCollecTab->tab[i].src->value.size = modbusCollecTab->tab[i].src->value.lenght;
          modbusCollecTab->tab[i].src->value.payload = memory_apply(modbusCollecTab->tab[i].src->value.size);
        }
        modbusCollecTab->tab[i].src->value.payload[0] = result;
      } else if (result == 0) {
        modbusCollecTab->tab[i].src->value.lenght = 0;
      }
    }
  }
}

/*******************************************************************************
  * @note   state grid point Data Collect
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t dataCheck(void *load, void *changeData) {
  int ret;
  DeviceCmd_t *cmd = (DeviceCmd_t *)load;

  if (0 == cmd->state) {
    ret = Modbus_CmdCheck(&cmd->cmd, &cmd->ack);
    cmd->state = (u8_t)ret;
    if (ret == 0) {
      *((u8_t *)changeData) = 1;
      pointDataSys(cmd);
    }
  }

  return 1;
}

/*******************************************************************************
  * @note   Collect status
  * @param  None
  * @retval None
*******************************************************************************/
int modbusStatus(void) {
  return dataState;
}

/*******************************************************************************
 * @brief   Process the modbus response
  * @note   data Ack
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t  dataAck(Device_t *dev) {
  StateGridDeviceInfo_t *info = (StateGridDeviceInfo_t *)dev->explain;
  u8_t result = 0;

  dev->lock = 0;
  list_trans(&dev->cmdList, dataCheck, &result);

  UpdateCommunicationState(info->addr, result);
  if (result == 1) {
    if (dataState != 1) {
      dataState = 1;
      log_save("State grid data collect OK!");
    }
  }

  return 0;
}

static void modbusCollectCmd(void) {
  Device_t *dev;
  ListHandler_t *list;
  PointTabCmd_t *point ;
  StateGridDeviceInfo_t *info;

  /* Delete the exited device in device list*/
  Device_removeType(DEVICE_GRID);

  /*通过flash中的点表数据生成采集所需的modbus命令*/
  list = pointTabCmomandGet();
  point = list_nextData(list, null);
  while (point != null) {
    ModbusGetCmd_t *cmd = list_nextData(&point->cmdList, null);
    if (cmd != null) {
      dev = list_nodeApply(sizeof(Device_t));
      info = memory_apply(sizeof(StateGridDeviceInfo_t));

      info->addr = point->addr;
      dev->cfg = null;
      dev->callBack = dataAck;
      dev->explain = info;
      dev->type = DEVICE_GRID;
      list_init(&dev->cmdList);
      Device_add(dev);
    }
    while (cmd != null) {
      /* Create the final modbus commands*/
      Modbus_GetCmdAdd(&dev->cmdList, point->addr, cmd, sizeof(ModbusGetCmd_t));
      cmd = list_nextData(&point->cmdList, cmd);
    }
    point = list_nextData(list, point);
  }
  list_trans(list, pointTabFree, null);
  list_delete(list);
  memory_release(list);
}
/******************************************************************************/

