/******************************************************************************           
* name:                        
* introduce:        
* author:           Luee                                     
******************************************************************************/ 
#include "eyblib_typedef.h"
#include "eyblib_swap.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_memory.h"
#include "eyblib_list.h"
#include "eyblib_algorithm.h"

#include "eybpub_SysPara_File.h"
#include "eybpub_run_log.h"
#include "eybpub_Debug.h"
#include "eybpub_UnixTime.h"

#include "eybpub_data_collector_parameter_table.h"
#include "history.h"

#ifdef _PLATFORM_M26_
#include "FlashFIFO.h"
#include "FlashHard.h"
#endif

#include "Device.h"

#include "StateGrid.h"
#include "StateGridData.h"

#include "grid_tool.h"
#include "L610Net_SSL.h"
#include "L610Net_TCP_EYB.h"

#define SEND_SPACE              (3)
#define STATION_COUNT           4
typedef struct StateGrid {
  u32_t number;
  u8_t  addr[8];
} NoAddr_t;

typedef struct {
  u16_t count;
  NoAddr_t No[STATION_COUNT];
} Number_t;

static const char stateGridServerAddr[] = "gfyfront.esgcc.com.cn:19020:SSL";  // "cie-bj.tpddns.cn:19020:SSL";//
static ListHandler_t rcveList;  // Net data receive list
ListHandler_t sslrecList;  // Net data receive list
static u8_t step;  // connext server step
static int heartbeatSpace;  // heartbeat sapce;
static int uploadDataSpace; // upload data space
static int historySapce;

// static FlashFIFOHead_t historyHead;

static Buffer_t  *StateGrid_create(StateGridCmd_t *st);
static StateGridCmd_t *stateGrid_parse(Buffer_t *data);
static void stateGrid_free(StateGridCmd_t *st);
static u8_t getSysPara(int num,  char **p);
static u8_t getName(char **p);
static void setControl(u16_t ackFlag, Control_t *ctr);
static Buffer_t  *StateGridCmd_create(u16_t code, u16_t ackFlag, Buffer_t pdu);
static Buffer_t  *StateGridCmd_ack(StateGridCmd_t *cmd, FS_e fs, Buffer_t pdu);

static void StateGrid_init(void);
static void StateGrid_run(u8_t status);
static void StateGrid_process(void);
static u8_t StateGrid_cmd(Buffer_t *buf, DataAck ch);
static ServerAddr_t *StateGrid_Addr(void);
static void StateGrid_close(void);
static void StateGrid_clear(void);
const CommonServer_t StateGrid_API = {
  "State Grid",
  180,
  StateGrid_init,
  StateGrid_dataCollect,
  StateGrid_run,
  StateGrid_process,
  StateGrid_cmd,
  StateGrid_Addr,
  StateGrid_close,
  StateGrid_clear
};

static void stateGrid_login(void);
static u8_t stateGrid_loginAck(StateGrid_t *sg);
static void stateGrid_register(void);
static u8_t stateGrid_registerAck(StateGrid_t *sg);
static void stateGrid_upload(void);
static u8_t stateGrid_uploadAck(StateGrid_t *sg);
static void stateGrid_historySave(void);
static u8_t stateGrid_historyUpload(void);
static u8_t stateGrid_historyUploadAck(StateGrid_t *sg);
//static u8_t stateGrid_getData(StateGrid_t *sg);
//static u8_t stateGrid_prooftime(StateGrid_t *sg);
static void stateGrid_heartbeat(void);
static u8_t stateGrid_heartbeatAck(StateGrid_t *sg);

