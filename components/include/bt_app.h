#ifndef BT_APP_H
#define BT_APP_H

#include "sci_types.h"
#include "bt_abs.h"

#define BT_OS_THREAD_STACK_SIZE (1024 * 4)
#define BT_THREAD_QUEUE_NUM (100)

#define BT_DEVICE_NAME_SIZE 41
#define BT_MAX_DEVICE_NUM 40

#define BT_SPP_SUPPORT 0
#define BT_BLE_GATT_CLIENT_SUPPORT 1
#define BT_BLE_GATT_SERVER_SUPPORT 1
#define BT_HFP_SUPPORT 1
#define BT_AVRCP_SUPPORT 1
#define BT_A2DP_SUPPORT 1

#define BT_OS_MSG_START (0x100)
#define BT_FP_MSG_START (0x200)
#define BT_APP_MSG_START (0x300)

#define ID_STATUS_INVALID 0xFFFFFFFFUL
//Profile Message define
#define ID_BT_MSG_BASE 0x10000000UL
#define ID_CM_MSG_BASE 0x10010000UL
#define ID_SC_MSG_BASE 0x10020000UL
#define ID_SD_MSG_BASE 0x10030000UL
#define ID_OPC_MSG_BASE 0x10040000UL
#define ID_OPS_MSG_BASE 0x10050000UL
#define ID_FTC_MSG_BASE 0x10060000UL
#define ID_FTS_MSG_BASE 0x10070000UL
#define ID_HFG_MSG_BASE 0x10080000UL
#define ID_A2DP_MSG_BASE 0x10090000UL
#define ID_AVRCP_MSG_BASE 0x100A0000UL
#define ID_DG_MSG_BASE 0x100B0000UL
#define ID_SPP_MSG_BASE 0x100C0000UL
#define ID_BPP_MSG_BASE 0x100D0000UL
#define ID_HFU_MSG_BASE 0x100E0000UL
#define ID_PBAP_MSG_BASE 0x100F0000UL
#define ID_MAP_MSG_BASE 0x10100000UL
#define ID_A2DP_SINK_BASE 0x10110000UL
#define ID_SAP_MSG_BASE 0x10120000UL
#define ID_SPP2_MSG_BASE 0x10130000UL
#define ID_HOGP_MSG_BASE 0x10140000UL
#define ID_ANCS_MSG_BASE 0x10150000UL
#define ID_ANTILOST_MSG_BASE 0x10160000UL
#define ID_PXP_MSG_BASE 0x10170000UL
#define ID_HID_MSG_BASE 0x10180000UL
#define ID_GATT_SERVER_MSG_BASE 0x101A0000UL
#define ID_AT_MSG_BASE 0x101B0000UL
#define ID_BLE_CMD_CONTROL_BASE 0x10190000UL

/* Message values */
//BT Message Define
#define ID_STATUS_BT_ON_RES (ID_BT_MSG_BASE | 0x0001)
#define ID_STATUS_BT_OFF_RES (ID_BT_MSG_BASE | 0x0002)
#define ID_STATUS_BT_SERVICE_OFF_RES (ID_BT_MSG_BASE | 0x0003)
#define ID_STATUS_BT_READ_RSSI_RES (ID_BT_MSG_BASE | 0x0004)
#define ID_STATUS_BT_SET_RSSI_MONITOR_RES (ID_BT_MSG_BASE | 0x0005)
#define ID_STATUS_BT_RSSI_ALERT_RES (ID_BT_MSG_BASE | 0x0006)
#define ID_STATUS_BT_PRIVATE_KEY_FINISH_IND (ID_BT_MSG_BASE | 0x0007)
#define ID_STATUS_BT_REOPEN_IND (ID_BT_MSG_BASE | 0x0008)
#define ID_STATUS_BT_REOPEN_ACTION_IND (ID_BT_MSG_BASE | 0x0009)

#define ID_STATUS_BT_LINK_CONNECT_RES (ID_BT_MSG_BASE | 0x0020)
#define ID_STATUS_BT_LINK_DISC_RES (ID_BT_MSG_BASE | 0x0021)
#define ID_STATUS_BT_LINK_DISCONNECT_RES (ID_BT_MSG_BASE | 0x0022)
#define ID_STATUS_GATT_SERVICE_STARTED_RES (ID_BT_MSG_BASE | 0x0023)
#define ID_STATUS_GATT_MTU_EXCHANGE_RES (ID_BT_MSG_BASE | 0x0024)
#define ID_STATUS_UPDATE_NV (ID_BT_MSG_BASE | 0x0025)
#define ID_STATUS_A2DP_MSG_TO_APP (ID_BT_MSG_BASE | 0x0026)
#define ID_GATT_READ_DYMATIC_ATTRIBUTE_VALUE (ID_BT_MSG_BASE | 0x0027)
#define ID_GATT_SEND_DATA_FLAG_ERROR (ID_BT_MSG_BASE | 0x0028)
#define ID_STATUS_BLE_LINK_PAIRED (ID_BT_MSG_BASE | 0x0029)
#define ID_STATUS_BLE_LINK_CONNECTED (ID_BT_MSG_BASE | 0x002A)
#define ID_STATUS_BT_NON_SIG_RX_DATA (ID_BT_MSG_BASE | 0x002B)
#define ID_STATUS_BLE_FINDER_FLAG_RES (ID_BT_MSG_BASE | 0x002C)
#define ID_STATUS_BLE_LAST_HANDLE_IND (ID_BT_MSG_BASE | 0x002D)

//HFG Message define
#define ID_STATUS_HFAG_SLC_CONNECTED (ID_HFG_MSG_BASE | 0x0001)
#define ID_STATUS_HFAG_SLC_DISCONNECTED (ID_HFG_MSG_BASE | 0x0002)
#define ID_STATUS_HFAG_HFU_CALL_ANSWER (ID_HFG_MSG_BASE | 0x0003)
#define ID_STATUS_HFAG_HFU_CALL_HANGUP (ID_HFG_MSG_BASE | 0x0004)
#define ID_STATUS_HFAG_HFU_DIAL_NUMBER (ID_HFG_MSG_BASE | 0x0005)
#define ID_STATUS_HFAG_HFU_DIAL_MEMORY (ID_HFG_MSG_BASE | 0x0006)
#define ID_STATUS_HFAG_HFU_REDIAL (ID_HFG_MSG_BASE | 0x0007)
#define ID_STATUS_HFAG_HFU_HOLD (ID_HFG_MSG_BASE | 0x0008)
#define ID_STATUS_HFAG_AUDIO_CONNECTED (ID_HFG_MSG_BASE | 0x0009)
#define ID_STATUS_HFAG_AUDIO_DISCONNECTED (ID_HFG_MSG_BASE | 0x000A)
#define ID_STATUS_HFAG_HFU_STATUS_REQ (ID_HFG_MSG_BASE | 0x000B)
#define ID_STATUS_HS_SPK_VOL_CHANGE (ID_HFG_MSG_BASE | 0x000C)
#define ID_STATUS_HS_MIC_VOL_CHANGE (ID_HFG_MSG_BASE | 0x000D)
#define ID_STATUS_HFAG_PLAY_CFM (ID_HFG_MSG_BASE | 0x000E)
#define ID_STATUS_HFAG_STOP_CFM (ID_HFG_MSG_BASE | 0x000F)
#define ID_STATUS_HFAG_HFU_DTMF_NUMBER (ID_HFG_MSG_BASE | 0x0010)
#define ID_STATUS_HFAG_SLC_CONNECTED_IND (ID_HFG_MSG_BASE | 0x0011)
#define ID_STATUS_HFAG_HFU_CLCC (ID_HFG_MSG_BASE | 0x0012)
#define ID_STATUS_HFAG_CALL_TYPE_WB_NB (ID_HFG_MSG_BASE | 0x0013)

