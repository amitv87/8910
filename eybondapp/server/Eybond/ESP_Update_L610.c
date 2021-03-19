/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
 *@brief   : eybond soft update process
 *@notes   : 2017.09.11 CGQ
*******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_stdlib.h"
#include "ql_system.h"
#endif

#include "eyblib_memory.h"
#include "eyblib_swap.h"
#include "eyblib_CRC.h"

#include "eybpub_Debug.h"
#include "eybpub_File.h"
#include "eybpub_utility.h"

#include "ESP_Update_L610.h"
#include "eybond.h"

#include "DeviceUpdate.h"
#include "UpdateTask.h"

#include "Modbus.h"
// #include "FlashHard.h"
#include "StateGridData.h"
#include "L610Net_SSL.h"

// #define FILE_FLAG_ADDR  (FLASH_UPDATE_FILE_ADDR) // POINT_TAB_FILE_ADDR// 
// #define FILE_SAVE_ADDR  (FILE_FLAG_ADDR + 0x02000)
// #define FILE_MAX_SIZE (FLASH_UPDATE_FILE_SIZE)

#define FILE_MAX_SIZE        (0x00380000)

File_t *update = null;
DeviceType_t *deviceType = null;
u32_t updateID = 0;
u32_t fileFlagMode = FOTA_FILE_FLAG;

static void enterUpdate(void);

/******************************************************************************
  * @brief
  * @note   None 0x20
  * @param  None
*******************************************************************************/
u8_t Update_file(ESP_t *esp) {  // 升级CA、电表文件
  APP_DEBUG("Update_file\r\n");
#pragma pack(1)
  typedef struct {
    u8_t type;
    u8_t sizeHH;
    u8_t sizeHL;
    u8_t sizeLH;
    u8_t sizeLL;
    u8_t sliceCntH;
    u8_t sliceCntL;
    u8_t sliceSizeH;
    u8_t sliceSizeL;
  } Rcve_t;
  typedef struct {
    u8_t state;
  } Ack_t;
#pragma pack()
  u16_t blockNum;
  int i;
  Buffer_t buf;
  Rcve_t *rcvePara = (Rcve_t *)esp->PDU;
  EybondHeader_t *ackHead = &esp->head;
  Ack_t *ackPara = (Ack_t *)(ackHead + 1);

  updateID = 0;
  blockNum = (rcvePara->sliceCntH << 8) | rcvePara->sliceCntL;    //得到数据块数量
  i = rcvePara->sizeHH << 24 | rcvePara->sizeHL << 16 | rcvePara->sizeLH << 8 | rcvePara->sizeLL; //文件总大小
  APP_DEBUG("fiile Size %x \r\n", i);
  if (blockNum == 0) {
    ackPara->state = 0;
  } else if (i > 0x10000) {
    ackPara->state = 1;
  } else {
    if (update != NULL) {
      memory_release(update);
    }
    update = null;
    if (rcvePara->type == 0x10) { // CA file
      fileFlagMode = CA_FILE_FLAG;
      update = File_init(fileFlagMode, blockNum, (rcvePara->sliceSizeH << 8) | rcvePara->sliceSizeL);
    } else if (rcvePara->type == 0x20) { // point file
      fileFlagMode = POINT_TAB_FILE_FLAG;
      update = File_init(fileFlagMode, blockNum, (rcvePara->sliceSizeH << 8) | rcvePara->sliceSizeL);
    }

    if (update != null) {
      update->area = rcvePara->type ;
      ackPara->state = 0;
    } else {
      ackPara->state = 1;
    }
  }

  ackHead->msgLen = ENDIAN_BIG_LITTLE_16(sizeof(Ack_t) + 2);
  buf.lenght = sizeof(EybondHeader_t) + sizeof(Ack_t);
  buf.size = buf.lenght + 1;
  buf.payload = memory_apply(buf.lenght + 1);
  r_memcpy(buf.payload, (u8_t *)ackHead, buf.lenght);
//  buf.payload = (u8_t *)ackHead;
  esp->ack(&buf);
  memory_release(buf.payload);

  return 0;
}