static const StateGridTab_t stateGridCmdTab[] = {
  {0x00, stateGrid_loginAck},             // 登录
  {0x01, stateGrid_registerAck},          // 注册厂站
  {0x04, stateGrid_uploadAck},            // 上报实时数据
  {0x05, stateGrid_historyUploadAck},     // 上报断点续传数据
  {0x10, stateGrid_getData},              // 召测实时数据
  {0x21, stateGrid_prooftime},            // 对时
  {0x99, stateGrid_heartbeatAck}          // 链路测试（心跳）
};
/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
int StateGrid_check(void) {
  Buffer_t buf;
  int i = -1;

  SysPara_Get(65, &buf);
  //parametr_get(STATE_GRID_SN, &buf);
  //if ((buf.payload != null) && (buf.lenght > 1)
  if ((buf.lenght > 1)
      && (r_strlen((char *)buf.payload) > 2 && r_strlen((char *)buf.payload) < 50)
      && (StateGrid_pointTab() == 0)) {
    memory_release(buf.payload);
    SysPara_Get(66, &buf);
    //parametr_get(STATE_GRID_USER_NAME, &buf);
    //if ((buf.payload != null) && (buf.lenght > 1)
    if ((buf.lenght > 1)
        && (r_strlen((char *)buf.payload) > 2 && r_strlen((char *)buf.payload) < 50)) {
      u32_t number[STATION_COUNT];
      memory_release(buf.payload);
      SysPara_Get(68, &buf);
      //parametr_get(STATE_GRID_REGISTER_ID, &buf);
     //if ((buf.payload != null) && (buf.lenght > 1)
      if ((buf.lenght > 1)
          && (r_strlen((char *)buf.payload) > 5 && StateGrid_station(0, number) > 0)) {
        i = 0;
        APP_DEBUG("\r\n-->state grid check ok\r\n")
      }
    }
  }

  memory_release(buf.payload);

  return i;
}
/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void StateGrid_init(void) {
  step = 0;
  uploadDataSpace = (3 * 60 / SEND_SPACE); // 国网上传实时数据间隔时间
  heartbeatSpace = 0;
  historySapce = 0;
  list_init(&rcveList);
  StateGrid_dataCollect();
  //FlashFIFO_init(&historyHead, FLASH_HANERGY_HISTORY_ADDR, FLASH_HANERGY_HISTORY_SIZE);
  history_init();
}
/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void StateGrid_run(u8_t status)
{
    s32 ret=-1;
    static u8_t space = 0;
    if (status == L610_SUCCESS)
    {
        if (++space > SEND_SPACE)
        {
            space = 0;
            if (step == 0)
            {
                stateGrid_login();  
            }
            else if (step == 1)
            {
                stateGrid_register(); 
            }
            else if ((step == 2)
                    && (++uploadDataSpace  >  (60 * 5/SEND_SPACE))
                    
                    && (stateGrid_historyUpload())        
                    && (StateGrid_dataStatus() == 1
                    )
                )
            {
                uploadDataSpace -= 2;
                stateGrid_upload();
                clear_overtime();   
            }
            else if (++heartbeatSpace > (60/SEND_SPACE))
            {
                heartbeatSpace -= 2;
                stateGrid_heartbeat();         
            }
            
        }
    }
    else 
    {
        step = 0;
        if (historySapce++ > (60 * 5))
        {
            stateGrid_historySave();
        }
    }
}
/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void StateGrid_process(void) {
  StateGrid_t *sp = (StateGrid_t *)list_nextData(&rcveList, null);

  if (null == sp) {
    return;
  }

  heartbeatSpace = 0;
  while (sp != null && sp->waitCnt != 0) {
    if (++sp->waitCnt > STATE_GRID_CNT) { //wait prcesso overtime
      stateGrid_free(sp->cmd);
      list_nodeDelete(&rcveList, sp);
      return;
    }

    sp = (StateGrid_t *)list_nextData(&rcveList, sp);
  }

  if (sp != null) {
    StateGridTab_t *exe = (StateGridTab_t *)ALG_binaryFind(sp->cmd->func, (void *)stateGridCmdTab,
                          (void *)(&stateGridCmdTab[sizeof(stateGridCmdTab) / sizeof(stateGridCmdTab[0])]), sizeof(stateGridCmdTab[0]));

    if (exe != null) {
      if (0 != exe->func(sp)) {
        sp->waitCnt++;
        return;
      }
    }
  }
  stateGrid_free(sp->cmd);
  list_nodeDelete(&rcveList, sp);
}
/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t StateGrid_cmd(Buffer_t *buf, DataAck ch) {
  u8_t e = 0;
  StateGridCmd_t *cmd = null;

  if (buf == null || buf->payload == null || buf->lenght <= 0 || ch == null) {
    e = 1;
  } else if (null == (cmd = stateGrid_parse(buf))) {
    e = 2;
  } else {
    StateGrid_t *sg = list_nodeApply(sizeof(StateGrid_t));

    if (sg == null) {
      e = 3;
      log_save("stateGrid memory apply fail!");
    } else {
      sg->cmd = cmd;
      sg->waitCnt = 0;
      sg->ack = ch;
      list_topInsert(&rcveList, sg);
    }
  }

  return e;
}

u8_t StateGrid_cmd2(Buffer_t *buf) 
{
  u8_t e = 0;
  StateGridCmd_t *cmd = null;

  if (buf == null || buf->payload == null || buf->lenght <= 0 ) {
    e = 1;
  } else if (null == (cmd = stateGrid_parse(buf))) {
    e = 2;
  } else {
    StateGrid_t *sg = list_nodeApply(sizeof(StateGrid_t));

    if (sg == null) {
      e = 3;
      log_save("stateGrid memory apply fail!");
    } else {
      sg->cmd = cmd;
      sg->waitCnt = 0;
      //sg->ack = ch;
      list_topInsert(&sslrecList, sg);
    }
  }

  return e;
}

