/******************************************************************************           
* name:             eybpub_run_log.c           
* introduce:        log save & read
* author:           Luee                                     
******************************************************************************/ 
#include "eybpub_run_log.h"
#include "grid_tool.h"

#define log_file  "/log_file.ini"        //文件名

s32 iFd_log = 0;                     //文件描述符
static char log_head_buf[log_headoffset_len];
log_head_t *log_head = (log_head_t *)log_head_buf;
u16 sw_loghead_rp;
static u8 log_writeing=0;    //=1 writing =0 write finish
static u8 log_reading=0;    //=1 readng =0 read finish

static s32 log_head_get(void);
static s32 log_write(log_head_t *head,  Buffer_t *buf);
static u16 log_read(log_head_t *head,  Buffer_t *buf);

/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
s32 logpara_write(u8 index,u16 data)
{
  s32 ret=-1;
  iFd_log = fibo_file_open(log_file, FS_O_WRONLY|FS_O_APPEND);
  if (iFd_log < 0) {
    APP_DEBUG("\r\n-->log:open log file fail\r\n");
    return ret;
  }

  ret=0;
  // 写指针加1并写入文件
  fibo_file_seek(iFd_log, 0, FS_SEEK_SET);
  log_head->log_para[index]=data;
  fibo_file_write(iFd_log,(u8_t *)log_head_buf,sizeof(log_head_buf));

  APP_DEBUG("\r\n-->log para write index=%d,log para data=%d",index,log_head->log_para[index]); 
  fibo_file_fsync(iFd_log);
  fibo_file_close(iFd_log);
  return ret;
}

/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
u16 logpara_read(u8 index)
{
  s32 ret=-1;
  u16 data;
  iFd_log = fibo_file_open(log_file, FS_O_RDONLY);
  if (iFd_log < 0) {
    APP_DEBUG("\r\n-->log:open log file fail!\r\n");
    return ret;
  }

  ret=0;
  // 写指针加1并写入文件
  fibo_file_seek(iFd_log, 0, FS_SEEK_SET);
  ret = fibo_file_read(iFd_log, (u8_t *)log_head_buf, sizeof(log_head_buf));
  data=log_head->log_para[index];

  APP_DEBUG("\r\n-->log para read index=%d,log para data=%d",index,log_head->log_para[index]); 
  fibo_file_fsync(iFd_log);
  fibo_file_close(iFd_log);
  return data;
}

