/**************Copyright(C) 2015-2026 Eybond Co., Ltd.******************
  *@brief   : file option
  *@notes   : 2017.05.12 CGQ
*******************************************************************************/
#include "eyblib_r_stdlib.h"
#include "eyblib_HashMD5.h"
#include "eyblib_CRC.h"
#include "eyblib_memory.h"

#include "eybpub_File.h"
#include "eybpub_Debug.h"
#include "eybpub_watchdog.h"
#include "eybpub_run_log.h"

#ifdef _PLATFORM_M26_
#include "x25qxx.h"
#endif

#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#include "ESP_Update_L610.h"
#include "L610Net_SSL.h"
#include "StateGridData.h"
#endif
#define FILE_FLAG (0xAA550088)

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
File_t *File_init(u32_t addr, u16_t sliceCnt, u16_t sliceSize) {
  File_t *file = NULL;
  u16_t len = 0;

  len = (sliceCnt >> 3) + ((sliceCnt & 0x07) ? 1 : 0);
  file = memory_apply(sizeof(File_t) + len + 8);

  if (file != null) {
    int i = 0;

    file->seat = 0;
    file->addr = addr;
    file->flag = FILE_FLAG;
    file->sliceCnt = sliceCnt;
    file->sliceSize = sliceSize;
    file->sliceStateLen = len;
    r_memset(file->sliceState, 0, len);

    for (i = (file->sliceCnt & 0x07); i < 8 && i != 0; i++) {
      file->sliceState[len - 1] |= (1 << i);
    }
#ifdef _PLATFORM_L610_  // 在文件系统中addr只是标识flag，不再是文件读写的地址
    r_memset(file->name, '\0', sizeof(file->name));
    switch (addr) {
      case FOTA_FILE_FLAG:
        r_strncpy(file->name, FOTA_FILE_NAME, r_strlen(FOTA_FILE_NAME));
        break;
      case DEVICE_FOTA_FILE_FLAG:
        r_strncpy(file->name, DEVICE_FOTA_FILE_NAME, r_strlen(DEVICE_FOTA_FILE_NAME));
        break;
      case CA_FILE_FLAG:
        r_strncpy(file->name, CA_FILE_NAME, r_strlen(CA_FILE_NAME));
        break;
      case POINT_TAB_FILE_FLAG:
        r_strncpy(file->name, POINT_TAB_FILE_NAME, r_strlen(POINT_TAB_FILE_NAME));
        break;
      default:
        break;
    }
    s32_t nfile_size = fibo_file_getSize(file->name);  // delete old file when init
    s32_t ret = 0;
    if (nfile_size >= 0) {   // 文件存在或长度为0
      APP_DEBUG("File %s is existing, delete it!!\r\n", file->name);
      ret = fibo_file_delete(file->name);
      if (ret < 0) {
        APP_DEBUG("Delete %s file fail\r\n", file->name);
        return NULL;
      }
    }
#endif
  }
  return file;
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
u8_t File_validCheck(File_t *file) {
  if ((file != null) && (file->flag == FILE_FLAG) && (file->size < 0x400000)) {     //文件尺寸小于4MB有效
  //if ((file != null) && (file->flag == FILE_FLAG) && (file->size < 0xf0000)) {     //文件尺寸小于1MB有效,升级文件136KB只能升级到91%，之后无法再升级
    return 0;
  }
  return 1;
}

#ifdef _PLATFORM_M26_
/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
u8_t File_rcve(File_t *file, u16_t offset, u8_t *data, u16_t len) {
  if (((file->sliceSize == len) || ((offset + 1) == file->sliceCnt))
      && (file->sliceCnt > offset)) {
    u32_t saveAddr = file->addr + (offset * file->sliceSize);

    file->sliceState[offset >> 3] |= (1 << (offset & 0x07));

    if (0 == (saveAddr & (X25Q_SECTOR_SIZE - 1))) {
      APP_DEBUG("Save area earse addr: %x \r\n", saveAddr);
      x25Qxx_earse(saveAddr);
    }
    x25Qxx_wrtie(saveAddr, len, data);

    return 0;
  }

  APP_DEBUG("file size %d - %d, slice %d - %d  \r\n ", file->sliceSize, len, file->sliceCnt, offset);
  return 1;
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
s16_t File_read(File_t *file, u8_t *data, u16_t len) {
  static MD5_t *md5 = null;
  int readLen;

  if (file->seat == 0) {
    memory_release(md5);
    md5 = memory_apply(sizeof(MD5_t));
    Hash_MD5Init(md5);
    APP_DEBUG("File Satrt read\r\n");
  }
  readLen = (file->size - file->seat) > len ? len : (file->size - file->seat);

  x25Qxx_read(file->addr + file->seat, readLen, data);
  file->seat += readLen;

  if (md5 != null) {
    Hash_MD5Update(md5, data, readLen);
    if (file->size == file->seat) {
      Hash_MD5Final(md5);

      if (r_memcmp(file->md5, md5->md, 16) != 0) {
        readLen = -1;
        log_save("File read rusult fail\r\n");
      }
      memory_release(md5);
      md5 = null;
    }

    if (0 == (file->seat & 0x001FFFF)) {
      Watchdog_feed();
    }
  } else {
    readLen = 0;
    log_save("MD5 memory apply fail");
  }

  return readLen;
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
u8_t File_Check(File_t *file) {
  MD5_t *md5;
  u8_t *buf;
  int i;
  u16_t len;

  //Watchdog_feed();
  md5 = memory_apply(sizeof(MD5_t));
  buf = memory_apply(X25Q_READ_SIZE);

  Hash_MD5Init(md5);

  for (i = 0; i < file->size; i += len) {
    len = (file->size - i) > X25Q_READ_SIZE ? X25Q_READ_SIZE : (file->size - i);

    x25Qxx_read(file->addr + i, len, buf);
    Hash_MD5Update(md5, buf, len);
    if (0 == (i & 0x001FFFF)) {
      Watchdog_feed();
    }
  }

  Hash_MD5Final(md5);

  i = (r_memcmp(file->md5, md5->md, 16) == 0) ? 0 : 1;
  memory_release(md5);
  memory_release(buf);

  return i;
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void File_state(File_t *file, u32_t addr) {
  x25Qxx_read(addr, sizeof(File_t), (u8_t *)file);
  file->seat = 0;
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void File_save(File_t *file, u32_t addr) {
  x25Qxx_earse(addr);
  x25Qxx_wrtie(addr, sizeof(File_t), (u8_t *)file);
}
#endif

#ifdef _PLATFORM_L610_
#define FILE_READ_SIZE 2048
/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
u8_t File_rcve(File_t *file, u16_t offset, u8_t *data, u16_t len) {
  if (((file->sliceSize == len) || ((offset + 1) == file->sliceCnt)) && (file->sliceCnt > offset)) {
    u32_t saveAddr = offset * file->sliceSize;
    file->sliceState[offset >> 3] |= (1 << (offset & 0x07));
    s32_t nfile_size = fibo_file_getSize(file->name);
    INT32 iFd_File = 0;
    s32_t ret = 0;
    APP_DEBUG("%s file old size:%ld\r\n", file->name, nfile_size);

    if (nfile_size > 0) {   // 文件已存在
      iFd_File = fibo_file_open(file->name, FS_O_RDWR | FS_O_APPEND);
      if (iFd_File < 0) {
        APP_DEBUG("Open %s file fail\r\n", file->name);
        return 1;
      }
    } else {    // 文件不存在或者长度为0
      iFd_File = fibo_file_open(file->name, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC);
      if (iFd_File < 0) {
        APP_DEBUG("Create %s file fail\r\n", file->name);
        return 1;
      }
    }
    ret = fibo_file_seek(iFd_File, 0, FS_SEEK_END);
    if (ret < 0) {
      APP_DEBUG("seek %s file to end fail\r\n", file->name);
      ret = fibo_file_close(iFd_File);
      if (ret < 0) {
        APP_DEBUG("Close %s file fail\r\n", file->name);
      }
      return 1;
    }
    ret = fibo_file_write(iFd_File, data, len);
    if (ret < 0) {
      APP_DEBUG("Write %s file %d len data fail\r\n", file->name, len);
      ret = fibo_file_close(iFd_File);
      if (ret < 0) {
        APP_DEBUG("Close %s file fail\r\n", file->name);
      }
      return 1;
    }
    ret = fibo_file_fsync(iFd_File);
    if (ret < 0) {
      APP_DEBUG("sync %s file fail\r\n", file->name);
      ret = fibo_file_close(iFd_File);
      if (ret < 0) {
        APP_DEBUG("Close %s file fail\r\n", file->name);
      }
      return 1;
    }
    ret = fibo_file_close(iFd_File);
    if (ret < 0) {
      APP_DEBUG("Close %s file fail\r\n", file->name);
      return 1;
    }
    return 0;
  }
  APP_DEBUG("file size %d - %d, slice %d - %d  \r\n ", file->sliceSize, len, file->sliceCnt, offset);
  return 1;
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
s16_t File_read(File_t *file, u8_t *data, u16_t len) {
  static MD5_t file_md5;
  u8_t file_md5_value[16 + 1] = {0};
  u16_t readLen = 0;
  s16_t readenLen = 0;
  s32_t nfile_size = fibo_file_getSize(file->name);
  INT32 iFd_File = 0;
  s32_t ret = 0;

  if (nfile_size <=0) {   // 文件不存在
    APP_DEBUG("File %s is not existing\r\n", file->name);
    readenLen = -1;
    return readenLen;
  }

  if (file->seat == file->size) {   // 已经读完一次，再读需要先重置一下
    APP_DEBUG("finish reading %s file,reset file->seat\r\n", file->name);
    file->seat = 0;                 //seat 读升级文件指针
    readenLen = 0;
    return readenLen;
  }

//  APP_DEBUG("Begin read File %s, size:%ld seat:%ld\r\n", file->name, file->size, file->seat);
  iFd_File = fibo_file_open(file->name, FS_O_RDONLY);   //读取升级文件
  if (iFd_File <= 0) {
    APP_DEBUG("Open %s file fail\r\n", file->name);
    readenLen = -1;
    return readenLen;
  }
  if (file->seat == 0) {
    r_memset(&file_md5, 0, sizeof(MD5_t));
    Hash_MD5Init(&file_md5);
    APP_DEBUG("File Start read\r\n");
  }
  ret = fibo_file_seek(iFd_File, file->seat, FS_SEEK_SET);    //设置文件读取指针
  if (ret < 0) {
    APP_DEBUG("seek %s file to begin fail\r\n", file->name);
    readenLen = -1;
    ret = fibo_file_close(iFd_File);
    if (ret < 0) {
      APP_DEBUG("Close %s file fail\r\n", file->name);
    }
    return readenLen;
  }
  readLen = (file->size - file->seat) > len ? len : (file->size - file->seat);  //读取文件到数列data
  readenLen = fibo_file_read(iFd_File, data, readLen);
  if (readenLen != readLen) {
    APP_DEBUG("read %s file %d len data fail:%d\r\n", file->name, readLen, readenLen);
    readenLen = -1;
    ret = fibo_file_close(iFd_File);
    if (ret < 0) {
      APP_DEBUG("Close %s file fail\r\n", file->name);
    }
    return readenLen;
  }
  ret = fibo_file_close(iFd_File);
  if (ret < 0) {
    APP_DEBUG("Close %s file fail\r\n", file->name);
    readenLen = -1;
    return readenLen;
  }
//  x25Qxx_read(file->addr + file->seat, readLen, data);
  file->seat += readenLen;
//  APP_DEBUG("End read File %s %d len, size:%ld seat:%ld\r\n", file->name, readenLen, file->size, file->seat);
  
  Hash_MD5Update(&file_md5, data, readenLen);
  if (file->size == file->seat) {
    //readenLen=0;
    r_memset(file_md5_value, '\0', sizeof(file_md5_value));
    Hash_MD5Final(file_md5_value, &file_md5);
    APP_DEBUG("File read md5 %s, source md5 %s \r\n", file_md5_value, file->md5);
    if (r_memcmp(file_md5_value, file->md5, 16) != 0) {
      readenLen = -1;
      APP_DEBUG("File read rusult fail\r\n");
      log_save("File read rusult fail");
    }
  }
  if (0 == (file->seat & 0x001FFFF)) {
//  Watchdog_feed();
  }

  return readenLen;
}

/*
s16_t File_read(File_t *file, u8_t *data, u16_t len) {
  static MD5_t file_md5;
  u8_t file_md5_value[16 + 1] = {0};
  u16_t readLen = 0;
  s16_t readenLen = 0;
  s32_t nfile_size = fibo_file_getSize(file->name);
  INT32 iFd_File = 0;
  s32_t ret = 0;

  if (nfile_size <= 0) {   // 文件不存在或长度为0
    APP_DEBUG("File %s is not existing\r\n", file->name);
    readenLen = -1;
    return readenLen;
  }

  if (file->seat == file->size) {   // 已经读完一次，再读需要先重置一下
    APP_DEBUG("finish reading %s file,reset file->seat\r\n", file->name);
    file->seat = 0;
    readenLen = -1;
    return readenLen;
  }
//  APP_DEBUG("Begin read File %s, size:%ld seat:%ld\r\n", file->name, file->size, file->seat);
  iFd_File = fibo_file_open(file->name, FS_O_RDONLY);
  if (iFd_File < 0) {
    APP_DEBUG("Open %s file fail\r\n", file->name);
    readenLen = -1;
    return readenLen;
  }
  if (file->seat == 0) {
    r_memset(&file_md5, 0, sizeof(MD5_t));
    Hash_MD5Init(&file_md5);
    APP_DEBUG("File Start read\r\n");
  }
  ret = fibo_file_seek(iFd_File, file->seat, FS_SEEK_SET);
  if (ret < 0) {
    APP_DEBUG("seek %s file to begin fail\r\n", file->name);
    readenLen = -1;
    ret = fibo_file_close(iFd_File);
    if (ret < 0) {
      APP_DEBUG("Close %s file fail\r\n", file->name);
    }
    return readenLen;
  }
  readLen = (file->size - file->seat) > len ? len : (file->size - file->seat);
  readenLen = fibo_file_read(iFd_File, data, readLen);
  if (readenLen != readLen) {
    APP_DEBUG("read %s file %d len data fail:%d\r\n", file->name, readLen, readenLen);
    readenLen = -1;
    ret = fibo_file_close(iFd_File);
    if (ret < 0) {
      APP_DEBUG("Close %s file fail\r\n", file->name);
    }
    return readenLen;
  }
  ret = fibo_file_close(iFd_File);
  if (ret < 0) {
    APP_DEBUG("Close %s file fail\r\n", file->name);
    readenLen = -1;
    return readenLen;
  }
//  x25Qxx_read(file->addr + file->seat, readLen, data);
  file->seat += readenLen;
//  APP_DEBUG("End read File %s %d len, size:%ld seat:%ld\r\n", file->name, readenLen, file->size, file->seat);
  Hash_MD5Update(&file_md5, data, readenLen);
  if (file->size == file->seat) {
    r_memset(file_md5_value, '\0', sizeof(file_md5_value));
    Hash_MD5Final(file_md5_value, &file_md5);
    APP_DEBUG("File read md5 %s, source md5 %s \r\n", file_md5_value, file->md5);
    if (r_memcmp(file_md5_value, file->md5, 16) != 0) {
      readenLen = -1;
      APP_DEBUG("File read rusult fail\r\n");
      log_save("File read rusult fail");
    }
  }
  if (0 == (file->seat & 0x001FFFF)) {
//  Watchdog_feed();
  }

  return readenLen;
}
*/
/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
u8_t File_Check(File_t *file) {
  MD5_t md5;
  u8_t *buf = NULL;
  u32_t i = 0;
  u16_t len = 0;
  u8_t md5_str[32 + 1] = {0};

  s32_t nfile_size = fibo_file_getSize(file->name);
  INT32 iFd_File = 0;
  s32_t ret = 0;
  u32_t readenLen = 0;

  if (nfile_size <= 0) {   // 文件不存在或长度为0
    APP_DEBUG("File %s is not existing\r\n", file->name);
    i = 1;
    return i;
  }
  iFd_File = fibo_file_open(file->name, FS_O_RDONLY);
  if (iFd_File < 0) {
    APP_DEBUG("Open %s file fail\r\n", file->name);
    i = 1;
    return i;
  }

  r_memset(&md5, 0, sizeof(MD5_t));
  buf = memory_apply(FILE_READ_SIZE);

  Hash_MD5Init(&md5);
  for (i = 0; i < file->size; i += len) {
    r_memset(buf, 0, FILE_READ_SIZE);
    ret = fibo_file_seek(iFd_File, i, FS_SEEK_SET);
    if (ret < 0) {
      APP_DEBUG("seek %s file to %ld fail\r\n", file->name, i);
      i = -1;
      ret = fibo_file_close(iFd_File);
      if (ret < 0) {
        APP_DEBUG("Close %s file fail\r\n", file->name);
        break;
      }
      break;
    }
    len = (file->size - i) > FILE_READ_SIZE ? FILE_READ_SIZE : (file->size - i);
    readenLen = fibo_file_read(iFd_File, buf, len);
    if (readenLen != len) {
      APP_DEBUG("read %s file %d len data fail\r\n", file->name, len);
      i = 1;
      ret = fibo_file_close(iFd_File);
      if (ret < 0) {
        APP_DEBUG("Close %s file fail\r\n", file->name);
        break;
      }
      break;
    }
//  x25Qxx_read(file->addr + i, len, buf);
    Hash_MD5Update(&md5, buf, len);
    if (0 == (i & 0x001FFFF)) {
//    Watchdog_feed();
    }
  }
  memory_release(buf);
  ret = fibo_file_close(iFd_File);
  if (ret < 0) {
    i = 1;
    APP_DEBUG("Close %s file fail\r\n", file->name);
    return i;
  }
  r_memset(md5_str, '\0', sizeof(md5_str));
  Hash_MD5Final(md5_str, &md5);
  i = (r_memcmp(file->md5, md5_str, 16) == 0) ? 0 : 1;
  if (i != 0) {
    APP_DEBUG("%s file md5 check fail, delete it\r\n", file->name);
    ret = fibo_file_delete(file->name);
    if (ret < 0) {
      APP_DEBUG("Delete %s file fail\r\n", file->name);
    }
  }
  return i;
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void File_state(File_t *file, u32_t addr) {
  char strFileName[32] = {0};
  file->seat = 0;
  r_memset(strFileName, '\0', sizeof(strFileName));
  switch (addr) {
    case FOTA_FILE_FLAG:
      r_strncpy(strFileName, FOTA_STATUSFILE_NAME, r_strlen(FOTA_STATUSFILE_NAME));
      break;
    case DEVICE_FOTA_FILE_FLAG:
      r_strncpy(strFileName, DEVICE_FOTA_STATUSFILE_NAME, r_strlen(DEVICE_FOTA_STATUSFILE_NAME));
      break;
    case CA_FILE_FLAG:
      r_strncpy(strFileName, CA_STATUSFILE_NAME, r_strlen(CA_STATUSFILE_NAME));
      break;
    case POINT_TAB_FILE_FLAG:
      r_strncpy(strFileName, POINT_TABSTATUS_FILE_NAME, r_strlen(POINT_TABSTATUS_FILE_NAME));
      break;
    default:
      break;
  }

  s32_t nfile_size = fibo_file_getSize(strFileName);
  INT32 iFd_File = 0;
  s32_t ret = 0;
  s32_t readenLen = 0;
  APP_DEBUG("%s file size:%ld\r\n", file->name, nfile_size);

  if (nfile_size <= 0) {   // 文件不存在或长度为0
    APP_DEBUG("File %s is not existing\r\n", strFileName);
    return;
  }
  iFd_File = fibo_file_open(strFileName, FS_O_RDONLY);
  if (iFd_File < 0) {
    APP_DEBUG("Open %s file fail\r\n", strFileName);
    return;
  }

  ret = fibo_file_seek(iFd_File, 0, FS_SEEK_SET);
  if (ret < 0) {
    APP_DEBUG("seek %s file to begin fail\r\n", strFileName);
    ret = fibo_file_close(iFd_File);
    if (ret < 0) {
      APP_DEBUG("Close %s file fail\r\n", strFileName);
    }
    return;
  }

  readenLen = fibo_file_read(iFd_File, (u8_t *)file, sizeof(File_t));
  if (readenLen != sizeof(File_t)) {
    APP_DEBUG("read %s file %ld len data fail\r\n", strFileName, readenLen);
    ret = fibo_file_close(iFd_File);
    if (ret < 0) {
      APP_DEBUG("Close %s file fail\r\n", strFileName);
    }
    return;
  }
  ret = fibo_file_close(iFd_File);
  if (ret < 0) {
    APP_DEBUG("Close %s file fail\r\n", strFileName);
    return;
  }
}

/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void File_save(File_t *file, u32_t addr) {
  char strFileName[32] = {0};
  r_memset(strFileName, '\0', sizeof(strFileName));
  switch (addr) {
    case FOTA_FILE_FLAG:
      r_strncpy(strFileName, FOTA_STATUSFILE_NAME, r_strlen(FOTA_STATUSFILE_NAME));
      break;
    case DEVICE_FOTA_FILE_FLAG:
      r_strncpy(strFileName, DEVICE_FOTA_STATUSFILE_NAME, r_strlen(DEVICE_FOTA_STATUSFILE_NAME));
      break;
    case CA_FILE_FLAG:
      r_strncpy(strFileName, CA_STATUSFILE_NAME, r_strlen(CA_STATUSFILE_NAME));
      break;
    case POINT_TAB_FILE_FLAG:
      r_strncpy(strFileName, POINT_TABSTATUS_FILE_NAME, r_strlen(POINT_TABSTATUS_FILE_NAME));
      break;
    default:
      break;
  }

  s32_t nfile_size = fibo_file_getSize(strFileName);
  INT32 iFd_File = 0;
  s32_t ret = 0;
  s32_t writeenLen = 0;

  if (nfile_size >= 0) {   // 文件存在或长度为0
    APP_DEBUG("File %s is existing\r\n", strFileName);
    ret = fibo_file_delete(strFileName);
    if (ret < 0) {
      APP_DEBUG("Delete %s file fail\r\n", strFileName);
      return;
    }
  }

  iFd_File = fibo_file_open(strFileName, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC);
  if (iFd_File < 0) {
    APP_DEBUG("Create %s file fail\r\n", strFileName);
    return;
  }

  writeenLen = fibo_file_write(iFd_File, (u8_t *)file, sizeof(File_t));
  if (writeenLen != sizeof(File_t)) {
    APP_DEBUG("Write %s file %d len data fail\r\n", strFileName, sizeof(File_t));
    ret = fibo_file_close(iFd_File);
    if (ret < 0) {
      APP_DEBUG("Close %s file fail\r\n", strFileName);
    }
    return;
  }
  ret = fibo_file_fsync(iFd_File);
  if (ret < 0) {
    APP_DEBUG("sync %s file fail\r\n", strFileName);
    ret = fibo_file_close(iFd_File);
    if (ret < 0) {
      APP_DEBUG("Close %s file fail\r\n", strFileName);
    }
    return;
  }
  ret = fibo_file_close(iFd_File);
  if (ret < 0) {
    APP_DEBUG("Close %s file fail\r\n", strFileName);
    return;
  }
}
#endif
/******************************************************************************/