/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
ServerAddr_t *state_ServerAdrrGet(u8_t num)
{
	Buffer_t buf;
	//Buffer_t portBuf;       //Luee
	ServerAddr_t *serverAddr = null;
	//得到23号参数
	SysPara_Get(num, &buf);	

	//if (buf.payload != null && buf.lenght > 5)
  if (buf.lenght > 5)
	{
    //定义链表
		ListHandler_t cmdStr;
		int len;
		char *str = memory_apply(buf.lenght);

		r_memcpy(str, buf.payload, buf.lenght);
		r_strsplit(&cmdStr, str, '.');
    //验证网址格式是否正确，网址同两占三段组成，如：gfyfront.esgcc.com.cn:19020:SSL
		if (cmdStr.count < 3)
		{
			list_delete(&cmdStr);
			memory_release(str);
			return null;
		}
		list_delete(&cmdStr);
		memory_release(str);
    //得到以“：”分隔的结点
		r_strsplit(&cmdStr, (char*)buf.payload, ':');

		if (cmdStr.count > 0)
		{
			len = r_strlen((char*)*(int*)cmdStr.node->payload);
			serverAddr = memory_apply(sizeof(ServerAddr_t) + len);
			r_strcpy(serverAddr->addr, (char*)*(int*)cmdStr.node->payload);

			if(num == SARNATH_SERVER_ADDR)
			{
                //SysPara_Get(SARNATH_SERVER_PORT, &portBuf);       //Luee
                //serverAddr->port = Swap_charNum(portBuf.payload); //Luee
			}
			else
			{
			    serverAddr->port = 502;
			}
			serverAddr->type = 1;

			if (cmdStr.count > 1)
			{
        //字符串转为16进制实际值，得到端口号，如：国网：19020
				serverAddr->port = Swap_charNum((char*)*(int*)cmdStr.node->next->payload);
			}
			if (cmdStr.count > 2)
			{
				if (r_strfind("UDP", (char*)*(int*)cmdStr.node->next->next->payload) >= 0)
				{
					serverAddr->type = 0;
				}
				else if (r_strfind("SSL", (char*)*(int*)cmdStr.node->next->next->payload) >= 0)
				{
					serverAddr->type = 2;
				}
			}
      list_delete(&cmdStr);
		}
    else{
      list_delete(&cmdStr);
       goto END;
  }
		
	}
  else{
    goto END;
  }

	if(serverAddr->port == 0)
	{
	    memory_release(serverAddr);
	    serverAddr = null;
	}
END:
	//memory_release(portBuf.payload);    //未申请内存，死机的原因
	memory_release(buf.payload);

	return serverAddr;
}

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static ServerAddr_t *StateGrid_Addr(void) {
  
  ServerAddr_t *stateGridServer = state_ServerAdrrGet(STATE_GRID_SERVER_ADDR);
  APP_DEBUG("\r\n-->state grid addr:%s ,type:%d ,port:%d\r\n",stateGridServer->addr,stateGridServer->type,stateGridServer->port);

  if (stateGridServer == null) {
    Buffer_t buf;

    buf.lenght = sizeof(stateGridServerAddr);
    buf.payload = (u8_t *)stateGridServerAddr;

    SysPara_Set(STATE_GRID_SERVER_ADDR, &buf);
    //parametr_set(STATE_GRID_SERVER_ADDR, &buf);
    APP_DEBUG("\r\n-->init 23 para\r\n");
  }

  return stateGridServer;
}
/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void StateGrid_close(void) {
  list_delete(&rcveList);
  StateGrid_destroy();
}

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void StateGrid_clear(void) {
//  FlashFIFO_clear(&historyHead);
}

/*******************************************************************************            
 * introduce:        create state grid 
 * parameter:        StateGridCmd_t *st                 
 * return:           buf         
 * author:           Luee                                                    
 *******************************************************************************/
static Buffer_t  *StateGrid_create(StateGridCmd_t *st)
{
    Buffer_t *buf = null;

    if (st != null)
    {
        //buf = memory_apply(sizeof(Buffer_t) + sizeof(st->lenght) + st->lenght);
        buf = fibo_malloc(sizeof(Buffer_t) + sizeof(st->lenght) + st->lenght);
        if (buf != null)
        {
            u8_t *pData;

            log_d("\r\nbuf != null\r\n");

            buf->size = sizeof(st->lenght) + st->lenght;
            buf->lenght = buf->size;
            buf->payload = (u8_t*)(buf + 1);
            pData = buf->payload;

            *pData++ = (st->lenght>>24);
            *pData++ = (st->lenght>>16);
            *pData++ = (st->lenght>>8);
            *pData++ = (st->lenght>>0);
            *pData++ = st->nameLen;
            //r_memcpy(pData, st->name, st->nameLen);
            memcpy(pData, st->name, st->nameLen);
            pData += st->nameLen;
            //r_memcpy(pData++, &st->ctr, 1);
            memcpy(pData++, &st->ctr, 1);
            *pData++ = st->func;
            //r_memcpy(pData, st->pdu.payload, st->pdu.lenght);
            memcpy(pData, st->pdu.payload, st->pdu.lenght);
        }
        //stateGrid_free(st);
        fibo_free(st);
    }

    return buf;
}

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static StateGridCmd_t *stateGrid_parse(Buffer_t *data)
{
    int i=0;
    StateGridCmd_t *st = null;
    u8_t *pData = data->payload;

    ERRR ((data == null || data->lenght < 8 || pData == null), goto END);
    //i =  ((*pData++)<<24) | ((*pData++)<<16) | ((*pData++)<<8) | (*pData++);
    i=(*pData++)<<24;
    i|=(*pData++)<<16;
    i|=(*pData++)<<8;
    i|=(*pData++)<<0;
    APP_DEBUG("\r\n-->state grid ssl recbuf lenght=%x",i);
    ERRR (((i + 4) != data->lenght) , goto END);

    st = memory_apply(sizeof(StateGridCmd_t));
    ERRR((st == null), goto END);

    st->lenght = i;
    st->nameLen = *pData++;
    st->name = memory_apply(st->nameLen + 1);
    ERRR (st->name == null, goto END1);
    
    r_memcpy(st->name, pData, st->nameLen);
    st->name[st->nameLen] = '\0';
    pData += st->nameLen;
    r_memcpy(&st->ctr, pData++, 1);
    st->func = *pData++;
    st->pdu.size = data->lenght - (pData - data->payload);
    st->pdu.payload = memory_apply(st->pdu.size);
    ERRR (st->pdu.payload == null, goto END2);

    r_memcpy(st->pdu.payload, pData, st->pdu.size);
    st->pdu.lenght = st->pdu.size;
    return st;

END2:
    memory_release(st->name);
END1:
    memory_release(st);
END:
    return null;
}

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
static void stateGrid_free(StateGridCmd_t *st) {
  if (st != null) {
    if (st->name != null) {
      memory_release(st->name);
    }

    if (st->pdu.payload != null) {
      memory_release(st->pdu.payload);
    }

    memory_release(st);
  }
}

