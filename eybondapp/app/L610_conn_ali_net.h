#ifndef _L610_CONN_ALI_NET_H_
#define _L610_CONN_ALI_NET_H_
extern int rev_data_complete ;
extern bool is_ali_conn_success;
extern UINT32 g_sem_send_rrpc;
void mqtt_conn_ali_task(void *param);


#define RES_TOPIC_LEN 250
extern uint8_t rrpc_response_topic[RES_TOPIC_LEN];
extern void* aliyun_mqtt_thread_handle;
#endif