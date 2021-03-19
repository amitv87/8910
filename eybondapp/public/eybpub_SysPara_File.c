/*******************************************************************************
 * @Author       : Chen Zhu Liang
 * @Date         : 2020-06-02 19:41:07
 * @LastEditTime : 2020-07-03 15:05:56
 * @LastEditors  : Chen Zhu Liang
 * @Description  :
 * @FilePath     : public\eybpub_SysPara_File.c
 * @可以输入预定的版权声明、个性签名、空行等
*******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "custom_feature_def.h"
#include "ril.h"
#include "ril_util.h"
#include "ril_system.h"
#include "ril_network.h"
#include "ril_oceanconnect.h"

#include "ql_type.h"
#include "ql_stdlib.h"
#include "ql_memory.h"
#include "ql_gprs.h"
#endif

#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#endif

#include "eybpub_SysPara_File.h"
#include "eybpub_watchdog.h"
#include "eybpub_Debug.h"
#include "eybpub_run_log.h"
#include "eybpub_utility.h"
#include "eybpub_data_collector_parameter_table.h"
#include "eybpub_parameter_number_and_value.h"

#include "eyblib_r_stdlib.h"
#include "eyblib_memory.h"
#include "eyblib_swap.h"
#include "eyblib_list.h"

#include "DeviceIO.h"
#include "Device.h"
#include "Protocol.h"
#include "eybond.h"
#include "CommonServer.h"

#include "grid_tool.h"


#ifdef _PLATFORM_BC25_
#include "NB_net.h"
#endif
#ifdef _PLATFORM_L610_
#include "4G_net.h"
#endif

static u8_t Para_Init_flag = 0;
char produc_save_flag = 0;
// static char para_value[64] = {0};  // 参数值


#ifdef _PLATFORM_BC25_
void parametr_get(u32_t number, Buffer_t *databuf) {
  char *buf_value = NULL;
  u16_t len = 0;
  if (databuf == NULL) {
    return;
  }
//  APP_DEBUG("para_meter get begin:%d!\r\n", number);
  int j = 0;
  for (j = 0; j < number_of_array_elements; j++) {
    if (number == PDT[j].num) {
      APP_DEBUG("para_meter[%d]num = %d!\r\n", j, number);
      buf_value = memory_apply(sizeof(char) * 64);
      if (buf_value == NULL) {
        APP_DEBUG("MEM Alloc Error\r\n");
        return;
      }
      r_memset(buf_value, 0, sizeof(char) * 64);
      if (number == 5 || number == 6 || number == 11 || number == 16\
          || number == 49 || number == 50 || number == 51 || number == 52\
          || number == 55 || number == 56 || number == 58 ||  number == 54) {
        switch (number) {
          case 5:  // 软件版本号
            r_strcpy(buf_value, defaultPara[j].para);
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
            break;
          case 6:  // 硬件版本号
            r_strcpy(buf_value, defaultPara[j].para);
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
            break;
          case 11:  // 是否有设备在线
            r_strcpy(buf_value, "1");
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
            break;
          case 16: {
            s32_t ret = 0;
            ST_Addr_Info_t addr_info;
            r_memset(&addr_info, 0, sizeof(ST_Addr_Info_t));
            ret = Ql_GetLocalIPAddress(1, &addr_info);
            if (GPRS_PDP_SUCCESS == ret) {
              APP_DEBUG("Get localIPAddress successfully: %s\r\n", addr_info.addr);
            } else {
              APP_DEBUG("Get localIPAddress failed,ret = %d\r\n", ret);
            }
            Ql_snprintf(buf_value, 64, "%s", addr_info.addr);
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
          }
          break;
          case 49:  // 查询网络注册信息
            // cm_gprs_getcregstate(&creg);
            // cm_itoa(creg,creg_char,10);
            // memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            // PDT[j].wFunc(&PDT[j],creg_char, &len);
          {
            s32_t ret  = RIL_AT_FAILED;
            Enum_OCREG_State OC_reg_state = UNINITIALISED;
            ret = RIL_OCEANCONNECT_GET_NMSTATUS((s8_t *)&OC_reg_state);
            if (RIL_AT_SUCCESS != ret) {
              APP_DEBUG("OC query regstate failure,ret = %d\r\n", ret);
            }
            switch (OC_reg_state) {
              case REGISTERED_AND_OBSERVED:
                r_strcpy(buf_value, "0");
                break;
              case REGISTERING:
                r_strcpy(buf_value, "1");
                break;
              case REJECTED_BY_SERVER:
                r_strcpy(buf_value, "1");
                break;
              case TIMEOUT_AND_RETRYING:
                r_strcpy(buf_value, "1");
                break;
              case REGISTERED:
                r_strcpy(buf_value, "0");
                break;
              case DEREGISTERED:
                r_strcpy(buf_value, "1");
                break;
              case RESUMPTION_FAILED:
                r_strcpy(buf_value, "1");
                break;
              case UNINITIALISED:
              default:
                r_strcpy(buf_value, "1");
                break;
            }
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
          }
          break;
          case 50:  // GPRS 网络注册状态
            // cm_gprs_getcgregstate(&cgreg);
            // cm_itoa(cgreg,cgreg_char,10);
            // memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            // PDT[j].wFunc(&PDT[j],cgreg_char, &len);
            break;
          case 51:  // 固件编译日期
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], MAKE_DATE, &len);
            break;
          case 52:  // 固件编译时间
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], MAKE_TIME2, &len);
            break;
          case 54:  // 获取日志
            databuf->payload = memory_apply(log_line_len * log_max_line);
            PDT[j].rFunc(&PDT[j], databuf->payload, &databuf->lenght);
            // log_i(databuf->payload);
            APP_DEBUG("\r\n");
            APP_DEBUG("%s", databuf->payload);
            return;
            break;
          case 55:
            // CSQ =cm_get_signalLevel();
            // cm_itoa(CSQ,CSQ_char,10);
            // cm_get_signalLevel(para_value);   //mike 20200824
          {
            ST_CSQ_Reponse csq_status;
            RIL_NW_GetCSQ(&csq_status);
//            Ql_snprintf(buf_value, 64, "%d,%d", csq_status.rssi, csq_status.ber);
            Ql_snprintf(buf_value, 64, "%d", csq_status.rssi);
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
          }
          break;
          case 56:  // 通信卡CCID
          //   cm_get_iccid(para_value);
             RIL_SIM_GetCCID(buf_value); 
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
            break;
          case 58:  // CPUID IMEI
            // cm_get_imei(para_value);
            RIL_GetIMEI(buf_value);
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
            break;
          default:
            break;
        }
      }

      r_memset(buf_value, 0, sizeof(char) * 64);
      PDT[j].rFunc(&PDT[j], buf_value, &len);
//      APP_DEBUG("%d=%s len=%d\r\n", PDT[j].num, buf_value, len);
      if (len > 0) {
//        APP_DEBUG("%d=%s len %d\r\n", PDT[j].num, buf_value, len);
        databuf->size = len;
        databuf->lenght = len;
        databuf->payload = memory_apply(databuf->size + 1);  // 将获取的参数传递出去 申请后需要在函数外部释放
        if (databuf->payload == NULL) {
          APP_DEBUG("sys para databuf payload apply fail\r\n");
        } else {
          r_memset(databuf->payload, 0, databuf->size + 1);
          r_memcpy(databuf->payload, buf_value, databuf->lenght);
        }
      } else {
        APP_DEBUG("para fail!\r\n");
      }
      memory_release(buf_value);
      break;
    }
  }

//  APP_DEBUG("index: %d totle: %d\r\n", j, number_of_array_elements);
  if (j >= number_of_array_elements) {
    databuf->size = 0;
    databuf->lenght = 0;
    databuf->payload = NULL;
  }
}

u8_t parametr_set(u32_t number, Buffer_t *data) {
  int j = 0;
  u8_t ret = 0x00;
  char *str = NULL;
  for (j = 0; j < number_of_array_elements; j++) {
    if (number == PDT[j].num) {
      APP_DEBUG("para[%d] number is %d Old value is %s\r\n", j, number, PDT[j].a);
      APP_DEBUG("para[%d] number is %d New value will be %s\r\n", j, number, data->payload);
      str = memory_apply(sizeof(char) * 64);
      if (str == NULL) {
        APP_DEBUG("MEM Alloc Error\r\n");
        ret = 0x03;
        return ret;
      }
      r_memset(str, 0, sizeof(str));
      r_strncpy(str, (char *)data->payload, r_strlen((char *)data->payload));
      APP_DEBUG("para[%d] number is %d str %s\r\n", j, number, str);
      switch (number) {  // 01/02/03/07/08/12/14/29/34/57 生产是必须写的参数
        case 0:
        case 4:
        case 5:
        case 6:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 27:
        case 28:
        case 49:
        case 50:
        case 51:
        case 52:
        case 55:
        case 56:
        case 58:
          APP_DEBUG("para_meter can't be set \r\n");
          ret = 0x02;
          break;
        case 14: {
        }
        break;
        case 29:
          switch (*(data->payload)) {
            case '1':  // 采集器重启
//              Watchdog_stop();
              log_save("System Software Reset!");
              Ql_Reset(0);  // mike 20200909
              break;
            case '2':  // 恢复出厂设置 -- 指示按defult参数表恢复，生产时写的数据如果改了就恢复不了了
              log_save("System Para Reset!");
              parametr_default();
              Ql_OS_SendMessage(EYBDEVICE_TASK, SYS_PARA_CHANGE, number, 0);
              break;
            case '3':  // 看门狗停狗重启
              log_save("System Hardware Reset!");
              Watchdog_stop();
              break;
            default:
              break;
          }
          ret = 0x00;
          APP_DEBUG("PARA %d=%s\r\n", PDT[j].num, data->payload);
          return ret;
          break;
        case 34: {
          ListHandler_t uartCfgStr;
          r_strsplit(&uartCfgStr, str, '-');
          int tab[4] = {0};
          int i = 0;
          Node_t *node;
          node = uartCfgStr.node;
          do {
            tab[i++] = Swap_charNum((char *) * (int *)node->payload);
            APP_DEBUG("\r\nBand %d, %d.\r\n", i, tab[i - 1]);
            node = node->next;
          } while (node != uartCfgStr.node && i < 4);
          ST_UARTDCB uart;
          if ((tab[0] > 2000 && tab[0] < 1000000)   // 波特率
              && (tab[1] > 4 && tab[1] < 9)  // 数据位
              && (tab[2] > 0 && tab[2] < 4)  // 停止位
              && (tab[3] >= 0 && tab[3] < 5)) {   // 校验位
            uart.baudrate = tab[0];
            uart.dataBits = tab[1];
            uart.stopBits = tab[2];
            uart.parity = tab[3];
            uart.flowCtrl = FC_NONE;
          } else if (tab[0] > 2000 && tab[0] < 1000000) {
            uart.baudrate = tab[0];
            uart.dataBits = DB_8BIT;
            uart.stopBits = SB_ONE;
            uart.parity = PB_NONE;
            uart.flowCtrl = FC_NONE;
          } else {
            ret = 0x02;
            break;
          }
          list_delete(&uartCfgStr);   // mike 20200922
          char str_temp_1[64] = {0};
          char str_temp_2[64] = {0};
          u16_t len = 0;
          char *P1 = NULL;
          r_memset(str, 0, sizeof(str));
          r_strncpy(str, (char *)data->payload, r_strlen((char *)data->payload));
          r_memset(str_temp_1, 0, sizeof(str_temp_1));
          r_memset(str_temp_2, 0, sizeof(str_temp_2));
          Ql_sprintf(str_temp_2, "#%d-%d-%d-%d#", uart.baudrate, uart.dataBits, uart.stopBits, uart.parity);
          for (i = 0; i < number_of_array_elements; i++) {
            if (14 == PDT[i].num) {
              PDT[i].rFunc(&PDT[i], str_temp_1, &len);
              P1 = r_strstr(str_temp_1, "#");
              if (P1) {
                *P1 = '\0';   // 去掉#号
                len = r_strlen(str_temp_1);
                APP_DEBUG("%s len:%d\r\n", str_temp_1, len);
                r_strcat(str_temp_1, str_temp_2);
                len = r_strlen(str_temp_1);
                APP_DEBUG("%s len:%d\r\n", str_temp_1, len);
                r_memset((&PDT[i])->a, 0, sizeof((&PDT[i])->a));
                PDT[i].wFunc(&PDT[i], str_temp_1, &(len));
              }
            }
          }
        }
        break;
        default:
          break;
      }

      if (ret == 0x00) {
        //这里只是写入成功，但最后是不是更新成功未知
        ret = 0x00;
        //防止写入字符串过长
        if (data->lenght >= sizeof((&PDT[j])->a)) {
          APP_DEBUG("para so long!\r\n");
          ret = 0x03;
          break;
        }
        r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
//        PDT[j].wFunc(&PDT[j], data->payload, &(data->lenght));
        PDT[j].wFunc(&PDT[j], str, &(data->lenght));

        if (number == 21) {
          log_save("set para 21: %s", (&PDT[j])->a);
        }
        APP_DEBUG("PARA %d=%s\r\n", PDT[j].num, str);
        break;
      }
      memory_release(str);
    }
    if (j >= number_of_array_elements) {
      APP_DEBUG("para_meter %d is wrongth\r\n", number);
      ret = 0x03;
    }
  }
  // start_timer();
  if (ret == 0x00) {
    parameter_a_module();
    a_copy_to_b();
    parameter_init();  // 保持统一

//    if (number == DEVICE_MONITOR_NUM || number == DEVICE_PROTOCOL || number == METER_PROTOCOL || number == DEVICE_UART_SETTING) { // mike 20201029
    if (number == DEVICE_MONITOR_NUM || number == DEVICE_PROTOCOL || number == DEVICE_UART_SETTING) {
      Ql_OS_SendMessage(EYBDEVICE_TASK, SYS_PARA_CHANGE, number, 0);
//    } else if (number == NB_SERVER_ADDR || number == NB_SERVER_PORT) {
//      Ql_OS_SendMessage(EYBOND_TASK, SYS_PARA_CHANGE, number, 0);
    } else if (number == DEVICE_PNID || number == NB_SERVER_ADDR || number == NB_SERVER_PORT) {
      Net_close();
    }
  }
  return ret ;
}

void main_parametr_update(void) { // 由于APP固件升级会让系统保存的参数不匹配，需要实时更新
  u16_t len = 0;
  int j = 0;
  int number = 0;
  char *buf_value = NULL;
  buf_value = memory_apply(sizeof(char) * 64);
  if (buf_value == NULL) {
    APP_DEBUG("MEM Alloc Error\r\n");
    return;
  }
//  r_memset(buf_value, 0, sizeof(char) * 64);

  APP_DEBUG("main_parametr_update!\r\n");
  for (j = 0; j < number_of_array_elements; j++) {
    number = PDT[j].num;
    r_memset(buf_value, 0, sizeof(char) * 64);
    if (number == 5 || number == 6 || number == 11 || number == 49 || number == 50 \
        || number == 51 || number == 52 || number == 55 || number == 56 || number == 58) {
      switch (number) {
        case 5:  //软件版本号
          r_strcpy(buf_value, defaultPara[j].para);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(buf_value);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        case 6:  //硬件版本号
          r_strcpy(buf_value, defaultPara[j].para);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(buf_value);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        case 11:  //是否有设备在线
          r_strcpy(buf_value, "1");
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = Ql_strlen(buf_value);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        case 49:  //查询网络注册信息
          // cm_gprs_getcregstate(&creg);
          // cm_itoa(creg,creg_char,10);
          // r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          // PDT[j].wFunc(&PDT[j],creg_char, &len);
          break;
        case 50:  //GPRS 网络注册状态
          // cm_gprs_getcgregstate(&cgreg);
          // cm_itoa(cgreg,cgreg_char,10);
          // r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          // PDT[j].wFunc(&PDT[j],cgreg_char, &len);
          break;
        case 51:  //固件编译日期
          r_strcpy(buf_value, defaultPara[j].para);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(defaultPara[j].para);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        case 52:  //固件编译时间
          r_strcpy(buf_value, defaultPara[j].para);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(defaultPara[j].para);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        case 55:
          // CSQ =cm_get_signalLevel();
          // cm_itoa(CSQ,CSQ_char,10);
          // cm_get_signalLevel(para_value);    //mike 20200824
        {
          ST_CSQ_Reponse csq_status;
          RIL_NW_GetCSQ(&csq_status);
//          Ql_snprintf(buf_value, 64, "%d,%d", csq_status.rssi, csq_status.ber);
          Ql_snprintf(buf_value, 64, "%d", csq_status.rssi);
          len = r_strlen(buf_value);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          PDT[j].wFunc(&PDT[j], buf_value, &len);
        }
        break;
        case 56:  //通信卡CCID
          // cm_get_iccid(para_value);  //mike 20200824
          RIL_SIM_GetCCID((char *)buf_value);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(buf_value);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        case 58:  //CPUID IMEI
          // cm_get_imei(para_value);  //mike 20200824
          RIL_GetIMEI((char *)buf_value);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(buf_value);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        default:
          break;
      }
    }
  }
  memory_release(buf_value);
  parameter_a_module();
  a_copy_to_b();
  parameter_init();  // 保持统一
}
#endif

#ifdef _PLATFORM_L610_
void parametr_get(u32_t number, Buffer_t *databuf) {
  char *buf_value = NULL;
  u16_t len = 0;
  Buffer_t logbuf;
  u8 parageting=0;    //=0 got para finish =1 get paraing

  int j = 0;

  if (databuf == NULL||device_data_geting) {
    //parageting=0;   //参数获取完成
    //return;
    len=0;
    goto GETFAIL;
  }
//  APP_DEBUG("para_meter get begin:%d!\r\n", number);
  
  for (j = 0; j < number_of_array_elements; j++) {
    if (number == PDT[j].num) {
      

      APP_DEBUG("para_meter[%d]num = %ld \r\n", j, number);
      buf_value = memory_apply(sizeof(char) * 64);
      if (buf_value == NULL) {
        APP_DEBUG("MEM Alloc Error\r\n");
        return;
      }
      r_memset(buf_value, 0, sizeof(char) * 64);
      if (number == 5 || number == 6 || number == 11 || number == 16\
          || number == 49 || number == 50 || number == 51 || number == 52\
          || number == 55 || number == 56 || number == 58 ||  number == 54 || number == 98) {
        switch (number) {
          case 5:  // 软件版本号
            r_strcpy(buf_value, defaultPara[j].para);
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
            break;
          case 6:  // 硬件版本号
            r_strcpy(buf_value, defaultPara[j].para);
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
            break;
          case 11:  // 是否有设备在线
            r_strcpy(buf_value, "1");
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
            break;
          case 16: {  // 查询IP地址
            break;
          }
          case 49:  // 查询网络注册信息
            break;
          case 50:  // GPRS 网络注册状态
            break;
          case 51:  // 固件编译日期
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], MAKE_DATE, &len);
            break;
          case 52:  // 固件编译时间
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], MAKE_TIME2, &len);
            break;
          case 54:  // 获取日志
            if(parageting==0){
              parageting=1;
              logbuf.payload=fibo_malloc(64);
              len=log_get(&logbuf);
              if(len==0){
                APP_DEBUG("\r\n-->log:get log fail\r\n");
                fibo_free(logbuf.payload);
                break;
              }
              r_memcpy(buf_value, logbuf.payload, len);
              fibo_free(logbuf.payload);
              //r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
              len = r_strlen(buf_value);
              //PDT[j].wFunc(&PDT[j], buf_value, &len);
              break;
            }else{
              r_strcpy(buf_value,"system busying!!!\r\n");
              len = r_strlen(buf_value);
              break;
            }  
          case 55:  {  // 获取CSQ值
            s8_t nrssi = 0, nber = 0;
            fibo_get_csq((INT32 *)&nrssi, (INT32 *)&nber);
            snprintf(buf_value, 64, "%d", nrssi);
            len = r_strlen(buf_value);
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
            break;
          }
          case 56:  // 通信卡CCID
            fibo_get_ccid((u8_t *)buf_value);
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            len = r_strlen(buf_value);
            PDT[j].wFunc(&PDT[j], buf_value, &len);
            break;
          case 58:  // CPUID IMEI
            fibo_get_imei((u8_t *)buf_value, 0);
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            len = r_strlen(buf_value);
            PDT[j].wFunc(&PDT[j], buf_value, &len);
            break;
		  case 98:  // 锦浪版本号
            r_strcpy(buf_value, defaultPara[j].para);
            r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
            PDT[j].wFunc(&PDT[j], buf_value, &len);
            break;
          default:
            break;
        }
      }
      //获取日志,从日志中读取，不需从参数中读取
      if(number==54){
        
      }
      //其它从参数文件中读取
      else{        
        r_memset(buf_value, 0, sizeof(char) * 64);    
        PDT[j].rFunc(&PDT[j], buf_value, &len);
      }
GETFAIL:
      if(len==0){
        r_strcpy(buf_value,"para get fail!\r\n");
        len = r_strlen(buf_value);
      }

//      APP_DEBUG("%d=%s len=%d\r\n", PDT[j].num, buf_value, len);
      if (len > 0) {
//      APP_DEBUG("%d=%s len %d\r\n", PDT[j].num, buf_value, len);
        databuf->size = len;
        databuf->lenght = len;
        databuf->payload = memory_apply(databuf->size + 1);  // 将获取的参数传递出去 申请后需要在函数外部释放
        if (databuf->payload == NULL) {
          APP_DEBUG("sys para databuf payload apply fail\r\n");
        } else {
          r_memset(databuf->payload, 0, databuf->size + 1);
          r_memcpy(databuf->payload, buf_value, databuf->lenght);
          APP_DEBUG("\r\n-->tcp lose:get para num= %ld,value=%s,len=%d\r\n",number,databuf->payload,databuf->lenght);
        }
      } else {
        APP_DEBUG("para fail!\r\n");
      }
      memory_release(buf_value);
      parageting=0;   //参数获取完成
      break;
    }
    
  }
  parageting=0;   //参数获取完成
//  APP_DEBUG("index: %d totle: %d\r\n", j, number_of_array_elements);
  if (j >= number_of_array_elements) {
    databuf->size = 0;
    databuf->lenght = 0;
    databuf->payload = NULL;
  }
}

u8_t parametr_set(u32_t number, Buffer_t *data) {
  int j = 0;
  u8_t ret = 0x00;
  char *str = NULL;
  for (j = 0; j < number_of_array_elements; j++) {
    if (number == PDT[j].num) {
      APP_DEBUG("para[%d] number is %ld Old value is %s\r\n", j, number, PDT[j].a);
//    APP_DEBUG("para[%d] number is %ld New value will be %s\r\n", j, number, data->payload);
      str = memory_apply(sizeof(char) * 64);
      if (str == NULL) {
        APP_DEBUG("MEM Alloc Error\r\n");
        ret = 0x03;
        return ret;
      }
      r_memset(str, '\0', sizeof(char) * 64);
      r_strncpy(str, (char *)data->payload, r_strlen((char *)data->payload));
      APP_DEBUG("\r\n-->para[%d] number is %ld str %s\r\n", j, number, str);
      switch (number) {  // 01/02/03/07/08/12/14/29/34/57 生产是必须写的参数
        case 0:
        case 4:
        case 5:
        case 6:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 27:
        case 28:
        case 49:
        //case 50:
        case 51:
        case 52:
        case 55:
        case 56:
        case 58:
          APP_DEBUG("para_meter can't be set \r\n");
          ret = 0x02;
          break;
        case 14: {
          break;
        }
        case 29:
          APP_DEBUG("\r\n-->number is %ld str %s\r\n",number, data->payload);
          switch (*(data->payload)) {
            case '1':  // 采集器重启
              soft_reset_en();
              //log_save("System Software Reset!");
              //APP_DEBUG("\r\n-->System Software Reset!\r\n");
              //fibo_softReset();
              break;
            case '2':  // 恢复出厂设置 -- 指示按defult参数表恢复，生产时写的数据如果改了就恢复不了了
              log_save("System Para Reset!");
              parametr_default();
              Eybpub_UT_SendMessage(EYBDEVICE_TASK, SYS_PARA_CHANGE, number, 0);
              break;
            case '3':  // 看门狗停狗重启
              log_save("System Hardware Reset!");
              Watchdog_stop();
              break;
            case '6':  
              para_init();    //Luee 参数初始化
              soft_reset_en();
              break;
            case '7':  
              log_clean();    //Luee 日志清除
              break;
            #if 0
            case '4':  // 删除a文件测试
              log_save("delete a file success!");
              rm_file_A ();
              break;
            #endif
            default:
              break;
          }
          ret = 0x00;
          APP_DEBUG("PARA %d=%s\r\n", PDT[j].num, data->payload);
          return ret;
          break;
        case 34: {
          ListHandler_t uartCfgStr;
          r_strsplit(&uartCfgStr, str, '-');
          int tab[4] = {0};
          int i = 0;
          Node_t *node;
          node = uartCfgStr.node;
          do {
            tab[i++] = Swap_charNum((char *) * (int *)node->payload);
            APP_DEBUG("\r\nBand %d, %d.\r\n", i, tab[i - 1]);
            node = node->next;
          } while (node != uartCfgStr.node && i < 4);
          ST_UARTDCB uart;
          if ((tab[0] > 2000 && tab[0] < 1000000)   // 波特率
              && (tab[1] > 4 && tab[1] < 9)  // 数据位
              && (tab[2] > 0 && tab[2] < 4)  // 停止位
              && (tab[3] >= 0 && tab[3] < 5)) {   // 校验位
            uart.baudrate = tab[0];
            uart.dataBits = tab[1];
            uart.stopBits = tab[2];
            uart.parity = tab[3];
            uart.flowCtrl = FC_NONE;
          } else if (tab[0] > 2000 && tab[0] < 1000000) {
            uart.baudrate = tab[0];
            uart.dataBits = HAL_UART_DATA_BITS_8;
            uart.stopBits = HAL_UART_STOP_BITS_1;
            uart.parity = HAL_UART_NO_PARITY;
            uart.flowCtrl = FC_NONE;
          } else {
            ret = 0x02;
            break;
          }
          list_delete(&uartCfgStr);   // mike 20200922
          char str_temp_1[64] = {0};
          char str_temp_2[64] = {0};
          u16_t len = 0;
          char *P1 = NULL;
          r_memset(str, '\0', sizeof(char) * 64);
          r_strncpy(str, (char *)data->payload, r_strlen((char *)data->payload));
          r_memset(str_temp_1, 0, sizeof(str_temp_1));
          r_memset(str_temp_2, 0, sizeof(str_temp_2));
          snprintf(str_temp_2, sizeof(str_temp_2), "#%ld-%d-%d-%d#", uart.baudrate, uart.dataBits, uart.stopBits, uart.parity);
          for (i = 0; i < number_of_array_elements; i++) {
            if (14 == PDT[i].num) {
              PDT[i].rFunc(&PDT[i], str_temp_1, &len);
              P1 = r_strstr(str_temp_1, "#");
              if (P1) {
                *P1 = '\0';   // 去掉#号
                len = r_strlen(str_temp_1);
                APP_DEBUG("%s len:%d\r\n", str_temp_1, len);
                r_strcat(str_temp_1, str_temp_2);
                len = r_strlen(str_temp_1);
                APP_DEBUG("%s len:%d\r\n", str_temp_1, len);
                r_memset((&PDT[i])->a, 0, sizeof((&PDT[i])->a));
                PDT[i].wFunc(&PDT[i], str_temp_1, &(len));
              }
            }
          }
          break;
        }
        default:
          break;
      }

      if (ret == 0x00) {
        //这里只是写入成功，但最后是不是更新成功未知
        ret = 0x00;
        //防止写入字符串过长
        if (data->lenght >= sizeof((&PDT[j])->a)) {
          APP_DEBUG("para so long!\r\n");
          ret = 0x03;
          break;
        }
        r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
        PDT[j].wFunc(&PDT[j], str, &(data->lenght));

        if (number == 21) {
          log_save("set para 21: %s", (&PDT[j])->a);
        }
        APP_DEBUG("PARA %d=%s\r\n", PDT[j].num, str);
        break;
      }
      memory_release(str);
    }
    if (j >= number_of_array_elements) {
      APP_DEBUG("para_meter %ld is wrongth\r\n", number);
      ret = 0x03;
    }
  }
  // start_timer();
  if (ret == 0x00 && number != LOCAL_TIME) {  // mike 心跳时间不存到参数系统
    parameter_a_module();
    a_copy_to_b();
    // 将生产参数存进c文件
    if (1 == produc_save_flag) {
      APP_DEBUG("begin to backup parameter\r\n");
      a_copy_to_c();
    }
    parameter_init();  // 保持统一
    if (number == DEVICE_MONITOR_NUM || number == DEVICE_PROTOCOL || number == DEVICE_UART_SETTING) {
      Eybpub_UT_SendMessage(EYBDEVICE_TASK, SYS_PARA_CHANGE, number, 0);
    } else if (number == DEVICE_PNID || number == EYBOND_SERVER_ADDR) {
      Eybpub_UT_SendMessage(EYBNET_TASK, NET_CMD_RESTART_ID, 0, 0);    // 或者NET_CMD_RESTART_ID
    }
  }
  return ret ;
}

void main_parametr_update(void) { // 由于APP固件升级会让系统保存的参数不匹配，需要实时更新
  u16_t len = 0;
  int j = 0;
  int number = 0;
  char *buf_value = NULL;
  buf_value = memory_apply(sizeof(char) * 64);
  if (buf_value == NULL) {
    APP_DEBUG("MEM Alloc Error\r\n");
    return;
  }
//  r_memset(buf_value, 0, sizeof(char) * 64);

  APP_DEBUG("main_parametr_update!\r\n");
  for (j = 0; j < number_of_array_elements; j++) {
    r_memset(buf_value, 0, sizeof(char) * 64);
    number = PDT[j].num;
    if (number == 5 || number == 6 || number == 11 || number == 49 || number == 50 \
        || number == 51 || number == 52 || number == 55 || number == 56 || number == 58 || number == 98 ) {
      switch (number) {
        case 5:  // 软件版本号
          r_strcpy(buf_value, defaultPara[j].para);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(buf_value);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        case 6:  // 硬件版本号
          r_strcpy(buf_value, defaultPara[j].para);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(buf_value);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        case 11:  // 是否有设备在线
          r_strcpy(buf_value, "1");
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(buf_value);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        case 49:  // 查询网络注册信息
          // cm_gprs_getcregstate(&creg);
          // cm_itoa(creg,creg_char,10);
          // r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          // PDT[j].wFunc(&PDT[j],creg_char, &len);
          break;
        case 50:  // GPRS 网络注册状态
          // cm_gprs_getcgregstate(&cgreg);
          // cm_itoa(cgreg,cgreg_char,10);
          // r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          // PDT[j].wFunc(&PDT[j],cgreg_char, &len);
          break;
        case 51:  // 固件编译日期
          r_strcpy(buf_value, defaultPara[j].para);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(defaultPara[j].para);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        case 52:  // 固件编译时间
          r_strcpy(buf_value, defaultPara[j].para);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(defaultPara[j].para);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        case 55: {  // 获取CSQ值
          s8_t nrssi = 0, nber = 0;
          fibo_get_csq((INT32 *)&nrssi, (INT32 *)&nber);
          snprintf(buf_value, 64, "%d", nrssi);
          len = r_strlen(buf_value);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        }
        case 56:  // 通信卡CCID
          fibo_get_ccid((u8_t *)buf_value);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(buf_value);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
        case 58:  // CPUID IMEI
          fibo_get_imei((u8_t *)buf_value, 0);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(buf_value);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
          break;
	    case 98:  // 锦浪软件版本号
          r_strcpy(buf_value, defaultPara[j].para);
          r_memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
          len = r_strlen(buf_value);
          PDT[j].wFunc(&PDT[j], buf_value, &len);
		  APP_DEBUG("default 98 write = %s\r\n",buf_value);
          break;
        default:
          break;
      }
    }
  }
  memory_release(buf_value);
  parameter_a_module();
  a_copy_to_b();
  parameter_init();  // 保持统一
}
#endif

ServerAddr_t *ServerAdrrGet(u8_t num) {
  Buffer_t buf;
  Buffer_t portBuf;
  ServerAddr_t *serverAddr = null;
  r_memset(&buf, 0, sizeof(Buffer_t));
  r_memset(&portBuf, 0, sizeof(Buffer_t));
  parametr_get(num, &buf);  // TODO获取的buf长度有问题?


  if (buf.payload != null && buf.lenght > 5) {
    ListHandler_t cmdStr;
    int len = 0;

    r_strsplit(&cmdStr, (char *) buf.payload, ':');

    if (cmdStr.count > 0) {
      len = r_strlen((char *) * (int *) cmdStr.node->payload);
      serverAddr = memory_apply(sizeof(ServerAddr_t) + len);
      r_strcpy(serverAddr->addr, (char *) * (int *) cmdStr.node->payload);
      serverAddr->type = 1;
      if (num == SARNATH_SERVER_ADDR) {
        parametr_get(SARNATH_SERVER_PORT, &portBuf);
        serverAddr->port = Swap_charNum((char *)portBuf.payload);
      } else {
        serverAddr->port = 502;
      }

      if (num == HANERGY_SERVER_ADDR) {
        serverAddr->port = 8081;
      } else {
        serverAddr->port = 502;
      }

      if (cmdStr.count > 1) {
        serverAddr->port = Swap_charNum((char *) * (int *) cmdStr.node->next->payload);
        // TODO 有问题，一直会进入这里
      }

      if (cmdStr.count > 2) {
        if (r_strstr((char *) * (int *) cmdStr.node->next->next->payload, "UDP") != NULL) {
          serverAddr->type = 0;
        } else if (r_strstr((char *) * (int *) cmdStr.node->next->next->payload, "SSL") != NULL) {
          serverAddr->type = 2;
        }
      }
    }
    list_delete(&cmdStr);
  }

  if (serverAddr->port == 0) {
    if (serverAddr != NULL) {
      memory_release(serverAddr);
      serverAddr = null;
    }
  }

  memory_release(portBuf.payload);
  portBuf.size = 0;
  portBuf.lenght = 0;
  memory_release(buf.payload);
  buf.lenght = 0;
  buf.size = 0;

  return serverAddr;
}

// void SysPara_init(void) {
//   APP_DEBUG("SysPara_init\r\n");
//   Para_Init_flag = 0;
//   live_a_and_b();
//   main_parametr_update();  //
// }

int SysPara_init(void) {
  APP_DEBUG("SysPara_init\r\n");
  Para_Init_flag = 0;
  // 参数初始化失败
  if(live_a_and_b()) {
    log_save("sys para init fail.");
    return -1;
  }
  main_parametr_update();
  return 0;
}

u8_t SysPara_Get_State(void) {
  return Para_Init_flag;
}

void parametr_default(void) {   // mike 依据default配置重新生成PDT表
  APP_DEBUG("parametr_default\r\n");

  u16_t len = 0;
  int i = 0;
  for (i = 0; i < number_of_array_elements; i++) {
    if (r_strlen(defaultPara[i].para) <= 0) {
      continue;
    }
    r_memset((&PDT[i])->a, 0, sizeof((&PDT[i])->a));
    len = r_strlen(defaultPara[i].para);
    PDT[i].wFunc(&PDT[i], (void *)defaultPara[i].para, &len);
  }
  parameter_a_module();
  a_copy_to_b();
  parameter_init();  // 保持统一
}

void GET_ALL_data(void) {
  int j = 0;
  char *buf  = NULL;
  u16_t len = 64;

  for (j = 0; j < number_of_array_elements; j++) {
    //日志系统
    if (PDT[j].num == 54) {
      continue;
    }

    buf = memory_apply(sizeof(char) * 64);
    r_memset(buf, 0, sizeof(char) * 64);
    PDT[j].rFunc(&PDT[j], buf, &len);
    if (r_strlen(buf) > 0) {
      APP_PRINT("%d=%s\r\n", PDT[j].num, buf);
    }
    memory_release(buf);
  }
  Para_Init_flag = 1;
}

/******************************************************************************/