//CM message define
#define ID_STATUS_CM_INQUIRY_REQ (ID_CM_MSG_BASE | 0x0001)
#define ID_STATUS_CM_INQUIRY_RES (ID_CM_MSG_BASE | 0x0002)
#define ID_STATUS_CM_INQUIRY_FINISH_RES (ID_CM_MSG_BASE | 0x0003)
#define ID_STATUS_CM_CANCEL_INQUIRY_RES (ID_CM_MSG_BASE | 0x0004)
#define ID_STATUS_CM_VISIBLE_RES (ID_CM_MSG_BASE | 0x0005)
#define ID_STATUS_CM_DISVISIBLE_RES (ID_CM_MSG_BASE | 0x0006)
#define ID_STATUS_CM_SET_LOCAL_NAME_RES (ID_CM_MSG_BASE | 0x0007)
#define ID_STATUS_CM_READ_LOCAL_NAME_RES (ID_CM_MSG_BASE | 0x0008)
#define ID_STATUS_CM_READ_REMOTE_NAME_RES (ID_CM_MSG_BASE | 0x0009)
#define ID_STATUS_CM_READ_LOCAL_ADDR_RES (ID_CM_MSG_BASE | 0x000A)
#define ID_STATUS_CM_SET_LOCAL_ADDR_RES (ID_CM_MSG_BASE | 0x000B)
#define ID_STATUS_CM_SET_BLE_LOCAL_NAME_RES (ID_CM_MSG_BASE | 0x000C)
#define ID_STATUS_CM_CONNECT_IND (ID_CM_MSG_BASE | 0x000D)

//SC Message Define
#define ID_STATUS_SC_PASSKEY_REQ (ID_SC_MSG_BASE | 0x0001)
#define ID_STATUS_SC_PAIR_RES (ID_SC_MSG_BASE | 0x0002)
#define ID_STATUS_SC_DEPAIR_RES (ID_SC_MSG_BASE | 0x0003)
#define ID_STATUS_SC_PAIR_IND (ID_SC_MSG_BASE | 0x0004)
#define ID_STATUS_SC_SSP_NUM_IND (ID_SC_MSG_BASE | 0x0005)
#define ID_STATUS_SC_AUTH_IND (ID_SC_MSG_BASE | 0x0006)     //20120815: keddy
#define ID_STATUS_SC_KBD_CONN_IND (ID_SC_MSG_BASE | 0x0007) //for ble keypad pair
#define ID_STATUS_SC_BLE_SLAVE_PAIR_IND (ID_SC_MSG_BASE | 0x0008)

//SD Message Define
#define ID_STATUS_SD_READ_SERVICES_RES (ID_SD_MSG_BASE | 0x0001)
#define ID_STATUS_SD_CANCEL_READ_SERVICES_RES (ID_SD_MSG_BASE | 0x0002) //Added

//avrcp message  define
#define ID_STATUS_AVRCP_OPERATION_ID_FORWARD (ID_AVRCP_MSG_BASE | 0x0001)
#define ID_STATUS_AVRCP_OPERATION_ID_BACKWARD (ID_AVRCP_MSG_BASE | 0x0002)
#define ID_STATUS_AVRCP_OPERATION_ID_VOL_DOWN (ID_AVRCP_MSG_BASE | 0x0003)
#define ID_STATUS_AVRCP_OPERATION_ID_VOL_UP (ID_AVRCP_MSG_BASE | 0x0004)
#define ID_STATUS_AVRCP_OPERATION_ID_PLAY (ID_AVRCP_MSG_BASE | 0x0005)
#define ID_STATUS_AVRCP_OPERATION_ID_STOP (ID_AVRCP_MSG_BASE | 0x0006)
#define ID_STATUS_AVRCP_OPERATION_ID_PAUSE (ID_AVRCP_MSG_BASE | 0x0007)
#define ID_STATUS_AVRCP_CONNECT_COMPLETE (ID_AVRCP_MSG_BASE | 0x0008)
#define ID_STATUS_AVRCP_DISCONNECT_COMPLETE (ID_AVRCP_MSG_BASE | 0x0009)
#define ID_STATUS_AVRCP_COMMAND_RECEIVED (ID_AVRCP_MSG_BASE | 0x000A)

//avrcp ct message define
#define ID_STATUS_AVRCP_CT_CONNECT_COMPLETE (ID_AVRCP_MSG_BASE | 0x000B)
#define ID_STATUS_AVRCP_CT_DISCONNECT_COMPLETE (ID_AVRCP_MSG_BASE | 0x000C)
#define ID_STATUS_AVRCP_EVENT_PLAYBACK_CHANGED (ID_AVRCP_MSG_BASE | 0x000D)
#define ID_STATUS_AVRCP_EVENT_TRACK_CHANGED (ID_AVRCP_MSG_BASE | 0x000E)
#define ID_STATUS_AVRCP_EVENT_TRACK_REACHED_END (ID_AVRCP_MSG_BASE | 0x000F)
#define ID_STATUS_AVRCP_EVENT_TRACK_REACHED_START (ID_AVRCP_MSG_BASE | 0x0010)
#define ID_STATUS_AVRCP_EVENT_PLAYBACK_POS_CHANGED (ID_AVRCP_MSG_BASE | 0x0011)
#define ID_STATUS_AVRCP_EVENT_BATT_STATUS_CHANGED (ID_AVRCP_MSG_BASE | 0x0012)
#define ID_STATUS_AVRCP_EVENT_SYSTEM_STATUS_CHANGED (ID_AVRCP_MSG_BASE | 0x0013)
#define ID_STATUS_AVRCP_EVENT_PLAYER_APPLICATION_CHANGED (ID_AVRCP_MSG_BASE | 0x0014)
#define ID_STATUS_AVRCP_CT_PLAY_STATUS (ID_AVRCP_MSG_BASE | 0x0015)
#define ID_STATUS_AVRCP_CT_MEDIA_ATTRIBUTES (ID_AVRCP_MSG_BASE | 0x0016)
#define ID_STATUS_AVRCP_SUPPORTED_PLAYER_ATTRIBUTES (ID_AVRCP_MSG_BASE | 0x0017)
#define ID_STATUS_AVRCP_EXIST_PLAYER_ATTRIBUTES_VALUES (ID_AVRCP_MSG_BASE | 0x0018)
#define ID_STATUS_AVRCP_PLAYER_CURRENT_ATTRIBUTE (ID_AVRCP_MSG_BASE | 0x0019)
#define ID_STATUS_AVRCP_PLAYER_ATTRIBUTES_TXT (ID_AVRCP_MSG_BASE | 0x001A)
#define ID_STATUS_AVRCP_PLAYER_VALUES_TXT (ID_AVRCP_MSG_BASE | 0x001B)
#define ID_STATUS_AVRCP_PANEL_KEY_RSP (ID_AVRCP_MSG_BASE | 0x001C)
#define ID_STATUS_AVRCP_SET_PLAYER_VALUES (ID_AVRCP_MSG_BASE | 0x001D)
#define ID_STATUS_AVRCP_ABORT_METADATA (ID_AVRCP_MSG_BASE | 0x001E)
#define ID_STATUS_AVRCP_SET_CHARSETS (ID_AVRCP_MSG_BASE | 0x001F)
#define ID_STATUS_AVRCP_INFORM_BATT_STATUS (ID_AVRCP_MSG_BASE | 0x0020)

