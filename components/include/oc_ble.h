#ifndef COMPONENTS_FIBOCOM_OPENCPU_OC_BLE_H
#define COMPONENTS_FIBOCOM_OPENCPU_OC_BLE_H

//#ifdef CONFIG_BLUEU_BT_ENABLE

#include <stdio.h>
#include <stdlib.h>
#include "vfs.h"
#include <stdint.h>
// #include "utrace_dsp_log.h"  // build err
#include "osi_log.h"
#include "bluetooth/bt_driver.h"
#include "bluetooth/bt_gatt.h"
#include "bluetooth/bluetooth.h"
#include "sci_types.h"
#include "bt_abs.h"
#include "bluetooth/bt_types.h"
#include "bt_app.h"
#include "connectivity_config.h"


#define FIBO_BLE_LOG(format, ...) OSI_PRINTFI("[AUTO_BLE][%s:%d]" format, __FUNCTION__, __LINE__, ##__VA_ARGS__)


typedef enum
{
    GATT_CLIENT_LINK_DISCONNECT,
    GATT_CLIENT_LINK_CONNECTING,
    GATT_CLIENT_LINK_CONNECTED,
    GATT_CLIENT_DISCOVER_PRME_SER,
    GATT_CLIENT_DISCOVER_PRME_SER_DONE,
    GATT_CLIENT_READ_INCLUDE,
    GATT_CLIENT_READ_INCLUDE_DONE,
    GATT_CLIENT_READ_CHARA_DEC,
    GATT_CLIENT_READ_CHARA_DEC_DONE,
    GATT_CLIENT_FIND_INFO,
    GATT_CLIENT_FIND_INFO_DONE,
    GATT_CLIENT_SET_CONFIG,
    GATT_CLIENT_SET_CONFIG_DONE,
} GATT_CLIENT_STATE;


enum
{
    FIBO_BLE_CLIENT_MSG1 = 0,
    FIBO_BLE_CLIENT_MSG2 = 1,
    FIBO_BLE_CLIENT_MSG3 = 2,
    FIBO_BLE_CLIENT_MSG4 = 3,
    FIBO_BLE_CLIENT_MSG5 = 4,
    FIBO_BLE_CLIENT_MSG6 = 5,
    FIBO_BLE_CLIENT_MSG7 = 6,
    FIBO_BLE_CLIENT_MSG8 = 7,
    FIBO_BLE_CLIENT_MSG9 = 8,
    FIBO_BLE_CLIENT_MSG10 = 9,
    FIBO_BLE_CLIENT_MSG11 = 10,
    FIBO_BLE_CLIENT_MSG12 = 11,
    FIBO_BLE_CLIENT_MSG13 = 12,
    FIBO_BLE_CLIENT_MSG14 = 13,
    FIBO_BLE_CLIENT_MSG15 = 14,
    FIBO_BLE_CLIENT_MSG16 = 15,
    FIBO_BLE_CLIENT_MSG17 = 16,
    FIBO_BLE_CLIENT_MSG18 = 17,
    FIBO_BLE_CLIENT_MSG19 = 18,
    FIBO_BLE_CLIENT_MSG20 = 19,
    FIBO_BLE_CLIENT_MSG21 = 20,
};




typedef struct
{
	UINT8 *uuid_l;
	UINT16 uuid_s;
	UINT16 acl_handle;

}fibo_discover_primary_service_by_uuid_t;

typedef struct
{
	UINT16 start_handle;
	UINT16 end_handle;
	UINT16 acl_handle;
}fibo_discover_all_characteristic_t;

typedef struct
{
	UINT16 att_handle;
	UINT8 is_long_att;
	UINT16 offset;
	UINT16 acl_handle;
}fibo_read_char_value_by_handle_t;

