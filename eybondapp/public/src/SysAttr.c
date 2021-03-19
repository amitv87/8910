/**************Copyright(C) 2015-2026 Eybond Temp Co., Ltd.******************
  *@brief   : SysAttr.c
  *@notes   : 2017.12.21 CGQ
  *@notes   : 防逆流相关逻辑
*******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_system.h"
#include "ql_stdlib.h"
#include "ql_memory.h"

#include "ril_network.h"
#include "ril_system.h"
#endif

#ifdef _PLATFORM_L610_
#endif

#include "eyblib_typedef.h"
#include "eyblib_list.h"
#include "eyblib_swap.h"

#include "eybpub_SysPara_File.h"
#include "eybpub_watchdog.h"
#include "eybpub_run_log.h"
// #include "eybpub_Debug.h"
#include "eybpub_Clock.h"

#include "NB_net.h"
#include "Device.h"
#include "ModbusDevice.h"
#include "DeviceIO.h"

#include "Eybond.h"
//#include "CommonServer.h"
#include "Protocol.h"

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
int Attr_onlineCnt(Buffer_t *buf) {

  buf->size = 8;
  buf->payload = memory_apply(buf->size);
  buf->lenght = Swap_numChar((char *)buf->payload, onlineDeviceList.count);

  return 0;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
int Attr_timeGet(Buffer_t *buf) {
//  int offset;
//  int timeZone;
//  int day;
//  int hour;
  Clock_t clock;

  Clock_timeZone(&clock);
  buf->size = 40;
  buf->payload = memory_apply(buf->size);

//  offset = Swap_numChar((char *)&buf->payload[offset], clock.year);
//  buf->payload[offset++] = '-';
//  offset += Swap_numChar((char *)&buf->payload[offset], clock.month);
//  buf->payload[offset++] = '-';
//  offset += Swap_numChar((char *)&buf->payload[offset], clock.day);
//  buf->payload[offset++] = ' ';
//  offset += Swap_numChar((char *)&buf->payload[offset], clock.hour);
//  buf->payload[offset++] = ':';
//  offset += Swap_numChar((char *)&buf->payload[offset], clock.min);
//  buf->payload[offset++] = ':';
//  offset += Swap_numChar((char *)&buf->payload[offset], clock.secs);
//  buf->lenght = offset;

  r_memset(buf->payload, 0, buf->size);
  Ql_sprintf((char *)buf->payload, "%d-%d-%d %d:%d:%d", clock.year, clock.month, clock.day, clock.hour, clock.min, clock.secs);
  buf->lenght = Ql_strlen((char *)buf->payload);

  return 0;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
int Attr_option(Buffer_t *buf) {
  if (buf->lenght > 0 && buf->payload != null) {
    switch (buf->payload[0]) {
      case '1': //soft reboot
        log_save("Command soft reboot!!");
        Watchdog_stop();
        break;
      case '2': // reset to default val
        log_save("Command reset to default value!!");
//          SysPara_default();   // mike 20200909
        parametr_default();
//        HistoryDataClean();  // mike 20200909
//        CommonServerData_clean();   // mike 20200909
        break;
      case '3': // reset device Modular
        log_save("Command reset device module!!");
        Ql_OS_SendMessage(EYBDEVICE_TASK, DEVICE_RESTART_ID, 0, 0);
        break;
      case '4': // reset GPRS Modular
        log_save("Command reset GPRS module!!");
        Ql_OS_SendMessage(EYBNET_TASK, NET_CMD_RESTART_ID, 0, 0);
        break;
      case '9':
//          SysParaErase();     // mike 20200909
        break;
    }
  }

  return 0;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
int Attr_bandrateGet(Buffer_t *buf) {  
  ST_UARTDCB cfg; 
  int ret; 
  ret = Ql_UART_GetDCBConfig(DEVICE_IO_PORT, &cfg); 
//  u16_t offset = 0; 
  if (ret == 0) {
    buf->size = 16;
    buf->payload = memory_apply(buf->size);
    r_memset(buf->payload, 0, buf->size);
    Ql_sprintf((char *)buf->payload, "%d-%d-%d-%d", cfg.baudrate, cfg.dataBits, cfg.stopBits, cfg.parity);
    buf->lenght = r_strlen((char *)buf->payload);
  } else {
    buf->lenght = 0;
    buf->payload = null;
  }
  return ret;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
int Attr_bandrateSet(Buffer_t *buf) {
  int ret = 0;
/*  ListHandler_t uartCfgStr;
  r_strsplit(&uartCfgStr, (char*)buf->payload, '-');
  int tab[4];
  int i = 0;
  Node_t *node;
  node = uartCfgStr.node;
  do { 
    tab[i++] = Swap_charNum((char*)*(int*)node->payload);
    APP_DEBUG("\r\nBand %d, %d.\r\n", i, tab[i-1]);
    node = node->next;
  } while(node != uartCfgStr.node && i < 4);
  if ( (tab[0] > 2000 && tab[0] < 1000000)  // 波特率 
    && (tab[1] > 4 && tab[1] < 9)  //数据位 
    && (tab[2] > 0 && tab[2] < 4)  //停止位
    && (tab[3] >= 0 && tab[3] < 5) ) {  //校验位 
    ProtocolAttr_t attr;
    ST_UARTDCB uart;
    char str[64] = {0};
    int offset = 0;
    Buffer_t buf_temp;
    attr.code = 0x02FF;
    attr.startAddr = 1;
    attr.endAddr = 1;
    attr.cfg = null;
    uart.baudrate = tab[0];
    uart.dataBits = tab[1];
    uart.stopBits = tab[2];
    uart.parity = tab[3];
    uart.flowCtrl = FC_NONE;
    protocolAttrGet(DEVICE_PROTOCOL, &attr); // 14 号参数
    if (attr.cfg != null) {
//    memory_release(attr.cfg);
      Ql_MEM_Free(attr.cfg);
      attr.cfg = NULL;
    }
    Ql_memset(str, 0, sizeof(str));
    attr.cfg = &uart;
    Swap_numHexChar(str, attr.code);
//    offset = r_stradd(str, ",");
    Ql_strcat(str, ",");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.startAddr);
    Ql_strcat(str, ",");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.endAddr);
    Ql_strcat(str, ",#");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset],attr.cfg->baudrate);
    Ql_strcat(str, "-");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->dataBits);
    Ql_strcat(str, "-");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->stopBits);
    Ql_strcat(str, "-");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->parity);
    Ql_strcat(str, "#");
    offset = Ql_strlen(str);
    APP_DEBUG("str:%s len:%d\r\n", str, offset);
    buf_temp.lenght = offset;
    buf_temp.payload = (u8_t*)str;
//    ret = SysPara_Set(DEVICE_PROTOCOL, &buf);
    ret = parametr_set(DEVICE_PROTOCOL, &buf_temp);
    Ql_memset(str, 0, sizeof(str));
    Ql_strcpy(str, "");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset],attr.cfg->baudrate);
    Ql_strcat(str, "-");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->dataBits);
    Ql_strcat(str, "-");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->stopBits);
    Ql_strcat(str, "-");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->parity);
    Ql_strcat(str, "");
    offset = Ql_strlen(str);
    APP_DEBUG("str:%s len:%d\r\n", str, offset);
    buf_temp.lenght = offset;
    buf_temp.payload = (u8_t*)str;
    ret = parametr_set(DEVICE_UART_SETTING, &buf_temp);
  } else if (tab[0]>2000 && tab[0]<1000000) {  // 波特率
    ProtocolAttr_t attr;
    ST_UARTDCB uart;
    char str[64] = {0};
    int offset = 0;
    Buffer_t buf_temp;
    attr.code = 0x02FF;
    attr.startAddr = 1;
    attr.endAddr = 1;
    attr.cfg = null;
    uart.baudrate = tab[0];
    uart.dataBits = DB_8BIT;
    uart.stopBits = SB_ONE;
    uart.parity = PB_NONE;
    uart.flowCtrl = FC_NONE;
    protocolAttrGet(DEVICE_PROTOCOL, &attr);
    if (attr.cfg != null) { 
//    memory_release(attr.cfg);
      Ql_MEM_Free(attr.cfg);
      attr.cfg = NULL;
    }
    Ql_memset(str, 0, sizeof(str));
    attr.cfg = &uart;
    Swap_numHexChar(str, attr.code);
    Ql_strcat(str, ",");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.startAddr);
    Ql_strcat(str, ",");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.endAddr);
    Ql_strcat(str, ",#");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->baudrate);
    Ql_strcat(str, "-");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->dataBits);
    Ql_strcat(str, "-");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->stopBits);
    Ql_strcat(str, "-");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->parity);
    Ql_strcat(str, "#");
    offset = Ql_strlen(str);
    APP_DEBUG("str:%s len:%d\r\n", str, offset);
    buf_temp.lenght = offset;
    buf_temp.payload = (u8_t*)str;
    ret = parametr_set(DEVICE_PROTOCOL, &buf_temp);
    Ql_memset(str, 0, sizeof(str));
    Ql_strcpy(str, "");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset],attr.cfg->baudrate);
    Ql_strcat(str, "-");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->dataBits);
    Ql_strcat(str, "-");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->stopBits);
    Ql_strcat(str, "-");
    offset = Ql_strlen(str);
    Swap_numChar(&str[offset], attr.cfg->parity);
    Ql_strcat(str, "");
    offset = Ql_strlen(str);
    APP_DEBUG("str:%s len:%d\r\n", str, offset);
    buf_temp.lenght = offset;
    buf_temp.payload = (u8_t*)str;
    ret = parametr_set(DEVICE_UART_SETTING, &buf_temp);
  } else {
    ret = -1;
  } */
  return ret;
}