/*******************************************************************************
  * @note   get device system seting or
  * @param  None
  * @retval  name lenght and **p name value point
*******************************************************************************/
static u8_t getSysPara(int num,  char **p) {
  Buffer_t buf;
  u8_t lenght = 0;

  SysPara_Get(num, &buf);
  //parametr_get(num, &buf);
  if (buf.payload != null) {
    if (*p == null) {
      *p = (char *)buf.payload;
      lenght = buf.lenght;
    } else {
      lenght = r_stradd(*p, (char *)buf.payload);
      memory_release(buf.payload);
    }
  }

  return lenght;
}
/*******************************************************************************
  * @note   get device name  name = eybond + cpuid
  * @param  None
  * @retval  name lenght and **p name value point
*******************************************************************************/
static u8_t getName(char **p) {
  static u8_t lenght = 0;
  static char nameHead[31] = {0};//"123456";//

  if (lenght == 0) {
    char *name = nameHead;
    lenght = getSysPara(STATE_GRID_SN, &name);
  }

  *p = nameHead;

  return lenght;
}

/*******************************************************************************
  * @note   get device power station number
  * @param   addr: address number, No: numberValue;
  *          addr = 0, No: return all numberValue
  * @retval  No return power station count
*******************************************************************************/

/**
 * @brief get device power station number
 * @param ST
 * @param para
 * @return
 */
static void getStationTab(Number_t *ST, char *para) {
  ListHandler_t NumberStr;

  APP_DEBUG("State Grid %s\r\n", para);
  r_strsplit(&NumberStr, para, ';');
  ST->count = NumberStr.count;    //获取总共的电站数量, 总数不能超过STATION_COUNT=4
  if (NumberStr.count == 0) {
    log_save("State Grid station number error!");
  }
  if (NumberStr.count == 1) {
    ST->No[0].number = Swap_charNum(para);
  } else {
    ListHandler_t NoAddr;
    u8_t *addr;
    int *station;
    int *value;
    int i = 0;

    station = list_nextData(&NumberStr, null);
    while (station != null && i < STATION_COUNT) {
      APP_DEBUG("State Grid number %s\r\n", (char *)*station);
      r_strsplit(&NoAddr, (char *)*station, ',');
      value = list_nextData(&NoAddr, null);
      if (NoAddr.count < 2) {
        log_save("State Grid station addr error!");
        list_delete(&NoAddr);
        break;
      }
      APP_DEBUG("State Grid number NO %s\r\n", (char *)*value);
      ST->No[i].number = Swap_charNum((char *)*value); //提交电站ID
      addr = ST->No[i].addr;
      value = list_nextData(&NoAddr, value);

      while (value != null) {
        APP_DEBUG("State Grid number addr %s\r\n", (char *)*value);
        *addr++ = Swap_charNum((char *)*value);
        value = list_nextData(&NoAddr, value);
      }
      i++;
      list_delete(&NoAddr);
      station = list_nextData(&NumberStr, station);
    }
  }

  list_delete(&NumberStr);
}

/**
 *
 * @param addr
 * @param No
 * @return
 */
u16_t StateGrid_station(u8_t addr, u32_t *No) {
  static Number_t Num = {0};
  int i = 0;
  int n = 0;
  int cnt = 0;

  if (Num.count == 0 || Num.No[0].number == 0) {
    char *name = null;
    r_memset(&Num, 0, sizeof(Number_t));
    getSysPara(STATE_GRID_REGISTER_ID, &name);
    getStationTab(&Num, name);
    memory_release(name);
  }

  for (i = 0; i < Num.count; i++) {
    n = 0;
    do {
      if (Num.No[i].addr[n] == 0 || Num.No[i].addr[n] == addr || addr == 0) {
        cnt++;
        *No++ = Num.No[i].number;
        if (addr != 0) {
          return 1;
        }

        break;
      }
      APP_DEBUG("State Grid number addr get %ld, %d\r\n", Num.No[i].number, Num.No[i].addr[n]);
    } while (Num.No[i].addr[++n] != 0);
  }
  APP_DEBUG("State Grid number return\r\n");
  return cnt;
}

static void setControl(u16_t ackFlag, Control_t *ctr) {
  static u8_t serial = 0;

  ctr->dir = 1;
  ctr->fs = 3;
  ctr->con = ackFlag;
  ctr->sn = serial++;
}

