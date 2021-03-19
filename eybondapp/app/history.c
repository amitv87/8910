
/******************************************************************************           
* name:             history.c           
* introduce:        history device data save for state grid 
* author:           Luee                                     
******************************************************************************/ 
#include "history.h"
#include "grid_tool.h"
#include "StateGrid.h"

#define history_file  "/history_file.ini"        //文件名

s32     iFd_history = 0;                     //文件描述符
static char history_head_buf[history_headoffset_len];
//static char history_head_buf[history_head_len];

history_head_t *history_head = (history_head_t *)history_head_buf;

u16_t sw_historyhead_rp = 0;


/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
/*
s32 history_head_get(void) {
  s32_t ret = 0;
  s32_t readenLen = 0;
  iFd_history = fibo_file_open(history_file, FS_O_RDONLY);
  if (iFd_history >= 0) {
    ret = fibo_file_seek(iFd_history, 0, FS_SEEK_SET);
    readenLen = fibo_file_read(iFd_history, (u8_t *)history_head_buf, sizeof(history_head_buf));
    ret = fibo_file_close(iFd_history);
    APP_DEBUG("get:run log read po = %d,write po = %d,l = %ld\r\n", \
              history_head->file_logr_pointer, history_head->file_logw_pointer, readenLen);
    if (readenLen < sizeof(history_head_buf)) {
      APP_DEBUG("Read %s history file head error!\r\n", history_file);
      ret = -1;
    }
  } else {
    APP_DEBUG("Read %s history file head error!\r\n", history_file);
    ret = -1;
  }
  return ret;
}
*/

s32 history_head_get(void)
{
  s32 ret=0;
  s32 readlen=-1;

  iFd_history = fibo_file_open(history_file, FS_O_RDONLY);
  if (iFd_history >= 0) {
    ret = fibo_file_seek(iFd_history, 0, FS_SEEK_SET);
    readlen = fibo_file_read(iFd_history, (u8_t *)history_head_buf, sizeof(history_head_buf));
    ret = fibo_file_fsync(iFd_history);
    ret = fibo_file_close(iFd_history);
  }else{
    ret=-1;
  }
  return ret;
}
/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
s32 history_init(void)
{
  s32 ret = 0;

  //fibo_file_delete(history_file);

  s32_t history_size = fibo_file_getSize(history_file);
  if (history_size > 0) {
    APP_DEBUG("found old history file!\r\n");
    //头信息获取失败
    if (history_head_get() != 0) {
      APP_DEBUG("run history file head err!\r\n");
      fibo_file_delete(history_file);
      history_size = -1;
    } else if ((history_head->file_logw_pointer >= history_pointer_size) ||
               (history_head->file_logr_pointer >= history_pointer_size)) {
      //文件大小和当前读写指针不匹配
      APP_DEBUG("history log file size err!\r\n");
      fibo_file_delete(history_file);
      history_size = -1;
    }
  }

  if ((fibo_file_exist(history_file)!=1)||(history_size <= 0)) {
    APP_DEBUG("run history file create\r\n");
    history_head->file_logw_pointer = 0;
    history_head->file_logr_pointer = 0;
    //iFd_history = fibo_file_open(history_file, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC);   // 读写、创建
    iFd_history = fibo_file_open(history_file, FS_O_RDWR|FS_O_CREAT|FS_O_APPEND);//读写、创建、追加
    ret = fibo_file_seek(iFd_history, 0, FS_SEEK_SET);
    history_size = fibo_file_write(iFd_history, (u8_t *)history_head_buf, sizeof(history_head_buf));
    ret = fibo_file_fsync(iFd_history);
    ret = fibo_file_close(iFd_history);
  } 

  iFd_history = fibo_file_open(history_file, FS_O_RDONLY);
  ret = fibo_file_seek(iFd_history, 0, FS_SEEK_SET);
  ret = fibo_file_read(iFd_history, (u8_t *)history_head_buf, sizeof(history_head_buf));
  ret = fibo_file_fsync(iFd_history);
  ret = fibo_file_close(iFd_history);
  history_size = fibo_file_getSize(history_file);
  APP_DEBUG("\r\n-->state grid:history file read pointer = %d,write pointer = %d,lenght = %ld\r\n", \
              history_head->file_logr_pointer, history_head->file_logw_pointer, history_size);
    
  return history_size;
}

