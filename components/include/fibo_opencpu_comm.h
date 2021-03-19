#ifndef COMPONENTS_FIBOCOM_OPENCPU_FIBO_OPENCPU_COMM_H
#define COMPONENTS_FIBOCOM_OPENCPU_FIBO_OPENCPU_COMM_H

#include <stdbool.h>
#include <stdint.h>
#include <osi_log.h>

#include "osi_api.h"

#include "lwip_mqtt_api.h"

typedef int INT32;
typedef unsigned int UINT32;
typedef char INT8;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef short INT16;
typedef bool BOOL;
typedef unsigned long long  UINT64;

typedef enum {
    FIBO_MBEDTLS_MD_NONE = 0,
    FIBO_MBEDTLS_MD_MD2,
    FIBO_MBEDTLS_MD_MD4,
    FIBO_MBEDTLS_MD_MD5,
    FIBO_MBEDTLS_MD_SHA1,
    FIBO_MBEDTLS_MD_SHA224,
    FIBO_MBEDTLS_MD_SHA256,
    FIBO_MBEDTLS_MD_SHA384,
    FIBO_MBEDTLS_MD_SHA512,
    FIBO_MBEDTLS_MD_RIPEMD160,
} fibo_mbedtls_md_type_t;



#define SAPP_IO_ID_T uint16_t
#define SAPP_GPIO_CFG_T uint16_t
typedef void (*ISR_CB)(void* param);

typedef struct
{
	bool is_debounce;   // debounce enabled
	bool intr_enable;   //interrupt enabled, only for GPIO input
	bool intr_level;    // true for level interrupt, false for edge interrupt
	bool intr_falling;    //falling edge or level low interrupt enabled
	bool inte_rising;   //rising edge or level high interrupt enabled
	ISR_CB callback;    //interrupt callback
}oc_isr_t;

typedef void (*fota_download_cb_t)(UINT32 size, UINT32 total);

/**
 * @brief User callback for at response
 * 
 */
