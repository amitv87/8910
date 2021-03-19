
#include "ali_data_packet.h"
#include "eyblib_memory.h"

static GinlongMonitor__MNotification notification = {0} ;
static GinlongMonitor__MCollector1 collector1   =  {0};
static GinlongMonitor__Packet   packet = {0};
static GinlongMonitor__MInverter1 ** inverter = NULL;
static GinlongMonitor__Packet respone_package = {0};
static GinlongMonitor__MTransmissionAck transmission_ack = {0};

Inverter_Packet1 *inverter_data1 = NULL;
Inverter_Packet2 *inverter_data2 = NULL;
Inverter_Packet3 *inverter_data3 = NULL;
Inverter_Packet4 *inverter_data4 = NULL;
Inverter_Packet5 *inverter_data5 = NULL;
Inverter_Packet6 *inverter_data6 = NULL;

Inverter_Packet1 packet1 = {0};
Inverter_Packet2 packet2 = {0};
Inverter_Packet3 packet3 = {0};
Inverter_Packet4 packet4 = {0};
Inverter_Packet5 packet5 = {0};
Inverter_Packet6 packet6 = {0};


Para_Table para;


#define  CLOUD_TEST_STR  "AT+TEST="                // 判断是不是AT+TEST指令
#define  UPDATE_FIRMWARE "GINFTPC"                 // 升级采集器固件
#define  RS485_TRANSPARENT "GIN485"                // rs485透传


char http_url[HTTP_URL_LEN] = {0};
//char cmd_ret_str[STR_RET_SIZE]={0};//返回
uint8_t send_packet[STR_RET_SIZE] = {0};
uint8_t recv_packet[STR_RET_SIZE] = {0};


u16_t Swap_hex_Char( char *buf,const char * hex, u16_t len, char fill)
{
    u8_t i;
    u16_t l = 0;

    if(fill != 0)
    {
        l = (len << 1) + len;
    }
    else
    {
        l = len << 1;
    }
    while (len-- > 0)
    {
        i = ((*hex)&0xF0) >> 4;
        *buf++ = i > 9 ? i + ('A' - 10) : i + '0';
        i = (*hex++)&0x0F;
        *buf++ = i > 9 ? i + ('A' - 10) : i + '0';
        if (fill != 0)
        {
            *buf++ = fill;
        }
    }
    *buf = '\0';

    return l;
}

//0xA1B2 ----> 0x1A2B 变成
uint16_t byte_swap(uint16_t data)
{
    uint16_t temp = 0;
    temp = ((data & 0xF000)>>4) | ((data & 0xF00)<<4) | ((data & 0x00F0)>>4) | ((data & 0x000F)<<4);
    return temp;
}

//逆变器SN号字符串
void calc_inverter_str()
{
    uint16_t inverter_sn_h = 0 ;
    uint16_t inverter_sn_m1= 0;
    uint16_t inverter_sn_m2 = 0;
    uint16_t inverter_sn_l = 0;

    inverter_sn_h = byte_swap(inverter_data5->inverter_sn_h);
    inverter_sn_m1 = byte_swap(inverter_data5->inverter_sn_m1);
    inverter_sn_m2 = byte_swap(inverter_data5->inverter_sn_m2);
    inverter_sn_l = byte_swap(inverter_data5->inverter_sn_l);

    char hex[] = { (inverter_sn_h&0xff00)>>8,(inverter_sn_h&0xff),(inverter_sn_m1&0xff00)>>8,(inverter_sn_m1&0xff),(inverter_sn_m2&0xff00)>>8,(inverter_sn_m2&0xff),(inverter_sn_l&0xff00)>>8,(inverter_sn_l&0xff)};
    Swap_hex_Char(para.inverter_str,hex,sizeof(hex)/sizeof(hex[0]),0);
    para.inverter_str[15] = 0;
    APP_PRINT("sn : %s\r\n",para.inverter_str);
}

