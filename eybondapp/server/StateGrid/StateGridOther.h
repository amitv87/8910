/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
  *@brief   : StateGrid other device data get.h
  *@notes   : 2018.12.30 CGQ establish
*******************************************************************************/
#ifndef __STATE_GRID_OTHER_H
#define __STATE_GRID_OTHER_H

#include "eyblib_typedef.h"
#include "StateGridData.h"

#pragma pack(1)
typedef struct {
  u32_t id;
  u16_t number;
  u16_t offset;
  u8_t  type;
  u8_t  note;
} FileOtherPoint_t;

typedef struct {
  u32_t id;
  u16_t number;
  u16_t offset;
  u16_t rsv;
  u8_t  type;
  u8_t  note;
} FileOtherPointV2_t;
#pragma pack()

typedef struct {
  u16_t  number;
  u16_t  offset;
} OtherPoint_t;

typedef struct {
  OtherPoint_t point;
  StateGridPoint_t *src;
} OtherCollectPoint_t;

typedef struct {
  int count;
  OtherCollectPoint_t tab[];
} OtherCollectTab_t;

typedef struct {
  u8_t lenght;
  u8_t data[0];
} OtherCmd_t;

typedef struct {
  int  flag;
  int  size;
  int  cmdCount;
  u8_t *cmdTab;
  int pointCount;
  u8_t *point;
} OtherPointFile_t;

typedef struct {
  u8_t result;
  u8_t number;
} OtherCmdResult_t;

extern const StateGridDevice_t StateGridOtherDevice;

#endif //__STATE_GRID_OTHER_H
/******************************************************************************/

