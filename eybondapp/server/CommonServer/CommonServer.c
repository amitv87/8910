/******************************************************************************           
* name:             CommonServer.c           
* introduce:        
* author:           Luee                                     
******************************************************************************/ 
#ifdef _PLATFORM_M26_
#include "ql_system.h"
#include "net.h"
#endif

#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#include "4G_net.h"
#endif

#include "eyblib_memory.h"
#include "eyblib_swap.h"
#include "eyblib_algorithm.h"
#include "eyblib_r_stdlib.h"

#include "eybpub_Debug.h"
#include "eybpub_utility.h"
#include "eybpub_Clock.h"

#include "eybapp_appTask.h"
#include "Device.h"
#include "ModbusDevice.h"
#include "Modbus.h"
#include "eybpub_run_log.h"

// #include "SAJ.h"
// #include "Hanergy.h"
#include "StateGrid.h"
#include "CommonServer.h"
#include "L610Net_SSL.h"
#include "L610Net_TCP_EYB.h"
#include "eybpub_Status.h"

net_para_t eybnet_para;
net_para_t statenet_para;

typedef int (*ServerCheck)(void);

typedef struct {
  ServerCheck check;
  const CommonServer_t *api;
} CommonServerTab_t;

static int common(void);

static CommonServerTab_t serverTab[] = {
//  {Hanergy_check, &HanergyServer},
//  {SAJ_check, &SAJ_API},
  {StateGrid_check, &StateGrid_API},
  {common, null},
};

static const CommonServerTab_t server_con={
   StateGrid_check, &StateGrid_API
};

static u8_t sPort;
static int overtime;
//static CommonServerTab_t  const *server;
static CommonServerTab_t *server;

static u16_t relinkTime = 0;
static u8 state_send_flag=0;


static void output(Buffer_t *buf);
static void dataProcess(u8_t port, Buffer_t *buf);

int common(void) {
  return 0;
}

static u8 soft_reset_flag=0;
static u16 soft_reset_counter=0;
static void soft_reset_handle(void);

/******************************************************************************                    
 * introduce:            
 * parameter:                       
 * return:                
 * author:           Luee                                              
 *****************************************************************************/
void ssl_relink(void)
{
  sslsock=-1;
  relinkTime=0;
  overtime = 0;
  sPort=0;
  CommonServer_close();
  sPort = 0xff;  
  server = null;
  SSL_init();

  APP_DEBUG("\r\n-->state grid relink!!!\r\n");
  log_save("state grid relink!!!\r\n");
}

/******************************************************************************                    
 * introduce:            
 * parameter:                       
 * return:                
 * author:           Luee                                              
 *****************************************************************************/
void soft_reset_en(void)
{
  soft_reset_flag=1;
  soft_reset_counter=10;   //after 5s,reset

}

/******************************************************************************                    
 * introduce:            
 * parameter:                       
 * return:                
 * author:           Luee                                              
 *****************************************************************************/
static void soft_reset_handle(void)
{
  if(soft_reset_flag){
    if(soft_reset_counter)
      soft_reset_counter--;
    APP_DEBUG("\r\n-->soft reset downcount:%d\r\n",soft_reset_counter);
    if(soft_reset_counter==0){
      soft_reset_flag=0;
      log_save("System Software Reset!");
      APP_DEBUG("\r\n-->System Software Reset!\r\n");
      fibo_softReset();
    }
  }
}

/******************************************************************************                    
 * introduce:            
 * parameter:                       
 * return:                
 * author:           Luee                                              
 *****************************************************************************/
void clear_overtime(void){
  overtime=0;
}

/******************************************************************************                    
 * introduce:        ssl 数据接收任务     
 * parameter:        none                 
 * return:           返回ret,=-1接收失败，否则返回数据长度       
 * author:           Luee                                              
 *****************************************************************************/
#ifndef SSLREC_M26
void sslrec_task(void *param)
{
  s32 len;
  Buffer_t recbuf;
  u16 ssl_relink_times=0;
  
  while(1){
    fibo_taskSleep(500);

    //if((state_status(sPort)==L610_SUCCESS)&&state_send_flag){
    if(state_status(sPort)==L610_SUCCESS){
      //state_send_flag=0;
      ssl_rec();
    }   
  }
  fibo_thread_delete();
}
#else

