/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
  *@brief   : state grid Server Update data
  *@notes   : 2018.10.23 CGQ
*******************************************************************************/
#include "eyblib_swap.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_memory.h"

#include "eybpub_File.h"
#include "eybpub_SysPara_File.h"
#include "eybpub_run_log.h"
#include "eybpub_Debug.h"

#include "StateGridData.h"
#include "StateGridModbus.h"
#include "StateGridOther.h"
#include "StateGrid.h"

static StateGridDevice_t  *s_Device = null;
static StateGridPointTab_t *pointTab = null;

/**
 * @brief Other task and function get the station grid data handle
 * @param pDevice
 * @param pPointTab
 * @return
 */
int getGridDataHandle(StateGridDevice_t **pDevice, StateGridPointTab_t **pPointTab) {
  *pDevice = s_Device;
  *pPointTab = pointTab;

  if (*pDevice == null || *pPointTab == null) {
    return -1;
  }

  return 0;
}

/**
 * ****************************************************************************
 * @name    StateGrid_pointTab
 * @brief   Determine the type of national network communication protocol
 * @note    Modbus or other
 * @return
 * ****************************************************************************
 */
int StateGrid_pointTab(void) {
  int i = -1;
  File_t *file = memory_apply(sizeof(File_t));
  void *pointFile;

  s_Device = null;

  /* Read the source datas from the flash*/
#ifdef _PLATFORM_M26_  
  File_state(file, POINT_TAB_FILE_ADDR);
#endif
#ifdef _PLATFORM_L610_
  File_state(file, POINT_TAB_FILE_FLAG);
#endif
  APP_DEBUG("Point file size %lx \r\n", file->size);

  /* Check the state grid point tab datas*/
  if ((file->size > 0x10000)
      || (File_Check(file) != 0)) {
    log_save("State grid point table NO find!!");
  } else {
    if (pointTab != null) {
      int i;

      for (i = 0; i < pointTab->count; i++) {
        memory_release(pointTab->tab[i].value.payload);
      }
      memory_release(pointTab);
      pointTab = null;
    }

    pointFile = memory_apply(file->size);

    if (pointFile != null
        && (File_read(file, (u8_t *)pointFile, file->size) == file->size)) {
      if ((pointTab = StateGridModbusDevice.check(pointFile, file->size)) != null) {
        s_Device = (StateGridDevice_t *)&StateGridModbusDevice;
        i = 0;
      } else if ((pointTab = StateGridOtherDevice.check(pointFile, file->size)) != null) {
        s_Device = (StateGridDevice_t *)&StateGridOtherDevice;
        i = 0;
      } else {
        log_save("State grid no find device!!");
      }

    } else {
      log_save("State grid point file err");
    }
    memory_release(pointFile);
  }
  memory_release(file);

  return i;
}

/*******************************************************************************
  * @note   state grid point data status
  * @param  None
  * @retval dataState > 0 data is valict
*******************************************************************************/
int StateGrid_dataStatus(void) {
  if (s_Device == null) {
    APP_DEBUG("s_Device = null\r\n");
    return 0;
  } else {
    return  s_Device->status();
  }
}

/*******************************************************************************
  * @note   state grid point data get
  * @param  None
  * @retval 0: success, -1: buf size mix,
*******************************************************************************/

