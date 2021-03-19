/**********************    COPYRIGHT 2014-2100,  Eybond ************************
 * @File    : L610Net_TCP_EYB.c
 * @Author  : MWY
 * @Date    : 2020-11-20
 * @Brief   :
 ******************************************************************************/
#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"

#include "eyblib_swap.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_memory.h"

#include "eybpub_Debug.h"
#include "eybpub_SysPara_File.h"
#include "eybpub_run_log.h"
#include "eybpub_Status.h"
#include "eybpub_utility.h"
#include "eybpub_parameter_number_and_value.h"

#include "L610Net_TCP_EYB.h"
#include "eybond.h"


// INT8  g_socketid = -1;

#include "oc_uart.h"
#include "DeviceIO.h"
#include "grid_tool.h"
#include "L610Net_SSL.h"
#include "CommonServer.h"

#include "grid_tool.h"


#define r_in_range(c, lo, up) ((u8_t)c >= lo && (u8_t)c <= up)
#define r_isdigit(c) r_in_range(c, '0', '9')
#define r_isxdigit(c) (r_isdigit(c) || r_in_range(c, 'a', 'f') || r_in_range(c, 'A', 'F'))
#define r_islower(c) r_in_range(c, 'a', 'z')
#define r_isspace(c) (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')

static char testIP[64] = {0};
//static u16_t testPort = 0;
u16_t testPort = 0;
static char produc_flag = 0;
extern char produc_save_flag;


int ip4addr_aton(const char *cp, ip4_addr_t *addr)
{
    u32_t val;
    u8_t base;
    char c;
    u32_t parts[4];
    u32_t *pp = parts;

    c = *cp;
    for (;;)
    {
        /*
     * Collect number up to ``.''.
     * Values are specified as for C:
     * 0x=hex, 0=octal, 1-9=decimal.
     */
        if (!r_isdigit(c))
        {
            return 0;
        }
        val = 0;
        base = 10;
        if (c == '0')
        {
            c = *++cp;
            if (c == 'x' || c == 'X')
            {
                base = 16;
                c = *++cp;
            }
            else
            {
                base = 8;
            }
        }
        for (;;)
        {
            if (r_isdigit(c))
            {
                val = (val * base) + (u32_t)(c - '0');
                c = *++cp;
            }
            else if (base == 16 && r_isxdigit(c))
            {
                val = (val << 4) | (u32_t)(c + 10 - (r_islower(c) ? 'a' : 'A'));
                c = *++cp;
            }
            else
            {
                break;
            }
        }
        if (c == '.')
        {
            /*
       * Internet format:
       *  a.b.c.d
       *  a.b.c   (with c treated as 16 bits)
       *  a.b (with b treated as 24 bits)
       */
            if (pp >= parts + 3)
            {
                return 0;
            }
            *pp++ = val;
            c = *++cp;
        }
        else
        {
            break;
        }
    }
    /*
   * Check for trailing characters.
   */
    if (c != '\0' && !r_isspace(c))
    {
        return 0;
    }
    /*
   * Concoct the address according to
   * the number of parts specified.
   */
    switch (pp - parts + 1)
    {

    case 0:
        return 0; /* initial nondigit */

    case 1: /* a -- 32 bits */
        break;

    case 2: /* a.b -- 8.24 bits */
        if (val > 0xffffffUL)
        {
            return 0;
        }
        if (parts[0] > 0xff)
        {
            return 0;
        }
        val |= parts[0] << 24;
        break;

    case 3: /* a.b.c -- 8.8.16 bits */
        if (val > 0xffff)
        {
            return 0;
        }
        if ((parts[0] > 0xff) || (parts[1] > 0xff))
        {
            return 0;
        }
        val |= (parts[0] << 24) | (parts[1] << 16);
        break;

    case 4: /* a.b.c.d -- 8.8.8.8 bits */
        if (val > 0xff)
        {
            return 0;
        }
        if ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff))
        {
            return 0;
        }
        val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
        break;
    default:
        break;
    }
    if (addr)
    {
        ip4_addr_set_u32(addr, val);
    }
    return 1;
}

#define SIM_REGISTER_TIMES 60
GSMState_e m_GprsActState = STATE_TOTAL_NUM;
#define NET_PING_HOSTNAME    "www.baidu.com"
//#define NET_PING_HOSTNAME  "114.114.114.114"

//#define EYB_SOCKET_COUNTS 6
L610Net_t netManage[EYB_SOCKET_COUNTS]={0};

static s32_t L610pdpCntxtId;
static u8_t registe = 0;
static u16_t registe_times = 0;
static s32_t customPara = 0;

// fd_set g_readfds;
// fd_set g_errorfds;
// fd_set g_writefds;
u32_t g_SemFlag = 0;
static u32_t g_netmutex = 0;
static u8 ssl_neting=0;     //国网结束后，才能重新调用

/************************************************************************/
/* Definition for GPRS PDP context                                      */
/************************************************************************/
static ST_GprsConfig m_GprsConfig = {
  "",    // APN name
  "",    // User name for APN
  "",    // Password for APN
  0,
  NULL,
  NULL,
};

static void socketClose(s32_t socketid);
void L610_TCP_Callback(u8_t *param);