void out_put_buffer(const char *buf, uint32_t len)
{
    u32_t displayNum = 0;

    if (len < 512)
    {
        u8_t *str = fibo_malloc(len * 3 + 8);
        if (str != null)
        {
            int l = Swap_hex_Char((char *)str, buf, len, ' ');
            while (l)
            {
                if (l >= 16 * 3)
                {
                    Debug_output(str + displayNum, 16 * 3);
                    l -= 16 * 3;
                    displayNum += 16 * 3;
                }
                else
                {
                    Debug_output(str + displayNum, l);
                    l = 0;
                }
                Debug_output((u8_t *)"\r\n", 2);
            }

            if (str != NULL)
            {
                fibo_free(str);
            }
        }
    }
}




/*******************************************************************************
 * 将时间格式字符串转成时间戳形式
 * 如 2011-12-31 11:43:07 转成 1325302987
*******************************************************************************/

uint32_t get_tick(char *str_time)
{
    struct tm stm;
    int iY, iM, iD, iH, iMin, iS;

    APP_PRINT("tick = %s\r\n",str_time);
    memset(&stm,0,sizeof(stm));

    iY = atoi(str_time);
    iM = atoi(str_time+5);
    iD = atoi(str_time+8);
    iH = atoi(str_time+11);
    iMin = atoi(str_time+14);
    iS = atoi(str_time+17);

    stm.tm_year=iY-1900;
    stm.tm_mon=iM-1;
    stm.tm_mday=iD;
    stm.tm_hour=iH-8;
    stm.tm_min=iMin;
    stm.tm_sec=iS;

    return mktime(&stm);
}

/*******************************************************************************
 * 将字符串字符转成16进制数  如 "6032011300000003" --> 0x6032011300000003
*******************************************************************************/

uint64_t str_to_hex(char *str)
{
    uint64_t get_int=0 ;
    if(str==NULL)
        return 1 ;
    while(*str !='\0')
    {
        get_int = get_int*16;
        if( (*str>=0x30) && (*str<=0x39)) //0-9
        {
            get_int += *str-0x30;
        }
        else if( (*str>=0x41) && (*str<=0x46))//A-F
            get_int += (*str-0x41+10);
        else if((*str>=0x61) && (*str<=0x66))//a-f
            get_int += (*str-0x61+10);
        else
            return 0;
        str++;
    }
    return get_int ;
}

/*******************************************************************************
 * 删除字符串中某个字符
*******************************************************************************/

char *del_char(char *str, char ch)
{
    unsigned char i=0,j=0;
    if((0==strlen(str))||str ==NULL)
        return NULL;

    while(str[i] != '\0')
    {
        if(str[i] != ch)
        {
            str[j++] = str[i];
        }
        i++;  //源一直移动
    }

    str[j] = '\0';
    return str;
}


/*******************************************************************************
 * 将5号参数固件版本中的'.'去掉,并转成16进制整数
 * uint32_t collector_version_convert()
*******************************************************************************/
uint32_t collector_version_convert(char * str)
{
    if(NULL != del_char(str,'.'))
        return(str_to_hex(str));
    else
        return 0;
}


void set_collector_packet_data(GinlongMonitor__MCollector1 *ginlongmonitor__mcollector1)
{

    if (ginlongmonitor__mcollector1 == NULL)
        return;

    ginlongmonitor__mcollector1->collector_sn                  = str_to_hex(para.device_name);
    ginlongmonitor__mcollector1->total_working_time            = para.total_working_time + para.current_working_tick;
    ginlongmonitor__mcollector1->current_working_time          = para.current_working_tick;
    ginlongmonitor__mcollector1->data_upload_cycle             = para.data_upload_cycle;
    ginlongmonitor__mcollector1->maximum_number                = 1;
    ginlongmonitor__mcollector1->actual_number                 = 1;
    ginlongmonitor__mcollector1->collector_version             = para.software_ver;
    // ginlongmonitor__mcollector1->collector_version             = 554107154;//采集器的版本需与锦浪沟通
    //ginlongmonitor__mcollector1->collector_version             = 0x3A010000;

    ginlongmonitor__mcollector1->factory_time                  = para.factory_time;
    ginlongmonitor__mcollector1->rssi                          = para.rssi;
    ginlongmonitor__mcollector1->rssi_level                    = para.rssi/5;
    ginlongmonitor__mcollector1->iccid                         = para.iccid;
    ginlongmonitor__mcollector1->ci                            = para.ci;
    ginlongmonitor__mcollector1->lac                           = para.lac;
    ginlongmonitor__mcollector1->is_realtime                   = 0;
}