#if AVRCP_BROWSING_CONTROLLER == XA_ENABLED
//avrcp ct message define for browser
#define ID_STATUS_AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED (ID_AVRCP_MSG_BASE | 0x0021)
#define ID_STATUS_AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED (ID_AVRCP_MSG_BASE | 0x0022)
#define ID_STATUS_AVRCP_EVENT_UIDS_CHANGED (ID_AVRCP_MSG_BASE | 0x0023)
#define ID_STATUS_AVRCP_EVENT_VOLUME_CHANGED (ID_AVRCP_MSG_BASE | 0x0024)
#define ID_STATUS_AVRCP_EVENT_PLAYING_CONTENT_CHANGED (ID_AVRCP_MSG_BASE | 0x0025)
#define ID_STATUS_AVRCP_SET_ADDRESSED_PLAYER (ID_AVRCP_MSG_BASE | 0x0026)
#define ID_STATUS_AVRCP_PLAY_ITEM (ID_AVRCP_MSG_BASE | 0x0027)
#define ID_STATUS_AVRCP_ADD_NOW_PLAYING (ID_AVRCP_MSG_BASE | 0x0028)
#define ID_STATUS_AVRCP_SEARCH (ID_AVRCP_MSG_BASE | 0x0029)
#define ID_STATUS_AVRCP_SET_BROWSED_PLAYER (ID_AVRCP_MSG_BASE | 0x002A)
#define ID_STATUS_AVRCP_CHANGE_PATH (ID_AVRCP_MSG_BASE | 0x002B)
#define ID_STATUS_AVRCP_GET_FOLDER_ITEMS (ID_AVRCP_MSG_BASE | 0x002C)
#define ID_STATUS_AVRCP_SET_ABSOLUTE_VOLUME (ID_AVRCP_MSG_BASE | 0x002D)
#endif
#ifdef BT_A2DP_SUPPORT
//avrcp tg message define
#define ID_STATUS_AVRCP_IND_TG_PLAY_POS_EVENT (ID_AVRCP_MSG_BASE | 0x002E)
#define ID_STATUS_AVRCP_IND_TG_PLAYER_VALUES_CHANGED (ID_AVRCP_MSG_BASE | 0x002F)
#define ID_STATUS_AVRCP_IND_TG_DISP_CHARSET_CHANGED (ID_AVRCP_MSG_BASE | 0x0030)
#define ID_STATUS_AVRCP_IND_TG_BATT_STATUS_CHANGED (ID_AVRCP_MSG_BASE | 0x0031)
#define ID_STATUS_AVRCP_OPERATION_ID_NEXT_GROUP (ID_AVRCP_MSG_BASE | 0x0032)
#define ID_STATUS_AVRCP_OPERATION_ID_PREV_GROUP (ID_AVRCP_MSG_BASE | 0x0033)
#endif
#define ID_STATUS_AVRCP_SEND_REWIND_OR_FASTFWD_SINGNAL (ID_AVRCP_MSG_BASE | 0x0034)
#define ID_STATUS_AVRCP_OPERATION_ID_REWIND_PASS_THROUGH_RELEASE (ID_AVRCP_MSG_BASE | 0x0035)
#define ID_STATUS_AVRCP_OPERATION_ID_REWIND_PASS_THROUGH_PRESS (ID_AVRCP_MSG_BASE | 0x0036)
#define ID_STATUS_AVRCP_OPERATION_ID_FAST_FORWARD_PASS_THROUGH_PRESS (ID_AVRCP_MSG_BASE | 0x0037)
#define ID_STATUS_AVRCP_OPERATION_ID_FAST_FORWARD_PASS_THROUGH_RELEASE (ID_AVRCP_MSG_BASE | 0x0038)
//a2dp message  define
#define ID_STATUS_A2DP_SRC_CONFIG_STREAM (ID_A2DP_MSG_BASE | 0x0001)
#define ID_STATUS_A2DP_SRC_OPEN_STREAM (ID_A2DP_MSG_BASE | 0x0002)
#define ID_STATUS_A2DP_SRC_START_STREAM (ID_A2DP_MSG_BASE | 0x0003)
#define ID_STATUS_A2DP_SRC_SUSPEND_STREAM (ID_A2DP_MSG_BASE | 0x0004)
#define ID_STATUS_A2DP_SRC_CLOSE_STREAM (ID_A2DP_MSG_BASE | 0x0005)
#define ID_STATUS_A2DP_SRC_RESUME_STREAM (ID_A2DP_MSG_BASE | 0x0006)
#define ID_STATUS_A2DP_SRC_REGISTER (ID_A2DP_MSG_BASE | 0x0007)
#define ID_STATUS_A2DP_CONNECTED (ID_A2DP_MSG_BASE | 0x0008)
#define ID_STATUS_A2DP_DISCONNECTED (ID_A2DP_MSG_BASE | 0x0009)
#define ID_STATUS_A2DP_SNK_START_STREAM (ID_A2DP_MSG_BASE | 0x000A)
#define ID_STATUS_A2DP_SNK_RESUME_STREAM (ID_A2DP_MSG_BASE | 0x000B)
#define ID_STATUS_A2DP_SNK_SUSPEND_STREAM (ID_A2DP_MSG_BASE | 0x000C)
#define ID_STATUS_A2DP_SRC_INIT (ID_A2DP_MSG_BASE | 0x000D)
#define ID_STATUS_A2DP_SRC_DEINIT (ID_A2DP_MSG_BASE | 0x000E)
#define ID_STATUS_A2DP_SRC_CONFIG_IND (ID_A2DP_MSG_BASE | 0x000F)
#define ID_STATUS_A2DP_DATA_SENT_RSP (ID_A2DP_MSG_BASE | 0x0010)
#define ID_STATUS_A2DP_DATA_CONSMD (ID_A2DP_MSG_BASE | 0x0011)
#define ID_STATUS_A2DP_ABORT_CFM (ID_A2DP_MSG_BASE | 0x0012)
#define ID_STATUS_A2DP_ABORT_IND (ID_A2DP_MSG_BASE | 0x0013)
#define ID_STATUS_A2DP_CONNECTED_IND (ID_A2DP_MSG_BASE | 0x0014)
#define ID_STATUS_A2DP_SRC_START_STREAM_IND (ID_A2DP_MSG_BASE | 0x0015) /*Changes_for_handling_start_stream_ind*/
#define ID_STATUS_A2DP_SRC_CODEC_INFO (ID_A2DP_MSG_BASE | 0x0016)
#define ID_STATUS_A2DP_SRC_REMOTE_CODEC_INFO (ID_A2DP_MSG_BASE | 0x0017)
#define ID_STATUS_A2DP_SRC_PRE_SBC_IND (ID_A2DP_MSG_BASE | 0x0018)
#define ID_STATUS_A2DP_SRC_CLEAR_BUF_IND (ID_A2DP_MSG_BASE | 0x0019)

//OPP Client message define
#define ID_STATUS_OPC_CONNECT_RES (ID_OPC_MSG_BASE | 0x0001)
#define ID_STATUS_OPC_SERVER_ABORT (ID_OPC_MSG_BASE | 0x0002)
#define ID_STATUS_OPC_PUT_ABORT (ID_OPC_MSG_BASE | 0x0003)
#define ID_STATUS_OPC_PUT_OBJECT (ID_OPC_MSG_BASE | 0x0004)
#define ID_STATUS_OPC_PUT_FINAL (ID_OPC_MSG_BASE | 0x0005)
#define ID_STATUS_OPC_DISCONNECT_IND (ID_OPC_MSG_BASE | 0x0006)
#define ID_STATUS_OPC_GET_DATA_FROM_APP (ID_OPC_MSG_BASE | 0x0007)
#define ID_STATUS_OPC_SEND_REQ (ID_OPC_MSG_BASE | 0x0008)
#define ID_STATUS_OPC_CLOSE_RES (ID_OPC_MSG_BASE | 0x0009)
#define ID_STATUS_OPP_RESTORE_CLK (ID_OPC_MSG_BASE | 0x000A)
#define ID_STATUS_OPP_CHANGE_CLK (ID_OPC_MSG_BASE | 0x000B)
#define ID_STATUS_OPS_CLOSE_RES (ID_OPC_MSG_BASE | 0x000C)