/*******************************************************************************
 Brief    : L610Net para init
 Parameter:
 return   :
*******************************************************************************/
void L610Net_init(void) {
  Buffer_t buf;
  parametr_get(GPRS_APN_ADDR, &buf);

  if (buf.lenght > 2 && buf.payload != null) {
    r_strcpy((char *)m_GprsConfig.apnName, (char *)buf.payload);
  }
  memory_release(buf.payload);

  APP_DEBUG("APN:%s\r\n", (char *)m_GprsConfig.apnName);  

  m_GprsActState = STATE_GSM_QUERY_STATE;
  L610pdpCntxtId = 0;
  registe = 0;
  registe_times = 0;

  NetLED_Off();
  GSMLED_Off();
  
  r_memset(netManage, 0, sizeof(netManage));    //Luee
  
  g_netmutex = 0;

  INT8 strTaskname[32] = {0};
  UINT32 l610tcp_thread_id[EYB_SOCKET_COUNTS] = {0};
  u16_t nIndex = 0;
  for(nIndex = 0; nIndex < EYB_SOCKET_COUNTS; nIndex ++) {
    r_memset(strTaskname, '\0', sizeof(strTaskname));
    snprintf(strTaskname, 32, "L610 TCP rev Callback %d", nIndex);
    fibo_thread_create_ex(L610_TCP_Callback, (INT8 *)strTaskname, 1024*8*3, (void *)&nIndex, OSI_PRIORITY_NORMAL, &l610tcp_thread_id[nIndex]);
    fibo_taskSleep(1000);
    APP_DEBUG("L610 TCP Callback[%d] %X\r\n", nIndex, l610tcp_thread_id[nIndex]);
  }  
/*    UINT32 l610tcp_thread_id = 0;
    fibo_thread_create_ex(L610_TCP_Callback, "L610 TCP Callback", 1024*8*8, NULL, OSI_PRIORITY_NORMAL, &l610tcp_thread_id);
    fibo_taskSleep(1000);
    APP_PRINT("L610 TCP Callback %X\r\n", l610tcp_thread_id); */
}

void L610Net_init2(void) {
  Buffer_t buf;
  parametr_get(GPRS_APN_ADDR, &buf);

  if (buf.lenght > 2 && buf.payload != null) {
    r_strcpy((char *)m_GprsConfig.apnName, (char *)buf.payload);
  }
  memory_release(buf.payload);

  APP_DEBUG("APN:%s\r\n", (char *)m_GprsConfig.apnName);  

  m_GprsActState = STATE_GSM_QUERY_STATE;
  L610pdpCntxtId = 0;
  registe = 0;
  registe_times = 0;

  NetLED_Off();
  GSMLED_Off();
  
  r_memset(netManage, 0, sizeof(netManage));    //Luee
  
  g_netmutex = 0;
}


/*******************************************************************************
 Brief    : L610Net_open
 Parameter:
 return   : connect number 0~ 5; oxFF: full no space
*******************************************************************************/
u8_t L610Net_open(u8_t mode, char *ip, u16_t port, NetDataCallback netCallback) {
  s32 i = 0;

  for (i = 0; i < sizeof(netManage) / sizeof(netManage[0]); i++) {
    if (netManage[i].flag == 1
        && netManage[i].port == port
        && 0 == r_strncmp(ip, netManage[i].ipStr, r_strlen(netManage[i].ipStr))) {
          if(netManage[i].mode==2)
            break;

          netManage[i].flag = 1;
          netManage[i].mode = mode;
          netManage[i].callback = netCallback;
      /*
      APP_DEBUG("\r\n-->netManage[%ld] ID:%ld status:%d flag:%d mode:%d ipStr:%s ip:%lx port:%d \r\n", \
        i, netManage[i].socketID, netManage[i].status, netManage[i].flag, netManage[i].mode, \
        netManage[i].ipStr, netManage[i].ip, netManage[i].port);
      */
      break;
    } else if (netManage[i].flag == 0) {
      
      netManage[i].flag = 1;
      netManage[i].mode = mode;
      netManage[i].port = port;
      netManage[i].callback = netCallback;
      netManage[i].status = L610_WAIT;
      netManage[i].socketID = -1;
      netManage[i].ip = 0;  
      r_strcpy(netManage[i].ipStr, ip);
      
      APP_DEBUG("\r\n-->state grid netManage[%ld] ID:%ld status:%d flag:%d mode:%d ipStr:%s port:%d \r\n", \
        i, netManage[i].socketID, netManage[i].status, netManage[i].flag, netManage[i].mode, \
        netManage[i].ipStr, netManage[i].port);
      
      break;
    }
  }
  
  return ((i >= sizeof(netManage) / sizeof(netManage[0])) ? 0xFF : i);
}

/*******************************************************************************
 Brief    : L610Net_close
 Parameter:
 return   :
*******************************************************************************/
u8_t L610Net_status(u8_t nIndex) {
  u8_t ret = 0xff;

  if (nIndex < sizeof(netManage) / sizeof(netManage[0])) {
    if (netManage[nIndex].flag == 1) {
      ret = netManage[nIndex].status;
      if (ret == L610_SUCCESS) {
        NetLED_On();
        GSMLED_On();      //硬件网络连接成功
      } else {
        NetLED_Off();
      }
    }
  }

  return ret;
}

u8_t state_status(u8_t nIndex) {
  u8_t ret = 0xff;

  if (nIndex < sizeof(netManage) / sizeof(netManage[0])) {
    if (netManage[nIndex].flag == 1) {
      ret = netManage[nIndex].status;
      if (ret == L610_SUCCESS) {
        NetLED_On();      //网络有数据，服务器连接成功
        GSMLED_On();      //硬件网络连接成功
      } else {
        NetLED_Off();
      }
    }
  }

  return ret;
}

/*******************************************************************************
 Brief    : 
 Parameter:
 return   :
*******************************************************************************/
void state_close(u8_t nIndex) 
{
  if (nIndex < sizeof(netManage) / sizeof(netManage[0])) {
    if (netManage[nIndex].flag != 0) {
      APP_DEBUG("netManage[%d] sockitID(%ld) Close\r\n", nIndex, netManage[nIndex].socketID);
      if(netManage[nIndex].mode==2){
        NetLED_Off();
        netManage[nIndex].flag = 0;
        netManage[nIndex].socketID = -1;
        netManage[nIndex].ip = 0;
        netManage[nIndex].status = L610_CLOSE;
      }
    } 
    nIndex++; 
  }
}