/*******************************************************************************
  * @brief
  * @note   None 0x21
  * @param  None
*******************************************************************************/
u8_t Update_soft(ESP_t *esp) {  // 升级数采器固件
  APP_DEBUG("Update_soft\r\n");
#pragma pack(1)       //一个字节对齐
  typedef struct {
    u16_t sliceCnt;   //数据块数
    u16_t sliceSize;  //每个数据块长度
  } Rcve_t;
  typedef struct {
    u8_t state;
  } Ack_t;            //回复结构体
#pragma pack()
  Buffer_t buf;
  Rcve_t *rcvePara = (Rcve_t *)esp->PDU;    //得到接收数据区
  EybondHeader_t *ackHead = &esp->head;     //得到回复头
  Ack_t *ackPara = (Ack_t *)(ackHead + 1);  //得到回复数据区第一个地址

  APP_DEBUG("ESP=pdu len:%04X, waitCnt:%04X\r\n", esp->PDULen, esp->waitCnt);
  APP_DEBUG("ackHead serail:%04X, code:%04X\r\n", ackHead->serial, ackHead->code);
  APP_DEBUG("ackHead msg len:%04X, addr:%02X func:%02X\r\n", ackHead->msgLen, ackHead->addr, ackHead->func);
  APP_DEBUG("ackPara state :%04X\r\n", ackPara->state);
  
  updateID = SELF_UPDATE_ID;
  Swap_bigSmallShort(&rcvePara->sliceCnt);   //高低8位互换

  if (rcvePara->sliceCnt == 0) {
    if ((update != null) && (update->area == 0) && (0 == File_validCheck(update))) {
      enterUpdate();
      ackPara->state = 0;
    } else {
      ackPara->state = 1;
    }
  } else {
    if (update != NULL) {
      memory_release(update);
    }
    fileFlagMode = FOTA_FILE_FLAG;
    update = File_init(fileFlagMode, (rcvePara->sliceCnt), ENDIAN_BIG_LITTLE_16(rcvePara->sliceSize));
    if (update != null) {
      update->area = 0;
      ackPara->state = 0;
    } else {
      ackPara->state = 1;
    }
  }
  APP_DEBUG("ackPara new state :%04X\r\n", ackPara->state);

  ackHead->msgLen = ENDIAN_BIG_LITTLE_16(sizeof(Ack_t) + 2);
  buf.lenght = sizeof(EybondHeader_t) + sizeof(Ack_t);
  buf.size = buf.lenght + 1;
  buf.payload = memory_apply(buf.lenght + 1);
  r_memcpy(buf.payload, (u8_t *)ackHead, buf.lenght);
//  buf.payload = (u8_t *)ackHead;
  esp->ack(&buf);
  memory_release(buf.payload);
  return 0;
}

/*******************************************************************************
  * @brief
  * @note   None 0x22
  * @param  None
*******************************************************************************/
u8_t Update_device(ESP_t *esp) {    // 升级设备固件
  APP_DEBUG("Update_device\r\n");
#pragma pack(1)
  typedef struct {
    u8_t type;
    u8_t bandrateH;
    u8_t bandrateL;
    u8_t sliceCntH;
    u8_t sliceCntL;
    u8_t sliceSizeH;
    u8_t sliceSizeL;
  } Rcve_t;
  typedef struct {
    u8_t state;
  } Ack_t;
#pragma pack()
  u16_t blockNum;
  Buffer_t buf;
  Rcve_t *rcvePara = (Rcve_t *)esp->PDU;
  EybondHeader_t *ackHead = &esp->head;
  Ack_t *ackPara = (Ack_t *)(ackHead + 1);

  if (deviceType != NULL) {
    memory_release(deviceType);
  }
  deviceType = memory_apply(sizeof(DeviceType_t));
  if (deviceType != null) {
    deviceType->addr = esp->head.addr;
    deviceType->type = rcvePara->type;
    deviceType->bandrate = (rcvePara->bandrateH << 8) | rcvePara->bandrateL;
    updateID = DEVICE_UPDATE_ID;

    blockNum = (rcvePara->sliceCntH << 8) | rcvePara->sliceCntL;
    if (blockNum == 0) {
      if ((update != null)
          && (update->area == 1)
          && (0 == File_validCheck(update))
         ) {
        enterUpdate();
        ackPara->state = 0;
      } else {
        ackPara->state = 1;
      }
    } else {
      if (update != NULL) {
        memory_release(update);
      }
      fileFlagMode = DEVICE_FOTA_FILE_FLAG;
      update = File_init(fileFlagMode, blockNum, (rcvePara->sliceSizeH << 8) | rcvePara->sliceSizeL);
      if (update != null) {
        update->area = 1;
        ackPara->state = 0;
      } else {
        ackPara->state = 1;
      }
    }
  } else {
    ackPara->state = 1;
  }

  ackHead->msgLen = ENDIAN_BIG_LITTLE_16(sizeof(Ack_t) + 2);
  buf.lenght = sizeof(EybondHeader_t) + sizeof(Ack_t);
  buf.size = buf.lenght + 1;
  buf.payload = memory_apply(buf.lenght + 1);
  r_memcpy(buf.payload, (u8_t *)ackHead, buf.lenght);
//  buf.payload = (u8_t *)ackHead;
  esp->ack(&buf);
  memory_release(buf.payload);

  return 0;
}