//OPP Server Message Define
#define ID_STATUS_OPS_PUT_REQ (ID_OPS_MSG_BASE | 0x0001)
#define ID_STATUS_OPS_CLIENT_ABORT (ID_OPS_MSG_BASE | 0x0002)
#define ID_STATUS_OPS_PUT_ABORT (ID_OPS_MSG_BASE | 0x0004)
#define ID_STATUS_OPS_PUT_OBJECT (ID_OPS_MSG_BASE | 0x0005)
#define ID_STATUS_OPS_PUT_FINAL (ID_OPS_MSG_BASE | 0x0006)
#define ID_STATUS_OPS_CONNECT_REQ (ID_OPS_MSG_BASE | 0x0007)
#define ID_STATUS_OPS_DISCONNECT_IND (ID_OPS_MSG_BASE | 0x0008)

//FTP Client message define
#define ID_STATUS_FTC_CONNECT_RES (ID_FTC_MSG_BASE | 0x0001)
#define ID_STATUS_FTC_SERVER_ABORT (ID_FTC_MSG_BASE | 0x0002)
#define ID_STATUS_FTC_PUT_ABORT (ID_FTC_MSG_BASE | 0x0003)
#define ID_STATUS_FTC_PUT_OBJECT (ID_FTC_MSG_BASE | 0x0004)
#define ID_STATUS_FTC_PUT_FINAL (ID_FTC_MSG_BASE | 0x0005)
#define ID_STATUS_FTC_GET_ABORT (ID_FTC_MSG_BASE | 0x0006)
#define ID_STATUS_FTC_GET_OBJECT (ID_FTC_MSG_BASE | 0x0007)
#define ID_STATUS_FTC_GET_FINAL (ID_FTC_MSG_BASE | 0x0008)
#define ID_STATUS_FTC_GET_LIST_FOLDER (ID_FTC_MSG_BASE | 0x0009)
#define ID_STATUS_FTC_ADD_FOLDER_RES (ID_FTC_MSG_BASE | 0x000A)
#define ID_STATUS_FTC_DEL_OBJ_RES (ID_FTC_MSG_BASE | 0x000B)
#define ID_STATUS_FTC_DISCONNECT_IND (ID_FTC_MSG_BASE | 0x000C)

//FTP Server Message Define
#define ID_STATUS_FTS_PUT_REQ (ID_FTS_MSG_BASE | 0x0001)
#define ID_STATUS_FTS_CLIENT_ABORT (ID_FTS_MSG_BASE | 0x0002)
#define ID_STATUS_FTS_PUT_ABORT (ID_FTS_MSG_BASE | 0x0003)
#define ID_STATUS_FTS_PUT_OBJECT (ID_FTS_MSG_BASE | 0x0004)
#define ID_STATUS_FTS_PUT_FINAL (ID_FTS_MSG_BASE | 0x0005)
#define ID_STATUS_FTS_GET_REQ (ID_FTS_MSG_BASE | 0x0006)
#define ID_STATUS_FTS_GET_ABORT (ID_FTS_MSG_BASE | 0x0007)
#define ID_STATUS_FTS_GET_OBJECT (ID_FTS_MSG_BASE | 0x0008)
#define ID_STATUS_FTS_GET_FINAL (ID_FTS_MSG_BASE | 0x0009)
#define ID_STATUS_FTS_ADD_FOLDER_REQ (ID_FTS_MSG_BASE | 0x000A)
#define ID_STATUS_FTS_DEL_REQ (ID_FTS_MSG_BASE | 0x000B)
#define ID_STATUS_FTS_UPDATE_ROOT (ID_FTS_MSG_BASE | 0x000C)
#define ID_STATUS_FTS_DEL_BUSY (ID_FTS_MSG_BASE | 0x000D)
#define ID_STATUS_FTS_CONNECT_REQ (ID_FTS_MSG_BASE | 0x000E)
#define ID_STATUS_FTS_DISCONNECT_IND (ID_FTS_MSG_BASE | 0x000F)

/* Added by eddie.li */
/* DUN Gateway Message Define */
#define ID_STATUS_DG_LINK_STATUS_RES (ID_DG_MSG_BASE | 0x0001)
#define ID_STATUS_DG_LINK_REQ (ID_DG_MSG_BASE | 0x0002)
#define ID_STATUS_DG_LINK_CLOSE_IND (ID_DG_MSG_BASE | 0x0003)
/* Added by eddie.li end.*/

/* Added by eddie.li */
#define ID_STATUS_SPP_LINK_STATUS_RES (ID_SPP_MSG_BASE | 0x0001)
#define ID_STATUS_SPP_DATA_RECIEVE_IND (ID_SPP_MSG_BASE | 0x0002)
#define ID_STATUS_SPP_DATA_RECIEVE_CFM (ID_SPP_MSG_BASE | 0x0003)
#define ID_STATUS_SPP_CONNECT_IND (ID_SPP_MSG_BASE | 0x0004)
#define ID_STATUS_SPP_DISCONNECT_IND (ID_SPP_MSG_BASE | 0x0005)
#define ID_STATUS_SPP_STATUS_IND (ID_SPP_MSG_BASE | 0x0006)
#define ID_STATUS_SPP_SERVICE_NAME_IND (ID_SPP_MSG_BASE | 0x0007)
#define ID_STATUS_SPP_DATA_RECEIVE_EX_IND (ID_SPP_MSG_BASE | 0x0008)
#define ID_STATUS_SPP_CONNECT_RES (ID_SPP_MSG_BASE | 0x0009)
#define ID_STATUS_SPP_DISCONNECT_RES (ID_SPP_MSG_BASE | 0x000A)
#define ID_STATUS_SPP_DATA_SEND_RES (ID_SPP_MSG_BASE | 0x000B)

/*  BPP MESSAGE GROUTP Begin*/
#define ID_STATUS_BPPC_CONNECT_CFM (ID_BPP_MSG_BASE | 0x0001)
#define ID_STATUS_BPPC_CONNECT_IND (ID_BPP_MSG_BASE | 0x0002)
#define ID_STATUS_BPPC_DISCONNECT_IND (ID_BPP_MSG_BASE | 0x0003)
#define ID_STATUS_BPPC_SEND_FINISH_CFM (ID_BPP_MSG_BASE | 0x0004)
#define ID_STATUS_BPPC_DOCUMENT_IND (ID_BPP_MSG_BASE | 0x0005)
#define ID_STATUS_BPPC_ABORT_CFM (ID_BPP_MSG_BASE | 0x0006)
#define ID_STATUS_BPPC_PRINTER_ATTRIBUTES_CFM (ID_BPP_MSG_BASE | 0x0007)
#define ID_STATUS_BPPC_JOB_ATTRIBUTES_IND (ID_BPP_MSG_BASE | 0x0008)
#define ID_STATUS_BPPC_JOB_ATTRIBUTES_CFM (ID_BPP_MSG_BASE | 0x0009)
#define ID_STATUS_BPPC_EVENT_IND (ID_BPP_MSG_BASE | 0x000A)
#define ID_STATUS_BPPC_PRINTER_ATTRIBUTES_IND (ID_BPP_MSG_BASE | 0x000B)
#define ID_STATUS_BPPC_JOB_CREATE_CFM (ID_BPP_MSG_BASE | 0x000C)
#define ID_STATUS_BPPC_REF_OBJ_IND (ID_BPP_MSG_BASE | 0x000D)