/*******************************************************************************            
 * introduce:        state grid command create   
 * parameter:        code : command code , ackFlag: 0 No ack, 1 must ack , pdu                
 * return:           Buffer_t        
 * author:           Luee                                                    
 *******************************************************************************/
static Buffer_t  *StateGridCmd_create(u16_t code, u16_t ackFlag, Buffer_t pdu)
{
    //StateGridCmd_t *cmd = memory_apply(sizeof(StateGridCmd_t));
    StateGridCmd_t *cmd = (StateGridCmd_t *)fibo_malloc(sizeof(StateGridCmd_t));
    ERRR(cmd == null, goto END);
    log_d("\r\ncmd != null\r\n");

    //cmd->nameLen = getName(&cmd->name);
    char namebuf[64]={0};
    cmd->name=namebuf;
    for (u8 j = 0; j <number_of_array_elements; j++){
	    if(65 == PDT[j].num){ 
            u16 cmd_len  = 64;  
		    PDT[j].rFunc(&PDT[j],cmd->name, &cmd_len);
	    }
	}
    cmd->nameLen=strlen(cmd->name);
    log_d("name len=%d",cmd->nameLen);

    setControl(ackFlag, &cmd->ctr);
    cmd->func = code;
    //r_memcpy(&cmd->pdu, &pdu, sizeof(Buffer_t));
    memcpy(&cmd->pdu, &pdu, sizeof(Buffer_t));
    cmd->lenght = 1 + 1 + cmd->nameLen + 1 + pdu.lenght;

    log_d("\r\nnext:StateGrid_create\r\n");
    return StateGrid_create(cmd);
END:
    return null;
}

/*******************************************************************************            
 * introduce: 0x00 login       
 * parameter:                        
 * return:                 
 * author:           Luee                                                    
 *******************************************************************************/
static void stateGrid_login(void)
{
    typedef struct
    {
        u8_t nameLen;
        u8_t passwordLen;
        char *name;
        char *password;
    }login_t;

    Buffer_t pdu;
    login_t login = {0};
    
    //login.nameLen = getSysPara(STATE_GRID_USER_NAME, &login.name);
    char tempbuf[64]={0};
    login.name=tempbuf;
    for (u8 j = 0; j <number_of_array_elements; j++){
	    if(66 == PDT[j].num){
        u16      cmd_len  = 64;  
		PDT[j].rFunc(&PDT[j],login.name, &cmd_len);
	    }
	}
    login.nameLen=strlen(login.name);
    log_d("login name len=%d",login.nameLen);

    //login.passwordLen = getSysPara(STATE_GRID_PASSWORD, &login.password);
    char tempbuf2[64]={0};
    login.password=tempbuf2;
    for (u8 j = 0; j <number_of_array_elements; j++){
	    if(67 == PDT[j].num){
        u16      cmd_len  = 64;  
		PDT[j].rFunc(&PDT[j],login.password, &cmd_len);
	    }
	}
    login.passwordLen=strlen(login.password);
    log_d("login password len=%d",login.passwordLen);

    pdu.size = login.nameLen + login.passwordLen + 2;
    //pdu.payload = memory_apply(pdu.size);
    pdu.payload = fibo_malloc(pdu.size);

    if (pdu.payload != null)
    {
        u8_t *p = pdu.payload;
        Buffer_t *buf;

        *p++ = login.nameLen;
        //r_memcpy(p, login.name, login.nameLen);
        memcpy(p, login.name, login.nameLen);
        p += login.nameLen;
        *p++ = login.passwordLen;
        //r_memcpy(p, login.password, login.passwordLen);
        memcpy(p, login.password, login.passwordLen);

        pdu.lenght = pdu.size;
        buf = StateGridCmd_create(0x00, 1, pdu);
        fibo_free(pdu.payload);

        CommonServerDataSend(buf);
        //memory_release(buf);
        fibo_free(buf);
    }
    //memory_release(login.name);
    //memory_release(login.password);
}


/*******************************************************************************
  * @note   0x00 login ack
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t stateGrid_loginAck(StateGrid_t *sg) {
#pragma pack(1)
  typedef struct {
    u8_t result;
  } loginAck_t;
#pragma pack()

  if (sg->cmd->pdu.lenght == sizeof(loginAck_t)) {
    loginAck_t *ack = (loginAck_t *)sg->cmd->pdu.payload;

    if (ack->result == 0x01) {
      log_save("State Grid login OK");
      APP_DEBUG("\r\n-->state grid login success!!!\r\n");
      step = 1; //login success
    } else {
      log_save("State Grid login ERR:%d", ack->result);
      APP_DEBUG("\r\n-->state grid login fail\r\n");
    }
  } else {
    log_save("State Grid login ack lenght ERR %d", sg->cmd->pdu.lenght);
  }

  return 0;
}

/*******************************************************************************
  * @note   0x01 register  
  * @param  None
  * @retval None
*******************************************************************************/
static void stateGrid_register(void)
{

#pragma pack(1)
    typedef struct
    {
        s16_t count;
        s32_t number;
    }register_t;
#pragma pack() 
    
    Buffer_t pdu;
       
    pdu.size = sizeof(register_t) * STATION_COUNT;
    pdu.lenght = 0;
    //pdu.payload = memory_apply(pdu.size);
    pdu.payload = fibo_malloc(pdu.size);

    if (pdu.payload != null)
    {
        u32_t number[STATION_COUNT];
        Buffer_t *buf;
        int i;
        u8_t  *p = pdu.payload;

        log_d("\r\npdu.payload != null\r\n");
        i = StateGrid_station(0, number);
        log_d("\r\nStateGrid_station finish\r\n");
        //station_count
        *p++ = 0;
        *p++ = i;
        //station id array
        while ((i--) > 0)
        {
            *p++ = (number[i]>>24)&0xFF;
            *p++ = (number[i]>>16)&0xFF;
            *p++ = (number[i]>>8)&0xFF;
            *p++ = (number[i]>>0)&0xFF;
        }
        pdu.lenght = p - pdu.payload;
        buf = StateGridCmd_create(0x01, 1, pdu);
        fibo_free(pdu.payload);
        
        CommonServerDataSend(buf);
        //memory_release(buf);
        fibo_free(buf);
    }

}