/*
 *高8位与低8位数据交换
 *如 0xA1A2 --> 0xA2A1
 */
uint16_t H8_Swap_L8(uint16_t data)
{
    return ((data&0xff00)>>8) | ((data&0x00ff)<<8);
}

/**
 *两个16位数据变成一个32位数
 *0x1234 0x4567 -->  0x34126745
 */
uint32_t data32_calc(uint16_t H16 ,uint16_t L16)
{
    uint32_t datah = 0;
    uint32_t datal = 0;
    datah = H8_Swap_L8(H16);
    datal = H8_Swap_L8(L16);
    return (datah << 16) | datal;
}


void set_inverter_packet_data(GinlongMonitor__MInverter1  *ginlongmonitor__minverter1)
{

    if (ginlongmonitor__minverter1 == NULL)
        return;
    calc_inverter_str();
    ginlongmonitor__minverter1->is_realtime                         = 0;
    ginlongmonitor__minverter1->inverter_sn.len                     =0;
    ginlongmonitor__minverter1->inverter_sn.data                    =NULL;
    ginlongmonitor__minverter1->inverter_sn2                        =para.inverter_str ;
    ginlongmonitor__minverter1->inverter_temperature                =H8_Swap_L8(inverter_data4->inverter_temperature);
    ginlongmonitor__minverter1->product_model                       =H8_Swap_L8(inverter_data1->product_model);
    ginlongmonitor__minverter1->national_standards                  =H8_Swap_L8(inverter_data5->national_standards);
	
    ginlongmonitor__minverter1->u_pv1                               =H8_Swap_L8(inverter_data2->u_pv1);
    ginlongmonitor__minverter1->i_pv1                               =H8_Swap_L8(inverter_data2->i_pv1);

    ginlongmonitor__minverter1->u_pv2                               =H8_Swap_L8(inverter_data2->u_pv2);
    ginlongmonitor__minverter1->i_pv2                               =H8_Swap_L8(inverter_data2->i_pv2);
    ginlongmonitor__minverter1->u_pv3                               =H8_Swap_L8(inverter_data2->u_pv3);
    ginlongmonitor__minverter1->i_pv3                               =H8_Swap_L8(inverter_data3->i_pv3);

	
    ginlongmonitor__minverter1->u_pv4                               =H8_Swap_L8(inverter_data3->u_pv4);
    ginlongmonitor__minverter1->i_pv4                               =H8_Swap_L8(inverter_data3->i_pv4);
    ginlongmonitor__minverter1->u_ac1                               =H8_Swap_L8(inverter_data3->u_ac1);
    ginlongmonitor__minverter1->u_ac2                               =H8_Swap_L8(inverter_data3->u_ac2);
    ginlongmonitor__minverter1->u_ac3                               =H8_Swap_L8(inverter_data3->u_ac3);
    ginlongmonitor__minverter1->i_ac1                               =H8_Swap_L8(inverter_data3->i_ac1);
    ginlongmonitor__minverter1->i_ac2                               =H8_Swap_L8(inverter_data3->i_ac2);
    ginlongmonitor__minverter1->i_ac3                               =H8_Swap_L8(inverter_data3->i_ac3);
    ginlongmonitor__minverter1->fac                                 =H8_Swap_L8(inverter_data4->fac);
    ginlongmonitor__minverter1->e_today                             =H8_Swap_L8(inverter_data2->e_today);


    ginlongmonitor__minverter1->current_state                       =H8_Swap_L8(inverter_data4->current_state);
    ginlongmonitor__minverter1->inverter_software_version           =data32_calc(inverter_data1->tft_software_version,inverter_data1->dsp_software_version);//tft version  in front
    APP_PRINT("ginlongmonitor__minverter1->inverter_software_version = %lX\r\n ",ginlongmonitor__minverter1->inverter_software_version);
    ginlongmonitor__minverter1->pac                                 =data32_calc(inverter_data1->pac_h,inverter_data1->pac_l);
    ginlongmonitor__minverter1->e_month                             =data32_calc(inverter_data1->e_month_h,inverter_data1->e_month_l);
    ginlongmonitor__minverter1->e_year                              =data32_calc(inverter_data2->e_year_h,inverter_data2->e_year_l);
    ginlongmonitor__minverter1->e_total                             =data32_calc(inverter_data1->e_total_h,inverter_data1->e_total_l);

    APP_PRINT("ginlongmonitor__minverter1->e_today = %lX\r\n ",ginlongmonitor__minverter1->e_today);
    APP_PRINT("ginlongmonitor__minverter1->e_month = %llX\r\n ",ginlongmonitor__minverter1->e_month);
    APP_PRINT("ginlongmonitor__minverter1->e_year = %llX\r\n ",ginlongmonitor__minverter1->e_year);
    APP_PRINT("ginlongmonitor__minverter1->e_total = %llX\r\n ",ginlongmonitor__minverter1->e_total);

	APP_PRINT("ginlongmonitor__minverter1->e_today = %ld\r\n ",ginlongmonitor__minverter1->e_today);
    APP_PRINT("ginlongmonitor__minverter1->e_month = %lld\r\n ",ginlongmonitor__minverter1->e_month);
    APP_PRINT("ginlongmonitor__minverter1->e_year = %lld\r\n ",ginlongmonitor__minverter1->e_year);
    APP_PRINT("ginlongmonitor__minverter1->e_total = %lld\r\n ",ginlongmonitor__minverter1->e_total);

}
void set_transmission_ack_packet(GinlongMonitor__MTransmissionAck * transmission_ack)
{
    if(NULL == transmission_ack)
        return;
    transmission_ack->send.len =strlen((char *)send_packet) ;
    transmission_ack->send.data = send_packet;

    transmission_ack->recv.len = strlen((char *)recv_packet) ;
    transmission_ack->recv.data = recv_packet;
}