//HFU Message Define
#define ID_STATUS_HFU_SLC_CONNECTED (ID_HFU_MSG_BASE | 0x0001)
#define ID_STATUS_HFU_SLC_DISCONNECTED (ID_HFU_MSG_BASE | 0x0002)
#define ID_STATUS_HFU_AUDIO_CONNECTED (ID_HFU_MSG_BASE | 0x0003)
#define ID_STATUS_HFU_AUDIO_DISCONNECTED (ID_HFU_MSG_BASE | 0x0004)
#define ID_STATUS_HFU_CIND_IND (ID_HFU_MSG_BASE | 0x0005)
#define ID_STATUS_HFU_RING_IND (ID_HFU_MSG_BASE | 0x0006)
#define ID_STATUS_HFU_CLIP_IND (ID_HFU_MSG_BASE | 0x0007)
#define ID_STATUS_HFU_CIEV_IND (ID_HFU_MSG_BASE | 0x0008)
#define ID_STATUS_HFU_CLCC_RECORD_IND (ID_HFU_MSG_BASE | 0x0009)
#define ID_STATUS_HFU_DIAL_NUMBER_RESULT_IND (ID_HFU_MSG_BASE | 0x000a)
#define ID_STATUS_HFU_ATA_RESULT_IND (ID_HFU_MSG_BASE | 0x000b)
#define ID_STATUS_HFU_HANGUP_RESULT_IND (ID_HFU_MSG_BASE | 0x000c)
#define ID_STATUS_HFU_SEND_DTMF_RESULT_IND (ID_HFU_MSG_BASE | 0x000d)
#define ID_STATUS_HFU_CALLHOLD_RESULT_IND (ID_HFU_MSG_BASE | 0x000e)
#define ID_STATUS_HFU_CLCC_RESULT_IND (ID_HFU_MSG_BASE | 0x000f)
#define ID_STATUS_HFU_SPK_VOL_IND (ID_HFU_MSG_BASE | 0x0010)
#define ID_STATUS_HFU_MIC_VOL_IND (ID_HFU_MSG_BASE | 0x0011)
#define ID_STATUS_HFU_CCWA_IND (ID_HFU_MSG_BASE | 0x0012)
#define ID_STATUS_HFU_ENABLE_VOICE_RECOGN_RESULT_IND (ID_HFU_MSG_BASE | 0x0013)
#define ID_STATUS_HFU_VOICE_RECOGN_STATE_IND (ID_HFU_MSG_BASE | 0x0014)
#define ID_STATUS_HFU_REDIAL_RESULT_IND (ID_HFU_MSG_BASE | 0x0015)
#define ID_STATUS_HFU_MEMORY_DIAL_RESULT_IND (ID_HFU_MSG_BASE | 0x0016)
#define ID_STATUS_HFP_CONNECT_RES (ID_HFU_MSG_BASE | 0x0017)
#define ID_STATUS_HFP_DISCONNECT_RES (ID_HFU_MSG_BASE | 0x0018)

/*  PBAP MESSAGE GROUTP Begin*/
#define ID_STATUS_PBAP_TP_DISCONNECTED (ID_PBAP_MSG_BASE | 0x0002)
#define ID_STATUS_PBAP_COMPLETE (ID_PBAP_MSG_BASE | 0x0003)
#define ID_STATUS_PBAP_ABORT (ID_PBAP_MSG_BASE | 0x0004)
#define ID_STATUS_PBAP_PARAMS_RX (ID_PBAP_MSG_BASE | 0x0006)
#define ID_STATUS_PBAP_CLIENT_PARAMS_RX (ID_PBAP_MSG_BASE | 0x0006)
#define ID_STATUS_PBAP_SERVER_PARAMS_RX (ID_PBAP_MSG_BASE | 0x0007)
#define ID_STATUS_PBAP_DATA_IND (ID_PBAP_MSG_BASE | 0x000C)

/*  MAP MESSAGE GROUTP Begin*/
#define ID_STATUS_MAP_TP_DISCONNECTED (ID_MAP_MSG_BASE | 0x0001)
#define ID_STATUS_MAP_COMPLETE (ID_MAP_MSG_BASE | 0x0002)
#define ID_STATUS_MAP_ABORT (ID_MAP_MSG_BASE | 0x0003)
#define ID_STATUS_MAP_CLIENT_PARAMS_RX (ID_MAP_MSG_BASE | 0x0004)
#define ID_STATUS_MAP_SERVER_PARAMS_RX (ID_MAP_MSG_BASE | 0x0005)
#define ID_STATUS_MAP_DATA_IND (ID_MAP_MSG_BASE | 0x0006)
#define ID_STATUS_MAP_MNS_SERVER_CONNECTED (ID_MAP_MSG_BASE | 0x0007)
#define ID_STATUS_MAP_MNS_SERVER_PARAMS_RX (ID_MAP_MSG_BASE | 0x0008)
#define ID_STATUS_MAP_MNS_SERVER_DATA_IND (ID_MAP_MSG_BASE | 0x0009)
#define ID_STATUS_MAP_CALLBACK_CALL (ID_MAP_MSG_BASE | 0x000A)
/*  A2DP SINK MESSAGE GROUTP Begin*/
#define ID_STATUS_A2DP_SINK_CONNECT_COMPLETED (ID_A2DP_SINK_BASE | 0x0001)
#define ID_STATUS_A2DP_SINK_DISCONNECT_COMPLETED (ID_A2DP_SINK_BASE | 0x0002)
#define ID_STATUS_A2DP_SINK_STREAM_STARTED (ID_A2DP_SINK_BASE | 0x0003)
#define ID_STATUS_A2DP_SINK_STREAM_SUSPENDED (ID_A2DP_SINK_BASE | 0x0004)
#define ID_STATUS_A2DP_SINK_CDEC_INFO (ID_A2DP_SINK_BASE | 0x0005)

//SAP Message define
#define ID_STATUS_SAP_CONNECTED (ID_SAP_MSG_BASE | 0x0001)
#define ID_STATUS_SAP_DISCONNECTED (ID_SAP_MSG_BASE | 0x0002)
#define ID_STATUS_SAP_POWERON_SIM_CNF (ID_SAP_MSG_BASE | 0x0003)
#define ID_STATUS_SAP_POWEROFF_SIM_CNF (ID_SAP_MSG_BASE | 0x0004)
#define ID_STATUS_SAP_RESET_SIM_CNF (ID_SAP_MSG_BASE | 0x0005)
#define ID_STATUS_SAP_TRANSFER_APDU_CNF (ID_SAP_MSG_BASE | 0x0006)
#define ID_STATUS_SAP_TRANSFER_ATR_CNF (ID_SAP_MSG_BASE | 0x0007)
#define ID_STATUS_SAP_TRANSFER_READER_STATUS_CNF (ID_SAP_MSG_BASE | 0x0008)
#define ID_STATUS_SAP_SET_TRANSFER_PROTOCOL_CNF (ID_SAP_MSG_BASE | 0x0009)
#define ID_STATUS_SAP_DISCONNECT_IND (ID_SAP_MSG_BASE | 0x000a)
#define ID_STATUS_SAP_CARD_STATUS_IND (ID_SAP_MSG_BASE | 0x000b)

#ifdef BT_SPP_2ND_CHANNEL_SUPPORT
#define ID_STATUS_SPP2_LINK_STATUS_RES (ID_SPP2_MSG_BASE | 0x0001)
#define ID_STATUS_SPP2_DATA_RECIEVE_IND (ID_SPP2_MSG_BASE | 0x0002)
#define ID_STATUS_SPP2_DATA_RECIEVE_CFM (ID_SPP2_MSG_BASE | 0x0003)
#define ID_STATUS_SPP2_CONNECT_IND (ID_SPP2_MSG_BASE | 0x0004)
#define ID_STATUS_SPP2_DISCONNECT_IND (ID_SPP2_MSG_BASE | 0x0005)
#define ID_STATUS_SPP2_STATUS_IND (ID_SPP2_MSG_BASE | 0x0006)
#define ID_STATUS_SPP2_SERVICE_NAME_IND (ID_SPP2_MSG_BASE | 0x0007)
#endif

