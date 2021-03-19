#include <ctype.h>
#include <stdio.h>
//#include "FreeRTOS.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "nvm.h"
#include <vfs.h>
#include "drv_gpio.h"
#include <cfw.h>
#include "at_command.h"
//#include "osi_api.h"

#define FIBOCOM_COMMON_NV_BUF_SIZE (512 * 24)
#define FIBOCOM_COMMON_SNV_BUF_SIZE (512 * 4) //  need reback

#define FIBOCOM_WAKELOCK_STACK_SIZE (512 * 2)
#define FIBOCOM_WAKELOCK_ENEVT_MAX 16

#define FIBOCOM_COMMON_NV_FNAME CONFIG_FS_AP_NVM_DIR "/fibo_common.nv"
#define FIBOCOM_COMMON_SNV_FNAME CONFIG_FS_AP_NVM_DIR "/fibo_sback.nv"
#define FIBOCOM_NV_NAME_MAX 40


#ifdef CONFIG_FIBOCOM_LC610
#define HAL_WAKEUP_OUT_PIN_INDEX 144
#define HAL_DRING_PIN_INDEX 13

#define HAL_WAKEUP_IN_PIN_INDEX 120
#define HAL_DTR_PIN_INDEX 46

#define FIBO_GTFMODE_PIN_INDEX 43
#define FIBO_DCD_PIN_INDEX 45

#define FIBO_HEADSET_PIN_INDEX 56
#elif defined CONFIG_FIBOCOM_LG610
#define HAL_WAKEUP_OUT_PIN_INDEX 144
#define HAL_DRING_PIN_INDEX 144

#define HAL_WAKEUP_IN_PIN_INDEX 73
#define HAL_DTR_PIN_INDEX 144

#define FIBO_GTFMODE_PIN_INDEX 144
#define FIBO_DCD_PIN_INDEX 144

#define FIBO_HEADSET_PIN_INDEX 144
#elif defined (CONFIG_FIBOCOM_MC610)||defined (CONFIG_FIBOCOM_MC615)
#define HAL_WAKEUP_OUT_PIN_INDEX 61
#define HAL_DRING_PIN_INDEX 22

#define HAL_WAKEUP_IN_PIN_INDEX 41
#define HAL_DTR_PIN_INDEX 15

#define FIBO_GTFMODE_PIN_INDEX 144
#define FIBO_DCD_PIN_INDEX 17

#define FIBO_HEADSET_PIN_INDEX 85

#else
#define HAL_WAKEUP_OUT_PIN_INDEX 144
#if defined(CONFIG_FIBOCOM_HAISL)
#define HAL_DRING_PIN_INDEX 2
#else
#define HAL_DRING_PIN_INDEX  62
#endif

#define HAL_WAKEUP_IN_PIN_INDEX 1
#define HAL_DTR_PIN_INDEX 66

#define FIBO_GTFMODE_PIN_INDEX 4
#define FIBO_DCD_PIN_INDEX 63
#endif

#define FIBO_COMMON_QUEUE_LEN 8

#define FIBO_AT_CMD_COMMON_RSP_LEN 64
#define FIBO_AT_CMD_ENLARGE_RSP_LEN 512


#define FIBO_FFS_DIR CONFIG_FS_SYS_MOUNT_POINT "FFS"
#ifdef CONFIG_FIBOCOM_HELLOBIKE
#ifdef CONFIG_BOARD_WITH_EXT_FLASH
#define FIBO_HB_FFS_AUDIO_DIR CONFIG_FS_EXT_MOUNT_POINT "/Audio"
#define FIBO_HB_FFS_LOG_DIR CONFIG_FS_EXT_MOUNT_POINT "/Log"
#define FIBO_HB_FFS_TMP_DIR CONFIG_FS_EXT_MOUNT_POINT "/Tmp"
#define FIBO_HB_FFS_FW_DIR CONFIG_FS_EXT_MOUNT_POINT "/Firmware"
#define FIBO_HB_FFS_MD_DIR CONFIG_FS_EXT_MOUNT_POINT "/Media"
#define FIBO_HB_FFS_PT_DIR CONFIG_FS_EXT_MOUNT_POINT "/Picture"
#define FIBO_HB_FFS_SK_DIR CONFIG_FS_EXT_MOUNT_POINT "/Socket"
#else
#define FIBO_HB_FFS_DIR  CONFIG_FS_SYS_MOUNT_POINT "ST"
#define FIBO_HB_FFS_AUDIO_DIR CONFIG_FS_SYS_MOUNT_POINT "/ST/Audio"
#define FIBO_HB_FFS_LOG_DIR CONFIG_FS_SYS_MOUNT_POINT "/ST/Log"
#define FIBO_HB_FFS_TMP_DIR CONFIG_FS_SYS_MOUNT_POINT "/ST/Tmp"
#define FIBO_HB_FFS_FW_DIR CONFIG_FS_SYS_MOUNT_POINT "/ST/Firmware"
#define FIBO_HB_FFS_MD_DIR CONFIG_FS_SYS_MOUNT_POINT "/ST/Media"
#define FIBO_HB_FFS_PT_DIR CONFIG_FS_SYS_MOUNT_POINT "/ST/Picture"
#define FIBO_HB_FFS_SK_DIR CONFIG_FS_SYS_MOUNT_POINT "/ST/Socket"
#endif
#endif

