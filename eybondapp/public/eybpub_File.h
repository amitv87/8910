/**************Copyright(C) 2015-2026 eybond  *********************************
  *@brief   : eybpub_File.h
  *@notes   : 2017.09.08
*******************************************************************************/
#ifndef __EYBPUB_FILE_H_
#define __EYBPUB_FILE_H_

#include "eyblib_typedef.h"

typedef struct {
  char    name[32];
  u8_t    md5[16];        //
  u32_t   addr;           // file save start address
  u32_t   size;           // file user size
  u32_t   seat;
  u32_t   flag;           // file valid flag
  u16_t   area;           // file area
  u16_t   sliceCnt;       // file trans slice count num
  u16_t   sliceSize;      // file slice size;
  u16_t   sliceStateLen;  // file slice rcveice state user len
  u8_t    sliceState[];   // file slice rcveice state, 1: rcve success, 0: no rcve
} File_t;

File_t *File_init(u32_t addr, u16_t sliceCnt, u16_t sliceSize);
u8_t    File_rcve(File_t *file, u16_t offset, u8_t *data, u16_t len);
s16_t   File_read(File_t *file, u8_t *data, u16_t len);
u8_t    File_Check(File_t *file);
u8_t    File_validCheck(File_t *file);
void    File_state(File_t *file, u32_t addr);
void    File_save(File_t *file, u32_t addr);

#endif //__EYBPUB_FILE_H
/******************************************************************************/

