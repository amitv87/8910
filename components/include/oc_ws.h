#ifndef OPENCPU_API_OC_WS
#define OPENCPU_API_OC_WS

#include <stdbool.h>
#include <stdint.h>

#include "fibo_opencpu_comm.h"

typedef void (*fibo_ws_data_t)(void *c, void *data, UINT32 len, void *arg);
typedef void (*fibo_ws_close_t)(void *c, INT32 reason, void *arg);

typedef struct fibo_ws_opt_s
{
    UINT32 ping_interval;
    UINT32 ping_timout;
    bool should_handshake;
    const INT8 *origin;
    const INT8 *key;
} fibo_ws_opt_t;

void *fibo_ws_open(const INT8 *url, fibo_ws_opt_t *opt, fibo_ws_data_t data_cb, fibo_ws_close_t close_cb, void *arg);

int fibo_ws_send_binary(void *ws_client, const void *data, UINT32 size);

int fibo_ws_send_text(void *ws_client, const INT8 *data);

int fibo_ws_close(void *c);

#endif /* OPENCPU_API_OC_WS */