/*******************************************************************************
  * @note   0x01 register ack
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t stateGrid_registerAck(StateGrid_t *sg) {
#pragma pack(1)
  typedef struct {
    u8_t result;
    s16_t count;
  } registerAck_t;
#pragma pack()

  if (sg->cmd->pdu.lenght == sizeof(registerAck_t)) {
    registerAck_t *ack = (registerAck_t *)sg->cmd->pdu.payload;

    if (ack->result == 0x01) {
      log_save("State Grid register OK!");
      APP_DEBUG("\r\n-->state grid register success!!!\r\n");
      step = 2; //register success
    } else {
      log_save("State Grid register ERR:%d", ack->result);
      APP_DEBUG("\r\n-->state grid register fail!!!\r\n");
    }
  } else {
    log_save("State Grid login ack register ERR %d", sg->cmd->pdu.lenght);
  }

  return 0;
}
/*******************************************************************************
  * @note   0x04  point collect data upload
  * @param  None
  * @retval None
*******************************************************************************/
static void save(Buffer_t *buf) {
  Buffer_t paraBuf;

//  SysPara_Get(BUZER_EN_ADDR, &paraBuf);
  parametr_get(BUZER_EN_ADDR, &paraBuf);
  if (paraBuf.payload != null && paraBuf.lenght > 0) {
    int num;
    num = Swap_charNum((char *)paraBuf.payload);
    memory_release(paraBuf.payload);
    if (num == 8) {
      char *str = memory_apply(buf->lenght * 3 + 8);
      if (str != null) {
        int l = Swap_hexChar(str, buf->payload, buf->lenght, 0);
        log_save(str);
        memory_release(str);
      }
    }
  }
}