/*******************************************************************************
 Brief    : get eybnet index
 Parameter:
 return   :
*******************************************************************************/
/*
u8_t get_eybnet_index(void) {
  u16_t ret = 0xff;
  u8_t nIndex = 0;
  Buffer_t *buf;

  SysPara_Get(21,buf);

  while (nIndex < sizeof(netManage) / sizeof(netManage[0])) {
    if(netManage[nIndex].ipStr==buf->payload){
      memory_release(buf->payload);
      return nIndex;
    }
    nIndex++;
  }
  memory_release(buf->payload);
  return ret;
}
*/

/*******************************************************************************
 Brief    : L610Net_close
 Parameter:
 return   :
*******************************************************************************/
void L610Net_closeAll(void) {
  int i = 0;

  for (i = 0; i < sizeof(netManage) / sizeof(netManage[0]); i++) {
    L610Net_close(i);
  }
}

/*******************************************************************************
 Brief    : L610Net_close
 Parameter:
 return   :
*******************************************************************************/
void L610Net_close(u8_t nIndex) {
  if (nIndex < sizeof(netManage) / sizeof(netManage[0])) {
    if (netManage[nIndex].flag != 0) {
      APP_DEBUG("netManage[%d] sockitID(%ld) Close\r\n", nIndex, netManage[nIndex].socketID);
      if(netManage[nIndex].mode==2){
      }
      else{
        fibo_sock_close(netManage[nIndex].socketID);
        NetLED_Off();
        netManage[nIndex].flag = 0;
        netManage[nIndex].socketID = -1;
        netManage[nIndex].ip = 0;
        netManage[nIndex].status = L610_CLOSE; 
      }   
    }  
  }
}

/*******************************************************************************
 Brief    : L610Net_close
 Parameter:
 return   :
*******************************************************************************/
void L610Net_ready(void) {
  s32_t ret;

//  registe = 1;
  L610pdpCntxtId = 0;
}

int findEybondIP(int nIndex) {
  if (0 == r_strncmp(netManage[nIndex].ipStr, "www.shinemonitor.com", r_strlen("www.shinemonitor.com"))) {
    return 1;
  }

  return 0;
}