/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
int Attr_logGet(Buffer_t *buf) {
  int ret;
  Buffer_t getBuf;

  buf->size = 4096;
  buf->payload = memory_apply(buf->size);
  buf->lenght = 0;

  while (buf->payload != null) {
    getBuf.size = buf->size - buf->lenght;
    getBuf.payload = &buf->payload[buf->lenght];
    ret = log_get(&getBuf);

    if (ret > 0) {
      buf->lenght += ret;
      if (buf->lenght > 900) {
        break;
      } else {
        buf->lenght--;
      }
    } else {
      break;
    }
  }

  return 0;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
int Attr_logSet(Buffer_t *buf) {
//  log_clear();    // mike 20200909
  return 0;
}


/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t cmdCode(void *load, void *dest) {
  int offset;
  DeviceCmd_t *cmd = (DeviceCmd_t *)load;
  char *str = (char *)dest;

  offset = r_strlen(str);
  Swap_numChar(&str[offset], cmd->state);
  r_strcat(str, ", ");
  offset =  r_strlen(str);
  Swap_hexChar(&str[offset], cmd->cmd.payload, cmd->cmd.lenght, 0);
  if (cmd->ack.payload != null && cmd->ack.size != cmd->ack.lenght) {
    r_strcat(str, "<");
    offset =  Ql_strlen(str);
    Swap_hexChar(&str[offset], cmd->ack.payload, cmd->ack.lenght, 0);
  }
  r_strcat(str, ";");
  offset =  r_strlen(str);
  return 1;
}
static u8_t addrCode(void *load, void *dest) {
  int offset;
  char *str = (char *)dest;
  Device_t *device = (Device_t *)load;

  if (device->type == DEVICE_MOBUS) {
    DeviceExplain_t *exp = device->explain;

    r_strcat(str, "[");
    offset =  Ql_strlen(str);
    Swap_numChar(&str[offset], exp->addr);
    r_strcat(str, ", ");
    offset =  Ql_strlen(str);
    Swap_numHexChar(&str[offset], exp->code);
    r_strcat(str, "]");
    offset =  Ql_strlen(str);
  } else if (device->type == DEVICE_GRID) {
    r_strcat(str, "[State Grid]");
  } else if (device->type == DEVICE_ARTI) {
    r_strcat(str, "[Adj power]");
  } else {
    r_strcat(str, "[TRANS]");
  }
  list_trans(&device->cmdList, cmdCode, str);
  offset = r_strlen(str);

  return (offset > 3072 ? 0 : 1);
}
int Attr_onlineType(Buffer_t *buf) {
  buf->size = 4096;
  buf->payload = memory_apply(buf->size);
  r_strcpy((char *)buf->payload, "");

  list_trans(&DeviceList, addrCode, buf->payload);

  buf->lenght = r_strlen((const char *)buf->payload);

  return 0;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
int Attr_CSQ(Buffer_t *buf) {
  int ret = 0;
  /*  u32 rssi;       // mike 20200909
    u32 ber;

    ret = RIL_NW_GetSignalQuality(&rssi, &ber);
    if (ret == 0)
    {
      buf->size = 8;
      buf->payload = memory_apply(buf->size);
      buf->lenght = Swap_numChar((char*)buf->payload, rssi);
    }
    else
    {
      buf->payload = null;
      buf->lenght = 0;
    } */

  return ret;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
int Attr_CPUID(Buffer_t *buf) {
  int ret = 0;
/*  char str[64] = {0};

  u8_t buf[12];

  Ql_GetUID(buf, sizeof(buf));
  Swap_hexChar(str, buf, sizeof(buf), 0);//*/

  /*  ret = RIL_GetIMEI(str);     // mike 20200909

    if (ret != 0)
    {
      buf->lenght = 0;
      buf->payload = null;
    }
    else
    {
      char *p = str;
      buf->size = 64;
      buf->payload = memory_apply(buf->size);
      while (*p != '\0')
      {
        if (*p <= '9' && *p >= '0')
        {
          buf->payload[ret++] = *p;
        }

        p++;
      }

      buf->lenght = 15;
      buf->payload[15] = '\0';
    } */
  return ret;
}


/******************************************************************************/