static void stateGrid_upload(void) {
#pragma pack(1)
  typedef struct {
    u32_t number;
    u8_t type;
    u8_t time[8];
    u8_t point[0];
  } upload_t;
#pragma pack()
  Buffer_t pdu;
  pdu.size = STATE_GRID_CMD_SIZE;
  pdu.lenght = 0;
  pdu.payload = memory_apply(pdu.size);
  if (pdu.payload != null) {
    int i;
    Buffer_t pointBuf;
    Buffer_t *buf;
    upload_t *upload = (upload_t *)pdu.payload;

    upload->type = 0x01;
    UnixTime_get(upload->time);
    Swap_headTail(upload->time, 8);
    pointBuf.lenght = 0;
    pointBuf.size = pdu.size - sizeof(upload_t);
    pointBuf.payload = upload->point;
    i = StateGrid_dataGet(&pointBuf, null, null, &upload->number);
    if (i  ==  0) {
      uploadDataSpace = 0;
      historySapce = 0;
    }
    pdu.lenght = pointBuf.lenght + sizeof(upload_t);
    buf = StateGridCmd_create(0x04, 1, pdu);
    CommonServerDataSend(buf);
    save(buf);
    memory_release(buf);
  }
}
/*******************************************************************************
  * @note   0x04  point collect data upload ack
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t stateGrid_uploadAck(StateGrid_t *sg) {
#pragma pack(1)
  typedef struct {
    u8_t result;
  } uploadAck_t;
#pragma pack()

  if (sg->cmd->pdu.lenght == sizeof(uploadAck_t)) {
    uploadAck_t *ack = (uploadAck_t *)sg->cmd->pdu.payload;

    if (ack->result == 0x01) {
      APP_DEBUG("State Grid upload data OK\r\n ");
      log_save("State Grid upload data OK :%d", ack->result);
      APP_DEBUG("\r\n-->state grid upload success!!!\r\n");
    } else {
      log_save("State Grid upload data ERR:%d", ack->result);
      APP_DEBUG("\r\n-->state grid upload fail!!!\r\n");
    }
  } else {
    log_save("State Grid upload data ERR %d", sg->cmd->pdu.lenght);
  }

  return 0;
}

/*******************************************************************************
  * @note   history save
  * @param  None
  * @retval None
*******************************************************************************/
static void stateGrid_historySave(void) {
#pragma pack(1)
  typedef struct {
    u32_t number;
    u8_t type;
    u8_t point[0];
  } history_t;
#pragma pack()

  if (StateGrid_dataStatus() == 1) {
    Buffer_t pdu;

    pdu.size = STATE_GRID_CMD_SIZE;
    pdu.lenght = 0;
    pdu.payload = memory_apply(pdu.size);
    log_save("State grid histor data save!!");
    if (pdu.payload != null) {
      int i;
      u8_t time[8];
      Buffer_t pointBuf;
      history_t *history = (history_t *)pdu.payload;

      history->type = 0x01;
      UnixTime_get(time);
      Swap_headTail(time, sizeof(time));
      pointBuf.lenght = 0;
      pointBuf.size = pdu.size - sizeof(history_t);
      pointBuf.payload = history->point;
      i = StateGrid_dataGet(&pointBuf, time, null, &history->number);
      if (i == 0) {
        historySapce = 0;
      }
      pdu.lenght = pointBuf.lenght + sizeof(history_t);
//      FlashFIFO_put(&historyHead, &pdu);
      //写历史数据到history_file
      history_put(history_head,&pdu);
    }
    memory_release(pdu.payload);
  }
}
/*******************************************************************************
  * @note   0x05  history data upload
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t stateGrid_historyUpload(void) {
  Buffer_t pdu;
  u16 len;

  pdu.size = 1024;
  pdu.lenght = 0;
  pdu.payload = memory_apply(pdu.size);

//  pdu.lenght = FlashFIFO_get(&historyHead, &pdu);
  len=history_get(history_head,&pdu);
  if (len > 0&&len<=STATE_GRID_CMD_SIZE) {
    Buffer_t *buf;

    pdu.lenght=len;

    buf = StateGridCmd_create(0x05, 1, pdu);
    CommonServerDataSend(buf);
    memory_release(buf);
    return 0;
  }

  memory_release(pdu.payload);
  return 1;
}
/*******************************************************************************
  * @note   0x05  history data upload ack
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t stateGrid_historyUploadAck(StateGrid_t *sg) {
#pragma pack(1)
  typedef struct {
    u8_t result;
  } uploadAck_t;
#pragma pack()

  if (sg->cmd->pdu.lenght == sizeof(uploadAck_t)) {
    uploadAck_t *ack = (uploadAck_t *)sg->cmd->pdu.payload;

    if (ack->result == 0x01) {
      APP_DEBUG("State Grid history upload data OK\r\n ");
      APP_DEBUG("\r\n-->state grid upload history success!!!\r\n");
    } else {
      log_save("State Grid history upload data ERR:%d", ack->result);
      APP_DEBUG("\r\n-->state grid upload history fail!!!\r\n");
    }
  } else {
    log_save("State Grid history upload data ERR %d", sg->cmd->pdu.lenght);
  }

  return 0;
}

static Buffer_t  *StateGridCmd_ack(StateGridCmd_t *cmd, FS_e fs, Buffer_t pdu)
{
    ERRR(cmd == null, goto END);
    
    fibo_free(cmd->pdu.payload);

   char tempbuf[64]={0};
    cmd->name=tempbuf;
    for (u8 j = 0; j <number_of_array_elements; j++){
	    if(65 == PDT[j].num){
            u16 cmd_len  = 64;  
		    PDT[j].rFunc(&PDT[j],cmd->name, &cmd_len);
	    }
	}

    cmd->nameLen=strlen(cmd->name);

    cmd->ctr.dir = 1; 
    cmd->ctr.fs = fs;
    cmd->ctr.con = 0;
    //r_memcpy(&cmd->pdu, &pdu, sizeof(Buffer_t));
    memcpy(&cmd->pdu, &pdu, sizeof(Buffer_t));
    cmd->lenght = 1 + cmd->nameLen + 1 + 1+ pdu.lenght;

    return StateGrid_create(cmd);
END:
    return null;
}

/*******************************************************************************
  * @note   0x10 server get station point data
  * @param  None
  * @retval None
*******************************************************************************/
u8_t stateGrid_getData(StateGrid_t *sg) {
#pragma pack(1)
  typedef struct {
    u8_t code;
    u32_t number;
    u8_t flag;
  } rcve_t;

  typedef struct {
    u32_t number;
    u8_t type;
    u8_t time[8];
    u8_t point[0];
  } ack_t;
#pragma pack()

  int i;
  FS_e e;
  Buffer_t pdu;
  Buffer_t pointBuf;
  void *dataBuf;
  Buffer_t *buf;
  ack_t *ack;
  PointArray_t *pointArray;
  rcve_t *rcve = (rcve_t *)sg->cmd->pdu.payload;

  ERRR(sg->cmd->pdu.lenght < sizeof(rcve_t), goto ERR);

  pointArray = null;
  pdu.size = 1024;
  pdu.lenght = 0;

  dataBuf = memory_apply(pdu.size + 4);
  ERRR(dataBuf == null, goto ERR2);
  pdu.payload = ((u8_t *)dataBuf) + 4;

  if (rcve->flag != 0x01) {
    r_memcpy(&i, &sg->cmd->pdu.payload[sizeof(rcve_t)], 4);
    Swap_bigSmallLong((u32_t *)&i);
    ERRR(sg->cmd->pdu.lenght < (4 + sizeof(rcve_t) + 4 * i), goto ERR1);

    pointArray = memory_apply(i * 4);
    ERRR(pointArray == null, goto ERR2);

    pointArray->count = i;
    r_memcpy(pointArray->array, &sg->cmd->pdu.payload[sizeof(rcve_t) + 4], i * 4);
    while (i-- > 0) {
      Swap_bigSmallLong((u32_t *)&pointArray->array[i]);
    }
  }

  ack = (ack_t *)pdu.payload;
  ack->type = 0x01;
  UnixTime_get(ack->time);
  Swap_headTail(ack->time, 8);
  pointBuf.lenght = 0;
  pointBuf.size = pdu.size - sizeof(ack_t);
  pointBuf.payload = ack->point;
  e = FS_ONCE;
NEXT:
  i = StateGrid_dataGet(&pointBuf, null, pointArray, &ack->number);
  if (i != 0) {
    if (e == FS_ONCE) {
      e = FS_START;
    } else if (e == FS_START) {
      e = FS_MID;
    }
  } else if (e != FS_ONCE) {
    e = FS_END;
  }
  APP_DEBUG("\r\n-->state grid get data success!!!\r\n");

  pdu.lenght = pointBuf.lenght + sizeof(ack_t);

  buf = StateGridCmd_ack(sg->cmd, e,  pdu);
  CommonServerDataSend(buf);
  memory_release(buf);
  if (e != FS_ONCE || e != FS_END) {
    goto NEXT;
  }

  return 0;
ERR2:
  log_save("state get data memory full!!");
ERR1:
  memory_release(pointArray);
  memory_release(dataBuf);
  return 0;
ERR:
  log_save("State get rcveice lenght ERR %d", sg->cmd->pdu.lenght);
  return 0;
}
/*******************************************************************************
  * @note   0x21 proof time
  * @param  None
  * @retval None
*******************************************************************************/
u8_t stateGrid_prooftime(StateGrid_t *sg) {
#pragma pack(1)
  typedef struct {
    u8_t timer[8];
  } prooftimeRcve_t;

  typedef struct {
    u8_t result;
  } prooftimeAck_t;
#pragma pack()

  prooftimeAck_t ack ;
  Buffer_t akcBuf;
  Buffer_t *buf;

  if (sg->cmd->pdu.lenght == sizeof(prooftimeRcve_t)) {
    ack.result = 0x01;
  } else {
    log_save("State proof time rcveice lenght ERR %d", sg->cmd->pdu.lenght);
    ack.result = 0x10;
  }

  akcBuf.lenght = sizeof(prooftimeAck_t);
  akcBuf.payload = (u8_t *)&ack;
  buf = StateGridCmd_ack(sg->cmd, FS_ONCE, akcBuf);

  CommonServerDataSend(buf);
  memory_release(buf);
  sg->cmd = null;

  APP_DEBUG("\r\n-->state grid prooftime success!!!\r\n");

  return 0;
}