/*******************************************************************************
 Brief    : L610Net_close
 Parameter:
 return   :
*******************************************************************************/
void L610Net_manage(void) {
  APP_DEBUG("L610Net_manage is running :%ld\r\n", g_netmutex);
  if (g_netmutex == 1) { // lock L610Net_manage, make sure it is running one by one    
    return;
  }
  g_netmutex = 1;
  static s16_t ping_ret = -2;

  s32_t ret = 0;
  ip_addr_t  addr_para;
  GAPP_TCPIP_ADDR_T addr;
  u8_t ip[50];
  u8_t cid_status = 0;

  switch(m_GprsActState) {
    case STATE_GSM_QUERY_STATE: {   // 查询SIM卡插入状态
      s32_t simret = 0;
      u8_t  simstatus = 0;
      simret = fibo_get_sim_status(&simstatus);
      if ((simstatus == 1) && (simret == 0)) {
        // SIM卡已插入
        registe_times = 0;
        ret = fibo_set_prior_RAT(0, SINGLE_SIM);
        if (ret < 0) {
          APP_DEBUG("set sim RAT to LTE mode fail:%ld\r\n", ret);
        }
        ret = fibo_get_curr_prior_RAT(SINGLE_SIM);
        APP_DEBUG("sim RAT mode after SIM inserted:%ld\r\n", ret);
        log_save("SIM inserted!!!");
        Eybpub_UT_SendMessage(EYBNET_TASK, NET_MSG_SIM_INSERTED, 0, 0);
        m_GprsActState = STATE_SIM_INSERTED;
      } else {
        registe_times++;
        APP_DEBUG("sim is not checked, please insert sim & retry%d\r\n", registe_times);
        if (registe_times >= SIM_REGISTER_TIMES) {
          registe_times = 0;
          m_GprsActState = STATE_SIM_NOT_INSERTED;
          log_save("SIM no insert!!!");
        }
      }
      break;
    }
    case STATE_SIM_INSERTED: {  // 查询SIM卡注网状态
      reg_info_t sim_reg_info;
      r_memset(&sim_reg_info,0,sizeof(sim_reg_info));
      ret = fibo_getRegInfo(&sim_reg_info, SINGLE_SIM);  // 注册频段
//      APP_DEBUG("sim getRegInfo ret:%d, reg_state = %d, curr_rat=%d\r\n", ret, sim_reg_info.nStatus ,sim_reg_info.curr_rat);
	  if(1 == sim_reg_info.nStatus || 5 == sim_reg_info.nStatus) { // SIM卡已注册
        APP_DEBUG("sim regitster success\r\n");
        log_save("sim register success!!!");
        if(sim_reg_info.curr_rat == 4 || sim_reg_info.curr_rat ==7) {
          APP_DEBUG("sim regitster, LTE tac = %ld ,cell id = %ld\r\n", sim_reg_info.lte_scell_info.tac, sim_reg_info.lte_scell_info.cell_id);
        } else {
          APP_DEBUG("sim regitster, gsm lac = %ld,cell id = %ld\r\n", sim_reg_info.gsm_scell_info.lac, sim_reg_info.gsm_scell_info.cell_id);
        }
        ret = fibo_get_curr_prior_RAT(SINGLE_SIM);
        APP_DEBUG("sim RAT mode after register :%ld\r\n", ret);
        r_memset(ip, 0, sizeof(ip));
        ret = fibo_PDPActive(1, (UINT8*)m_GprsConfig.apnName, NULL, NULL, 0, SINGLE_SIM, ip);        
        APP_DEBUG("active APN(%s) ret = %ld,ip=%s\r\n", m_GprsConfig.apnName, ret, ip);
        if (ret == 0) {
          Eybpub_UT_SendMessage(EYBNET_TASK, NET_MSG_SIM_READY, 0, 0);
          m_GprsActState = STATE_SIM_READY;
          registe_times = 0;
          log_save("pdp success & sim ready!!!");
        } else {
          APP_DEBUG("sim PDPActive APN %s fail\r\n", m_GprsConfig.apnName);
          log_save("sim PDPActive APN %s fail", m_GprsConfig.apnName);
          registe_times++;
          if (registe_times >= SIM_REGISTER_TIMES) { // 确认sim注册失败
            registe_times = 0;
            m_GprsActState = STATE_SIM_NOT_READY;
            log_save("sim pdp fail & sim no ready!!!");
          }
        }
      } else {
        APP_DEBUG("sim register processing %d\r\n", registe_times);
        registe_times++;
        if (registe_times >= SIM_REGISTER_TIMES) { // 确认sim注册失败
          registe_times = 0;
          m_GprsActState = STATE_SIM_NOT_READY;
          log_save("sim regitser fail!!!");
        }
      }
      break;
    }
    case STATE_SIM_NOT_INSERTED:
      Eybpub_UT_SendMessage(EYBNET_TASK, NET_MSG_SIM_NOT_INSERTED, 0, 0);
      m_GprsActState = STATE_GSM_QUERY_STATE;
      break;
    case STATE_SIM_READY: { // 确认注网后获得IP
      r_memset(&ip, 0, sizeof(ip));
      if (0 == fibo_PDPStatus(1, ip, &cid_status, SINGLE_SIM)) {
         APP_DEBUG("sim ip = %s,cid_status=%d\r\n", ip, cid_status);
         ret = fibo_get_curr_prior_RAT(SINGLE_SIM);
         APP_DEBUG("sim RAT mode after PDP active:%ld\r\n", ret);
         if (cid_status == 1 && r_strlen((char *)ip) != 0) {
           Eybpub_UT_SendMessage(EYBNET_TASK, NET_MSG_GSM_READY, 0, 0);
           m_GprsActState = STATE_GSM_READY;
           registe_times = 0;
         } else {           
           registe_times++;
           if (registe_times >= SIM_REGISTER_TIMES) {
             APP_DEBUG("sim try %d times to get IP fail\r\n", registe_times);
             registe_times = 0;             
             m_GprsActState = STATE_GSM_NOT_READY;
             log_save("realtime check sim no ready!!!");
           }
         }
      } else {
        registe_times++;
        if (registe_times >= SIM_REGISTER_TIMES) {
          APP_DEBUG("can't get sim PDP status and try fibo_PDPStatus %d times\r\n", registe_times);
          registe_times = 0;
          m_GprsActState = STATE_GSM_NOT_READY;
          log_save("realtime check sim no ready!!!");
        }
      }
      break;
    }
    case STATE_SIM_NOT_READY:
      Eybpub_UT_SendMessage(EYBNET_TASK, NET_MSG_SIM_FAIL, 0, 0);
      m_GprsActState = STATE_GSM_QUERY_STATE;
      break;
    case STATE_GSM_READY: {
      ret = fibo_getHostByName(EYBOND_DEFAULT_SERVER, &addr_para, 1, SINGLE_SIM);  // 0成功 小于0失败
      if (ret == 0) {
        registe_times = 0;
        APP_DEBUG("%s DNS IP is:%ld:%ld:%ld:%ld\r\n", EYBOND_DEFAULT_SERVER, (addr_para.u_addr.ip4.addr >> 0) & 0x000000FF, (addr_para.u_addr.ip4.addr >> 8) & 0x000000FF, (addr_para.u_addr.ip4.addr >> 16) & 0x000000FF, (addr_para.u_addr.ip4.addr >> 24) & 0x000000FF);
        APP_DEBUG("Try PING %s last ret = %d\r\n", NET_PING_HOSTNAME, ping_ret);
        ping_ret = fibo_mping(1, NET_PING_HOSTNAME, 4, 32, 64, 0, 4000);
        if (ping_ret == 0) {
//        fibo_mping(0, 0, 0, 0, 0, 0, 0);
          APP_DEBUG("PING %s OK ret = %d\r\n", NET_PING_HOSTNAME, ping_ret);
          Eybpub_UT_SendMessage(EYBNET_TASK, NET_MSG_DNS_READY, 0, 0);
          fibo_sem_signal(g_SemFlag);
          registe = 1;
          m_GprsActState = STATE_DNS_READY;
        } else {
          log_save("PING %s Fail ret = %d", NET_PING_HOSTNAME, ping_ret);
          registe = 0;
          m_GprsActState = STATE_DNS_NOT_READY;
        }
      } else {
        registe_times++;
        if (registe_times >= 6) {
          log_save("GSM DNS Fail");
          registe_times = 0;
          m_GprsActState = STATE_DNS_NOT_READY;
          registe = 0;
        }
//        log_save("GSM DNS Fail\r\n");
//        m_GprsActState = STATE_DNS_NOT_READY;
//        registe = 0;
      }
      break;
    }
    case STATE_GSM_NOT_READY:
      Eybpub_UT_SendMessage(EYBNET_TASK, NET_MSG_GSM_FAIL, 0, 0);
      m_GprsActState = STATE_GSM_QUERY_STATE;
      break;
    case STATE_DNS_READY: {
      //DNS ready 才执行
      if (registe == 1) {
        u8_t nIndex = 0;
        for (nIndex = 0; nIndex < sizeof(netManage) / sizeof(netManage[0]); nIndex++) {
          //国网不在此处处理
          if (netManage[nIndex].mode != 2 && netManage[nIndex].flag == 1 && netManage[nIndex].status != L610_SUCCESS && netManage[nIndex].status != L610_CONNECTING) {
            if (netManage[nIndex].socketID < 0) {
              //Luee mode=2为ssl国网专用，在国网处理
              //if (netManage[nIndex].mode == 1 || netManage[nIndex].mode == 2) {
                if (netManage[nIndex].mode == 1) {          
                  netManage[nIndex].socketID = fibo_sock_create(GAPP_IPPROTO_TCP);
              } else {
                  netManage[nIndex].socketID = fibo_sock_create(GAPP_IPPROTO_UDP);
              }
              fibo_taskSleep((UINT32)1000);
              //socket 建立失败
              if (netManage[nIndex].socketID < 0) {
  		        netManage[nIndex].status = L610_SOCKET_FAIL;
                APP_DEBUG("Fail to create socket, ret = %ld\r\n", netManage[nIndex].socketID);
//              Eybpub_UT_SendMessage(EYBNET_TASK, NET_CMD_RESTART_ID, 0, 1);   // mike需要修改逻辑?
                //退出前，需g_netmutex = 0;
                g_netmutex = 0;
		        return;
              }

              APP_DEBUG("nIndex %d socketID %ld mode %d\r\n", nIndex, netManage[nIndex].socketID, netManage[nIndex].mode);
              
              int NODELAY = 1;
              ret = fibo_sock_setOpt(netManage[nIndex].socketID, IPPROTO_TCP, TCP_NODELAY, &NODELAY, sizeof(NODELAY));
              if (ret < 0) {
                APP_DEBUG("fibo_sock_setOpt %d socket %ld opt fail\r\n", nIndex, netManage[nIndex].socketID);
              }
              fibo_taskSleep((UINT32)1000);
              int opt = 1;
              ret = fibo_sock_setOpt(netManage[nIndex].socketID, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(int));
              if (ret < 0) {
                APP_DEBUG("fibo_sock_setOpt %d socket %ld opt fail\r\n", nIndex, netManage[nIndex].socketID);
              }
              fibo_taskSleep((UINT32)1000);

//            INT32 socket_block = fibo_sock_lwip_fcntl(netManage[nIndex].socketID, F_GETFL,0);
	          // default block
//		      APP_DEBUG("socket_block = %x,O_NONBLOCK=%x\n", socket_block, O_NONBLOCK);
			  // set non block
//			  fibo_sock_lwip_fcntl(netManage[nIndex].socketID, F_SETFL, fibo_sock_lwip_fcntl(netManage[nIndex].socketID, F_GETFL, 0) | O_NONBLOCK);
//			  APP_DEBUG("socket_block_status = %x,O_NONBLOCK=%x\n", fibo_sock_lwip_fcntl(netManage[nIndex].socketID, F_GETFL, 0), O_NONBLOCK);
			  // set to block
//			  fibo_sock_lwip_fcntl(socketid, F_SETFL, fibo_sock_lwip_fcntl(socketid, F_GETFL, 0) & ~O_NONBLOCK );
              
              // DNS做判断
          //得到IP
		      if (netManage[nIndex].ip == 0 && netManage[nIndex].status != L610_DNS_FAIL) {
                ret = fibo_getHostByName(netManage[nIndex].ipStr, &addr_para, 1, SINGLE_SIM);  // 0成功 小于0失败
                if (ret < 0) {
                  netManage[nIndex].status = L610_DNS_FAIL;
                  fibo_sock_close(netManage[nIndex].socketID);
                  netManage[nIndex].socketID = -1;
                } else {
                  netManage[nIndex].ip = addr_para.u_addr.ip4.addr;
                  APP_DEBUG("nIndex %d %s DNS IP is:%ld:%ld:%ld:%ld\r\n", nIndex, netManage[nIndex].ipStr, (addr_para.u_addr.ip4.addr >> 0) & 0x000000FF, (addr_para.u_addr.ip4.addr >> 8) & 0x000000FF, (addr_para.u_addr.ip4.addr >> 16) & 0x000000FF, (addr_para.u_addr.ip4.addr >> 24) & 0x000000FF);
                  netManage[nIndex].status = L610_IP_OK;
                }
              }
              //TCP连接
              if (netManage[nIndex].status == L610_IP_OK) {
                if (netManage[nIndex].mode == 1) {
                  r_memset(&addr, 0, sizeof(GAPP_TCPIP_ADDR_T));
                  addr.sin_port                 = r_htons(netManage[nIndex].port);
                  addr.sin_addr.u_addr.ip4.addr = netManage[nIndex].ip;
                  addr.sin_addr.type            = AF_INET;
//                addr.sin_port = r_htons(502);
//                ip4addr_aton("39.108.19.162", &addr.sin_addr.u_addr.ip4);
//                addr.sin_addr.u_addr.ip4.addr = r_htonl(addr.sin_addr.u_addr.ip4.addr);
                  ret = fibo_sock_connect(netManage[nIndex].socketID, &addr);
//                  fibo_taskSleep((UINT32)5000);
                  APP_DEBUG("connet %s server:IP %ld:%ld:%ld:%ld port %d(sin_port %d) ret = %ld\r\n", \
                      netManage[nIndex].ipStr, \
                      (addr.sin_addr.u_addr.ip4.addr>> 0) & 0x000000FF, \
                      (addr.sin_addr.u_addr.ip4.addr >> 8) & 0x000000FF, \
                      (addr.sin_addr.u_addr.ip4.addr >> 16) & 0x000000FF, \
                      (addr.sin_addr.u_addr.ip4.addr >> 24) & 0x000000FF, \
                      netManage[nIndex].port, addr.sin_port, ret);
                  if (ret == 0) {
                    //TCP连接成功
                    netManage[nIndex].status = L610_SUCCESS;
//                  FD_SET(netManage[nIndex].socketID, &g_readfds);
//                	FD_SET(netManage[nIndex].socketID, &g_errorfds);
//                  FD_SET(netManage[nIndex].socketID, &g_writefds);
                  } else {
                    //TCP连接失败
                    netManage[nIndex].status = L610_CONNECT_FAIL;
                    //fibo_sock_close(netManage[nIndex].socketID);
                    log_save("Fail to connect to %s server, port %d ret= %ld", netManage[nIndex].ipStr, netManage[nIndex].port, ret);
                    //L610Net_close(nIndex);
                    netManage[nIndex].status = L610_DNS_FAIL;
                    fibo_sock_close(netManage[nIndex].socketID);
                    netManage[nIndex].socketID = -1;
                  }
                }
              }
            }
          } else if (netManage[nIndex].flag == 1 && netManage[nIndex].status == L610_SUCCESS) {
//          APP_DEBUG("nIndex %d socketID %d is connected\r\n", nIndex, netManage[nIndex].socketID);
          } else {
//          APP_DEBUG("nIndex %d socketID %ld is not ready\r\n", nIndex, netManage[nIndex].socketID);
          }
        }
      }       //if (registe == 1) end
      //实时每30秒检测PDP
      if (registe_times == 0) {
        r_memset(&ip, 0, sizeof(ip));
        if (0 == fibo_PDPStatus(1, ip, &cid_status, SINGLE_SIM)) {
          APP_DEBUG("sim ip = %s, cid_status=%d\r\n", ip, cid_status);
          ret = fibo_get_curr_prior_RAT(SINGLE_SIM);
          APP_DEBUG("sim RAT mode when DNS is ready:%ld\r\n", ret);
          registe_times++;
        } else {
          log_save("get sim ip fail, cid_status=%d", cid_status);
          m_GprsActState = STATE_DNS_NOT_READY;
          registe_times = 0;
        }
      } else {          
        if (registe_times >= SIM_REGISTER_TIMES) {
          registe_times = 0;
        } else {
          registe_times++;
        }
      }
      break;
    }
    case STATE_DNS_NOT_READY: {
      L610Net_closeAll();
      ret = fibo_PDPRelease(0, SINGLE_SIM);
      APP_DEBUG("fibo_PDPRelease ret = %ld\r\n", ret);
      m_GprsActState = STATE_GSM_QUERY_STATE;
      Eybpub_UT_SendMessage(EYBNET_TASK, NET_MSG_DNS_FAIL, 0, 0);

      tcp_relink();     //立即重连TCP Luee
      
      registe = 0;
      break;
    }
    case STATE_TOTAL_NUM:
      APP_DEBUG("STATE_TOTAL_NUM\r\n");
      break;
    default:
      break;
  }
  g_netmutex = 0;
}
/*      if (registe_times == 1) { // ping
        APP_DEBUG("Try PING %s last ret = %d\r\n", PING_HOSTNAME, ping_ret);
        ping_ret = fibo_mping(1, PING_HOSTNAME, 4, 32, 64, 0, 4000);
        if (ping_ret == 0) {
          registe = 1;
        } else {
          APP_DEBUG("PING %s Fail ret = %d\r\n", PING_HOSTNAME, ping_ret);
          registe = 0;
          m_GprsActState = STATE_DNS_NOT_READY; 
        }
      } else {
        registe_times++;
        if (registe_times > SIM_REGISTER_TIMES * 5) { // 每5分钟ping一次
          registe_times = 1;
        }
      } */


