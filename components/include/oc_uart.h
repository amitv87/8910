#ifndef COMPONENTS_FIBOCOM_OPENCPU_OC_UART_H
#define COMPONENTS_FIBOCOM_OPENCPU_OC_UART_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
//#ifdef CONFIG_FIBOCOM_MAX_APP
#include "fibo_opencpu_comm.h"
//#endif

typedef int hal_uart_port_t;
typedef struct hal_uart_config_s hal_uart_config_t;
typedef enum hal_uart_data_bits_s hal_uart_data_bits_t;
typedef enum hal_uart_stop_bits_s hal_uart_stop_bits_t;


#define UART_RX_BUF_SIZE (4 * 1024)
#define UART_TX_BUF_SIZE (4 * 1024)


/**
* @brief Callback for recv uart data
* 
 */
typedef void (*uart_input_callback_t)(hal_uart_port_t uart_port, UINT8 *data, UINT16 len, void *arg);

enum hal_uart_data_bits_s
{
    HAL_UART_DATA_BITS_7 = 7,
    HAL_UART_DATA_BITS_8 = 8
};

enum hal_uart_stop_bits_s
{
    HAL_UART_STOP_BITS_1 = 1,
    HAL_UART_STOP_BITS_2 = 2
};

typedef enum
{
    HAL_UART_NO_PARITY,   ///< No parity check
    HAL_UART_ODD_PARITY,  ///< Parity check is odd
    HAL_UART_EVEN_PARITY, ///< Parity check is even
}hal_uart_parity_t;

struct hal_uart_config_s
{
    uint32_t baud;                  ///< baudrate, 0 for auto baud
    hal_uart_data_bits_t data_bits; ///< data bits
    hal_uart_stop_bits_t stop_bits; ///< stop bits
    hal_uart_parity_t parity;
    bool cts_enable;                ///< enable cts or not
    bool rts_enable;                ///< enable rts or not
    size_t rx_buf_size;             ///< rx buffer size
    size_t tx_buf_size;             ///< tx buffer size
    uint32_t recv_timeout;          //ms
};

/**
* @brief 
* 
* @param uart_port 
* @param uart_config 
* @param recv_cb 
* @param arg 
* @return INT32 
 */
INT32 fibo_hal_uart_init(hal_uart_port_t uart_port, hal_uart_config_t *uart_config, uart_input_callback_t recv_cb, void *arg);

/**
* @brief 
* 
* @param uart_port 
* @param buff 
* @param len 
* @return INT32 
 */
INT32 fibo_hal_uart_put(hal_uart_port_t uart_port, UINT8 *buff, UINT32 len);


/**
* @brief 
* 
* @param uart_port 
* @return INT32 
 */
INT32 fibo_hal_uart_data_to_send(int uart_port);

/**
* @brief 
* 
* @param uart_port 
* @return INT32 
 */
INT32 fibo_hal_uart_deinit(hal_uart_port_t uart_port);

#endif /* COMPONENTS_FIBOCOM_OPENCPU_OC_UART_H */