int  StateGrid_dataGet(Buffer_t *buf, u8_t *time, PointArray_t *point, u32_t *numbers) {
  static int i = 0;

  u8_t *pPointCount;
  u8_t *dataPoint;
  u32_t pointStation;

  *numbers = 0;
  dataPoint = buf->payload;
  /*point count*/
  *dataPoint++ = 0;
  *dataPoint++ = 0;
  *dataPoint++ = 0;
  *dataPoint = 0;
  pPointCount = dataPoint++;


  /*point array*/
  for (; i < pointTab->count; i++) {
    /*Get station number*/
    if (*numbers == 0) {
      if (StateGrid_station(pointTab->tab[i].addr, numbers) != 1) {
        continue;
      }
    }

    if ((dataPoint - buf->payload + 60) > buf->size) {
      goto END;
    }
    /* Data acquisition at designated measuring points*/
    if (point != null) {
      int n;
      for (n = 0; n < point->count; n++) {
        if (pointTab->tab[i].id == point->array[n]) {
          StateGrid_station(pointTab->tab[i].addr, numbers);
          goto LOADPOINT;
        }
      }

      continue;
    }
    /*Verification of power stations belonging to survey points*/
    StateGrid_station(pointTab->tab[i].addr, &pointStation);
    if (pointStation != *numbers) {
      goto END;
    }
  LOADPOINT:

    /*point Count*/
    (*pPointCount)++;
    /*point number*/
    *dataPoint++ = (pointTab->tab[i].id >> 24) & 0xFF;
    *dataPoint++ = (pointTab->tab[i].id >> 16) & 0xFF;
    *dataPoint++ = (pointTab->tab[i].id >> 8) & 0xFF;
    *dataPoint++ = (pointTab->tab[i].id >> 0) & 0xFF;
    /*timer*/
    if (time != null) {
      r_memcpy(dataPoint, time, 8);
      dataPoint += 8;
    }
    /*data count*/
    *dataPoint++ = 1;
    /*data array*/
    {
      /*data quality  3F 7D 70 A3*/
      // *dataPoint++ = 0;
      // *dataPoint++ = 0;
      // *dataPoint++ = 0;
      // *dataPoint++ = 0;
      *dataPoint++ = 0x3F;
      *dataPoint++ = 0x7D;
      *dataPoint++ = 0x70;
      *dataPoint++ = 0xA3;
      /*data id */
      *dataPoint++ = 1;
      /*value type*/
      if (pointTab->tab[i].value.lenght == 1) {
        *dataPoint++ = 'B';
      } else if (pointTab->tab[i].value.lenght == 0) {
        *dataPoint++ = 'N';
      } else if (pointTab->tab[i].type == 'T') {
        *dataPoint++ = 'T';
        *dataPoint++ = pointTab->tab[i].value.lenght;
      } else if (((pointTab->tab[i].lenght & 0x80) == 0x80)
                 && ((pointTab->tab[i].type == 'S')
                     || (pointTab->tab[i].type == 'I')
                     || (pointTab->tab[i].type == 't')
                     || (pointTab->tab[i].type == 'M')
                     || (pointTab->tab[i].type == 'V')
                     || (pointTab->tab[i].type == 'C')
                     || (pointTab->tab[i].type == 'G'))) {
        *dataPoint++ = 'F';
      } else if (pointTab->tab[i].type == 'd') {
        *dataPoint++ = 'D';

      } else if (pointTab->tab[i].type == 'E'
                 || (pointTab->tab[i].type == 'A')
                ) {
        *dataPoint++ = 'F';
      } else {
        *dataPoint++ = pointTab->tab[i].type;
      }
      /*value*/
      r_memcpy(dataPoint, pointTab->tab[i].value.payload, pointTab->tab[i].value.lenght);
      dataPoint += pointTab->tab[i].value.lenght;
    }

  }

  i = 0;
END:
  /*PDU leng*/
  buf->lenght = dataPoint - buf->payload;
  Swap_bigSmallLong(numbers);
  return i;
}


/*******************************************************************************
  * @note   state grid point new resource destroy
  * @param  None
  * @retval dataState > 0 data is valict
*******************************************************************************/
void StateGrid_destroy(void) {
  if (s_Device != null) {
    s_Device->release();
  }
  s_Device = null;
}

/*******************************************************************************
  * @note   state grid point data lenght
  * @param  None
  * @retval None
*******************************************************************************/
int StateGrid_pointLenght(u8_t type, u8_t note, u16_t rev) {
  int i = 0;

  switch (type) {
    case 'T':
      i = note;
      break;
    case 'L':
    case 'D':
      i = 4;
      break;
    case 'I':
    case 'F':
    case 't':
    case 'G':
      i = 2;
      break;
    case 'e':
    case 'b':
    case 'B':
    case 'S':
    case 'M':
    case 'V':
      i = 1;
      break;
    case 'd':
      i = note & 0x3F;
      break;
    case 'E':
      i = rev;
      break;
  }
  return i;
}

int StateGrid_pointValueLenght(u8_t type, u8_t note, u16_t rev) {
  int i = 0;

  switch (type) {
    case 'T':
      if (rev) {
        i = rev;
      } else {
        i = note << 1;
      }
      break;
    case 'L':
    case 'D':
      i = 8;
      break;
    case 'I':
    case 'F':
    case 't':
    case 'G':
      i = 4;
      break;
    case 'e':
    case 'b':
    case 'B':
      i = 1;
      break;
    case 'S':
    case 'M':
    case 'V':
      if ((note & 0x80) == 0x80) {
        i = 4;
      } else {
        i = 2;
      }

      break;
    case 'd':
      i = 8;
      break;
    case 'E':
    case 'C':
      i = 4;
      break;
    case 'A':
      i = 4;
      break;

  }
  return i;
}

void StateGrid_dataCollect(void) {
  if (s_Device != null) {
    s_Device->cmd();
  }
}
/******************************************************************************/