//HOGP Message Info
#define ID_STATUS_HOGP_NOTIFY_RECIEVE_IND (ID_HOGP_MSG_BASE | 0x0001)
#define ID_STATUS_HOGP_ENABLE (ID_HOGP_MSG_BASE | 0x0002)
#define ID_STATUS_HOGP_DISABLE (ID_HOGP_MSG_BASE | 0x0003)
#define ID_STATUS_HOGP_GET_BATTERY_LEVEL_IND (ID_HOGP_MSG_BASE | 0x0004)
//ANCS Message Info
#define ID_STATUS_ANCS_GATT_SERVER_RES (ID_ANCS_MSG_BASE | 0x0001)
#define ID_STATUS_CTSC_DATE_RES (ID_ANCS_MSG_BASE | 0x0002)
#define ID_STATUS_CTSC_TIME_RES (ID_ANCS_MSG_BASE | 0x0003)
#define ID_STATUS_CTSC_TIME_FORMAT_RES (ID_ANCS_MSG_BASE | 0x0004)
#define ID_STATUS_CTSC_LANGUAGE_RES (ID_ANCS_MSG_BASE | 0x0005)
//HID Message Info
#define ID_STATUS_HID_CONNECTED_IND (ID_HID_MSG_BASE | 0x0001)
#define ID_STATUS_HID_DISCONNECTED_IND (ID_HID_MSG_BASE | 0x0002)
//AntiLost Messgae Info
#define ID_STATUS_ANTILOST_READ_RSSI_RES (ID_ANTILOST_MSG_BASE | 0x0001)
#define ID_STATUS_ANTILOST_SET_RSSI_MONITOR_RES (ID_ANTILOST_MSG_BASE | 0x0002)
#define ID_STATUS_ANTILOST_RSSI_ALERT_RES (ID_ANTILOST_MSG_BASE | 0x0003)
// PXP AND FDM
#define ID_PXP_LLS_SERVICE_CONN_COMP (ID_PXP_MSG_BASE | 0X0001)
#define ID_PXP_LLS_SERVICE_DISCONN_COMP (ID_PXP_MSG_BASE | 0X0002)
#define ID_PXP_IAS_SERVICE_ALERT_LEVEL_IND (ID_PXP_MSG_BASE | 0X0003)
#define ID_STATUS_GATT_SERVER_REG_READ_RES (ID_GATT_SERVER_MSG_BASE | 0x0001)
#define ID_STATUS_GATT_SERVER_REG_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0002)
#define ID_STATUS_GATT_SERVER_DEREG_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0003)
#define ID_STATUS_GATT_SERVER_SERVICE_READ_RES (ID_GATT_SERVER_MSG_BASE | 0x0004)
#define ID_STATUS_GATT_SERVER_SERVICE_ADD_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0005)
#define ID_STATUS_GATT_SERVER_SERVICE_DEL_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0006)
#define ID_STATUS_GATT_SERVER_CHAR_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0007)
#define ID_STATUS_GATT_SERVER_DESC_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0008)
#define ID_STATUS_GATT_SERVER_SERVICE_START_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x0009)
#define ID_STATUS_GATT_SERVER_SERVICE_STOP_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x000A)
#define ID_STATUS_GATT_SERVER_INDICAT_NOTIFY_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x000B)
#define ID_STATUS_GATT_SERVER_REQ_RECIEVE_IND (ID_GATT_SERVER_MSG_BASE | 0x000C)
#define ID_STATUS_GATT_SERVER_RSP_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x000D)
#define ID_STATUS_GATT_SERVER_CONN_READ_RES (ID_GATT_SERVER_MSG_BASE | 0x000E)
#define ID_STATUS_GATT_SERVER_DISCONN_WRITE_RES (ID_GATT_SERVER_MSG_BASE | 0x000F)

//BLE CMD CONTROL
#define ID_BLE_CMD_CONTROL_IND (ID_BLE_CMD_CONTROL_BASE | 0x0001)
#define ID_BLE_CMD_CONTROL_MAKE_WATCH_RING (ID_BLE_CMD_CONTROL_BASE | 0x0002)
#define ID_BLE_CMD_CONTROL_UPDATE_VERSION (ID_BLE_CMD_CONTROL_BASE | 0x0003)

#define ID_STATUS_AT_BLE_SET_PUBLIC_ADDR_RES (ID_AT_MSG_BASE | 0x0001)
#define ID_STATUS_AT_BLE_SET_RANDOM_ADDR_RES (ID_AT_MSG_BASE | 0x0002)
#define ID_STATUS_AT_BLE_ADD_WHITE_LIST_RES (ID_AT_MSG_BASE | 0x0003)
#define ID_STATUS_AT_BLE_REMOVE_WHITE_LIST_RES (ID_AT_MSG_BASE | 0x0004)
#define ID_STATUS_AT_BLE_CLEAR_WHITE_LIST_RES (ID_AT_MSG_BASE | 0x0005)
#define ID_STATUS_AT_BLE_CONNECT_RES (ID_AT_MSG_BASE | 0x0006)
#define ID_STATUS_AT_BLE_DISCONNECT_RES (ID_AT_MSG_BASE | 0x0007)
#define ID_STATUS_AT_BLE_UPDATA_CONNECT_RES (ID_AT_MSG_BASE | 0x0008)
#define ID_STATUS_AT_BLE_SET_ADV_PARA_RES (ID_AT_MSG_BASE | 0x0009)
#define ID_STATUS_AT_BLE_SET_ADV_ENABLE_RES (ID_AT_MSG_BASE | 0x000A)
#define ID_STATUS_AT_BLE_SET_SCAN_RSP_RES (ID_AT_MSG_BASE | 0x000B)
#define ID_STATUS_AT_BLE_SET_SCAN_PARA_RES (ID_AT_MSG_BASE | 0x000C)
#define ID_STATUS_AT_BLE_SET_SCAN_ENABLE_RES (ID_AT_MSG_BASE | 0x000D)
#define ID_STATUS_AT_BLE_SET_SCAN_DISABLE_RES (ID_AT_MSG_BASE | 0x000E)
#define ID_STATUS_AT_BLE_SET_SCAN_REPORT_RES (ID_AT_MSG_BASE | 0x000F)
#define ID_STATUS_AT_BLE_SET_SCAN_FINISH_RES (ID_AT_MSG_BASE | 0x00010)
#define ID_STATUS_AT_BLE_SET_ADV_DATA_RES (ID_AT_MSG_BASE | 0x0011)

/******************************************************************************
 * BT service type
 ******************************************************************************/
#define BT_SERVICE_NONE 0x0000
#define BT_SERVICE_HANDFREE 0x0001
#define BT_SERVICE_HANDSET 0x0002
#define BT_SERVICE_OPP 0x0004
#define BT_SERVICE_FTP 0x0008
#define BT_SERVICE_A2DP 0x0010
#define BT_SERVICE_AVRCP 0x0020
#define BT_SERVICE_SPP 0x0040
#define BT_SERVICE_A2DP_SRC 0x0080
#define BT_SERVICE_AVRCP_TG 0x0100
#define BT_SERVICE_MAP_SERVER 0x0200
#define BT_SERVICE_PBAP_SERVER 0x0400
#define BT_SERVICE_HANDFREE_GATEWAY 0x0800
#define BT_SERVICE_HEADSET_GATEWAY 0x1000
#define BT_SERVICE_PBAP_CLIENT 0x2000
#define BT_SERVICE_BLE_HID 0x3000
#define BT_SERVICE_ALL 0xFFFF

/***********************************************************************************
* Uart Define
************************************************************************************/
#define BT_COM0 0
#define BT_COM1 1
#define BT_COM2 2
#define BT_COM3 3
#define BT_COM_MAX 4

