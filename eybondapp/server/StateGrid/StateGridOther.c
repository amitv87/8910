/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
  *@brief   : state grid Other device data get
  *@notes   : 2018.10.23 CGQ
*******************************************************************************/
#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#endif
#include "eyblib_list.h"
#include "eyblib_swap.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_memory.h"

#include "eybpub_Debug.h"
#include "eybpub_SysPara_File.h"
#include "eybpub_run_log.h"

#include "Device.h"
#include "StateGridOther.h"

static OtherCollectTab_t *otherCollecTab = null;
static int dataState = 0;
static ListHandler_t *comdlist = null;

static int aifuDataProcess(char *data);
static void pointDataSys(u16_t number, DeviceCmd_t *cmd);
static void UpdateCommunicationState(u8_t number, u8_t result);
static u8_t dataCheck(void *load, void *changeData);
static u8_t  dataAck(Device_t *dev);
static u8_t cmdAdd(void *load, void *changeData);
static void otherCollectCmd(void);

static StateGridPointTab_t *otherCheck(void *pointFile, int fileSize);
static void otherCollectCmd(void);
static int otherStatus(void);
static void otherRelease(void);

const StateGridDevice_t StateGridOtherDevice = {
  otherCheck,
  otherCollectCmd,
  otherStatus,
  otherRelease
};

static void save(Buffer_t *buf) {
  Buffer_t paraBuf;

//  SysPara_Get(48, &paraBuf);
  parametr_get(48, &paraBuf);
  if (paraBuf.payload != null && paraBuf.lenght > 0) {
    int num;
    num = Swap_charNum((char *)paraBuf.payload);
    memory_release(paraBuf.payload);
    if (num == 8) {
      char *str = memory_apply(buf->lenght * 3 + 8);
      if (str != null) {
        int l = Swap_hexChar(str, buf->payload, buf->lenght, 0);
        log_save(str);
        memory_release(str);
      }
    }
  }
}

/**
 * @brief 艾伏设备回复数据的特殊处理 计算出来的值放在后面多余的位置
 * @param cmd
 */
static int aifuProcess(DeviceCmd_t *cmd) {
  int ret = 0;
  int i = 0;
  int j = 0;
  if (cmd->ack.payload[5] == 0x10) {
//        log_save("aifu data process");
//        save(&cmd->ack);
    for (int i = 0; i < cmd->ack.lenght; i++) {
      if (cmd->ack.payload[i] == 0x1B && (cmd->ack.payload[i + 1] == 0xE7
                                          || cmd->ack.payload[i + 1] == 0xE8
                                          || cmd->ack.payload[i + 1] == 0x00)) {
        if (cmd->ack.payload[i + 1] == 0xE7) {
          cmd->ack.payload[i] = 0x02;
        }
        if (cmd->ack.payload[i + 1] == 0xE8) {
          cmd->ack.payload[i] = 0x03;
        }
        if (cmd->ack.payload[i + 1] == 0x00) {
          cmd->ack.payload[i] = 0x1B;
        }

        for (j = i + 2; j < cmd->ack.lenght; j++) {
          cmd->ack.payload[j - 1] = cmd->ack.payload[j];
          cmd->ack.payload[j] = 0;
        }
      }
    }
    ret = 0;
  } else if ((cmd->ack.payload[5] == 0x01))
    // ||(cmd->ack.payload[5] == 0x00))//02E4艾伏特殊处理
  {
    ret = 0;
  } else {
    ret = -1;
  }

  return ret;
}

/**
 * 锦浪数据处理 有位置没值
 * @param cmd
 * @return
 */
static int jinlangProcess(DeviceCmd_t *cmd) {
  int ret = 0;
  int i = 0;
  int j = 0;
  u32_t activePower1 = 0;
  u32_t activePower2 = 0;
  u32_t activePower3 = 0;
  u16_t totalPower = 0;
  u16_t currentVoltage = 0;
  u16_t currentCurrent = 0;
  r_memcpy(&currentVoltage, &cmd->ack.payload[4], 2);
  r_memcpy(&currentCurrent, &cmd->ack.payload[6], 2);
  activePower1 = currentVoltage * currentCurrent;


  r_memcpy(&currentVoltage, &cmd->ack.payload[28], 2);
  r_memcpy(&currentCurrent, &cmd->ack.payload[30], 2);
  activePower2 = currentVoltage * currentCurrent;

  r_memcpy(&currentVoltage, &cmd->ack.payload[49], 2);
  r_memcpy(&currentCurrent, &cmd->ack.payload[51], 2);
  activePower3 = currentVoltage * currentCurrent;

  totalPower = (activePower1 + activePower2 + activePower3) / 10;

  r_memcpy(&cmd->ack.payload[53], &totalPower, sizeof(totalPower));

  return ret;
}

