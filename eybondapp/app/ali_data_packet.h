#ifndef __ALI_DATA_PACKET_H__
#define __ALI_DATA_PACKET_H__
#include <time.h>
#include "fibo_opencpu.h"
#include "ginlong_monitor.pb-c.h"
#include "eybpub_Debug.h"
#include "L610_conn_ali_net.h"
#include "eybpub_SysPara_File.h"
#include "sci_types.h"
#include "Device.h"
#include "eybapp_appTask.h"
#include "eybpub_utility.h"


#pragma pack(1)
typedef struct
{
    uint16_t product_model;
    uint16_t dsp_software_version;
    uint16_t tft_software_version;
    uint16_t ac_output_type;
    uint16_t dc_input_type;  //10
    uint16_t pac_h;
	uint16_t pac_l;
    uint32_t total_dc_output_power;
    uint16_t e_total_h;//总发电量
    uint16_t e_total_l;
    uint16_t e_month_h;
	uint16_t e_month_l;
    uint16_t e_last_month_h;  //28个字节

} Inverter_Packet1;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint16_t e_last_month_l;
    uint16_t e_today;
    uint16_t e_yesterday;
    uint16_t e_year_h;
    uint16_t e_year_l;
    uint16_t e_last_year_h;
    uint16_t e_last_year_l;
    uint16_t reserve;
    uint16_t u_pv1;
    uint16_t i_pv1;
    uint16_t u_pv2;
    uint16_t i_pv2;
    uint16_t u_pv3;//26个字节

} Inverter_Packet2;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint16_t i_pv3;
    uint16_t u_pv4;
    uint16_t i_pv4;
    uint16_t reserve1;
    uint16_t reserve2;
    uint16_t dc_bus ;//直流母线电压
    uint16_t dc_bus_half;
    uint16_t u_ac1; //A相电压
    uint16_t u_ac2;
    uint16_t u_ac3;
    uint16_t i_ac1;//A相电流
    uint16_t i_ac2;
    uint16_t i_ac3;//26个字节

} Inverter_Packet3;
#pragma pack()

#pragma pack(1)
typedef struct
{
    uint16_t reserve3;
    uint16_t reserve4;
    int16_t  inverter_temperature;
    uint16_t fac; //电网频率
    uint16_t current_state;
    uint16_t reserve5;
    uint16_t reserve6;
    uint16_t reserve7;
    uint16_t reserve8;
    uint16_t reserve9;
    uint16_t p_limit_set;//限功率实际值
    int16_t  p_factor_limit_set;//功率因数实际调节值
    uint16_t reserve10;//26个字节

} Inverter_Packet4;
#pragma pack()

#pragma pack(1)
typedef struct
{

    uint16_t reserve11;
    uint16_t national_standards;
    uint16_t power_curve_version;//功率曲线号
    uint32_t reactive_power;//无功功率
    uint32_t apparent_power;
    uint16_t real_time_power_factor;
    uint16_t inverter_sn_h;//逆变器序列高 4 位
    uint16_t inverter_sn_m1;//逆变器序列中 4 位
    uint16_t inverter_sn_m2;//逆变器序列中 4 位
    uint16_t inverter_sn_l;//逆变器序列低 4 位
    uint16_t reserve12;  //26个字节
} Inverter_Packet5;
#pragma pack()

#pragma pack(1)
typedef struct
{

    uint16_t reserve13;
    uint16_t fault1;
    uint16_t fault2;
    uint16_t fault3;
    uint16_t fault4;
    uint16_t fault5;
    uint16_t fault6;
    uint16_t sys_year;
    uint16_t sys_month;
    uint16_t sys_day;
    uint16_t sys_hour;
    uint16_t sys_minute;
    uint16_t sys_second; //26字节

} Inverter_Packet6;
#pragma pack()


void packet_init(void);

int send_monitor_packet(void *aliyun_mqtt_thread_handle);
uint32_t get_data_upload_cycle(void);
uint64_t* get_current_working_tick(void);
uint64_t* get_total_tick(void);
void out_put_buffer(const char *buf, uint32_t len);

extern Inverter_Packet1 *inverter_data1;
extern Inverter_Packet2 *inverter_data2;
extern Inverter_Packet3 *inverter_data3;
extern Inverter_Packet4 *inverter_data4;
extern Inverter_Packet5 *inverter_data5;
extern Inverter_Packet6 *inverter_data6;

extern Inverter_Packet1 packet1 ;
extern Inverter_Packet2 packet2 ;
extern Inverter_Packet3 packet3 ;
extern Inverter_Packet4 packet4 ;
extern Inverter_Packet5 packet5 ;
extern Inverter_Packet6 packet6 ;



typedef struct
{
    uint32_t software_ver;
    char product_key[64];
    char device_name[64];
    char device_secret[64];
    char pub_topic[64];
    char sub_topic[64];
    char TotoalWorkTime[21];
    char inverter_str[16];

    uint32_t data_upload_cycle;
    uint32_t factory_time;
    uint64_t current_working_tick;
    uint64_t total_working_time;
    int32 rssi;
	char iccid[21];
	
	uint32_t lac;//定位LAC,位置区代码
	uint64_t ci;//定位ci,小区代码
} Para_Table;

extern Para_Table para;

void load_config_para(void);
//void set_par_lac_ci(uint32_t lac ,uint32_t ci);
void rrpc_web_at_handle(uint32_t payloadlen,uint8_t *payload);
int send_rrpc_response_packet(void *aliyun_mqtt_thread_handle,uint8_t *respon_topic);
void set_par_lac_ci(uint32_t lac ,uint32_t ci);

#define  HTTP_URL_LEN 200
#define  STR_RET_SIZE 100

extern char http_url[HTTP_URL_LEN];


#endif