typedef void (*fibo_at_resp_t)(UINT8 *rsp, UINT16 rsplen);
typedef enum
{
    GAPP_SIG_PDP_RELEASE_IND = 0,
    GAPP_SIG_PDP_ACTIVE_IND = 10,
    GAPP_SIG_PDP_ACTIVE_OR_DEACTIVE_FAIL_IND = 11,
    GAPP_SIG_PDP_DEACTIVE_ABNORMALLY_IND = 12,
    GAPP_SIG_LWM2M_TIANYI_REG_SUCCESS = 16,
    GAPP_SIG_LWM2M_TIANYI_OBSERVE_SUCCESS = 17,
    GAPP_SIG_LWM2M_TIANYI_UPDATE_SUCCESS = 18,
    GAPP_SIG_LWM2M_TIANYI_DEREG_SUCCESS = 19,
    GAPP_SIG_LWM2M_TIANYI_REG_FAIL = 20,
    GAPP_SIG_LWM2M_TIANYI_NOTIFY_SUCCESS = 21,
    GAPP_SIG_LWM2M_TIANYI_RCV_DATA = 22,
    GAPP_SIG_LWM2M_TIANYI_NOTIFY_FAIL = 23,
    GAPP_SIG_ONENET_EVENT_BASE = 30,
    GAPP_SIG_ONENET_EVENT_BOOTSTRAP_START = 31,
    GAPP_SIG_ONENET_EVENT_BOOTSTRAP_SUCCESS = 32,
    GAPP_SIG_ONENET_EVENT_BOOTSTRAP_FAILED = 33,
    GAPP_SIG_ONENET_EVENT_CONNECT_SUCCESS = 34,
    GAPP_SIG_ONENET_EVENT_CONNECT_FAILED = 35,
    GAPP_SIG_ONENET_EVENT_REG_SUCCESS = 36,
    GAPP_SIG_ONENET_EVENT_REG_FAILED = 37,
    GAPP_SIG_ONENET_EVENT_REG_TIMEOUT = 38,
    GAPP_SIG_ONENET_EVENT_LIFETIME_TIMEOUT = 39,
    GAPP_SIG_ONENET_EVENT_STATUS_HALT = 40,
    GAPP_SIG_ONENET_EVENT_UPDATE_SUCCESS = 41,
    GAPP_SIG_ONENET_EVENT_UPDATE_FAILED = 42,
    GAPP_SIG_ONENET_EVENT_UPDATE_TIMEOUT = 43,
    GAPP_SIG_ONENET_EVENT_UPDATE_NEED = 44,
    GAPP_SIG_ONENET_EVENT_UNREG_DONE = 45,
    GAPP_SIG_ONENET_EVENT_RESPONSE_FAILED = 50,
    GAPP_SIG_ONENET_EVENT_RESPONSE_SUCCESS = 51,
    GAPP_SIG_ONENET_EVENT_NOTIFY_FAILED = 55,
    GAPP_SIG_ONENET_EVENT_NOTIFY_SUCCESS = 56,
    GAPP_SIG_ONENET_EVENT_OBSERVER_REQUEST = 57,
    GAPP_SIG_ONENET_EVENT_DISCOVER_REQUEST = 58,
    GAPP_SIG_ONENET_EVENT_READ_REQUEST = 59,
    GAPP_SIG_ONENET_EVENT_WRITE_REQUEST = 60,
    GAPP_SIG_ONENET_EVENT_EXECUTE_REQUEST = 61,
    GAPP_SIG_ONENET_EVENT_SET_PARAM_REQUEST = 62,
    GAPP_SIG_CONNECT_RSP = 63,
    GAPP_SIG_CLOSE_RSP = 64,
    GAPP_SIG_SUB_RSP = 65,
    GAPP_SIG_UNSUB_RSP = 66,
    GAPP_SIG_PUB_RSP = 67,
    GAPP_SIG_INCOMING_DATA_RSP = 68,
    GAPP_SIG_PDP_ACTIVE_ADDRESS = 69,
    GAPP_SIG_DNS_QUERY_IP_ADDRESS = 70,
    GAPP_SIG_MPING_RECV = 71,
    GAPP_SIG_MPINGSTAT_RECV= 72,
    GAPP_SIG_TTS_END = 73,
    GAPP_SIG_OPENCPU_API_TEST = 200,
    GAPP_SIG_REPORT_URC_TEXT=201,
    GAPP_SIG_BT_ON_IND  = 300,
    GAPP_SIG_BT_OFF_IND,
    GAPP_SIG_BT_VISIBILE_IND,
    GAPP_SIG_BT_HIDDEN_IND,
    GAPP_SIG_BT_SET_LOCAL_NAME_IND,
    GAPP_SIG_BT_SET_LOCAL_ADDR_IND,
    GAPP_SIG_BT_SCAN_DEV_REPORT,
    GAPP_SIG_BT_SCAN_COMP_IND,
    GAPP_SIG_BT_INQUIRY_CANCEL_IND,
    GAPP_SIG_BT_PAIR_COMPLETE_IND,
    GAPP_SIG_BT_DELETE_DEV_IND,
    GAPP_SIG_BT_SSP_NUM_IND,
    GAPP_SIG_BT_CONNECT_IND,
    GAPP_SIG_BT_DISCONNECT_IND,
    GAPP_SIG_BT_DATA_RECIEVE_IND,
    GAPP_SIG_BLE_SET_ADDR_IND = 350,
    GAPP_SIG_BLE_ADD_CLEAR_WHITELIST_IND,
    GAPP_SIG_BLE_CONNECT_IND,
    GAPP_SIG_BLE_SET_ADV_IND,
    GAPP_SIG_BLE_SET_SCAN_IND,
    GAPP_SIG_BLE_SCAN_REPORT,
    GAPP_SIG_AUDIO_LIST_PLAY_END,
    GAPP_SIG_FTPOPEN_SUCCESS,
    GAPP_SIG_FTPOPEN_FAIL,
    GAPP_SIG_FTPWRITE_SUCCESS,
    GAPP_SIG_FTPWRITE_FAIL,
    GAPP_SIG_FTPREAD_SUCCESS,
    GAPP_SIG_FTPREAD_FALI,
    GAPP_SIG_FTPCLOSE_SUCCESS,
    GAPP_SIG_FTPCLOSE_FAIL,
    GAPP_SIG_BLE_REMOTE_MTU_IND,
    GAPP_SIG_AUDREC_END,
    GAPP_SIG_KEYPAD_VALUE,    
} GAPP_SIGNAL_ID_T;