#define EV_FIBO_DTR_FALLING 0xB0000000
#define EV_FIBO_FTP_CLOSE 0xB0000001

typedef uint8_t (*temperature_callback)();

typedef struct
{
    uint8_t type;
    uint8_t rate;
    temperature_callback callback;
} st_mtsm;

typedef struct
{
    osiTimer_t *mtsm_event_timer;
    uint32_t period;
} mtsm_timer;

typedef struct
{
    uint16_t wakeup_mode;
    uint16_t wakeup_state;
} wakeup_cfg;

typedef struct
{
    uint16_t wrim_type0;
    uint16_t duration0;
    uint16_t wrim_type1;
    uint16_t duration1;
    uint16_t wrim_type2;
    uint16_t duration2;
} wrim_cfg;

typedef struct
{
    uint16_t lpm_mode;
    uint16_t lpm_submode;
} lpmmode;

typedef struct
{
    uint16_t delay_out;
    uint16_t delay_in;
    uint16_t sleep_again;
} gttime;

typedef enum
{
    NV_ITEM_LPG_SWITCH,
    NV_ITEM_LPMMODE,
    NV_ITEM_GTTIME,
    NV_ITEM_GTFMODE,
    NV_ITEM_LPG_CFG,
    NV_ITEM_KEYPAD_SWITCH,
    NV_ITEM_CHARGE_SWITCH,
	NV_ITEM_DUMP_CAUSE,
    NV_ITEM_MAX
} nv_item_index;

typedef struct
{
    nv_item_index nv_item_id;
    //uint16_t nv_item_size;
} fibo_nv_cfg;

typedef struct
{
    bool lpg_switch;
    lpmmode lpm_mode;
    gttime gt_time;
	uint8_t gt_fmode;
	bool lpg_cfg;
	bool keypad_switch;
	bool charge_switch;
	bool bootcause_switch;
} fibo_nv_data;

typedef struct
{
    uint16_t pin_dex;
    uint16_t gpio_index;
	volatile uint32_t *iomux_reg;
	uint8_t  reg_mode;
} pin_map_gpio_t;

typedef struct
{
	uint16_t pin_dex;	
	volatile uint32_t *key_reg;
}key_map_pin_t;



typedef struct
{
	uint16_t pin_index;	
	uint16_t gpio_index;
	volatile uint32_t *key_reg;
	uint8_t default_mode;
	int  gpio_mode;
	bool is_high_resistance;
}pin_map_t;


typedef enum
{
    HAL_GPIO_DATA_LOW,
    HAL_GPIO_DATA_HIGH,
    HAL_GPIO_DATA_MAX
} hal_gpio_data_t;

typedef void (*wakeupin_callback)(void* param);

typedef enum FIBO_COMMON_MSG
{
    FIBO_WAKEUP_MODEM_MSG,
    FIBO_SLEEP_MODEM_MSG,
    FIBO_WAKELOCK_RESET_MSG,
    FIBO_FMODE_ENTER_MSG,
    FIBO_FMODE_QUIT_MSG,
    FIBO_DTR_TRIGGER_MSG,    
    FIBO_WAKEUPOUT_MSG,    
    FIBO_HEADSET_PLUG_IN_MSG,
    FIBO_HEADSET_PLUG_OUT_MSG,
    FIBO_WAKEUP_TY_PININT_MSG,
    FIBO_SLEEP_TY_PININT_MSG,
    //#ifdef CONFIG_FIBOCOM_LIANDI
    FIBO_START_SCAN_CODE_MSG,
    FIBO_STOP_SCAN_CODE_MSG,
	//#endif
    FIBO_COMMON_MSG_MAX
} fibo_common_msg_id;

typedef struct
{
    fibo_common_msg_id msg_id;
} fibo_quene_msg_t;