#define EYBOND_DEFAULT_ALI_PRODUCTKEY    "a1WBg7jGkkT"
#define EYBOND_DEFAULT_ALI_DEVICENAME    "L610_MQTT_ALI_DEV_1"
#define EYBOND_DEFAULT_ALI_DEVICESECRET  "13bb5d52d7c846f193df838c441e710a"


void load_config_para(void)
{
    Buffer_t databuf;
    char software_ver[10]= {0};
    r_memset(software_ver, '\0', sizeof(software_ver));
    databuf.lenght = 0;
    databuf.size = 0;
    databuf.payload = NULL;
    parametr_get(98, &databuf);//软件版本要遵循锦浪的版本规则
    if (databuf.payload != NULL) {
      strncpy(software_ver, (char *)databuf.payload,databuf.lenght+1);//软件版本
      memory_release(databuf.payload);
    } else {
      strncpy(software_ver, "0.0.0.0", r_strlen("0.0.0.0"));
    }
    para.software_ver = str_to_hex(software_ver);
    APP_PRINT("after of cov software_ver = %lx\r\n",para.software_ver);

    databuf.lenght = 0;
    databuf.size = 0;
    parametr_get(92, &databuf);
    if (databuf.payload != NULL) {
      if (databuf.lenght > 1) {
        strncpy(para.product_key,(char *)databuf.payload,databuf.lenght+1);//product_key
      } else {
        strncpy(para.product_key,EYBOND_DEFAULT_ALI_PRODUCTKEY,r_strlen(EYBOND_DEFAULT_ALI_PRODUCTKEY));
      }
      memory_release(databuf.payload);
    } else {
      strncpy(para.product_key,EYBOND_DEFAULT_ALI_PRODUCTKEY,r_strlen(EYBOND_DEFAULT_ALI_PRODUCTKEY));
    }
    APP_PRINT("product_key = %s\r\n",para.product_key);

    databuf.lenght = 0;
    databuf.size = 0;
    parametr_get(93, &databuf);
    if (databuf.payload != NULL) {
      if (databuf.lenght > 1) {
        strncpy(para.device_name,(char *)databuf.payload,databuf.lenght+1);//device_name
      } else {
        strncpy(para.device_name, EYBOND_DEFAULT_ALI_DEVICENAME, r_strlen(EYBOND_DEFAULT_ALI_DEVICENAME));
      }
      memory_release(databuf.payload);
    } else {
      strncpy(para.device_name, EYBOND_DEFAULT_ALI_DEVICENAME, r_strlen(EYBOND_DEFAULT_ALI_DEVICENAME));
    }
    APP_PRINT("device_name = %s\r\n",para.device_name);

    databuf.lenght = 0;
    databuf.size = 0;
    parametr_get(94, &databuf);
    if (databuf.payload != NULL) {
      if (databuf.lenght > 1) {
        strncpy(para.device_secret,(char *)databuf.payload,databuf.lenght+1);//device_secret
      } else {
        strncpy(para.device_secret, EYBOND_DEFAULT_ALI_DEVICESECRET, r_strlen(EYBOND_DEFAULT_ALI_DEVICESECRET));
      }
      memory_release(databuf.payload);
    } else {
      strncpy(para.device_secret, EYBOND_DEFAULT_ALI_DEVICESECRET, r_strlen(EYBOND_DEFAULT_ALI_DEVICESECRET));
    }
    APP_PRINT("device_secret = %s\r\n",para.device_secret);

    databuf.lenght = 0;
    databuf.size = 0;
    parametr_get(95, &databuf);
    if (databuf.payload != NULL) {
      if (databuf.lenght > 1) {
        strncpy(para.pub_topic,(char *)databuf.payload,databuf.lenght+1);//pub_topic
      } else {
        strncpy(para.pub_topic, "hello", r_strlen("hello"));
      }
      memory_release(databuf.payload);
    } else {
      strncpy(para.pub_topic, "hello", r_strlen("hello"));
    }
    APP_PRINT("pub_topic = %s\r\n",para.pub_topic);

    databuf.lenght = 0;
    databuf.size = 0;
    parametr_get(96, &databuf);
    if (databuf.payload != NULL) {
      if (databuf.lenght > 1) {
        strncpy(para.sub_topic,(char *)databuf.payload,databuf.lenght+1);//sub_topic
      } else {
        strncpy(para.sub_topic, "world", r_strlen("world"));
      }
      memory_release(databuf.payload);
    } else {
      strncpy(para.sub_topic, "world", r_strlen("world"));
    }
    APP_PRINT("sub_topic = %s\r\n",para.sub_topic);

    databuf.lenght = 0;
    databuf.size = 0;
    parametr_get(97, &databuf);//采集器总共工作时间不是26号参数系统当前运行时间
    
    if (databuf.payload != NULL) {
      if (databuf.lenght > 1) {
        strncpy(para.TotoalWorkTime,(char *)databuf.payload,databuf.lenght+1);//TotoalWorkTime
      } else {
        strncpy(para.TotoalWorkTime, "0", r_strlen("0"));
      }
      memory_release(databuf.payload);
    } else {
      strncpy(para.TotoalWorkTime, "0", r_strlen("0"));
    }
    APP_PRINT("TotoalWorkTime = %s\r\n", para.TotoalWorkTime);
    para.total_working_time = atoi(para.TotoalWorkTime);
    APP_PRINT("total_working_time = %lld\r\n", para.total_working_time);

    databuf.lenght = 0;
    databuf.size = 0;
    parametr_get(82, &databuf); //设备数据上报周期
    APP_PRINT("data_upload_cycle = %s \r\n",databuf.payload);
	APP_PRINT("data_upload_len = %d \r\n",databuf.lenght);
    if (databuf.payload != NULL) {
      if (databuf.lenght >= 1) {
        para.data_upload_cycle = atoi((char *)databuf.payload); //data_upload_cycle
      } else {
        para.data_upload_cycle = 300;
      }
      memory_release(databuf.payload);
    } else {
      para.data_upload_cycle = 300;
    }
    APP_PRINT("data_upload_cycle = %ld\r\n",para.data_upload_cycle);


    databuf.lenght = 0;
    databuf.size = 0;
    parametr_get(07, &databuf);//出厂时间
    if (databuf.payload != NULL) {
      if (databuf.lenght > 1) {
        para.factory_time = get_tick((char *)databuf.payload); //factory_time
      } else {
        para.factory_time = get_tick("2020-12-10 20:40:00");
      }
      memory_release(databuf.payload);
    } else {
      para.factory_time = get_tick("2020-12-10 20:40:00");
    }
    APP_PRINT("factory_time = %ld\r\n",para.factory_time );

    databuf.lenght = 0;
    databuf.size = 0;
    parametr_get(55, &databuf);//基站信号强度
    if (databuf.payload != NULL) {
      if (databuf.lenght > 1) {
        para.rssi = atoi((char *)databuf.payload); //rssi
      } else {
        para.rssi = 0;
      }
      memory_release(databuf.payload);
    } else {
      para.rssi = 0;
    }
    APP_PRINT("rssi = %ld\r\n", para.rssi);




    databuf.lenght = 0;
    databuf.size = 0;
    parametr_get(56, &databuf);
    
    if (databuf.payload != NULL) {
      if (databuf.lenght > 1) {
        strncpy(para.iccid,(char *)databuf.payload,databuf.lenght+1); //iccid
      } else {
        strncpy(para.iccid, "0", r_strlen("0"));
      }
      memory_release(databuf.payload);
    } else {
      strncpy(para.iccid, "0", r_strlen("0"));
    }
    APP_PRINT("para.iccid = %s\r\n",para.iccid);

}