/*******************************************************************************
  * @brief
  * @note   0x23
  * @param  None
*******************************************************************************/
u8_t Update_deviceState(ESP_t *esp) {
  APP_DEBUG("Update_deviceState\r\n");
#pragma pack(1)
  typedef struct {
    u8_t process;
  } Ack_t;
#pragma pack()
  Buffer_t buf;
  EybondHeader_t *ackHead;
  Ack_t *ackPara;

  ackHead = memory_apply(sizeof(EybondHeader_t) + sizeof(Ack_t) + 1);
  ackPara = (Ack_t *)(ackHead + 1);

  r_memcpy(ackHead, &esp->head, sizeof(EybondHeader_t));
  ackHead->msgLen = ENDIAN_BIG_LITTLE_16(sizeof(Ack_t) + 2);
  ackPara->process = Update_state();

  buf.lenght = sizeof(EybondHeader_t) + sizeof(Ack_t);
  buf.payload = (u8_t *)ackHead;
  esp->ack(&buf);
  memory_release(buf.payload);
  return 0;
}

/*******************************************************************************
  * @brief
  * @note   ox24
  * @param  None
*******************************************************************************/
u8_t Update_deviceCancel(ESP_t *esp) {
  APP_DEBUG("Update_deviceCancel\r\n");
#pragma pack(1)
  typedef struct {
    u8_t result;
  } Ack_t;
#pragma pack()
  Buffer_t buf;
  EybondHeader_t *ackHead;
  Ack_t *ackPara;

  ackHead = memory_apply(sizeof(EybondHeader_t) + sizeof(Ack_t) + 1);
  ackPara = (Ack_t *)(ackHead + 1);

  r_memcpy(ackHead, &esp->head, sizeof(EybondHeader_t));
  ackHead->msgLen = ENDIAN_BIG_LITTLE_16(sizeof(Ack_t) + 2);
  ackPara->result = 0;
  Update_end();

  buf.lenght = sizeof(EybondHeader_t) + sizeof(Ack_t);
  buf.payload = (u8_t *)ackHead;
  esp->ack(&buf);
  memory_release(buf.payload);
  return 0;
}

