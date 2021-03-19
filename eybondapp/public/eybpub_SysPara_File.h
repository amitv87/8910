/********************** COPYRIGHT 2014-2100, Eybond    ************************
 * @File    : eybpub_SysPara_File.h
 * @Author  : MWY
 * @Date    : 2020-08-19
 * @Brief   :
 ******************************************************************************/
#ifndef __EYBPUB_SYSPARA_FILE_H_
#define __EYBPUB_SYSPARA_FILE_H_

#include "eyblib_typedef.h"

#define SYY_PARA_LEN        40
#define SYY_PARA_SPACE      42
#define SYS_PARA_CHANGE     (0x01F000)

#define BUZER_EN_ADDR       (48)
#define GPRS_CSQ_VALUE      (55)
#define GPRS_CCID_ADDR      (56)
#define GPRS_APN_ADDR       (57)
#ifdef _PLATFORM_BC25_
#define NB_SERVER_ADDR      (21)
#define NB_SERVER_PORT      (24)
#endif
#ifdef _PLATFORM_L610_
#define EYBOND_SERVER_ADDR      (21)
// #define SAJ_SERVER_ADDR         (22)
#define STATE_GRID_SERVER_ADDR  (23)
#define HANERGY_SERVER_ADDR     (24)
/* Sarnath parameters*/
#define SARNATH_SERVER_ADDR     (71)
#define SARNATH_SERVER_PORT     (72)
#define SARNATH_FACTORY_ID      (73)
#define SARNATH_LOGGER_ID       (74)
#define SARNATH_UPLOAD_TIME     (75)
#endif

#define GPRS_BASE_ID            (59)
/* 国网参数 */
#define STATE_GRID_SN           (65)
#define STATE_GRID_USER_NAME    (66)
#define STATE_GRID_PASSWORD     (67)
#define STATE_GRID_REGISTER_ID  (68)

#define LOCAL_TIME              (26)
#define TIME_ZONE_ADDR          (25)

#define DEVICE_MONITOR_NUM      (12)
#define DEVICE_PROTOCOL         (14)
#define METER_PROTOCOL		    (15)    // 电表协议
#define DEVICE_UART_SETTING     (34)    // 采集串口参数

#define DEVICE_VENDER           (8)

#define DEVICE_PNID             (2)     // 获取PN号
#define DEVICE_UID_IMEI         (58)    // 获取模组IMEI

#define DEVICE_REPORT_TIME      (82)
#define DEVICE_SYSTEM           (29)

/* buf :para read or write char point, return : 0(opt OK)*/
typedef int (*paraFun)(Buffer_t *buf);

/*authority
    bit 0(para read);   0(unable), 1(able)
    bit 1(Para write);   0(unable),  1(able)
    bit 2(para W authority); 0(No authorized), 1(Be authorized)
    bit 4(R&W point type);  0(string point), 1(funcation point)
*/
typedef enum {
  P_R = (1 << 0),
  PNR = (0 << 0),
  P_W = (1 << 1),
  PNW = (0 << 1),
  P_A = (1 << 2),
  PNA = (0 << 2),
  P_F = (1 << 4),
  P_S = (0 << 4),
} SysParaAuth_e;

typedef struct {
  u16_t number;       // para number
  u8_t authority;     // goto authority
  u8_t offset;        // para inseat offset
  void *read;         // Read
  void *write;        // write
} SysParaTab_t;

typedef struct {
  u16_t code;
  u16_t  def;  // reset Factory setting, 0,invalid,  1: User,
  char str[28];
} SysDefParaTab_t;

typedef struct {
  u8_t  type;  // connect type 0: UDP 1:TCP 2:SSL 3: LWM2M 4: MQTT
  u16_t port;  // port
  //char addr[2];  //
  char addr[50];
} ServerAddr_t;

// void SysPara_printf(void_fun_bufp output);       // mike 20200825
// void SysPara_Get(u8_t num, Buffer_t *buf);       // mike 20200825
// int  SysPara_Set(u8_t num, Buffer_t *buf);       // mike 20200825
// void SysPara_init(void);
int SysPara_init(void);

void Get_Data(Buffer_t *buf); //test
u8_t SysPara_Get_State(void);

ServerAddr_t *ServerAdrrGet(u8_t num);

unsigned char parametr_set(u32_t number, Buffer_t *data);
void parametr_get(u32_t number, Buffer_t *databuf);
void parametr_default(void);

void GET_ALL_data(void);
void main_parametr_update(void);

#endif  // __EYBPUB_SYSPARA_FILE_H_
/******************************************************************************/