typedef enum
{
    hal_gpio_0,
    hal_gpio_1,
    hal_gpio_2,
    hal_gpio_3,
    hal_gpio_4,
    hal_gpio_5,
    hal_gpio_6,
    hal_gpio_7,
    hal_gpio_8,
    hal_gpio_9,
    hal_gpio_10,
    hal_gpio_11,
    hal_gpio_12,
    hal_gpio_13,
    hal_gpio_14,
    hal_gpio_15,
    hal_gpio_16,
    hal_gpio_17,
    hal_gpio_18,
    hal_gpio_19,
    hal_gpio_20,
    hal_gpio_21,
    hal_gpio_22,
    hal_gpio_23,
    hal_gpio_24,
    hal_gpio_25,
    hal_gpio_26,
    hal_gpio_27,
    hal_gpio_28,
    hal_gpio_29,
    hal_gpio_30,
    hal_gpio_31,
    hal_gpio_max
} hal_gpio_t; //  the 8910 support 32 gpio


typedef void (*ISR_callback)(void* param);

typedef struct
{
    drvGpio_t *drv_gpio;
	drvGpioConfig_t *drv_isr;
	hal_gpio_data_t level;
    ISR_callback isr_callback; //  drvGpioIntrCB_t
} hal_gpioisr_t;


typedef struct
{
	uint16_t pin_index;
	bool is_high_wake;
	uint16_t duration;
}wake_out_t;

int fibo_nvmReadItem(uint16_t nvid, void *buf, unsigned size);
int fibo_nvmWriteItem(uint16_t nvid, void *buf, unsigned size);

void atCmdHandleMMAD(atCommand_t *cmd);
void atCmdHandleMTSM(atCommand_t *cmd);
void atCmdHandleTGPIO(atCommand_t *cmd);
void atCmdHandleGPIO(atCommand_t *cmd);
void atCmdHandleWRIM(atCommand_t *cmd);
void atCmdHandleGTWAKE(atCommand_t *cmd);
void atCmdHandleGTLPMMODE(atCommand_t *cmd);
void atCmdHandleGTPMTIME(atCommand_t *cmd);
void atCmdHandleATS24(atCommand_t *cmd);
bool fibo_handleLpgSwitch(bool is_on);
extern void lpg_timer_fun(bool is_on);
bool fibo_hal_gpioGetLevel(uint16_t pin_id, hal_gpio_data_t *level);
bool fibo_hal_gpioIsrInit(uint8_t    pin_id, hal_gpioisr_t *gpio_Isr);
bool fibo_hal_gpioSetDir(uint16_t pin_id, drvGpioMode_t inout);
bool fibo_hal_gpioSetLevel(uint16_t pin_id, hal_gpio_data_t out_level);
bool fibo_hal_gpioModeSet(uint16_t pin_id,uint8_t mode);
bool fibo_hal_gpioModerelease(uint16_t gpio_id);
bool fibo_hal_gpioModeInit(uint16_t pin_id);
bool fibo_hal_gpioIsrDeInit(uint8_t pin_id);
int fibo_hal_getgpioindex(uint16_t pin_id);
bool fibo_openAts24Timer(uint16_t delay_time);
hal_gpio_t fibo_getGpioFromPin(uint16_t pin_index);
void atCmdHandleGTFOPEN(atCommand_t * cmd);
void atCmdHandleGTFCLOSE(atCommand_t * cmd);
void atCmdHandleGTFREAD(atCommand_t * cmd);
void atCmdHandleGTFWRITE(atCommand_t * cmd);
void atCmdHandleGTFSEEK(atCommand_t * cmd);
void atCmdHandleGTFSIZE(atCommand_t * cmd);
void atCmdHandleGTFDELETE(atCommand_t * cmd);
void atCmdHandleGTFLIST(atCommand_t * cmd);
void fibo_wakelock_modem_switch(void);
bool fibo_wakesleepIsrDeinit(uint8_t pin_index);
void atCmdHandleTLPG(atCommand_t *cmd);
void atCmdHandleLPGCFG(atCommand_t *cmd);
void atCmdHandlePWMTest(atCommand_t * cmd);
uint8_t fibocom_get_customize(void);
bool fibocom_set_customize(uint8_t value); /*1: xinguodu customer AT feature*/
void ftp_close_connect();
void fibo_keypad_switchoff(void);
void fibo_lpg_start(void);
void fibo_lpg_stop(void);
void atCmdHandleKEYSWITCH(atCommand_t * cmd);
bool fibo_setChargeSwitch(bool is_switchoff);
bool fibo_hal_gpio_pull_up_or_down(uint16_t pin_id,bool is_pull_up);
osiThread_t* fibo_get_open_timer_cb_task(void);
bool fibo_custom_get_power_state(void);
bool fibo_hal_gpio_pull_high_resistance(uint16_t pin_id,bool is_resistance);