/*******************************************************************************
  * @brief
  * @note   None  0x25
  * @param  None
*******************************************************************************/
u8_t Update_info(ESP_t *esp) {
  APP_DEBUG("Update_info\r\n");
#pragma pack(1)
  typedef struct {
    u8_t result;
  } Rcve_t;
  typedef struct {
    u8_t flag;
    u8_t state;
    u8_t md5[32];
  } Ack_t;
#pragma pack()
  Buffer_t buf;
  EybondHeader_t *ackHead = NULL;
  Ack_t *ackPara = NULL;
  Rcve_t *rcve = NULL;

  rcve = (Rcve_t *)esp->PDU;
  ackHead = memory_apply(sizeof(EybondHeader_t) + sizeof(Ack_t) + 1);
  ackPara = (Ack_t *)(ackHead + 1);

  r_memcpy(ackHead, &esp->head, sizeof(EybondHeader_t));
  ackHead->msgLen = ENDIAN_BIG_LITTLE_16(sizeof(Ack_t) + 2);

  if (update == null) {
    u32_t flagMode = FOTA_FILE_FLAG;    // DEVICE_FOTA_FILE_FLAG
    File_t *file = memory_apply(sizeof(File_t));
    if (file != NULL) {        
      r_memset(file, 0 , sizeof(File_t));
    } else {
      APP_DEBUG("memory_apply file memory fail\r\n");
      memory_release(ackHead);
      return -1;
    }

    if (rcve->result == 0x10) {
      flagMode = CA_FILE_FLAG;
    } else if (rcve->result == 0x20) {
      flagMode = POINT_TAB_FILE_FLAG;
    }
    File_state(file, flagMode);
    APP_DEBUG("file name:%s addr:%04lX\r\n", file->name, file->addr);
    APP_DEBUG("file md5:%s size:%04lX\r\n", file->md5, file->size);
    APP_DEBUG("file seat:%04lX flag:%04lX\r\n", file->seat, file->flag);
    APP_DEBUG("file area:%02X sliceCnt:%02X\r\n", file->area, file->sliceCnt);
    APP_DEBUG("file sliceSize:%02X sliceStateLen:%02X\r\n", file->sliceSize, file->sliceStateLen);    
    r_memset(ackPara->md5, '0', sizeof(ackPara->md5));
    if (file->area != esp->PDU[0] || (0 != File_validCheck(file))) {
      ackPara->state = 0x02;
    } else if (File_Check(file) != 0) {
      ackPara->state = 0x01;
    } else {
      ackPara->state = 0;
      update = file;
      Swap_hexChar((char *)ackPara->md5, file->md5, 16, 0);
      file = null;
    }    
    //get flash recode file flag
    ackPara->flag = esp->PDU[0];
    memory_release(file);
  }

  if (update != NULL) {
    APP_DEBUG("updae name:%s addr:%04lX\r\n", update->name, update->addr);
    APP_DEBUG("updae md5:%s size:%04lX\r\n", update->md5, update->size);
    APP_DEBUG("updae seat:%04lX flag:%04lX\r\n", update->seat, update->flag);
    APP_DEBUG("updae area:%02X sliceCnt:%02X\r\n", update->area, update->sliceCnt);
    APP_DEBUG("updae sliceSize:%02X sliceStateLen:%02X\r\n", update->sliceSize, update->sliceStateLen);
  }
  buf.lenght = sizeof(EybondHeader_t) + sizeof(Ack_t);
  buf.payload = (u8_t *)ackHead;
  esp->ack(&buf);
  memory_release(buf.payload);

  return 0;
}
/*******************************************************************************
  * @brief
  * @note   None 0x26
  * @param  None
*******************************************************************************/
u8_t Update_dataRcve(ESP_t *esp) {
  APP_DEBUG("Update_dataRcve\r\n");
#pragma pack(1)
  typedef struct {
    u16_t sliceNum;
    u8_t  data[];
  } Rcve_t;
  typedef struct {
    u16_t sliceNum;
    u8_t state;
  } Ack_t;
#pragma pack()
  Buffer_t buf;
  Rcve_t *rcvePara = (Rcve_t *)esp->PDU;
  EybondHeader_t *ackHead = &esp->head;
  Ack_t *ackPara = (Ack_t *)(ackHead + 1);
  u16_t offset = ENDIAN_BIG_LITTLE_16(rcvePara->sliceNum);
  u16_t dataLen = ENDIAN_BIG_LITTLE_16(esp->head.msgLen) - 2 - 4;

  if ((0 == crc16_standard(CRC_RTU, rcvePara->data, dataLen + 2))
      && (0 == File_rcve(update, offset, rcvePara->data, dataLen)) //
     ) {
    ackPara->state = 0;
  } else {
    ackPara->state = 1;
  }
  ackPara->sliceNum = ENDIAN_BIG_LITTLE_16(offset);
  ackHead->msgLen = ENDIAN_BIG_LITTLE_16(sizeof(Ack_t) + 2);
  buf.lenght = sizeof(EybondHeader_t) + sizeof(Ack_t);
  buf.size = buf.lenght + 1;
  buf.payload = memory_apply(buf.lenght + 1);
  r_memcpy(buf.payload, (u8_t *)ackHead, buf.lenght);
//  buf.payload = (u8_t *)ackHead;
  esp->ack(&buf);
  memory_release(buf.payload);

  return 0;
}

