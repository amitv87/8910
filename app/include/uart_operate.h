#ifndef _UART_OPERATE_H_
#define _UART_OPERATE_H_

#include "oc_uart.h"
#define UART1   0
#define UART3   2

#define UART_BUFFER_SIZE    1300
extern  char                uart1_recv_data[UART_BUFFER_SIZE];
extern  UINT16              uart1_recv_len;

extern  char                uart3_recv_data[UART_BUFFER_SIZE];
extern  UINT16              uart3_recv_len;

//更新串口信息
void uart1_cfg_update();

void uart1_recv_cb(hal_uart_port_t uart_port, UINT8 *data, UINT16 len, void *arg);
//串口3接收回调
void uart3_recv_cb(hal_uart_port_t uart_port, UINT8 *data, UINT16 len, void *arg);

//串口开
INT32 uart_init(hal_uart_port_t uart_port, hal_uart_config_t *uart_config, uart_input_callback_t recv_cb, void *arg);

//串口写
INT32 uart_write(hal_uart_port_t uart_port, UINT8 *buff, UINT32 len);

//串口关
INT32 uart_deinit(hal_uart_port_t uart_port);

#endif /* _UART_OPERATE_H_ */