/*******************************************************************************
 Brief    :
 Parameter: 
 return   : 
*******************************************************************************/
void grid_Net_manage(void)
{
	static s32 offset = 0;
	s8_t ret;

  if(ssl_neting)
    return;
  ssl_neting=1;     //国网结束后，才能重新调用

	//益邦云连上后才处理国网
	if(m_GprsActState == STATE_DNS_READY){
  //if(registe==1){
		if (offset < sizeof(netManage)/sizeof(netManage[0])){
			if (netManage[offset].flag == 1 && netManage[offset].status != L610_SUCCESS && netManage[offset].status != L610_CONNECTING){
				if (netManage[offset].mode == 2){
					APP_DEBUG("\r\n-->ready open ssl\r\n");
					ret = SSL_Open(&netManage[offset]);
					if (0 == ret){
                APP_DEBUG("\r\n-->connet ssl server SUCCESS ret=%d\r\n", ret);
                netManage[offset].status = L610_SUCCESS;	
			        }
					else{
						netManage[offset].status = L610_WAIT;	
					}
				}
			}
			offset++;
		}
		else{
			offset=0;
		}	
	}	
  ssl_neting=0;   //国网结束后，才能重新调用
}




/*******************************************************************************
 Brief    : socketClose
 Parameter:
 return   :
*******************************************************************************/
static void socketClose(s32_t socketid) {
  u8_t nIndex = 0;
  APP_DEBUG("socket %ld Close\r\n", socketid);
  L610Net_t *socke = L610Socket(socketid, &nIndex);

  if (socke != null) {
    L610Net_close(nIndex);
  }
}