/*******************************************************************************
  * @brief  0x27
  * @note   None
  * @param  None
*******************************************************************************/
u8_t Update_rcveState(ESP_t *esp) {
  APP_DEBUG("Update_rcveState\r\n");
#pragma pack(1)
  typedef struct {
    u16_t sliceCnt;
  } Rcve_t;
  typedef struct {
    u8_t state;
    u8_t  data[];
  } Ack_t;
#pragma pack()
  Buffer_t buf;
  Rcve_t *rcvePara = (Rcve_t *)esp->PDU;
  u16_t sliceCount = ENDIAN_BIG_LITTLE_16(rcvePara->sliceCnt);
  u16_t size = (sliceCount >> 3) + ((sliceCount & 0x07) ? 1 : 0);
  EybondHeader_t *ackHead = memory_apply(sizeof(EybondHeader_t) + sizeof(Ack_t) + size);
  Ack_t *ackPara = (Ack_t *)(ackHead + 1);

  r_memcpy(ackHead, &esp->head, sizeof(EybondHeader_t));

  if (update == null || update->sliceStateLen > size) {
    r_memset(ackPara->data, 0x00, size);
    ackPara->state = 1;
    if (update != NULL) {
      memory_release(update);
    }
  } else {
    ackPara->state = 0;
    r_memcpy(ackPara->data, update->sliceState, update->sliceStateLen);
  }

  ackHead->msgLen = ENDIAN_BIG_LITTLE_16(sizeof(Ack_t) + 2 + size);
  buf.lenght = sizeof(EybondHeader_t) + sizeof(Ack_t) + size;
  buf.size = buf.lenght + 1;
  buf.payload = memory_apply(buf.lenght + 1);
  r_memcpy(buf.payload, (u8_t *)ackHead, buf.lenght);
//  buf.payload = (u8_t *)ackHead;
  esp->ack(&buf);
  memory_release(buf.payload);

  return 0;
}
/*******************************************************************************
  * @brief  0x28
  * @note   None
  * @param  None
*******************************************************************************/
u8_t Update_dataCheck(ESP_t *esp) {
  APP_DEBUG("Update_dataCheck\r\n");
#pragma pack(1)
  typedef struct {
    u8_t size[4];
    char  md5[32];
  } Rcve_t;
  typedef struct {
    u8_t state;
  } Ack_t;
#pragma pack()
  Buffer_t buf;
  Rcve_t *rcvePara = (Rcve_t *)esp->PDU;
  EybondHeader_t *ackHead = &esp->head;
  Ack_t *ackPara = (Ack_t *)(ackHead + 1);
  char md5[64];
  r_memset(md5, '\0', sizeof(md5));

  update->size = (rcvePara->size[0] << 24) | (rcvePara->size[1] << 16) | (rcvePara->size[2] << 8) | (rcvePara->size[3]);
  r_memcpy(md5, rcvePara->md5, sizeof(rcvePara->md5));
  md5[sizeof(rcvePara->md5)] = '\0';
  Swap_charHex(update->md5, md5);

  File_save(update, fileFlagMode);
  ackPara->state = File_Check(update);

  if (ackPara->state == 0) {
    enterUpdate();
  }

  ackHead->msgLen = ENDIAN_BIG_LITTLE_16(sizeof(Ack_t) + 2);
  buf.lenght = sizeof(EybondHeader_t) + sizeof(Ack_t);
  buf.size = buf.lenght + 1;
  buf.payload = memory_apply(buf.lenght + 1);
  r_memcpy(buf.payload, (u8_t *)ackHead, buf.lenght);
//  buf.payload = (u8_t *)ackHead;
  esp->ack(&buf);
  memory_release(buf.payload);  
  return 0;
}
/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
*******************************************************************************/
u8_t Update_exit(ESP_t *esp) {
  APP_DEBUG("Update_exit\r\n");
#pragma pack(1)
  typedef struct {
    u32_t state;
  } Rcve_t;
  typedef struct {
    u8_t state;
  } Ack_t;
#pragma pack()
  Buffer_t buf;
  Rcve_t *rcvePara = (Rcve_t *)esp->PDU;
  EybondHeader_t *ackHead = &esp->head;
  Ack_t *ackPara = (Ack_t *)(ackHead + 1);

  ackPara->state = 0;

  ackHead->msgLen = ENDIAN_BIG_LITTLE_16(sizeof(Ack_t) + 2);
  buf.lenght = sizeof(EybondHeader_t) + sizeof(Ack_t);
  buf.size = buf.lenght + 1;
  buf.payload = memory_apply(buf.lenght + 1);
  r_memcpy(buf.payload, (u8_t *)ackHead, buf.lenght);
//  buf.payload = (u8_t *)ackHead;
  esp->ack(&buf);
  memory_release(buf.payload);
  if (update != NULL) {
    memory_release(update);
  }
  if (deviceType != NULL) {
    memory_release(deviceType);
  }
  update = null;
  deviceType = null;
  return 0;
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
*******************************************************************************/
static void enterUpdate(void) {
  if (updateID != 0) {
    Eybpub_UT_SendMessage(UPDATE_TASK, updateID, (u32_t)update, (u32_t)deviceType);
    update = null;  // 完成指针传递后，指NULL
    deviceType = null;
  }
}

/******************************************************************************/