typedef struct
{
	 UINT8 *uuid_l;
	 UINT16 uuid_s;
	 UINT16 start_handle;
	 UINT16 end_handle; 
	 UINT16 acl_handle;
}fibo_read_char_value_by_uuid_t;
typedef struct
{
	UINT8* handle;
	UINT16 length; 
	UINT16 acl_handle;
}fibo_read_multiple_char_value_t;
typedef struct
{
	UINT16 att_handle;
	void *value;
	UINT16 length;
	UINT16 acl_handle;
	UINT16 offset;
	UINT8 islong;

}fibo_write_char_value_t;
typedef struct
{
	UINT16 att_handle;
	void *value;
	UINT16 length;
	UINT16 acl_handle;

}fibo_write_char_value_without_rsp_t;
typedef struct
{
	UINT16 att_handle;
	void *value; 
	UINT16 length; 
	UINT16 acl_handle;

}fibo_write_char_value_signed_t;
typedef struct
{
	UINT16 att_handle;
	void *value; 
	UINT16 length;
	UINT16 acl_handle;

}fibo_write_char_value_reliable_t;
typedef struct
{
	UINT16 offset;
	UINT16 acl_handle;

}fibo_execute_write_t;
typedef struct
{
	UINT16 start_handle;
	UINT16 end_handle; 
	UINT16 acl_handle;

}fibo_find_include_service_t;
typedef struct
{
	UINT16 start_handle;
	UINT16 end_handle;
	UINT16 acl_handle;

}fibo_get_char_descriptor_t;
typedef struct
{

	UINT16 att_handle;
	UINT8 is_long_att;
	UINT16 acl_handle;

}fibo_read_char_descriptor_t;
typedef struct
{
	UINT16 att_handle;
	void *value;
	UINT16 length;
	UINT16 acl_handle;

}fibo_write_char_descriptor_t;


typedef struct{
	uint8_t addr_type;
	bdaddr_t address;
}fibo_ble_peer_device_info_t;




typedef struct
{
    bt_status_t (*connection_state_change_cb) (int conn_id, int connected, bdaddr_t *addr,UINT8 reason);
    bt_status_t (*adv_success_cb) (void);
    bt_status_t (*adv_failed_cb) (void);
    bt_status_t (*notification_send_cb) (UINT16 length);
    void        (*smp_pair_success_cb) (void);
    void        (*smp_pair_failed_cb) (void);
    void        (*num_of_complete_cb) (UINT8 current_buffer_num,UINT8 complet_buffer_num);
    bt_status_t (*read_white_list_cb)(UINT16 size);
} fibo_ble_server_callbacks_t;


typedef struct
{
    bt_status_t (*connection_state_change_cb) (int conn_id, int connected, bdaddr_t *addr);
    bt_status_t (*discover_service_by_uuid_cb) (gatt_prime_service_t *);
    bt_status_t (*discover_service_all_cb) (gatt_prime_service_t *);
    bt_status_t (*char_des_data) (att_server_t *);
    bt_status_t (*char_data) (att_server_t *, UINT8 more_data);
    bt_status_t (*read_cb) (att_server_t *);
    bt_status_t (*read_blob_cb) (att_server_t *);
    bt_status_t (*read_multi_cb) (att_server_t *);
    bt_status_t (*recv_notification_cb) (att_server_t *);
    bt_status_t (*recv_indication_cb) (att_server_t *);
    bt_status_t (*write_cb) (att_req_pdu_t *);
    bt_status_t (*write_rsp_cb) (UINT16 *);
    bt_status_t (*scan_cb) (void *,UINT8 adv_len);
    void        (*smp_pair_success_cb) (void);
    void        (*smp_pair_failed_cb) (void);
    void        (*att_error_cb) (UINT8 error_code);
    void        (*scan_timeout) (void *param);
} fibo_ble_client_callbacks_t;

typedef struct
{
    const fibo_ble_client_callbacks_t *client;
    const fibo_ble_server_callbacks_t *server;
    void  (*mtu_exchange_result_cb)(UINT16 handle, UINT16 mtu);
    void  (*le_data_length_result_cb)(UINT16 handle,UINT16 max_tx_octets,UINT16 max_tx_time,UINT16 max_rx_octets,UINT16 max_rx_time);
    bt_status_t (*conn_param_update_result_cb)(UINT8 status,gatt_connect_param_t *param);
    bt_status_t (*l2_conn_param_update_req_cb)(UINT16 status);
    bt_status_t (*le_phy_update_result_cb)(UINT16 handle,UINT8 status,UINT8 tx_phy,UINT8 rx_phy);
    void (*ble_hci_cmd_complete_cb)(UINT16 opcode, UINT8 status, UINT8 *pdu);
    void (*ble_hci_cmd_status_cb)(UINT16 opcode, UINT8 status, UINT8 *pdu);
} fibo_ble_btgatt_callback_t;