static float time_character_analysis(u16_t *year, u8_t *month, u8_t *day, u8_t *value) {
  u8_t *p                  = value;   //用于算时间
  u8_t cnt                 = 0;       //计数
  u8_t ConnectionPlace1    = 0;       //连接符1
  u8_t ConnectionPlace2    = 0;       //连接符2
  u8_t i                   = 0;

  u8_t *v    = value; //用于算数值
  u8_t cnt2  = 0;     //计数
  u8_t space = 0;     //空格
  u8_t end   = 0;     //结尾

  while (' ' != *p) {                 //不为空格
    if ('-' == *p) {
      if (1 == i) {
        ConnectionPlace2 = cnt;
      }
      if (0 == i) {
        ConnectionPlace1 = cnt;
        i++;
      }
    }
    p++;
    cnt++;
  }

  char *year_buf  =   NULL;
  char *month_buf =   NULL;
  char *day_buf   =   NULL;

  year_buf    = memory_apply(ConnectionPlace1 + 1);
  month_buf   = memory_apply(ConnectionPlace2 - ConnectionPlace1);
  day_buf     = memory_apply(cnt - ConnectionPlace2);

  r_memset(year_buf,  0, ConnectionPlace1 + 1);
  r_memset(month_buf, 0, ConnectionPlace2 - ConnectionPlace1);
  r_memset(day_buf,   0, cnt - ConnectionPlace2);

  r_memcpy(year_buf,    value,                      ConnectionPlace1);
  r_memcpy(month_buf,   value + ConnectionPlace1 + 1,   ConnectionPlace2 - ConnectionPlace1 - 1);
  r_memcpy(day_buf,     value + ConnectionPlace2 + 1,   cnt - ConnectionPlace2 - 1);

#ifdef _PLATFORM_M26_
  *year  = Ql_atoi(year_buf);
  *month = Ql_atoi(month_buf);
  *day   = Ql_atoi(day_buf);
#endif
#ifdef _PLATFORM_L610_
  *year  = atoi(year_buf);
  *month = atoi(month_buf);
  *day   = atoi(day_buf);
#endif
  memory_release(year_buf);
  memory_release(month_buf);
  memory_release(day_buf);

  while (0 != *v) {
    if (' ' == *v) {  // 空格
      space = cnt2;  // 空格
    }
    v++;
    cnt2++;
    if (0  == *v) {  // 结尾
      end = cnt2;  // 结尾
    }
  }
  char *value_buf  = NULL;
  value_buf        = memory_apply(end - space);
  r_memset(value_buf,  0, end - space);
  r_memcpy(value_buf, value + space + 1, end - space - 1);
#ifdef _PLATFORM_M26_
  float power_value  = Ql_atoi(value_buf);
#endif
#ifdef _PLATFORM_L610_
  float power_value  = atoi(value_buf);
#endif
  memory_release(value_buf);
  return power_value;
}

static float hongguang_time_comparison(float total_power) {
  Buffer_t sys_time;
//  SysPara_Get(26, &sys_time);
  parametr_get(26, &sys_time);
  u16_t  sys_year         =   0;              // [cMinYear,cMaxYear]
  u8_t   sys_month        =   0;              // [1,12]
  u8_t   sys_day          =   0;              // [1,31]
  time_character_analysis(&sys_year, &sys_month, &sys_day, sys_time.payload);

  Buffer_t hg_time_hg_value;
//  SysPara_Get(36, &hg_time_hg_value);
  parametr_get(36, &hg_time_hg_value);
  u16_t  hongguang_year   =   0;              // [cMinYear,cMaxYear]
  u8_t   hongguang_month  =   0;              // [1,12]
  u8_t   hongguang_day    =   0;              // [1,31]
  float hongguang_value = time_character_analysis(&hongguang_year, &hongguang_month, &hongguang_day,
                          hg_time_hg_value.payload);
  if ((hongguang_year  == sys_year)
      && (hongguang_month == sys_month)
      && (hongguang_day   == sys_day)) {   // 没有跨天
    memory_release(hg_time_hg_value.payload);
    memory_release(sys_time.payload);
    return hongguang_value;
  }

  if ((2015 == sys_year)
      && (1    == sys_month)
      && (1    == sys_day)) {               // 没有跨天
    memory_release(hg_time_hg_value.payload);
    memory_release(sys_time.payload);
    return hongguang_value;
  }

  //跨天了 刷新时间和值
  char buf3[64] = {0};
#ifdef _PLATFORM_M26_
  Ql_sprintf(buf3, "%d-%d-%d %f", sys_year, sys_month, sys_day, total_power);
#endif
#ifdef _PLATFORM_L610_
  snprintf(buf3, 64, "%d-%d-%d %f", sys_year, sys_month, sys_day, total_power);
#endif  
  Buffer_t time_value;
  time_value.lenght = 50;
  r_memcpy(time_value.payload, buf3, 50);
//  SysPara_Set(36, &time_value);
  parametr_set(36, &time_value);

  Buffer_t new_time_value;
//  SysPara_Get(36, &new_time_value);
  parametr_get(36, &new_time_value);
  hongguang_value = time_character_analysis(&hongguang_year, &hongguang_month, &hongguang_day, new_time_value.payload);
  memory_release(new_time_value.payload);
  memory_release(hg_time_hg_value.payload);
  memory_release(sys_time.payload);
  return hongguang_value;
}