int send_monitor_packet(void *aliyun_mqtt_thread_handle)
{
    int buf_len  = 0;
    int ret = 0;
    void *buf = NULL;

    inverter = (GinlongMonitor__MInverter1**)fibo_malloc(sizeof(GinlongMonitor__MInverter1*));
    inverter[0]=(GinlongMonitor__MInverter1*)fibo_malloc(sizeof(GinlongMonitor__MInverter1));


    ginlong_monitor__packet__init(&packet);
    ginlong_monitor__m__notification__init(&notification);
    ginlong_monitor__m_collector1__init(&collector1);
    ginlong_monitor__m__inverter1__init(inverter[0]);


    set_collector_packet_data(&collector1);
    set_inverter_packet_data(inverter[0]);
    notification.collector11 = &collector1;

    notification.n_inverter1      = 1;
    notification.inverter1 = inverter;

    packet.no++;
    packet.sn    = str_to_hex(para.device_name);
    packet.notification = &notification;

    buf_len = ginlong_monitor__packet__get_packed_size(&packet);//得到包的大小

    if ((buf = fibo_malloc(buf_len)) == NULL)
        return -1;

    ret = ginlong_monitor__packet__pack(&packet, buf);//打包

    APP_PRINT("mqtt send:\r\n");
    out_put_buffer((const char *)buf,buf_len);
    APP_PRINT("\r\n");

    fibo_aliyunMQTT_cloudPub_FixedLen(aliyun_mqtt_thread_handle,para.pub_topic,1,buf,buf_len);
    fibo_free(buf);
    fibo_free(inverter[0]);
    fibo_free(inverter);
    return 0;
}