typedef struct{
	
    UINT16 acl_handle;
	fibo_ble_peer_device_info_t fibo_ble_peer_device_info;
	fibo_discover_primary_service_by_uuid_t fibo_discover_primary_service_by_uuid;
	fibo_discover_all_characteristic_t fibo_discover_all_characteristic;
	fibo_read_char_value_by_handle_t fibo_read_char_value_by_handle;
	fibo_read_char_value_by_uuid_t fibo_read_char_value_by_uuid;
	fibo_read_multiple_char_value_t fibo_read_multiple_char_value;
	fibo_write_char_value_t fibo_write_char_value;
	fibo_write_char_value_without_rsp_t fibo_write_char_value_without_rsp;
	fibo_write_char_value_signed_t fibo_write_char_value_signed;
	fibo_write_char_value_reliable_t fibo_write_char_value_reliable;
	fibo_execute_write_t fibo_execute_write;
	fibo_find_include_service_t fibo_find_include_service;
	fibo_get_char_descriptor_t fibo_get_char_descriptor;
	fibo_read_char_descriptor_t fibo_read_char_descriptor;
	fibo_write_char_descriptor_t fibo_write_char_descriptor;
	
}fibo_ble_client_parm_t;


typedef struct {
	gatt_adv_param_t fibo_ble_adv;
	int adv_set_flag;
	int adv_data_set_flag;
	int adv_data_len;
	char adv_data[64];
	//CLIENT PARM SET CLIENT
	int client_connect;
	bdaddr_t addr ;
	int fibo_client_old_init_cb_set_flag;
	fibo_btgatt_callback_t *fibo_ble_old_btgatt_cb;
	int fibo_client_new_init_cb_set_flag;
	fibo_ble_btgatt_callback_t *fibo_ble_new_btgatt_cb;
	UINT16 acl_handle;
	fibo_ble_peer_device_info_t fibo_ble_peer_device_info;
	UINT8 *uuid_l;
	UINT16 uuid_s;
	void *buffer;
	UINT16 length;
	
}customer_param_t;



typedef enum{
    FIBO_BLE_CLI_MSG_I_CLIENT_INIT = 0,
    FIBO_BLE_CLI_MSG_I_CLIENT_SERVER_INIT,
    FIBO_BLE_CLI_MSG_I_SCAN_ENABLE,
    FIBO_BLE_CLI_MSG_I_CONNECT,
    FIBO_BLE_CLI_MSG_I_CONNECT_CANCEL,
    FIBO_BLE_CLI_MSG_I_DIS_ALL_PRI_SERVICE, //5
    FIBO_BLE_CLI_MSG_I_DIS_PRI_SERVICE_BY_UUID,
    FIBO_BLE_CLI_MSG_I_DIS_ALL_CHARACTERISTIC,
    FIBO_BLE_CLI_MSG_I_READ_CHAR_VAL_BY_HANDLE,
    FIBO_BLE_CLI_MSG_I_READ_CHAR_VAL_BY_UUID,
    FIBO_BLE_CLI_MSG_I_READ_MUL_CHAR_VAL, //10
    FIBO_BLE_CLI_MSG_I_WRITE_CHAR_VAL,
    FIBO_BLE_CLI_MSG_I_WRITE_CHAR_VAL_WITHOUT_RES,
    FIBO_BLE_CLI_MSG_I_WRITE_CHAR_VAL_SIGNED,
    FIBO_BLE_CLI_MSG_I_WRITE_CHAR_VAL_RELIABLE,
    FIBO_BLE_CLI_MSG_I_FIND_INCLUDE_SERVICE, //15
    FIBO_BLE_CLI_MSG_I_GET_CHAR_DESCRIPTOR,
    FIBO_BLE_CLI_MSG_I_READ_CHAR_DESCRIPTOR,
    FIBO_BLE_CLI_MSG_I_WRITE_CHAR_DESCRIPTOR,
    FIBO_BLE_CLI_MSG_I_FILTER_NAME,    
    FIBO_BLE_CLI_MSG_I_FILTER_UUID, //20    
    FIBO_BLE_CLI_MSG_I_FILTER_MANUFACTURER
    
}fibo_ble_client_msg_id;


typedef struct{
	int msg_id;
	void *param;
}fibo_ble_client_Q_t;

void fibo_ble_client_fun(void *param);

//#endif
#endif