/***********************************************************************************
* Uart Baudrate Define
************************************************************************************/
#define BT_BAUD_1200 1200
#define BT_BAUD_2400 2400
#define BT_BAUD_4800 4800
#define BT_BAUD_9600 9600
#define BT_BAUD_19200 19200
#define BT_BAUD_38400 38400
#define BT_BAUD_57600 57600
#define BT_BAUD_115200 115200
#define BT_BAUD_230400 230400
#define BT_BAUD_460800 460800
#define BT_BAUD_921600 921600
#define BT_BAUD_1382400 1382400
#define BT_BAUD_3250000 3250000

enum
{
    LEGACY_JUSTWORK,
    LEGACY_PASSKEY,
    SECURE_CONNECTION_JUSTWORK,
    SECURE_CONNECTION_PASSKEY,
};

#define DISPLAY_ONLY 0x00
#define DISPLAY_YESNO 0x01
#define KEYBOARD_ONLY 0x02
#define NOIN_NOOUT 0x03
#define KEYBOARD_DISP 0x04

typedef enum
{
    BT_START_SCHED_MSG = BT_OS_MSG_START,
    BT_STOP_SCHED_MSG,
    BT_INT_HANDLE_MSG,
    BT_GSM_L1_INFO_MSG,
    BT_START_CFM_MSG,
    BT_STOP_CFM_MSG,
    BT_START_GATT_CLIENT_MSG
} CSR_BT_SCHED_MSG_E;

typedef enum
{
    BT_AVRCP_PAUSE = 0x00,
    BT_AVRCP_START,
    BT_AVRCP_PREVI,
    BT_AVRCP_NEXT,
} bt_handle_avrcp_state_t;

enum
{
    BT_MODULE_HOST_CORE = 0,
    BT_MODULE_ADP = 1,
    BT_MODULE_HCI = 2,
    BT_MODULE_MANAGER = 3,
    BT_MODULE_L2CAP = 4,
    BT_MODULE_AMP = 5,
    BT_MODULE_SDP_CLI = 6,
    BT_MODULE_SDP_SRV = 7,
    BT_MODULE_RFCOMM = 8,
    BT_MODULE_OBEX_CLI = 9,
    BT_MODULE_OBEX_SRV = 10,
    BT_MODULE_A2DP = 11,
    BT_MODULE_AVRCP = 12,
    BT_MODULE_HFP = 13,
    BT_MODULE_HID = 14,
    BT_MODULE_TCS = 15,
    BT_MODULE_SPP = 16,
    BT_MODULE_HCRP = 17,
    BT_MODULE_PAN = 18,
    BT_MODULE_ATT = 19,
    BT_MODULE_SMP = 20,
    BT_MODULE_GATT = 21,
    BT_MODULE_MESH = 22,
    BT_MAX_MODULE = 36
};

typedef enum
{
    BT_STOPED,
    BT_STOPING,
    BT_STARTING,
    BT_STARTED
} BT_STATE_E;

typedef enum
{
    BT_NONE_STATE = 0x00,
    BT_ADV_STATE,
    BT_SCAN_STATE,
    BT_CONN_STATE
} bt_at_state_t;

typedef enum
{
    BT_TASK_STOP,
    BT_TASK_STARTED
} BT_SCHED_TASK_STATE_E;

typedef enum
{
    OS_TIME_EVENT = 0x1000,
    OS_MSG_EXTEND_EVM = 0X1001,
    OS_MSG_EXTEND_TRIG_DATA = 0X1002,
    OS_MSG_SAVE_PCM_DATA = 0X1003,
} OS_INT_MSG_TYPE_E;

typedef struct _st_white_list_info
{
    uint8 addr_type;
    uint8 addr[SIZE_OF_BDADDR];
} st_white_list_info;

typedef struct _st_white_list
{
    uint8 is_used;
    st_white_list_info white_list_info;
} st_white_list;

typedef struct _st_scan_report_info
{
    UINT8 name_length;
    UINT8 name[MGR_MAX_REMOTE_NAME_LEN];
    UINT8 addr_type;
    BT_ADDRESS bdAddress;
    UINT8 event_type;
    UINT8 data_length;
    UINT8 rssi;
} st_scan_report_info;

typedef struct _st_at_scan_report_info
{
    uint8 num;
    st_scan_report_info report_info[10];
} st_at_scan_report_info;

typedef struct _BT_DEVICE_INFO
{
    uint16 name[BT_DEVICE_NAME_SIZE];
    BT_ADDRESS addr;
    uint32 dev_class;
    uint32 rssi;
} BT_DEVICE_INFO;

typedef struct _BT_PCM_CONFIG
{
    uint8 slot_number; /* starting from 0 */
    uint32 config;     //config the pcm of bt chip
    uint32 format;
} BT_PCM_CONFIG;

typedef struct bt_config_info_tag
{
    BT_ADDRESS bd_addr;
    uint16 local_name[BT_DEVICE_NAME_SIZE];
    uint16 local_ble_name[BT_DEVICE_NAME_SIZE];
    uint32 port_num;
    uint32 baud_rate;
    BT_PCM_CONFIG pcm_config;
    uint16 bt_file_dir[16];
    uint32 bt_task_priority;
    uint32 bt_crystal_freq; //KHZ
    uint16 xtal_dac;
    uint32 host_nv_ctrl[2];
} bt_config_info_t;

typedef struct bt_nv_param_tag
{
    BT_ADDRESS bd_addr;
    uint16 xtal_dac;
} bt_nv_param_t;

typedef struct bt_pskey_nv_info_tag
{
    uint8 g_dbg_source_sink_syn_test_data;
    uint8 g_sys_sleep_in_standby_supported;
    uint8 g_sys_sleep_master_supported;
    uint8 g_sys_sleep_slave_supported;

    uint32 default_ahb_clk;
    uint32 device_class;
    uint32 win_ext;
    uint32 g_aGainValue[6];
    uint32 g_aPowerValue[5];

    uint8 feature_set[16];
    uint8 device_addr[6];
    uint8 g_sys_sco_transmit_mode;             //true tramsmit by uart, otherwise by share memory
    uint8 g_sys_uart0_communication_supported; //true use uart0, otherwise use uart1 for debug

    uint8 edr_tx_edr_delay;
    uint8 edr_rx_edr_delay;
    uint16 g_wbs_nv_117;

    uint32 is_wdg_supported;
    uint32 share_memo_rx_base_addr;
    uint32 share_memo_tx_base_addr;
    uint32 share_memo_tx_packet_num_addr;
    uint32 share_memo_tx_data_base_addr;
    uint32 g_PrintLevel;

    uint16 share_memo_tx_block_length;
    uint16 share_memo_rx_block_length;

    uint16 share_memo_tx_water_mark;
    uint16 share_memo_tx_timeout_value;

    uint16 uart_rx_watermark;
    uint16 uart_flow_control_thld;

    uint32 comp_id;

    uint16 pcm_clk_divd;
    uint16 g_nbs_nv_118;

    uint16 br_edr_diff_reserved;
    uint16 br_edr_powercontrol_flag;

    uint32 gain_br_channel_power[8];
    uint32 gain_edr_channel_power[8];
    uint32 g_aLEPowerControlFlag;
    uint16 g_aLEChannelpwrvalue[8];
    uint32 host_reserved[2];
    uint32 config0;
    uint32 config1;
    uint32 config2;
    uint32 config3;
    //uint16 bt_calibration_flag;//this is NOT in NV bt_sprd, this is defined in host/controller PSK structrue
    //uint16 bt_log_set;//this is NOT in NV bt_sprd, this is defined in host/controller PSK structrue
    uint32 reserved1[4];
    //uint32  chip_version;// 0 old 8910: 1 new 8910 -----
    uint16 reserved2[3];
} bt_pskey_nv_info_t;

typedef struct _BT_AT_SPP_DATA
{
    UINT8 *buf;
    UINT16 dataLen;
} BT_AT_SPP_DATA;