uint32_t get_data_upload_cycle(void)
{
    return para.data_upload_cycle;
}

uint64_t* get_current_working_tick(void)
{
    return &para.current_working_tick;
}

uint64_t* get_total_tick(void)
{
    return &para.total_working_time;
}


//收到逆变器数据应答
u8_t invertor_data_ack(Device_t *dev)
{

    DeviceCmd_t *cmd;
	uint8_t len = 0;
    cmd = (DeviceCmd_t *)dev->cmdList.node->payload;
	
	//out_put_buffer(cmd->ack.payload,cmd->ack.lenght);
	
	memset(send_packet,0,STR_RET_SIZE); 
    len = snprintf((char *)send_packet, STR_RET_SIZE, "%s","+GIN485:OK ");
	Swap_hex_Char((char *)send_packet+len,(char *)cmd->ack.payload,cmd->ack.lenght,' ');
	APP_PRINT("web up:%s\r\n",send_packet);
	send_rrpc_response_packet(aliyun_mqtt_thread_handle,rrpc_response_topic);
    Device_remove(dev);
    return 0;
}


//逆变器数据透传
u8_t invertor_data_trans(uint8_t *data_ptr ,uint8_t data_len)
{
    Device_t *dev;
    DeviceCmd_t *cmd;

    APP_DEBUG("devtrans\r\n");

    dev = list_nodeApply(sizeof(Device_t));
    cmd = list_nodeApply(sizeof(DeviceCmd_t));

    cmd->waitTime = 1500;     // 1500=1.5 sec
    cmd->state = 0;

    cmd->ack.size = DEVICE_ACK_SIZE;
    cmd->ack.payload = memory_apply(cmd->ack.size);
    cmd->ack.lenght = 0;
    cmd->cmd.size = data_len;
    cmd->cmd.lenght = cmd->cmd.size;
    cmd->cmd.payload = memory_apply(cmd->cmd.size);  // 重新申请存放输入指令的内存
    r_memcpy(cmd->cmd.payload, data_ptr, cmd->cmd.size);

    APP_PRINT("cmd.payload:");
    out_put_buffer((char *)cmd->cmd.payload,cmd->cmd.size);
    APP_PRINT("\r\n");

    dev->cfg = null;  // 配置一个执行该指令的设备
    dev->callBack = invertor_data_ack;  // 设置透传设备需要的返回函数
   // dev->callBack = NULL;  // 设置透传设备需要的返回函数
    //dev->explain = esp;   // 设备携带指令节点
    dev->type = DEVICE_TRANS;

    list_init(&dev->cmdList);
    list_bottomInsert(&dev->cmdList, cmd);
    Device_inset(dev);    // 将需要执行指令的设备放入Devicelist，交由deviceCmdSend处理

//  Eybpub_UT_SendMessage(EYBDEVICE_TASK, DEVICE_CMD_ID, 0, 0);

    return 1;
}




