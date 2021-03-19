/******************************************************************************           
* name:             history.h           
* introduce:        头文件
* author:           Luee                                     
******************************************************************************/ 
#ifndef _HISTORY_H_
#define _HISTORY_H_

#include "grid_tool.h"
#include "StateGrid.h"

#define history_headoffset_len  512   //STATE_GRID_CMD_SIZE
#define history_head_len        4   

// 历史行长度
#define history_line_len        512   //STATE_GRID_CMD_SIZE   
// 历史文件大小
#define history_file_size       ((100)*(history_line_len)+(history_headoffset_len))
#define history_pointer_size    ((history_file_size-history_headoffset_len)/history_line_len)

// 服务器获取历史最大的条数
//#define history_max_line        20

typedef struct {
  u16 file_logw_pointer;
  u16 file_logr_pointer;
  u8 arry[];
} history_head_t;

//size=size占用字节数+lenght占用字数+payload[]占用字节数
//lenght=payload[]占用字节数
#pragma pack(1)       //按1字节对齐，保证结构体数据可用数列代替
typedef struct {
  u16_t size;
  u16_t lenght;
  u8_t payload[];
} buf_t;
#pragma pack()

extern history_head_t *history_head;

s32 history_init(void);
s32 history_put(history_head_t *head,  Buffer_t *buf);
u16 history_get(history_head_t *head,  Buffer_t *buf);

#endif /* _HISTORY_H_ */