typedef void (*fibo_signal_t)(GAPP_SIGNAL_ID_T sig, va_list arg);
typedef struct FIBO_CALLBACK_S FIBO_CALLBACK_T;

struct FIBO_CALLBACK_S
{
    fibo_at_resp_t at_resp;
    fibo_signal_t fibo_signal;
};

#define FIBO_UNUSED(v) (void)(v)

#define FIBO_LOG(format, ...)                                                  \
    do                                                                         \
    {                                                                          \
        OSI_PRINTFI("[%s:%d]-" format, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define FIBO_CHECK(condition, err_code, format, ...) \
    do                                          \
    {                                           \
        if (condition)                          \
        {                                       \
            FIBO_LOG(format, ##__VA_ARGS__);    \
            ret = err_code;                     \
            goto error;                         \
        }                                       \
    } while (0);
#ifndef MIN
#define MIN(a, b)              \
    ({                         \
        typeof(a) __a = (a);   \
        typeof(b) __b = (b);   \
        __a > __b ? __b : __a; \
    })
#endif
#define MAX(a, b)              \
    ({                         \
        typeof(a) __a = (a);   \
        typeof(b) __b = (b);   \
        __a > __b ? __a : __b; \
    })

typedef enum fibo_result_s fibo_result_t;

enum fibo_result_s
{
    FIBO_R_FAILED = -1,
    FIBO_R_SUCCESS = 0,
};

extern FIBO_CALLBACK_T *g_user_callback;
void gapp_dispatch(GAPP_SIGNAL_ID_T sig, ...);

// MQTT event
#define AT_CMD_MQTT_CONN_RSP 1
#define AT_CMD_MQTT_SUB_RSP 2
#define AT_CMD_MQTT_UNSUB_RSP 3
#define AT_CMD_MQTT_PUB_RSP 4
#define AT_CMD_MQTT_PING_RSP 5
#define AT_CMD_MQTT_DISCONN_RSP 6
#define AT_CMD_MQTT_PUB_COMP 7
#define AT_CMD_MQTT_ERROR_RSP 8

#define AT_MQTT_TIMER_OPEN 1
#define AT_MQTT_TIMER_SUB 2
#define AT_MQTT_TIMER_UNSUB 3
#define AT_MQTT_TIMER_PUB 4
#define AT_MQTT_TIMER_CLOSE 5

#define AT_MQTT_STAT_IDLE 0
#define AT_MQTT_STAT_OPEN 1
#define AT_MQTT_STAT_SUB 2
#define AT_MQTT_STAT_UNSUB 3
#define AT_MQTT_STAT_PUB 4
#define AT_MQTT_STAT_CLOSE 5

#define AT_MQTT_TIMEOUT 100 // in seconds
#define AT_MQTT_CONNECT 10

#define AT_MQTT_RES_MAX_LEN (512)
#define AT_MQTT_USERNAME_MAX_LENGTH (128)
#define AT_MQTT_PASSWORD_MAX_LENGTH (128)
#define AT_MQTT_CLIENT_STR_MAX_LENGTH (23)
#define AT_MQTT_CLIENT_ID_MIN (1)
#define AT_MQTT_CLIENT_ID_MAX (2)
#define AT_MQTT_SN_CLIENT_ID_MIN (101)
#define AT_MQTT_SN_CLIENT_ID_MAX (102)
#define AT_MQTT_CLIENT_MAX_NUM (AT_MQTT_CLIENT_ID_MAX - AT_MQTT_CLIENT_ID_MIN + 1)
#define AT_MQTT_WILL_MAX_NUM (AT_MQTT_CLIENT_ID_MAX - AT_MQTT_CLIENT_ID_MIN + 1) + (AT_MQTT_SN_CLIENT_ID_MAX - AT_MQTT_SN_CLIENT_ID_MIN + 1)
#define AT_MQTT_WILL_TOPIC_MAX_LENGTH (255)
#define AT_MQTT_MESSAGE_MAX_LENGTH (1024)
//The max length of recv pulish message
#define AT_MQTT_RES_MESSAGE_MAX_LENGTH (1500)
#define AT_MQTT_CACHE_MESSAGE_MAX_LENGTH (1024)
#define AT_MQTT_QOS0 (0)
#define AT_MQTT_QOS1 (1)
#define AT_MQTT_QOS2 (2)
#define AT_MQTT_KEEPALIVE_MIN (1)
#define AT_MQTT_KEEPALIVE_MAX (300)
#define AT_MQTT_MSG_CACHE_NUM (2)
#define AT_MQTT_DEFAULT_TIMEOUT (5)

typedef enum mqtt_client_status_s mqtt_client_status_t;
typedef enum mqtt_disconnect_cause_s mqtt_disconnect_cause_t;
typedef struct mqtt_publish_msg_s mqtt_publish_msg_t;
typedef struct mqtt_user_conf_s mqtt_user_conf_t;
typedef struct mqtt_will_conf_s mqtt_will_conf_t;

enum mqtt_result_s
{
    MQTT_R_FAILED = -1,
    MQTT_R_SUCCESS = 0,
};

enum mqtt_client_status_s
{
    MQTT_CLIENT_DISCONNECTED = 0,

    MQTT_CLIENT_DISCONNECTTING,

    MQTT_CLIENT_CONNECTTING,

    MQTT_CLIENT_CONNECTED,

};

enum mqtt_disconnect_cause_s
{
    MQTT_DISCONNECT_ACCIDENT = 1,
    MQTT_DISCONNECT_WIRELESS_DISCONN,
    MQTT_DISCONNECT_GPRS_NO_REG,
};

struct mqtt_publish_msg_s
{
    char topic[AT_MQTT_WILL_TOPIC_MAX_LENGTH + 1];
    char *message;
    uint16_t recv_len;
    uint16_t message_length;
    uint8_t qos;
};

struct mqtt_user_conf_s
{
    uint8_t client_id;
    char username[AT_MQTT_USERNAME_MAX_LENGTH + 1];
    char password[AT_MQTT_PASSWORD_MAX_LENGTH + 1];
    char client_str[AT_MQTT_CLIENT_STR_MAX_LENGTH + 1];

    mqtt_client_status_t client_status;
    mqtt_client_t mqtt_client;

    osiTimer_t *timer;
    int stat;
    void *tls_mqtt_client;
    bool use_tls;

    mqtt_publish_msg_t *cur_pub_msg;
    osiMutex_t *mutexLock; 
};

struct mqtt_will_conf_s
{
    uint8_t client_id;
    bool will_flag;
    char will_topic[AT_MQTT_WILL_TOPIC_MAX_LENGTH + 1];
    uint8_t will_qos;
    bool will_retain_flag;
    char will_message[AT_MQTT_MESSAGE_MAX_LENGTH + 1];
};

#define CLINET_IS_CONNECTED() (g_mqtt_user_conf.client_status == MQTT_CLIENT_CONNECTED)
#define CLINET_STATUS_DISCONNECTED() (g_mqtt_user_conf.client_status == MQTT_CLIENT_DISCONNECTED)
#define STRING_IS_EMPTY(str) (0 == (str)[0])

#define mqtt_start_timer(t, timeout)           \
    do                                         \
    {                                          \
        if (!osiTimerStart(t, timeout * 1000)) \
        {                                      \
            MQTT_LOG("start timer failed");    \
        }                                      \
    } while (0)

#define mqtt_stop_timer(t)                 \
    do                                     \
    {                                      \
        if (!osiTimerStop(t))              \
        {                                  \
            MQTT_LOG("stop timer failed"); \
        }                                  \
    } while (0)

#endif /* COMPONENTS_FIBOCOM_OPENCPU_FIBO_OPENCPU_COMM_H */