//web下发的控制逆变器透传指令
void web_transparent(char *parm_ptr)
{
    uint8_t * data_ptr = NULL;
    char *p_str = NULL;
    uint8_t str_len = 0;
//   APP_PRINT("rs485_transparent = %s \r\n",parm_ptr);//GIN485:00 10 0b b7 00 06 0c ff e2 00 01 00 01 00 08 00 00 00 00 80 60
    str_len = strlen(parm_ptr + strlen("GIN485:"))+1;
//   APP_PRINT("rs485_len = %d \r\n",str_len);
    data_ptr = (uint8_t *)fibo_malloc(str_len/3);
    memset(data_ptr,0,str_len/3);

    data_ptr[0] = strtol(strtok(parm_ptr + strlen("GIN485:")," "),&p_str,16);//
    for(int i=1; i<str_len/3; i++)
    {
        data_ptr[i] = strtol(strtok(NULL," "),&p_str,16);
    }
    invertor_data_trans(data_ptr,str_len/3) ;
    fibo_free(data_ptr);
}


//解析web下发的AT指令
void web_atcmd_parse(char* parm_ptr)
{
    uint8_t len = 0;
    if(memcmp((const char*)parm_ptr,(const char*)UPDATE_FIRMWARE,strlen(UPDATE_FIRMWARE))==0)           /* 升级采集器固件*/
    {
        memset(http_url,0,HTTP_URL_LEN);
        char * begin_ptr = (char*)parm_ptr + strlen("GINFTPC,h");
        
        strcpy(http_url,begin_ptr);
        http_url[0] = 'h';
		http_url[1] = 't';
		http_url[2] = 't';
		http_url[3] = 'p';
        APP_PRINT("http_url = %s\r\n",http_url);
		

        memset(send_packet,0,STR_RET_SIZE); 
        snprintf((char *)send_packet, STR_RET_SIZE, "%s","+GINFTPC:OK");
		
		send_rrpc_response_packet(aliyun_mqtt_thread_handle,rrpc_response_topic);
        Eybpub_UT_SendMessage(FOTA_TASK, FIRMWARE_UPDATE, 0, 0);
    }
    else if(memcmp((const char*)parm_ptr,(const char*)RS485_TRANSPARENT,strlen(RS485_TRANSPARENT))==0)/*透传*/
    {
        web_transparent(parm_ptr) ;
    }

}