/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
s32 history_put(history_head_t *head,  Buffer_t *buf)
{
  s32 ret=-1;

  u8 history_buf[history_line_len]={0};
  buf_t *history_data=(buf_t *)history_buf;

  APP_DEBUG("\r\n-->state grid history put:buf->lenght=%d\r\n",buf->lenght);
  //国网历史数据长度不能超过450
  if(buf->lenght>STATE_GRID_CMD_SIZE)
    return ret;
  //print_buf(buf->payload,buf->lenght);

  //若写指针超出，删除文件，指针归零
  history_init();

  //得到国网历史数据写入BUF:history_buf
  history_data->lenght=buf->lenght;
  r_memcpy(history_data->payload,buf->payload, buf->lenght);
  history_data->size=sizeof(history_data->size)+sizeof(history_data->lenght)+history_data->lenght;
  //print_buf(history_buf,history_data->size);

  iFd_history = fibo_file_open(history_file, FS_O_WRONLY|FS_O_APPEND);
  if (iFd_history < 0) {
    APP_DEBUG("\r\n-->state grid:open history file fail!\r\n");
    return ret;
  }
  //将国网历史数据history_buf写入文件history file  
  ret = fibo_file_seek(iFd_history, head->file_logw_pointer * history_line_len + history_headoffset_len, FS_SEEK_SET);
  ret=fibo_file_write(iFd_history, (u8_t *)history_buf, sizeof(history_buf));
  if(ret!=sizeof(history_buf)){
    APP_DEBUG("\r\n-->state grid history write fail,ret=%ld\r\n",ret);
    fibo_file_fsync(iFd_history);
    fibo_file_close(iFd_history);
    return ret;
  }

  // 写指针加1并写入文件
  ret = fibo_file_seek(iFd_history, 0, FS_SEEK_SET);
  head->file_logw_pointer = (head->file_logw_pointer + 1) % history_pointer_size;
  fibo_file_write(iFd_history,(u8_t *)history_head_buf,sizeof(history_head_buf));

  APP_DEBUG("\r\n-->state grid history save lenght:%d,next w_printer:%d",history_data->lenght,head->file_logw_pointer); 
  ret = fibo_file_fsync(iFd_history);
  ret = fibo_file_close(iFd_history);
  return ret;
}


//  pdu.lenght = FlashFIFO_get(&historyHead, &pdu);
/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
u16 history_get(history_head_t *head,  Buffer_t *buf)
{
  s32 ret=-1;
  u16 history_size=0xffff;
  u8 history_buf[history_line_len]={0};
  buf_t *history_data=(buf_t *)history_buf;

  history_init();

  //iFd_history = fibo_file_open(history_file, FS_O_RDWR|FS_O_APPEND);
  iFd_history = fibo_file_open(history_file, FS_O_RDONLY);
  if (iFd_history < 0) {
    APP_DEBUG("open history file fail!\r\n");
    return history_size;
  }

  //从文件history_file读取指针head->file_logr_pointer指定的数据，存入history_buf  
  ret = fibo_file_seek(iFd_history, head->file_logr_pointer * history_line_len + history_headoffset_len, FS_SEEK_SET);
  history_size=fibo_file_read(iFd_history, (u8_t *)history_buf, sizeof(history_buf));
  ret = fibo_file_fsync(iFd_history);
  ret = fibo_file_close(iFd_history);

  //得到返回数据
  history_size=history_data->lenght;
  if(history_size==0||history_size>STATE_GRID_CMD_SIZE||(history_data->size!=history_size+4)){
    APP_DEBUG("\r\n-->state grid history read fail,history_size=%d\r\n",history_size);
    return history_size;
  }
  r_memcpy(buf->payload,history_data->payload,history_size);
  //print_buf(history_buf,history_data->size);

  head->file_logr_pointer = (head->file_logr_pointer + 1);    //% history_pointer_size;
  iFd_history = fibo_file_open(history_file, FS_O_WRONLY|FS_O_APPEND);
  ret = fibo_file_seek(iFd_history, 0, FS_SEEK_SET);
  fibo_file_write(iFd_history,(u8_t *)history_head_buf,sizeof(history_head_buf));
  ret = fibo_file_fsync(iFd_history);
  ret = fibo_file_close(iFd_history);

  APP_DEBUG("\r\n-->state grid history read lenght:%d,next r_printer:%d",history_data->lenght,head->file_logr_pointer); 
  return history_size;
}