void sslrec_task(void *param)
{
  s32 len;
  Buffer_t recbuf;
  u16 ssl_relink_times=0;

  while(1){
    fibo_taskSleep(500);

    if(state_status(sPort)==L610_SUCCESS){
      recbuf.payload=fibo_malloc(100);
      r_memset(recbuf.payload, '\0', sizeof(recbuf.payload)); 
      len = fibo_ssl_sock_recv(sslsock, recbuf.payload, sizeof(recbuf.payload));
      if(len>0){
        recbuf.lenght=len;
        recbuf.size=len+4;
        state_rec_process(&recbuf);
      }
      fibo_free(recbuf.payload);

      //收到有效ssl数据，心跳间隔时间清0
      //国网只需保证20分钟内有数据正确交换就不会断网
      if(len>0){
        set_heartbeatSpace(0);
      }
      //连接失败重连
      if(len<=0){
        APP_DEBUG("\r\n-->state grid ssl recieve fail times:%d\r\n",ssl_relink_times);
        log_save("\r\n-->state grid ssl recieve fail times:%d\r\n",ssl_relink_times);
        if(ssl_relink_times++>30){
          ssl_relink_times=0;
          ssl_relink();
        }
      }
    } 
  }   //while end
}   //end
#endif
/*******************************************************************************
  * @brief
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
void proc_commonServer_task(s32_t taskId) {
  u8_t ret;
  ST_MSG msg;
  //u16_t relinkTime = 0;
  u8_t eybnet_index=0xff;
  s32 rets32;

  APP_PRINT("Common server task run!!\r\n");
  r_memset(&msg, 0, sizeof(ST_MSG));
  server = null;
  sPort = 0xFF;
  while (1) {
#ifdef _PLATFORM_M26_
    Ql_OS_GetMessage(&msg);
#endif
#ifdef _PLATFORM_L610_
    fibo_queue_get(COMMON_SERVER_TASK, (void *)&msg, 0);
#endif
    switch (msg.message) {
      case SYS_PARA_CHANGE:
        ssl_relink();
      case APP_MSG_TIMER_ID:
        //软件复位
        soft_reset_handle();

        ret=state_status(sPort);
        APP_DEBUG("\r\n-->socket[%d] status %d relinkTime %d\r\n", sPort, ret, relinkTime);
        
        if (m_GprsActState == STATE_DNS_READY&&ret == 0xFF && relinkTime++ > 50) {
        //if (ret == 0xFF && relinkTime++ > 50) {    
          if (server == null) {
            //for (ret = 0; ret < sizeof(serverTab) / sizeof(serverTab[0]); ret++) {
              ret=0;        //仅有国网
              APP_DEBUG("\r\n-->CommonServer[%d] check!!!!!\r\n", ret);
              if (serverTab[ret].check() >= 0) {
                APP_DEBUG("\r\n-->state grid is exist\r\n");
                server = &serverTab[ret];
                break;
              }
          }

          if (server != null && server->api != null) {
            // Configuration the protocol initial parameters
             APP_DEBUG("\r\n-->server != null && server->api != null\r\n"); 
            ServerAddr_t *serverAddr = server->api->getAddr();

            server->api->init();
            APP_DEBUG("\r\n-->server init %s !\r\n", server->api->name);
            if (serverAddr != null) {
              overtime = 0;
              APP_DEBUG("\r\n-->state grid:sPort=%d\r\n",sPort);
              APP_DEBUG("\r\n-->state grid netManage[sPort].flag=%d\r\n",netManage[sPort].flag);
              sPort = Net_connect(serverAddr->type, serverAddr->addr, serverAddr->port, dataProcess);
              memory_release(serverAddr);
              
            }
            
          } else {
              APP_DEBUG("\r\n-->server->api == null?\r\n");
            break;
            
          }

        }
        else{
            //if (overtime++ > server->api->waitTime){
            if (overtime++ > 660){
                overtime = 20;
						    //Net_close(sPort);
                ssl_relink();
                log_save("state grid relink over and relink\r\n");
					  }
        }
        if (server != null && server->api != null){
              server->api->run(ret);
        }

        case COMMON_SERVER__DATA_PROCESS:
        #ifdef SSLREC_M26
          if (server != null && server->api != null){
              server->api->process();
            }
        #endif
        break;
    }
  }
}

/******************************************************************************                    
 * introduce:           
 * parameter:                      
 * return:                
 * author:           Luee                                              
 *****************************************************************************/
