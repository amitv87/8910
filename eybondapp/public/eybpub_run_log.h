/******************************************************************************           
* name:             eybpub_run_log.h           
* introduce:        头文件
* author:           Luee                                     
******************************************************************************/ 
#ifndef _EYBPUB_RUN_LOG_H_
#define _EYBPUB_RUN_LOG_H_

#include "grid_tool.h"
#include "history.h"

#define log_headoffset_len      64   
#define log_head_len            4   

// log行长度
#define log_line_len            64     
// 历史文件大小
#define log_file_size           ((200)*(log_line_len)+(log_headoffset_len))
#define log_pointer_size    ((log_file_size-log_headoffset_len)/log_line_len)

#define LOG_PARA_LEN  8
#define LOG_PARA_ST   8
#define APP_UPDATA    0

typedef struct {
  u16 file_logw_pointer;  //log writer pointer
  u16 file_logr_pointer;  //log read pointer
  u16 reserved0;            //reserved data
  u16 reserved1;            //reserved data
  u16 reserved2;            //reserved data
  u16 reserved3;            //reserved data
  u16 reserved4;            //reserved data
  u16 reserved5;            //reserved data
  u16 log_para[LOG_PARA_LEN];         //log para 
  u8 arry[];
}log_head_t;

extern u16 sw_loghead_rp;

s32 log_init(void);
void log_save(char *str, ...);
void log_clean(void);
u16_t log_get(Buffer_t *buf);

s32 logpara_write(u8 index,u16 data);
u16 logpara_read(u8 index);

#endif /* _EYBPUB_RUN_LOG_H_ */