typedef struct BT_INT_MSG_T
{
    unsigned int message[4];
} BT_INT_MSG_T;

typedef struct BT_SIGNAL_T
{
    uint16 SignalCode;
    uint16 SignalSize;
    void *SigP;
} BT_SIGNAL_T;

typedef struct _BT_NONSIG_DATA
{
    uint8 rssi;
    BT_STATUS status;
    uint32 pkt_cnt;
    uint32 pkt_err_cnt;
    uint32 bit_cnt;
    uint32 bit_err_cnt;
} BT_NONSIG_DATA;
typedef void (*BT_NONSIGCALLBACK)(const BT_NONSIG_DATA *data);

typedef struct _BT_NONSIG_PACKET
{
    uint16 pac_type; //enum{ NULLpkt, POLLpkt, FHSpkt, DM1, DH1, HV1, HV2, HV3, DV, AUX1, DM3, DH3, EV4, EV5, DM5, DH5, IDpkt, INVALIDpkt, EDR_2DH1, EV3, EDR_2EV3, EDR_3EV3, EDR_3DH1, EDR_AUX1, EDR_2DH3, EDR_3DH3, EDR_2EV5, EDR_3EV5, EDR_2DH5, EDR_3DH5};
    uint16 pac_len;  //according to packet type, max length is {0, 0, 18, 17, 27, 10, 20, 30, 9, 29, 121, 183, 120, 180, 224, 339, 0, 0, 54, 30, 60, 90, 83, 29, 367, 552, 360, 540, 679, 1021}
    uint16 pac_cnt;  //0~65536
} BT_NONSIG_PACKET;

typedef struct _BT_NONSIG_POWER
{
    uint8 power_type;   //0 or 1
    uint16 power_value; //0~33
} BT_NONSIG_POWER;

typedef struct _BT_NONSIG_PARAM
{
    uint8 pattern; //00000000 ---1, 11111111 ---2, 10101010 ---3, PRBS9 ---4, 11110000 ---9
    uint8 channel; //255 or 0~78
    union {
        uint8 rx_gain; //0~32
        BT_NONSIG_POWER power;
    } item;
    BT_NONSIG_PACKET pac;
    uint8 le_phy;          // 1:LE1M, 2:LE2M, 3:LE Coded S8, 4:LE Coded S2 |  3:LE Coded in RX
    uint8 le_rx_mod_index; // 0:Standard;  1:Stable
} BT_NONSIG_PARAM;

typedef struct _BT_MSG_Tag
{
    uint32 msg_id;
    uint8 status;
    uint32 body_size;
    void *body_ptr;
} BT_MSG_T;
/******************************************************************************
 * BT callback function
 ******************************************************************************/
typedef void (*BT_CALLBACK)(const BT_MSG_T *msg);
typedef void (*BT_CALLBACK_STACK)(unsigned int msg_id, char status, void *data_ptr);
typedef void (*BLE_WRITE_UART_CALLBACK)(short dlen, uint8 *data);

bool bt_SendMsgToSchedTask(uint32 event, void *msg_ptr);
void bt_at_callback_register(BT_CALLBACK_STACK callback);
void ble_at_callback_register(BLE_WRITE_UART_CALLBACK callback);

BT_STATUS app_bt_start(void);

BT_STATUS app_bt_stop(void);

BOOLEAN app_bt_state_get(void);

BT_STATUS app_bt_local_name_set(const uint16 *name);

const char *app_bt_version_info_get(void);

BT_STATUS app_ble_adv_enable(bool adv_en);

BT_STATUS app_bt_device_search_start(uint32 service_type);

BT_STATUS app_bt_device_search_cancel(void);

uint32 app_bt_device_search_state_get(void);

uint32 app_bt_searched_device_count_get(void);

uint32 app_bt_searched_device_info_get(uint8 *addr, uint8 *name, uint8 idx);

uint8 app_ble_connect(UINT8 addr_type, bdaddr_t *addr);

uint8 app_ble_disconnect(UINT16 handle);

void app_ble_adv_data_set(uint8 len, char *data);

uint8 app_ble_adv_para_set(unsigned short AdvMin, unsigned short AdvMax, unsigned char AdvType, unsigned char ownAddrType,
                           unsigned char directAddrType, uint8 *directAddr, unsigned char AdvChannMap, unsigned char AdvFilter);

uint8 app_ble_connect_para_update(unsigned short Handle, unsigned short MinInterval,
                                  unsigned short MaxInterval, unsigned short Latency, unsigned short Timeout);

BT_STATUS app_bt_scan_mode_set(uint8 discoverable, uint8 connectable);

uint8 app_bt_scan_mode_get(void);

BT_STATUS app_bt_local_name_get(uint16 *name);

BT_STATUS app_bt_public_addr_set(BT_ADDRESS *addr);

BT_STATUS app_bt_public_addr_get(BT_ADDRESS *addr);

BT_STATUS app_bt_device_pair(const BT_ADDRESS *btaddr);

BT_STATUS app_bt_device_pair_cancel(const BT_ADDRESS *btaddr);

int app_bt_paired_device_num_get(uint32 service_type);

BT_STATUS app_bt_paired_device_info_get(uint32 service_type, int index, BT_DEVICE_INFO *info);

BT_STATUS app_bt_paired_device_remove(const BT_ADDRESS *addr);

bool app_bt_is_spp_idle(void);

bool app_bt_is_spp_connected(void);

BT_STATUS app_bt_spp_connect(BT_ADDRESS *bd_addr, BT_SPP_PORT_SPEED_E spp_speed);

BT_STATUS app_bt_spp_disconnect(void);

bool app_bt_hfp_conn_status_get(const BT_ADDRESS *addr);

BT_STATUS app_bt_hfp_connect(const BT_ADDRESS *addr);

BT_STATUS app_bt_hfp_disconnect(const BT_ADDRESS *addr);

UINT8 app_bt_hfp_callstate_get(void);

UINT8 app_bt_hfp_call_setup_status_get(void);

bool app_bt_hfp_one_connected_link_exist(void);

BT_STATUS app_bt_hfp_set_vol(UINT8 Vol);

BT_STATUS app_bt_hfp_call_terminate(void);

BT_STATUS app_bt_hfp_call_answer(UINT8 call_status);

BT_STATUS app_bt_hfp_dial(UINT8 *phone_number);

BT_STATUS app_bt_hfp_vr_set(bool enable);

BT_STATUS app_bt_hfp_ThreeWayCall(UINT8 CtrlCmd);

bool app_bt_avrcp_handle(bt_handle_avrcp_state_t avrcp_state);

INT32 app_bt_avrcp_status_query(void);

bool app_bt_avrcp_volume_set(UINT8 volume);

int8 app_bt_avrcp_volume_get(void);

BT_STATUS app_ble_scan_rsp_data_set(uint8 *data, uint8 length);

BT_STATUS app_ble_scan_para_set(UINT8 type, UINT16 interval, UINT16 window, UINT8 filter, UINT8 own_addr_type);

bt_status_t app_ble_scan_enable(unsigned char ScanEnable);

void app_add_ble_whitelist(bdaddr_t *addr, UINT8 addr_type);

void app_remove_ble_whitelist(bdaddr_t *addr, UINT8 addr_type);

void app_clear_ble_whitelist(void);

bt_status_t app_ble_local_name_set(const char *name, uint8 len);

char *app_ble_local_name_get(void);

uint32 app_bt_spp_data_send(const uint8 *src_buf, uint32 data_len);

uint32 app_bt_get_at_cmd_state(void);

BT_STATUS app_bt_a2dp_connect(const BT_ADDRESS *addr);
BT_STATUS app_bt_a2dp_disconnect(void);
bta2dp_connection_state_t app_bt_a2dp_connect_state(void);

#endif