/*******************************************************************************            
 * introduce: 0x99 send  heartbeat         
 * parameter:                        
 * return:                 
 * author:           Luee                                                    
 *******************************************************************************/
static void stateGrid_heartbeat(void)
{
#pragma pack(1)
    typedef struct
    {
        u8_t code;
        u8_t timer[8];
    }heartbeat_t;
#pragma pack() 

    s32 ret;

    Buffer_t pdu;
    heartbeat_t *heartbeat;

    pdu.size = sizeof(heartbeat_t);
    pdu.lenght = pdu.size;
    //pdu.payload = memory_apply(pdu.size);
    pdu.payload = fibo_malloc(pdu.size);

    if (pdu.payload != null){
        Buffer_t *buf;

        log_d("\r\npdu.payload != null\r\n");

        heartbeat = (heartbeat_t*) pdu.payload;
        heartbeat->code = 0x01;
        UnixTime_get(heartbeat->timer);

        Swap_headTail(heartbeat->timer, sizeof(heartbeat->timer));

        buf = StateGridCmd_create(0x99, 1, pdu);
        fibo_free(pdu.payload);

        CommonServerDataSend(buf);

        //memory_release(buf);
        fibo_free(buf);
    }
}

/*******************************************************************************
  * @note   0x00 login ack
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t stateGrid_heartbeatAck(StateGrid_t *sg) {
#pragma pack(1)
  typedef struct {
    u8_t code;
  } heartbeatAck_t;
#pragma pack()

  if (sg != null && sg->cmd->pdu.lenght == sizeof(heartbeatAck_t)) {
    heartbeatAck_t *ack = (heartbeatAck_t *)sg->cmd->pdu.payload;

    if (ack->code == 0x02) {
      APP_DEBUG("\r\n-->state grid heartbeat success!!!\r\n");
    } else {
      log_save("State Grid heartbeat ERR:%d", ack->code);
      APP_DEBUG("\r\n-->state grid heartbeat fail!!!\r\n");
    }
  }

  return 0;
}

/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
void set_grid_step(u8_t grid_step)
{
  step=grid_step;
}

/*******************************************************************************            
* introduce:        
* parameter:                       
* return:                 
* author:           Luee                                                    
*******************************************************************************/
void set_heartbeatSpace(u32 space)
{
  heartbeatSpace = space;
}

/******************************************************************************/