/*
 * 解析锦浪web端以RRPC格式下发的AT+TEST控制命令和AT命令 rrpc_web_at_handle
 */
void rrpc_web_at_handle(uint32_t payloadlen,uint8_t *payload)
{
    GinlongMonitor__Packet *request_package = NULL ;
    char*  parm_ptr = NULL ;

    request_package = ginlong_monitor__packet__unpack(NULL,payloadlen,payload) ; //解包

    if(request_package->transmission !=NULL)     // RRPC 远程控制指令为空
    {
        APP_PRINT("request_package->transmission:%s",request_package->transmission->send.data) ;
        APP_PRINT("\r\n");
        /* 判断是AT+TEST 指令*/
        if(strstr((const char *)request_package->transmission->send.data,(const char *)CLOUD_TEST_STR) != NULL)
        {
            parm_ptr = fibo_malloc(request_package->transmission->send.len+3) ;    // 3 \r \n \0
            if(!parm_ptr)
            {
                ginlong_monitor__packet__free_unpacked(request_package,NULL) ;
                return;
            }
            memcpy(parm_ptr,request_package->transmission->send.data,request_package->transmission->send.len) ;
            parm_ptr[request_package->transmission->send.len] = '\0' ;
            APP_PRINT("parm_ptr = %s \r\n",parm_ptr);

            web_atcmd_parse((char *)parm_ptr+strlen(CLOUD_TEST_STR)) ;// 去除 AT+TEST=

            fibo_free(parm_ptr) ;
        }
    }
    ginlong_monitor__packet__free_unpacked(request_package,NULL) ;
}

//阿里rrpc应答包
int send_rrpc_response_packet(void *aliyun_mqtt_thread_handle,uint8_t *respon_topic)
{
    bool ret = false;
    uint8_t *rrpc_pack_buff  = NULL;
    uint16_t rrpc_pack_len  = 0;

    if(NULL == respon_topic)
        return ret;
    APP_PRINT("respon_topic = %s\r\n",respon_topic);
    ginlong_monitor__packet__init(&respone_package);
    ginlong_monitor__m__transmission__ack__init(&transmission_ack);
	
    set_transmission_ack_packet(&transmission_ack);

    respone_package.sn = str_to_hex(para.device_name);
    respone_package.transmission_ack = &transmission_ack ;

    rrpc_pack_len =  ginlong_monitor__packet__get_packed_size(&respone_package) ;
    rrpc_pack_buff = fibo_malloc(rrpc_pack_len) ;
    if(!rrpc_pack_buff)
    {
        APP_PRINT("rrpc_pack_buff malloc failure \r\n");
        return ret;
    }
    else
    {
        memset(rrpc_pack_buff,0,rrpc_pack_len) ;
        ginlong_monitor__packet__pack(&respone_package,rrpc_pack_buff) ;
        ret = fibo_aliyunMQTT_cloudPub_FixedLen(aliyun_mqtt_thread_handle,(char *)respon_topic,0,rrpc_pack_buff,rrpc_pack_len) ;
        if (!ret)
        {
            APP_PRINT("rrpc_response failed\r\n");
        }
        else
        {
            APP_PRINT("rrpc_response success\r\n");
        }
        fibo_free(rrpc_pack_buff) ;
    }
    return ret;
}

void set_par_lac_ci(uint32_t lac ,uint32_t ci)//设置参数lac ci
{
    para.lac = lac;
    para.ci = (uint64_t)ci;
//  APP_PRINT("tac = %d\r\n",para.lac);
//  APP_PRINT("ci  = %d\r\n",para.ci);
}