/**
 * 竑光数据处理 用于计算日发电量 没有对应寄存器的位置 计算出来的值放在前面不重要的位置
 * @param cmd
 * @return
 */
static int hongguangProcess(DeviceCmd_t *cmd) {
  int ret = 0;
  if (0x34 == cmd->cmd.payload[7]) { //定位到对应的指令
    //总发电量
    float total_power_real           = 0.00;
    char *total_power_strValue      = NULL;
    total_power_strValue             = (char *)memory_apply(7 + 1);
    r_memset(total_power_strValue, 0, 7 + 1);
    r_memcpy(total_power_strValue, &cmd->ack.payload[14], 7);
    total_power_real = (float)Swap_charNum(total_power_strValue);
    memory_release(total_power_strValue);

    //当天第1笔总发电量
    float total_power_1 = hongguang_time_comparison(total_power_real);

    //日发电量
    float day_power_real        = 0.00;
    char *day_power_strValue   = NULL;
    day_power_strValue          = (char *)memory_apply(5 + 1); //日发电量
    r_memset(day_power_strValue, 0, 5 + 1);
    day_power_real = total_power_real - total_power_1;//日发电量=总发电量-当天第1笔总发电量
    Swap_numChar(day_power_strValue, day_power_real);
    r_memcpy(&cmd->ack.payload[1], day_power_strValue, 5);
    memory_release(day_power_strValue);
  }
  return ret;
}

////欧姆尼克02F6有位置没值
static int omnikProcess(DeviceCmd_t *cmd) {
  int ret         = 0;
  u16_t Pac_R       = 0;
  u16_t Pac_S       = 0;
  u16_t Pac_T       = 0;
  u16_t Pac_total   = 0;
  r_memcpy(&Pac_R, &cmd->ack.payload[37], 2);
  r_memcpy(&Pac_S, &cmd->ack.payload[39], 2);
  r_memcpy(&Pac_T, &cmd->ack.payload[41], 2);
  Pac_total = Pac_R + Pac_S + Pac_T;
  r_memcpy(&cmd->ack.payload[45], &Pac_total, sizeof(Pac_total));
  return ret;
}

/**
 * @brief 设备回复数据的特殊处理
 * @param cmd
 */
