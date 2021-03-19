/******************************************************************************           
* name:             grid_tool.c           
* introduce:        tools 
* author:           Luee                                     
******************************************************************************/ 
#include "fibo_opencpu.h"
#include "eyblib_memory.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_swap.h"

#include "eybpub_utility.h"
#include "eybpub_Debug.h"
#include "eybpub_run_log.h"
#include "eybpub_SysPara_File.h"
#include "eybpub_data_collector_parameter_table.h"

#include "grid_tool.h"


/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
void print_buf(u8_t *buf,u16_t lenght) {
  u32_t displayNum = 0;
  APP_DEBUG("\r\n-->buf lenght: %d value: \r\n", lenght);
  //if (lenght < MAX_NET_BUFFER_LEN) {
  if (lenght <= 1024) {
    u8_t *str = memory_apply(lenght * 3 + 8);
    r_memset(str, 0, lenght * 3 + 8);
    if (str != null) {
      hextostr(buf, str, lenght);   
      int l = r_strlen((char *)str);
      while (l) {
        if (l >= 16 * 3) {
          Debug_output(str + displayNum, 16 * 3);
          l -= 16 * 3;
          displayNum += 16 * 3;
        } else {
          Debug_output(str + displayNum, l);
          l = 0;
        }
        Debug_output((u8_t *)"\r\n", 2);
      }
      APP_DEBUG("\r\n");
      memory_release(str);
    }
  }
}

/******************************************************************************                   
* introduce:        get para        
* parameter:        number                 
* return:           Buffer_t *buf         
* author:           Luee                                              
*******************************************************************************/

void SysPara_Get(u8 number, Buffer_t *databuf)
{
    char *buf6=NULL;
    u16 len=64;
    for (int j = 0; j < number_of_array_elements; j++){
        if(number == PDT[j].num){
            buf6 = memory_apply(sizeof(char)*64);
            memset(buf6, 0, sizeof(char)*64);
            PDT[j].rFunc(&PDT[j],buf6, &len);
        }
    }
    databuf->lenght=r_strlen(buf6);
    databuf->size=databuf->lenght;
    databuf->payload=fibo_malloc(sizeof(char)*64);      //需在外部释放内存
    r_memcpy(databuf->payload,buf6,sizeof(char)*64);
    fibo_free(buf6);
    APP_DEBUG("\r\n-->get para %d=%s lenght=%d\r\n",number,databuf->payload,databuf->lenght);
}

/******************************************************************************                   
* introduce:        set para        
* parameter:        number                 
* return:           Buffer_t *buf         
* author:           Luee                                              
*******************************************************************************/

int SysPara_Set(u8 number, Buffer_t *buf)
{
    s32 ret=-1;
    u16 len=64;
    u8 buffer[64]={0};

    r_memcpy(buffer,buf->payload,sizeof(char)*64);
    for (int j = 0; j <number_of_array_elements; j++){
	    if(number == PDT[j].num){
        memset((&PDT[j])->a, 0, sizeof((&PDT[j])->a));
        PDT[j].wFunc(&PDT[j],buffer, &len);
        log_d("\r\nwrite para=%s\r\n",buffer);
        ret=0;
	    }
	}
    return ret;
}