/*******************************************************************************
 Brief    : L610Socket
 Parameter:
 return   :
*******************************************************************************/
L610Net_t *L610Socket(s8_t socketid, u8_t *offset) {
  int i = 0;

  for (i = 0; i < sizeof(netManage) / sizeof(netManage[0]); i++) {
    if (netManage[i].flag == 1 && netManage[i].socketID == socketid) {
      *offset = i;
      return &netManage[i];
    }
  }

  return null;
}

/*******************************************************************************
 Brief    : L610Net_send
 Parameter:
 return   :
*******************************************************************************/
int L610Net_send(u8_t nIndex, u8_t *data, u16_t len) {
  int ret = -520;
  if ((nIndex < sizeof(netManage) / sizeof(netManage[0])) && netManage[nIndex].flag == 1 && netManage[nIndex].status == L610_SUCCESS) {
    if (netManage[nIndex].mode == 2) {
//      ret = SSL_Send(netManage[nIndex].socketID, (u8_t *)data, len);
    } else {
        while(statenet_para.send_status)    //Luee
          fibo_taskSleep(20);   
        eybnet_para.send_status=true;      

        ret = fibo_sock_send(netManage[nIndex].socketID, (u8_t *)data, len);       
      //  APP_DEBUG("socket[%d] %d send %d data ret %d\r\n", nIndex, netManage[nIndex].socketID, len, ret);  
        APP_DEBUG("ip: %s port:%d socket send OK!\r\n", netManage[nIndex].ipStr, netManage[nIndex].port);  
        APP_DEBUG("\r\n-->tcp lose:tcp send ok\r\n")


        eybnet_para.send_status=false;
    }
  }
//  ret = fibo_sock_send(g_socketid, (u8_t *)data, len);
  return ret;
}