static int SpecialProcess(DeviceCmd_t *cmd) {
  int ret = 0;

  Buffer_t Protocol;
  Buffer_t CompanyID;

//  SysPara_Get(14, &Protocol);
//  SysPara_Get(8, &CompanyID);
  parametr_get(14, &Protocol);
  parametr_get(8, &CompanyID);

  // 艾伏1BE7 E8 E9数据处理
  // 艾伏02E4
  if (!r_memcmp("02FF", Protocol.payload, strlen("02FF")) && !r_memcmp("89", CompanyID.payload, 2)) {
    ret = aifuProcess(cmd);
  } else if (!r_memcmp("02FF", Protocol.payload, strlen("02FF")) && !r_memcmp("55", CompanyID.payload, 2)) {
    ret = jinlangProcess(cmd);
  } else if (!r_memcmp("02FF", Protocol.payload, strlen("02FF")) && !r_memcmp("13", CompanyID.payload, 2)) {
    ret = hongguangProcess(cmd);//竑光
  } else if (!r_memcmp("02FF", Protocol.payload, strlen("02FF")) && !r_memcmp("77", CompanyID.payload, 2)) {
    ret = omnikProcess(cmd);//欧姆尼克02F6
  }

  memory_release(Protocol.payload);
  memory_release(CompanyID.payload);

  return ret;
}
/*******************************************************************************
  * @note   state grid point taber
  * @param  None
  * @retval None
*******************************************************************************/
static StateGridPointTab_t *otherCheck(void *pointFile, int fileSize) {
  OtherPointFile_t fileValue;
  StateGridPointTab_t *tab = null;
  u8_t *point = pointFile;

  if (otherCollecTab != null) {
    memory_release(otherCollecTab);
    otherCollecTab = null;
  }

  if (comdlist != null) {
    list_delete(comdlist);
    memory_release(comdlist);
    comdlist = null;
  }

  r_memcpy(&fileValue.flag, point, 4);
  point += 4;
  r_memcpy(&fileValue.size, point, 4);
  point += 4;
  fileValue.cmdCount = *point;
  point++;
  if (((fileValue.flag == -1) || (fileValue.flag == -2))
      && ((fileValue.size + 8) == fileSize)) {
    int i;

//    fileValue.cmdTab = (OtherCmd_t *)point;
    fileValue.cmdTab = point;

    for (i = 0; i < fileValue.cmdCount; i++) {
      point += *point + 1;
    }

    r_memcpy(&fileValue.pointCount, point, 4);
    point += 4;
    fileValue.point = point;

    /* i is total size of collect point */
    i = (fileSize - ((int)point - (int)pointFile));

    if (((fileValue.pointCount * sizeof(FileOtherPoint_t)) == (fileSize - ((int)point - (int)pointFile)))
        || ((fileValue.pointCount * sizeof(FileOtherPointV2_t)) == (fileSize - ((int)point - (int)pointFile)))
       ) {
      tab = memory_apply(fileValue.pointCount * sizeof(StateGridPoint_t) + 4);
      otherCollecTab = memory_apply(fileValue.pointCount * sizeof(OtherCollectPoint_t) + 4);

      if (tab != null && otherCollecTab != null) {
        FileOtherPointV2_t pointValue;

        r_memset(tab, 0, (fileValue.pointCount * sizeof(StateGridPoint_t) + 4));
        r_memset(otherCollecTab, 0, (fileValue.pointCount * sizeof(OtherCollectPoint_t)  + 4));
        tab->count = fileValue.pointCount;
        otherCollecTab->count = fileValue.pointCount;
        for (i = 0; i < fileValue.pointCount; i++) {
          r_memcpy(&pointValue, fileValue.point, sizeof(FileOtherPointV2_t));

          if (fileValue.flag == -1) {
            pointValue.type = pointValue.rsv >> 8;
            pointValue.note = pointValue.rsv;
            pointValue.rsv = 0;
            fileValue.point += sizeof(FileOtherPoint_t);
          } else {
            fileValue.point += sizeof(FileOtherPointV2_t);
          }
          tab->tab[i].id = pointValue.id;
          tab->tab[i].type = pointValue.type;
          tab->tab[i].lenght = pointValue.note;
          tab->tab[i].rsv = pointValue.rsv;
          tab->tab[i].addr = pointValue.number;

          otherCollecTab->tab[i].point.number = pointValue.number;
          otherCollecTab->tab[i].point.offset = pointValue.offset;
          otherCollecTab->tab[i].src = &tab->tab[i];
        }

        comdlist = memory_apply(sizeof(ListHandler_t));
        if (comdlist != null) {
          OtherCmd_t *cmd;
          u8_t *cmdData;
          int lenght;

          list_init(comdlist);
          cmdData = fileValue.cmdTab;
          for (i = 0; i < fileValue.cmdCount; i++) {
            lenght = *cmdData + 1;
            cmd = list_nodeApply(lenght);

            if (cmd != null) {
              r_memcpy(cmd, cmdData, lenght);
              cmdData += lenght;
              list_bottomInsert(comdlist, cmd);
            } else {
              list_delete(comdlist);
              memory_release(comdlist);
              comdlist = null;
              break;
            }
          }
        }

        if (comdlist == null) {
          APP_DEBUG("Point list fail!!\r\n");
          goto CMD_LIST_REE;
        }

        APP_DEBUG("Point table load OK : %d\r\n", fileValue.pointCount);
      } else {
      CMD_LIST_REE:
        memory_release(tab);
        memory_release(otherCollecTab);
        tab = null;
        otherCollecTab = null;

        log_save("State grid point buffer apply fail!!");
      }
    } else {
      log_save("State grid other file error!");
    }
  }

  return tab;
}


