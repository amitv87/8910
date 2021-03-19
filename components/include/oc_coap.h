#ifndef IDH_CODE_COMPONENTS_FIBOCOM_RACOON_OPENCPU_API_OC_COAP
#define IDH_CODE_COMPONENTS_FIBOCOM_RACOON_OPENCPU_API_OC_COAP

#include <stdbool.h>
#include <stdint.h>

#include "fibo_opencpu_comm.h"

enum fibo_coap_opt_s
{
    FIBO_OPTIONS_IF_MATCH = 1,        //,	If-Match	opaque	0-8	(non
    FIBO_OPTIONS_URI_HOST = 3,        //	Uri-Host	string	1-255	(see note
    FIBO_OPTIONS_ETAG = 4,            //ETag	opaque	1-8	(non
    FIBO_OPTIONS_IF_NONE_MATCH = 5,   //	If-None-Match	empty	0	(non
    FIBO_OPTIONS_URI_PORT = 7,        //Uri-Port	uint	0-2	(see note
    FIBO_OPTIONS_LOCATION_PATH = 8,   //Location-Path	string	0-255	(non
    FIBO_OPTIONS_URI_PATH = 11,       //Uri-Path	string	0-255	(non
    FIBO_OPTIONS_CONTENT_FORMAT = 12, //Content-Format	uint	0-2	(non
    FIBO_OPTIONS_MAX_AGE = 14,        //Max-Age	uint	0-4
    FIBO_OPTIONS_URI_QUERY = 15,      //Uri-Query	string	0-255	(non
    FIBO_OPTIONS_ACCEPT = 17,         //Accept	uint	0-2	(non
    FIBO_OPTIONS_LOCATION_QUERY = 20, //Location-Query	string	0-255	(non
    FIBO_OPTIONS_SIZE2 = 28,          //Size2	uint	0-4	(non
    FIBO_OPTIONS_PROXY_URI = 35,      //Proxy-Uri	string	1-1034	(non
    FIBO_OPTIONS_PROXY_SCHEME = 39,   //Proxy-Scheme	string	1-255	(non
};

enum fibo_copa_type_s
{
    FIBOCOM_COAP_MSG_CON = 0,
    FIBOCOM_COAP_MSG_NOC,
    FIBOCOM_COAP_MSG_ACK,
    FIBOCOM_COAP_MSG_RST,
    FIBOCOM_COAP_MSG_MAX
};

enum fibo_coap_method_s
{
    FIBOCOM_COAP_EMPYT,
    FIBOCOM_COAP_GET,
    FIBOCOM_COAP_POST,
    FIBOCOM_COAP_PUT,
    FIBOCOM_COAP_DELETE,
    FIBOCOM_COAP_INVALID_METHOD,
};

typedef enum fibo_coap_method_s fibo_coap_method_t;
typedef enum fibo_copa_type_s fibo_copa_type_t;
typedef enum fibo_coap_opt_s fibo_coap_opt_t;

void *fibo_coap_open(const INT8 *host, UINT16 port, int type);

void *fibo_coap_message_create(INT16 msg_id, fibo_copa_type_t type, UINT8 code_class, fibo_coap_method_t method_code, const UINT8 *value, INT32 len, const INT8 *token, INT32 token_len);

int fibo_coap_message_get_info(void *msg, INT16 *msg_id, fibo_copa_type_t *type, UINT8 *code_class, fibo_coap_method_t *method_code, const UINT8 **value, INT32 *len, const INT8 **token, INT32 *token_len);

int fibo_coap_message_add_uri(void *m, INT8 *uri);

int fibo_coap_message_get_uri(const void *m, char *buf, int len);

int fibo_coap_message_add_option(void *m, fibo_coap_opt_t number, INT8 *value, INT32 len);

int fibo_coap_message_get_option(void *msg, fibo_coap_opt_t number, const INT8 **value, INT32 *len);

int fibo_coap_message_get_payload(void *msg, const UINT8 **value, INT32 *len);

int fibo_coap_message_each_option(void *msg, void (*cb)(fibo_coap_opt_t number, const INT8 *value, INT32 len, void *arg), void *arg);

int fibo_coap_message_destroy(void *m);

int fibo_coap_send(void *c, void *m);

void *fibo_coap_recv(void *c);

int fibo_coap_send_ack(void *c, UINT16 msg_id);

int fibo_coap_close(void *c);

int fibo_copa_get_fd(void *c);

#endif /* IDH_CODE_COMPONENTS_FIBOCOM_RACOON_OPENCPU_API_OC_COAP */