static void dataProcess(u8_t port, Buffer_t *buf) {
  output(buf);
	overtime = 0;
	sPort = port;
	if (server->api->add(buf, CommonServerDataSend) == 0)
    {
        Eybpub_UT_SendMessage(COMMON_SERVER_TASK, COMMON_SERVER__DATA_PROCESS, 0, 0);
    }
    memory_release(buf);
}

/******************************************************************************                    
 * introduce:           
 * parameter:                      
 * return:                
 * author:           Luee                                              
 *****************************************************************************/
void state_rec_process(Buffer_t *buf)
{
  dataProcess(sPort,buf);

}

/******************************************************************************                     
* introduce:        向服务器发送数据通用接口       
* parameter:        none                 
* return:           none         
* author:           Luee                                              
*******************************************************************************/
void CommonServerDataSend(Buffer_t *buf)
{
	if (buf != null && buf->payload != null && buf->lenght > 0)
	{
        s32 ret;

        s32 len;
        Buffer_t recbuf;
        u16 ssl_relink_times=0;
        //overtime=0;
        //网络空闲才发送
       while(eybnet_para.send_status)
            fibo_taskSleep(20);   
        statenet_para.send_status=true;     //标志国网正在发送
        APP_DEBUG("\r\nssl sending\r\n");
        ret = fibo_ssl_sock_send(sslsock, (u8 *)buf->payload, buf->lenght);
        if(ret<0){
          APP_DEBUG("\r\n-->state grid ssl send fail ret=%ld\r\n", ret);
        }else{
          state_send_flag=1;
          //APP_DEBUG("\r\n-->state grid ssl send success ret=%ld\r\n", ret);
          //log_hex((UINT8 *)buf->payload, buf->lenght);
          //print_buf((UINT8 *)buf->payload, buf->lenght);
        }
        statenet_para.send_status=false;    //发送结束
	}
} 

/******************************************************************************                    
 * introduce:           
 * parameter:                      
 * return:                
 * author:           Luee                                              
 *****************************************************************************/
static void output(Buffer_t *buf) {
  APP_DEBUG("%s len: %d\r\n", server->api->name,  buf->lenght);
  if (buf->lenght < 0x60) {
    u8_t *str = memory_apply(buf->lenght * 3 + 8);
    if (str != null) {
      int l = Swap_hexChar((char *)str, buf->payload, buf->lenght, ' ');
      Debug_output(str, l);
      APP_DEBUG("\r\n");
      memory_release(str);
    }
  }
}

/******************************************************************************                    
 * introduce:           
 * parameter:                      
 * return:                
 * author:           Luee                                              
 *****************************************************************************/
void CommonServer_DeviceInit(void) {
  if (server != null && server->api != null && server->api->deviceInit != null) {
    server->api->deviceInit();
  }
}

/******************************************************************************                    
 * introduce:           
 * parameter:                      
 * return:                
 * author:           Luee                                              
 *****************************************************************************/
void CommonServer_close(void) {
  //Net_close(sPort);
  state_close(sPort);
  if (server != null && server->api != null) {
    server->api->sclose();
  }
}

/******************************************************************************                    
 * introduce:           
 * parameter:                      
 * return:                
 * author:           Luee                                              
 *****************************************************************************/
void CommonServerData_clean(void) {
  int i;

  for (i = 0; i < sizeof(serverTab) / sizeof(serverTab[0]); i++) {
    if (serverTab[i].api != null && serverTab[i].api->clean != null) {
      serverTab[i].api->clean();
    }
  }
}
/******************************************************************************/