/*******************************************************************************
  * @note   state grid point new resource destroy
  * @param  None
  * @retval dataState > 0 data is valict
*******************************************************************************/
static void otherRelease(void) {
  Device_removeType(DEVICE_GRID);
  memory_release(otherCollecTab);
  if (comdlist != null) {
    list_delete(comdlist);
    memory_release(comdlist);
    comdlist = null;
  }
  otherCollecTab = null;
  dataState = 0;
}

static void pointDataSys(u16_t number, DeviceCmd_t *cmd) {
  int i, lenght, offset, DataLength;

  if (SpecialProcess(cmd)) {
    return;
  }

  for (i = 0; i < otherCollecTab->count; i++) {
    lenght = StateGrid_pointValueLenght(otherCollecTab->tab[i].src->type, otherCollecTab->tab[i].src->lenght,
                                        otherCollecTab->tab[i].src->rsv);
    DataLength = lenght;
    if (otherCollecTab->tab[i].src->type == 'S') {
      DataLength = 2;
    }

    offset = otherCollecTab->tab[i].point.offset;

    if (otherCollecTab->tab[i].src->id == 288549063)
      APP_DEBUG("type = %d, id = %ld, lenght=%d, size = %d, valueLength = %d\r\n",
                otherCollecTab->tab[i].src->type,
                otherCollecTab->tab[i].src->id, otherCollecTab->tab[i].src->lenght,
                otherCollecTab->tab[i].src->value.size,  otherCollecTab->tab[i].src->value.lenght);
    if ((otherCollecTab->tab[i].point.number == number)
        && ((offset + DataLength) <= cmd->ack.lenght)) {  // 处理特雷斯_竑光问题时需屏蔽该行判断
      otherCollecTab->tab[i].src->value.lenght = lenght;
      if (otherCollecTab->tab[i].src->value.size == 0) {
        otherCollecTab->tab[i].src->value.size = lenght;
        otherCollecTab->tab[i].src->value.payload = memory_apply(lenght);
      }

      if (lenght == 1) {
        if (otherCollecTab->tab[i].src->type == 'b') {
          u16_t value = ((cmd->ack.payload[offset] << 8) | (cmd->ack.payload[offset + 1]));
          otherCollecTab->tab[i].src->value.payload[0] = (value >> otherCollecTab->tab[i].src->lenght) & 0x01;
        } else if (otherCollecTab->tab[i].src->type == 'B') {

          if ((otherCollecTab->tab[i].src->rsv & 0x8000) == 0x8000) {
            u8_t val  = cmd->ack.payload[offset + otherCollecTab->tab[i].src->lenght];
            u8_t offset = (otherCollecTab->tab[i].src->rsv >> 8) & 0x0F;
            val = (val >> offset)&otherCollecTab->tab[i].src->rsv;
            otherCollecTab->tab[i].src->value.payload[0] = val;
          } else {
            otherCollecTab->tab[i].src->value.payload[0] = cmd->ack.payload[offset + otherCollecTab->tab[i].src->lenght];
          }

        } else if (otherCollecTab->tab[i].src->type == 'e') {
          u16_t val;
          otherCollecTab->tab[i].src->value.payload[0] = 0;

          if (otherCollecTab->tab[i].src->lenght == 0) {  // big
            val = (u16_t)((cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
          } else {
            val = (u16_t)((cmd->ack.payload[offset]) | (cmd->ack.payload[offset + 1] << 8));
          }

          if (val == otherCollecTab->tab[i].src->rsv) {
            otherCollecTab->tab[i].src->value.payload[0] = 1;
          }
        }
      } else if (((otherCollecTab->tab[i].src->lenght & 0x80) == 0x80)
                 && ((otherCollecTab->tab[i].src->type == 'S')
                     || (otherCollecTab->tab[i].src->type == 'I')
                     || (otherCollecTab->tab[i].src->type == 't'))) {
        int k;
        float *pF = (float *)otherCollecTab->tab[i].src->value.payload;
        float m = 1.00;
        float real = 0.00;

        if (otherCollecTab->tab[i].src->type == 't') {
          int val = 0;
          switch (otherCollecTab->tab[i].src->rsv) {
            case 1:
              val = (((cmd->ack.payload[offset + 2] & 0xF0) << 4) | cmd->ack.payload[offset]);
              break;
            case 2:
              val  = (((cmd->ack.payload[offset + 1] & 0x0F) << 8) | cmd->ack.payload[offset]);
              break;
            case 3:
              val = cmd->ack.payload[offset];
              break;
            default:
              break;
          }
          real = val;
        } else if (otherCollecTab->tab[i].src->type == 'S') {
          if ((otherCollecTab->tab[i].src->lenght & 0x40) == 0x40) {  // 有符号
            s16_t val;
            if ((otherCollecTab->tab[i].src->lenght & 0x20) == 0x20) {  // 寄存器小端
              val = (s16_t)((cmd->ack.payload[offset]) | (cmd->ack.payload[offset + 1] << 8));
            } else {
              val = (s16_t)((cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
            }

            real = val;
          } else {
            u16_t val;
            if ((otherCollecTab->tab[i].src->lenght & 0x20) == 0x20) {  // 寄存器小端
              val = (u16_t)((cmd->ack.payload[offset]) | (cmd->ack.payload[offset + 1] << 8));
            } else {
              val = (u16_t)((cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
            }

            real = val;
//          APP_DEBUG("var = %02X, offset = %d\r\n", val, offset);
          }
        } else {
          if ((otherCollecTab->tab[i].src->lenght & 0x40) == 0x40) {  // 有符号
            s32_t val;
            if ((otherCollecTab->tab[i].src->lenght & 0x30) == 0x30) {  // 低位寄存器在前-寄存器小端
              val = (s32_t)((cmd->ack.payload[offset + 3] << 24) | (cmd->ack.payload[offset + 2] << 16) |
                            (cmd->ack.payload[offset + 1] << 8) | cmd->ack.payload[offset]);
            } else if ((otherCollecTab->tab[i].src->lenght & 0x30) == 0x10) {  // 低位寄存器在前-寄存器大端
              val = (s32_t)((cmd->ack.payload[offset + 2] << 24) | (cmd->ack.payload[offset + 3] << 16) |
                            (cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
            } else if ((otherCollecTab->tab[i].src->lenght & 0x30) == 0x20) {  // 高位寄存器在前-寄存器小端
              val = (s32_t)((cmd->ack.payload[offset + 1] << 24) | (cmd->ack.payload[offset] << 16) |
                            (cmd->ack.payload[offset + 3] << 8) | cmd->ack.payload[offset + 2]);
            } else {  // 高位寄存器在前-寄存器大端
              val = (s32_t)((cmd->ack.payload[offset] << 24) | (cmd->ack.payload[offset + 1] << 16) |
                            (cmd->ack.payload[offset + 2] << 8) | cmd->ack.payload[offset + 3]);
            }

            real = val;
          } else {
            u32_t val;
            if ((otherCollecTab->tab[i].src->lenght & 0x30) == 0x10) {  // 低位寄存器在前-寄存器大端
              val = (u32_t)((cmd->ack.payload[offset + 2] << 24) | (cmd->ack.payload[offset + 3] << 16) |
                            (cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
            } else if ((otherCollecTab->tab[i].src->lenght & 0x30) == 0x30) {  // 低位寄存器在前-寄存器小端
              val = (u32_t)((cmd->ack.payload[offset + 3] << 24) | (cmd->ack.payload[offset + 2] << 16) |
                            (cmd->ack.payload[offset + 1] << 8) | cmd->ack.payload[offset]);
            } else if ((otherCollecTab->tab[i].src->lenght & 0x30) == 0x20) {  // 高位寄存器在前-寄存器小端
              val = (u32_t)((cmd->ack.payload[offset + 1] << 24) | (cmd->ack.payload[offset] << 16) |
                            (cmd->ack.payload[offset + 3] << 8) | cmd->ack.payload[offset + 2]);
            } else {  // 高位寄存器在前-寄存器大端
              val = (u32_t)((cmd->ack.payload[offset] << 24) | (cmd->ack.payload[offset + 1] << 16) |
                            (cmd->ack.payload[offset + 2] << 8) | cmd->ack.payload[offset + 3]);
            }

            real = val;
          }
        }

        for (k = otherCollecTab->tab[i].src->lenght & 0x07; k > 0; k--) {
          if ((otherCollecTab->tab[i].src->lenght & 0x08) == 0x08) {  // 放大
            m *= 10;
          } else {  // 缩小
            m *= 0.1;
          }
        }

        // 自定义功能
        if ((0x4000 == ((otherCollecTab->tab[i].src->rsv) & 0x4000))) {
          real = ((otherCollecTab->tab[i].src->rsv) & (~0x4000));
        }

        *pF = real * m;
        Swap_headTail(otherCollecTab->tab[i].src->value.payload, 4);  // 转换成大端模式
      } else if (otherCollecTab->tab[i].src->type == 'E') {
        char *strValue = NULL;
        float m = 1.00;
        float real = 0.00;
        float *pF = (float *)otherCollecTab->tab[i].src->value.payload;

        strValue = memory_apply(otherCollecTab->tab[i].src->rsv + 1);
        r_memset(strValue, 0, otherCollecTab->tab[i].src->rsv + 1);

        r_memcpy(strValue, &(cmd->ack.payload[offset]), otherCollecTab->tab[i].src->rsv);

        real = Swap_charNum(strValue);

        memory_release(strValue);

        for (int k = otherCollecTab->tab[i].src->lenght & 0x07; k > 0; k--) {
          if ((otherCollecTab->tab[i].src->lenght & 0x08) == 0x08) {  // 放大
            m *= 10;
          } else {  // 缩小
            m *= 0.1;
          }
        }

        *pF = real * m;
        Swap_headTail(otherCollecTab->tab[i].src->value.payload, 4);  // 转换成大端模式

      } else if (otherCollecTab->tab[i].src->type == 'A') {
        char *strValue = NULL;
        float m = 1.00;
        float real = 0.00;
        float *pF = (float *)otherCollecTab->tab[i].src->value.payload;

        strValue = memory_apply(otherCollecTab->tab[i].src->value.lenght);
        r_memset(strValue, 0, otherCollecTab->tab[i].src->value.lenght);

        if (offset == 32) {
          r_memcpy(strValue, &(cmd->ack.payload[29]), 3);

          if (aifuDataProcess(strValue)) {
            r_memset(strValue, 0, otherCollecTab->tab[i].src->value.lenght);
            r_memcpy(strValue, &(cmd->ack.payload[offset - 1]), 3);
          } else {
            r_memset(strValue, 0, otherCollecTab->tab[i].src->value.lenght);
            r_memcpy(strValue, &(cmd->ack.payload[offset]), 3);
          }
        } else {
          r_memcpy(strValue, &(cmd->ack.payload[offset]), 3);
        }

        aifuDataProcess(strValue);

        real = (strValue[0] << 8 | strValue[1]);

        memory_release(strValue);

        for (int k = otherCollecTab->tab[i].src->lenght & 0x07; k > 0; k--) {
          if ((otherCollecTab->tab[i].src->lenght & 0x08) == 0x08) {  // 放大
            m *= 10;
          } else {  // 缩小
            m *= 0.1;
          }
        }

        *pF = real * m;
        Swap_headTail(otherCollecTab->tab[i].src->value.payload, 4);  // 转换成大端模式

      } else if (otherCollecTab->tab[i].src->type == 'V') {
        char *strValue = NULL;
        float m = 1.00;
        float real = 0.00;
        float *pF = (float *)otherCollecTab->tab[i].src->value.payload;

        s16_t val = (s16_t)((cmd->ack.payload[offset] << 8) | cmd->ack.payload[offset + 1]);
        real = (val * 1.000) / 1.732;

        for (int k = otherCollecTab->tab[i].src->lenght & 0x07; k > 0; k--) {
          if ((otherCollecTab->tab[i].src->lenght & 0x08) == 0x08) {  // 放大
            m *= 10;
          } else {  // 缩小
            m *= 0.1;
          }
        }

        *pF = real * m;
        Swap_headTail(otherCollecTab->tab[i].src->value.payload, 4);  // 转换成大端模式

      } else if (otherCollecTab->tab[i].src->type == 'C') {
        char *strValue = NULL;
        float m = 1.00;
        float real = 0.00;
        float *pF = (float *)otherCollecTab->tab[i].src->value.payload;
        u16_t val = 0;

        real = cmd->ack.payload[offset];

        for (int k = otherCollecTab->tab[i].src->lenght & 0x07; k > 0; k--) {
          if ((otherCollecTab->tab[i].src->lenght & 0x08) == 0x08) {  // 放大
            m *= 10;
          } else {  // 缩小
            m *= 0.1;
          }
        }

        *pF = real * m;
        Swap_headTail(otherCollecTab->tab[i].src->value.payload, 4);  // 转换成大端模式

      } else {
        r_memcpy(otherCollecTab->tab[i].src->value.payload, &cmd->ack.payload[offset],
                 otherCollecTab->tab[i].src->value.lenght);
        if ((otherCollecTab->tab[i].src->lenght == 1)) {
          if (otherCollecTab->tab[i].src->type == 'I') {
            u32_t *pi = (u32_t *)otherCollecTab->tab[i].src->value.payload;

            *pi = (*pi >> 16) | (*pi << 16);
          } else if (otherCollecTab->tab[i].src->type == 'S') {
            u16_t *ps = (u16_t *)otherCollecTab->tab[i].src->value.payload;

            *ps = (*ps >> 8) | (*ps << 8);
          }

        }
      }
    }
  }
}

static void UpdateCommunicationState(u8_t number, u8_t result) {
  int i;

  APP_DEBUG("communicationstate = %d\r\n", result);
  for (i = 0; i < otherCollecTab->count; i++) {
    if (otherCollecTab->tab[i].point.number == number) {
      if ((otherCollecTab->tab[i].src->type == 0)
          && (otherCollecTab->tab[i].src->lenght == 0)
         ) {
        otherCollecTab->tab[i].src->value.lenght = 1;
        if (otherCollecTab->tab[i].src->value.size == 0) {
          otherCollecTab->tab[i].src->value.size = otherCollecTab->tab[i].src->value.lenght;
          otherCollecTab->tab[i].src->value.payload = memory_apply(otherCollecTab->tab[i].src->value.size);
        }
        otherCollecTab->tab[i].src->value.payload[0] = result;
      } else if (result == 0) {
        otherCollecTab->tab[i].src->value.lenght = 0;
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
  OtherCmdResult_t *ret = (OtherCmdResult_t *)changeData;
  DeviceCmd_t *cmd = (DeviceCmd_t *)load;

  ret->number++;

  if (0 == cmd->state && cmd->ack.lenght > 0
      && ((cmd->cmd.payload[0] == cmd->ack.payload[0])
          || ((cmd->cmd.payload[0] == cmd->ack.payload[1])
              && (cmd->cmd.payload[1] == cmd->ack.payload[0])))) {
    ret->result = 1;
    pointDataSys(ret->number, cmd);
    UpdateCommunicationState(ret->number, 1);
  } else {
    UpdateCommunicationState(ret->number, 0);
  }

  return 1;
}

/*******************************************************************************
  * @note   Collect status
  * @param  None
  * @retval None
*******************************************************************************/
int otherStatus(void) {
  return dataState;
}

/*******************************************************************************
  * @note   data Ack
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t  dataAck(Device_t *dev) {
  OtherCmdResult_t ret;

  r_memset(&ret, 0, sizeof(ret));

  dev->lock = 0;
  list_trans(&dev->cmdList, dataCheck, &ret);

  if (ret.result == 1) {
    if (dataState != 1) {
      dataState = 1;
      log_save("State grid data collect OK!");
    }
  }

  return 0;
}

/*******************************************************************************
  * @note   Device collect command add
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t cmdAdd(void *load, void *changeData) {
  DeviceCmd_t *cmd;
  ListHandler_t *decCmdList = changeData;
  OtherCmd_t *deviceCmd = load;

  cmd = list_nodeApply(sizeof(DeviceCmd_t));
  cmd->waitTime = 2000;
  cmd->state = 0;
  cmd->ack.size = 512;
  cmd->ack.lenght = 0;
  cmd->ack.payload = memory_apply(cmd->ack.size);
  cmd->cmd.size = deviceCmd->lenght;
  cmd->cmd.lenght = deviceCmd->lenght;
  cmd->cmd.payload = memory_apply(cmd->cmd.size);
  r_memcpy(cmd->cmd.payload, deviceCmd->data, cmd->cmd.size);
  list_bottomInsert(decCmdList, cmd);

  return 1;
}
static void otherCollectCmd(void) {
  Device_t *dev;

  Device_removeType(DEVICE_GRID);
  dev = list_nodeApply(sizeof(Device_t));

  dev->cfg = null;
  dev->callBack = dataAck;
  dev->explain = null;
  dev->type = DEVICE_GRID;
  list_init(&dev->cmdList);
  Device_add(dev);

  list_trans(comdlist, cmdAdd, &dev->cmdList);
}

/**
 * @brief 对艾伏特殊数据的处理
 * @param data
 * @return
 */
static int aifuDataProcess(char *data) {
  if (data[0] == 0x1B && (data[1] == 0xE7 || data[1] == 0xE8)) {
    if (data[1] == 0xE7) {
      data[0] = 0x02;
    } else {
      data[0] = 0x03;
    }

    data[1] = data[2];
    data[2] = 0;

    return 0;
  } else if (data[1] == 0x1B && (data[2] == 0xE7 || data[2] == 0xE8)) {
    if (data[2] == 0xE7) {
      data[1] = 0x02;
    } else {
      data[1] = 0x03;
    }

    data[2] = 0;

    return 0;

  } else {
    return -1;
  }
}
/******************************************************************************/