/*******************************************************************************
 Brief    : L610_TCP_Callback
 Parameter:
 return   :
*******************************************************************************/
/* void L610_TCP_Callback(u8_t *param) {
  s16_t ret = 0;
  u8_t nIndex = 0;
  UINT8 strBuf[MAX_NET_BUFFER_LEN] = {0};

  while(1) {
    APP_DEBUG("L610_TCP_Callback task\r\n");
    if (m_GprsActState == STATE_TOTAL_NUM) {
      r_memset(strBuf, '\0', sizeof(strBuf));
      ret = fibo_sock_recv(g_socketid, strBuf, MAX_NET_BUFFER_LEN);
      if (ret > 0) {
        APP_DEBUG("socket: %d read %d data!\r\n", g_socketid, ret);
        Buffer_t cmdbuf;
        cmdbuf.size = MAX_NET_BUFFER_LEN;
        cmdbuf.lenght = 0;
        cmdbuf.payload = memory_apply(MAX_NET_BUFFER_LEN);
        memset(cmdbuf.payload, '\0', MAX_NET_BUFFER_LEN);
        memcpy(cmdbuf.payload, strBuf, ret);
        cmdbuf.lenght = ret;
        ESP_callback(nIndex, &cmdbuf);
      } else if (ret == 0) {
        APP_DEBUG("socket: %d remote is closed!\r\n", g_socketid);
      } else {
        APP_DEBUG("socket: %d read failt!\r\n", g_socketid);
      }
    } else {
      fibo_taskSleep(1000);
    }
  }
  fibo_thread_delete();
} */


 void L610_TCP_Callback(u8_t *param) {
  s16_t retlen = 0;
  u8_t nIndex = *param;
  s32_t ret = 0;
  UINT8 strBuf[MAX_NET_BUFFER_LEN] = {0};

  s32 relink=0;

  while(1) {
    //Luee
    //if (m_GprsActState == STATE_DNS_READY && netManage[nIndex].status == L610_SUCCESS) {
      if (netManage[nIndex].mode!=2&&m_GprsActState == STATE_DNS_READY && netManage[nIndex].status == L610_SUCCESS) {
      
      fibo_taskSleep(600);    //Luee

	#if 0
      r_memset(strBuf, '\0', sizeof(strBuf));
      retlen = fibo_sock_recv(netManage[nIndex].socketID, strBuf, MAX_NET_BUFFER_LEN);
      if (retlen > 0) {
        Buffer_t cmdbuf;
        cmdbuf.size = MAX_NET_BUFFER_LEN;
        cmdbuf.lenght = 0;
        cmdbuf.payload = memory_apply(MAX_NET_BUFFER_LEN);
        r_memset(cmdbuf.payload, '\0', MAX_NET_BUFFER_LEN);
        r_memcpy(cmdbuf.payload, strBuf, retlen);
        cmdbuf.lenght = retlen;
        APP_DEBUG("socket: %d read %d data!\r\n", netManage[nIndex].socketID, retlen);
	#endif

      Buffer_t cmdbuf;
      cmdbuf.size = MAX_NET_BUFFER_LEN;
      cmdbuf.lenght = 0;
      cmdbuf.payload = memory_apply(MAX_NET_BUFFER_LEN);
      r_memset(cmdbuf.payload, '\0', MAX_NET_BUFFER_LEN); 
      ret = fibo_sock_recv(netManage[nIndex].socketID, cmdbuf.payload, cmdbuf.size);
      if (ret > 0) {
        relink=0;
        cmdbuf.lenght = ret;
        APP_DEBUG("IP: %s socket: %ld read %ld data!\r\n", netManage[nIndex].ipStr,netManage[nIndex].socketID, ret);

        if (netManage[nIndex].port == EYBOND_DEFAULT_SERVER_PORT && \
            (r_strncmp(netManage[nIndex].ipStr, EYBOND_DEFAULT_SERVER, r_strlen(EYBOND_DEFAULT_SERVER)) == 0 || \
             r_strncmp(netManage[nIndex].ipStr, EYBOND_TEST_SERVER, r_strlen(EYBOND_TEST_SERVER)) == 0 || \
             r_strncmp(netManage[nIndex].ipStr, EYBOND_SOLAR_SERVER, r_strlen(EYBOND_SOLAR_SERVER)) == 0)) {  // 处理eybond云下发指令
          if(r_strncmp(netManage[nIndex].ipStr, EYBOND_TEST_SERVER, r_strlen(EYBOND_TEST_SERVER)) == 0){
              /*XXX*/ 
              /*生产调用同一接口会有问题*/
              APP_PRINT("receive test test data\r\n");
              TEST_esp_callback(nIndex, &cmdbuf);
          }else{

            ESP_callback(nIndex, &cmdbuf);
          }

          
        } else {
          memory_release(cmdbuf.payload);   // mike 20201210 可能会引起死机问题
        }
      } else {
        APP_DEBUG("socket: %ld read failt!\r\n", netManage[nIndex].socketID);
        log_save("TCP rec fail!!!");
        
        if(relink++>1){
            relink=0;
            //L610Net_close(nIndex);
            netManage[nIndex].status = L610_CONNECT_FAIL;
            fibo_sock_close(netManage[nIndex].socketID);
            netManage[nIndex].socketID = -1;
            //m_GprsActState = STATE_DNS_NOT_READY;
          //tcp_relink();
        }
        
      }
    } else {
      fibo_taskSleep(600);    //Luee
    }
  }
  fibo_thread_delete();
}

/* void L610_TCP_Callback(s32_t taskId) {
  s8_t ret = 0;
  u8_t nIndex = 0;
  struct timeval tv;
  tv.tv_sec  = 5;
  tv.tv_usec = 0;
  int maxfds = 0;
  FD_ZERO(&g_readfds);
  FD_ZERO(&g_errorfds);
//  FD_ZERO(&g_writefds);

  while(1) {
    APP_DEBUG("L610_TCP_Callback task maxfds:%d\r\n", maxfds);
    for (nIndex = 0; nIndex < sizeof(netManage) / sizeof(netManage[0]); nIndex++) {
      if (netManage[nIndex].flag == 1 && netManage[nIndex].status == L610_SUCCESS) {
        maxfds = netManage[nIndex].socketID + 1;
      }
    }
//    ret = fibo_sock_lwip_select(24, &g_readfds, &g_writefds, &g_errorfds, &tv);
    ret = fibo_sock_lwip_select(7, &g_readfds, NULL, &g_errorfds, &tv);
    if(ret <= 0) {
	  if (ret < 0){
		APP_DEBUG("select %d ret < 0, error\r\n", nIndex);
	  }
      if(ret == 0) {
		APP_DEBUG("select timeout.\r\n");
	  }
	  continue;
    }

    APP_DEBUG("select ret: %d \r\n", ret);
    
    for (nIndex = 0; nIndex < sizeof(netManage) / sizeof(netManage[0]); nIndex++) {
      if (netManage[nIndex].flag == 1 && netManage[nIndex].status == L610_SUCCESS) {
        if (FD_ISSET(netManage[nIndex].socketID, &g_errorfds)) {          
          int error =0;
		  int len =4;
		  ret = fibo_sock_getOpt(netManage[nIndex].socketID, SOL_SOCKET, SO_ERROR,&error, &len);
          APP_DEBUG("Index %d socket %d, get error code:%d\r\n", nIndex, netManage[nIndex].socketID, error);
        } else if (FD_ISSET(netManage[nIndex].socketID, &g_readfds)) {
          APP_DEBUG("Index %d MSG in from socket %d\r\n", nIndex, netManage[nIndex].socketID);
          Buffer_t cmdbuf;
          cmdbuf.size = MAX_NET_BUFFER_LEN;
          cmdbuf.lenght = 0;
          cmdbuf.payload = memory_apply(MAX_NET_BUFFER_LEN);
          r_memset(cmdbuf.payload, '\0', MAX_NET_BUFFER_LEN);
          ret = fibo_sock_recv(netManage[nIndex].socketID, cmdbuf.payload, cmdbuf.size);
          if (ret > 0) {
            cmdbuf.lenght = ret;
            APP_DEBUG("socket[%d]: %d read %d data!\r\n",nIndex, netManage[nIndex].socketID, ret);
//            FD_ZERO(&g_readfds);
//            FD_SET(netManage[nIndex].socketID, &g_readfds);
//          netManage[nIndex].callback(nIndex, &cmdbuf);
            ESP_callback(nIndex, &cmdbuf);
          } else if (ret == 0) {
            memory_release(cmdbuf.payload);
            APP_DEBUG("socket[%d]: %d remote is closed!\r\n", nIndex, netManage[nIndex].socketID);
          } else {
            memory_release(cmdbuf.payload);
            APP_DEBUG("socket: %d read fail!\r\n", netManage[nIndex].socketID);
          }
        }
//          else if (FD_ISSET(netManage[nIndex].socketID, &g_writefds)) {
//          APP_DEBUG("Index %d MSG out from socket %ld\r\n", nIndex, netManage[nIndex].socketID);
//        }
      } else {
        APP_DEBUG("socket[%d] is not ready.\r\n", nIndex);
      }
    }
//    fibo_taskSleep(1000);
  }
  fibo_thread_delete();
}*/

/*******************************************************************************
  * @brief  AT+TEST=tool.eybond.com,502\r\n 
  * @param  None
  * @retval None
*******************************************************************************/
int netInTest(Buffer_t *buf)
//int netInTest(Buffer_t *buf, void_fun_bufp output)
{
	s32_t ret = -1;
	Buffer_t ackBuf;
  
	 if (buf != null && buf->payload != null && buf->lenght > 20)	   
	 {		
	 	const char AT_TEST[] = "AT+TEST=";		  
	 	const char AT_ACK[] = "+OK\r\n";			
	 	int i = r_strfind(AT_TEST, (const char *)buf->payload); 				   
       
	 	if ( i >= 0) 		 
	 	{	
           		
	 		int port;
            APP_DEBUG("manufac produc_flag  = %d\r\n",produc_flag);
            if( 0 == produc_flag ){

                produc_flag = 1;

                produc_save_flag = 1;


                /*删除备份文件*/
                s32_t file_c_size = fibo_file_getSize(g_backup_parameter_c);    

                if (file_c_size >= 0) {
                    s32_t delete = fibo_file_delete(g_backup_parameter_c);
                    APP_DEBUG("delete c is %ld\r\n", delete);
                }
                int g_iFd_backup_c = fibo_file_open(g_backup_parameter_c, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC);
                if(g_iFd_backup_c < 0){
               
                    APP_PRINT("creat backup_c file  failed\r\n");

                }else{
                    APP_PRINT("creat backup_c file  succeed\r\n");
                    fibo_file_close(g_iFd_backup_c);
                }
            }
            

            			   
	 		char *p = (char*)&buf->payload[i];			   
	 		i = r_strfind(",", p);			   
	 		port = Swap_charNum((char*)(p+i+1)); 			 
	 		if ((i > 0)&&(i < 40) && port < 65535) 			 
	 		{				  
	 			p[i] = '\0';
	 			APP_PRINT("In test-\r\n");
                APP_DEBUG("%s\r\n",(char*)buf->payload);
	 			if (r_strlen(testIP) == 0 || 0 != r_strcmp( p + r_strlen(AT_TEST), testIP)) 			   
				{		
	 				testPort = port;
	 				r_strcpy(testIP, p + r_strlen(AT_TEST));
                    L610Net_closeAll();
	 			}
                  	  
             
                Net_connect(1, testIP, port, TEST_esp_callback);
				ackBuf.lenght = sizeof(AT_ACK);
				ackBuf.payload = (u8_t*)AT_ACK;
                Uart_write((UINT8 *)ackBuf.payload,ackBuf.lenght);
				ret = 0;
                
	 		}		   
		}	  
	 }
	return ret;
}

          
            

#endif
/*********************************FILE END*************************************/