/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
static s32 log_head_get(void)
{
  s32 ret=0;
  s32 readlen=-1;

  iFd_log = fibo_file_open(log_file, FS_O_RDONLY);
  if (iFd_log >= 0) {
    ret = fibo_file_seek(iFd_log, 0, FS_SEEK_SET);
    readlen = fibo_file_read(iFd_log, (u8_t *)log_head_buf, sizeof(log_head_buf));
    ret = fibo_file_fsync(iFd_log);
    ret = fibo_file_close(iFd_log);
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
s32 log_init(void)
{
  s32 ret = 0;
  u8 i;

  //fibo_file_delete(log_file);

  s32 log_size = fibo_file_getSize(log_file);
  if (log_size > 0) {
    //头信息获取失败
    if (log_head_get() != 0) {
      APP_DEBUG("run history file head err!\r\n");
      fibo_file_delete(log_file);
      log_size = -1;
    } else if ((log_head->file_logw_pointer >= log_pointer_size) ||
               (log_head->file_logr_pointer >= log_pointer_size)) {
      //文件大小和当前读写指针不匹配
      APP_DEBUG("logtory log file size err!\r\n");
      fibo_file_delete(log_file);
      log_size = -1;
    }
  }

  if ((fibo_file_exist(log_file)!=1)||(log_size <= 0)) {
    //初始化头参数
    log_head->file_logw_pointer = 0;
    log_head->file_logr_pointer = 0;
    for(i=0;i<LOG_PARA_LEN;i++){
      log_head->log_para[i]=0;
    }

    iFd_log = fibo_file_open(log_file, FS_O_RDWR|FS_O_CREAT|FS_O_APPEND);//读写、创建、追加
    ret = fibo_file_seek(iFd_log, 0, FS_SEEK_SET);
    log_size = fibo_file_write(iFd_log, (u8_t *)log_head_buf, sizeof(log_head_buf));
    ret = fibo_file_fsync(iFd_log);
    ret = fibo_file_close(iFd_log);
  } 

  iFd_log = fibo_file_open(log_file, FS_O_RDONLY);
  ret = fibo_file_seek(iFd_log, 0, FS_SEEK_SET);
  ret = fibo_file_read(iFd_log, (u8_t *)log_head_buf, sizeof(log_head_buf));
  ret = fibo_file_fsync(iFd_log);
  ret = fibo_file_close(iFd_log);
  log_size = fibo_file_getSize(log_file);
  APP_DEBUG("\r\n-->state grid:log file read pointer = %d,write pointer = %d,lenght = %ld\r\n", \
              log_head->file_logr_pointer, log_head->file_logw_pointer, log_size);
    
  return log_size;
}

/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
static s32 log_write(log_head_t *head,  Buffer_t *buf)
{
  s32 ret=-1;

  //数列log_buf是要存在文件中的数据
  //数据结构由结构体log_data组成，包括size lenght payload[]
  //size=size占用字节数+lenght占用字数+payload[]占用字节数
  //lenght=payload[]占用字节数
  u8 log_buf[log_line_len]={0};
  buf_t *log_data=(buf_t *)log_buf;

  APP_DEBUG("\r\n-->log put:buf->lenght=%d\r\n",buf->lenght);
  //日志每笔数据长度不能超过64
  if(buf->lenght>log_line_len)
    return ret;
  APP_DEBUG("\r\n-->log:input buf value:%s\r\n",buf->payload);

  //若写指针超出，删除文件，指针归零
  log_init();

  //得到国网历史数据写入BUF:history_bf
  log_data->lenght=buf->lenght;
  r_memcpy(log_data->payload,buf->payload, buf->lenght);
  log_data->size=sizeof(log_data->size)+sizeof(log_data->lenght)+log_data->lenght;

  iFd_log = fibo_file_open(log_file, FS_O_WRONLY|FS_O_APPEND);
  if (iFd_log < 0) {
    APP_DEBUG("\r\n-->log:open log file fail!\r\n");
    return ret;
  }
  //将日志数据log_buf写入文件log file  
  ret = fibo_file_seek(iFd_log, head->file_logw_pointer * log_line_len + log_headoffset_len, FS_SEEK_SET);
  ret=fibo_file_write(iFd_log, (u8_t *)log_buf, sizeof(log_buf));
  if(ret!=sizeof(log_buf)){
    APP_DEBUG("\r\n-->log: write fail,ret=%ld\r\n",ret);
    fibo_file_fsync(iFd_log);
    fibo_file_close(iFd_log);
    return ret;
  }

  // 写指针加1并写入文件
  fibo_file_seek(iFd_log, 0, FS_SEEK_SET);
  head->file_logw_pointer = (head->file_logw_pointer + 1) % log_pointer_size;
  fibo_file_write(iFd_log,(u8_t *)log_head_buf,sizeof(log_head_buf));

  APP_DEBUG("\r\n-->log: save lenght:%d,next w_printer:%d",log_data->lenght,head->file_logw_pointer); 
  APP_DEBUG("\r\n-->log:%s\r\n",buf->payload);
  fibo_file_fsync(iFd_log);
  fibo_file_close(iFd_log);
  return ret;
}

/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
static u16 log_read(log_head_t *head,  Buffer_t *buf)
{
  s32 ret=-1;
  u16 log_size=0xffff;
  u8 log_buf[log_line_len]={0};
  buf_t *log_data=(buf_t *)log_buf;
  //确保指针在许可范围之内
  log_init();

  iFd_log = fibo_file_open(log_file, FS_O_RDONLY);
  if (iFd_log < 0) {
    APP_DEBUG("\r\n-->log:open log file fail!\r\n");
    return log_size;
  }

  //从文件log_file读取指针head->file_logr_pointer指定的数据，存入log_buf  
  ret = fibo_file_seek(iFd_log, head->file_logr_pointer * log_line_len + log_headoffset_len, FS_SEEK_SET);
  log_size=fibo_file_read(iFd_log, (u8_t *)log_buf, sizeof(log_buf));
  ret = fibo_file_fsync(iFd_log);
  ret = fibo_file_close(iFd_log);

  //得到返回数据
  log_size=log_data->lenght;
  //if(log_size==0||log_size>log_line_len||(log_data->size!=log_size+4)){
  //  APP_DEBUG("\r\n-->log: read fail,log_size=%d\r\n",log_size);
  //  return log_size;
  //}
  if(log_size==0||log_size>log_line_len||(log_data->size!=log_size+4)){
    APP_DEBUG("\r\n-->log: read fail,log_size=%d\r\n",log_size);
    //读到空数据，读指针归0,重头再读
    head->file_logr_pointer=0;
    log_size=0;
  }
  else{
    r_memcpy(buf->payload,log_data->payload,log_size);
  //print_buf(log_buf,log_data->size);
  //循环读取日志
  head->file_logr_pointer = (head->file_logr_pointer + 1)% log_pointer_size;
  }
  
  iFd_log = fibo_file_open(log_file, FS_O_WRONLY|FS_O_APPEND);
  ret = fibo_file_seek(iFd_log, 0, FS_SEEK_SET);
  fibo_file_write(iFd_log,(u8_t *)log_head_buf,sizeof(log_head_buf));
  ret = fibo_file_fsync(iFd_log);
  ret = fibo_file_close(iFd_log);

  APP_DEBUG("\r\n-->log: read lenght:%d,next r_printer:%d",log_data->lenght,head->file_logr_pointer); 
  return log_size;
}

/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
void log_save(char *str, ...) 
{
  
  s32 ret;
  Buffer_t buf;
  u16 pointer=0;                  //开始写地址
  u16 leftover=0;        //数据剩余长度
  Buffer_t write_buf;      //待写入结构体

  va_list args;

//日志读完才写文件 
  if(log_reading)
    return;

  log_writeing=1;

  // 该函数用于获取本地时间
  hal_rtc_time_t time;
  ret = fibo_getRTC(&time);
  time.year+=2000;
  if (ret < 0) {
    time.year = 2015;
    time.month = 1;
    time.day = 1;
    time.hour = 0;
    time.min = 0;
    time.sec = 0;
    time.wDay = 4;
  }

  u16_t offset = r_strlen(str);
  buf.lenght=offset;
  buf.payload=fibo_malloc(buf.lenght+32);
  //if (buf.lenght < 64 && null != (buf.payload = memory_apply(offset + 32)))
  if(buf.payload!=null){
      //Clock_t clock;
	    //Clock_timeZone(&clock);
      r_memset(buf.payload,'\0',buf.lenght);
      offset = Swap_numChar((char *)buf.payload, time.year);
      buf.payload[offset++] = '-';
      offset += Swap_numChar((char *)&buf.payload[offset], time.month);
      buf.payload[offset++] = '-';
      offset += Swap_numChar((char *)&buf.payload[offset], time.day);
      buf.payload[offset++] = ' ';
      
      offset += Swap_numChar((char *)&buf.payload[offset], time.hour);
      buf.payload[offset++] = ':';
      offset += Swap_numChar((char *)&buf.payload[offset], time.min);
      buf.payload[offset++] = ':';
      offset += Swap_numChar((char *)&buf.payload[offset], time.sec);
      buf.payload[offset++] = '>';
      buf.payload[offset++] = ' ';
      
      //offset += r_strcpy_len((char *)&buf.payload[offset], str);
      va_start(args, str);        
      sprintf((char *)&buf.payload[offset],str, args);
      va_end(args);   
      offset=r_strlen((char *)buf.payload);
	
      buf.lenght = offset + 1;

      APP_DEBUG("\r\n-->log: log input buf.payload=%s buf.lenght=%d\r\n",buf.payload,buf.lenght);

      //以上得到要写的数据放在:buf.payload
      //长度：buf.lenght
	    //FlashFIFO_put(&s_logeHead, &buf);
      pointer=0;                  //开始写地址
      leftover=buf.lenght;        //数据剩余长度
      write_buf.lenght=log_line_len;
      write_buf.payload=null;
      write_buf.payload=fibo_malloc(write_buf.lenght);

      if(write_buf.payload==null){
        APP_DEBUG("\r\n-->log:write_buf malloc fail\r\n");
      }else{
        APP_DEBUG("\r\n-->log:write_buf malloc success leftover=%d\r\n",leftover);

      while(leftover>log_line_len){
        APP_DEBUG("\r\n-->log:leftover(%d)>64\r\n",leftover);
        //得到写入结构体数据
        r_memset(write_buf.payload, 0, write_buf.lenght);
        r_memcpy(write_buf.payload,buf.payload+pointer,write_buf.lenght);
        //写入文件
        log_write(log_head,&write_buf);
        pointer+=log_line_len;    //下一组数据开始指针
        leftover-=log_line_len;   //剩余长度
      }


      APP_DEBUG("\r\n-->log:leftover(%d)<64\r\n",leftover);
      if(leftover){
        //得到写入结构体数据
        r_memset(write_buf.payload, 0, write_buf.lenght);
        write_buf.lenght=leftover;
        r_memcpy((void*)write_buf.payload,(void*)(buf.payload+pointer),(u32)write_buf.lenght);
        //写入文件
        log_write(log_head,&write_buf);
      }
      fibo_free(write_buf.payload);
      }

	    fibo_free(buf.payload);
  }
  log_writeing=0;
}

/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
u16 log_get(Buffer_t *buf) 
{
  u16 ret=0;
  
  if(buf->payload==null){
    APP_DEBUG("\r\n-->log:buf->lenght=null\r\n");
    return ret;
  }
  log_reading=1;  
  ret=log_read(log_head,buf);
  log_reading=0;
  if(ret==0xffff){
    ret=0;
  }
  buf->lenght=ret;
  return ret;
}

void log_clean(void) {
  fibo_file_delete(log_file);
  log_init();
}

