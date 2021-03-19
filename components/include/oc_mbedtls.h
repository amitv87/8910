/**
*@tc mbedtls api 
*/
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum {
	TLS_RET_SUCCESS 				= 0,     //tls Successful return
	TLS_ERR_SSL_INIT        		= -101,  // TLS/SSL init fail
	TLS_ERR_SSL_CERT 		   		= -102,	 // TLS/SSL certificate issue
	TLS_ERR_SSL_CONNECT		   		= -103,	 // TLS/SSL connect fail
	TLS_ERR_SSL_CONNECT_TIMEOUT 	= -104,	 // TLS/SSL connect timeout
	TLS_ERR_SSL_WRITE_TIMEOUT   	= -105,	 // TLS/SSL write timeout
	TLS_ERR_SSL_WRITE		   		= -106,	 // TLS/SSL write error
	TLS_ERR_SSL_READ_TIMEOUT    	= -107,	 // TLS/SSL read timeout
	TLS_ERR_SSL_READ 		   		= -108,	 // TLS/SSL read error
	TLS_ERR_SSL_NOTHING_TO_READ 	= -109,	 // TLS/SSL nothing to read

	TLS_ERR_TCP_SOCKET_FAILED   	= -201,  // TLS TCP socket connect fail
    TLS_ERR_TCP_UNKNOWN_HOST   		= -202,  // TCP unknown host (DNS fail)
    TLS_ERR_TCP_CONNECT         	= -203,  // TCP/UDP socket connect fail
}tls_err;

typedef struct {
    const char *ca_crt;
    uint16_t    ca_crt_len;
#ifdef AUTH_MODE_CERT
    /**
     * Device with certificate
     */
    const char *cert_file;  // public certificate file
    const char *key_file;   // pravite certificate file
#else
    /**
     * Device with PSK
     */
    const char *psk;     // PSK string
    const char *psk_id;  // PSK ID
#endif

    size_t psk_length;  // PSK length

    unsigned int timeout_ms;  // SSL handshake timeout in millisecond

} GAPP_TLS_CONNECT;

typedef GAPP_TLS_CONNECT  GAPP_TLSConnectParams;


/**
 * @brief Setup TLS connection with server
 *
 * @param   pConnectParams reference to TLS connection parameters
 * @host    server address
 * @port    server port
 * @return  TLS connect handle when success, or 0 otherwise
 */
uintptr_t fibo_tls_connect(GAPP_TLSConnectParams *pConnectParams, const char *host, int port);
/**
 * @brief Disconnect with TLS server and release resources
 *
 * @param handle TLS connect handle
 */
void fibo_tls_disconnect(uintptr_t handle);
/**
 * @brief Write data via TLS connection
 *
 * @param handle        TLS connect handle
 * @param msg           source data to write
 * @param totalLen      length of data
 * @param timeout_ms    timeout value in millisecond
 * @param written_len   length of data written successfully
 * @return              0 for success, or err code for failure
 */
int fibo_tls_write(uintptr_t handle, unsigned char *msg, size_t totalLen, uint32_t timeout_ms, size_t *written_len);
/**
 * @brief Read data via TLS connection
 *
 * @param handle        TLS connect handle
 * @param msg           destination data buffer where to put data
 * @param totalLen      length of data
 * @param timeout_ms    timeout value in millisecond
 * @param read_len      length of data read successfully
 * @return              0 for success, or err code for failure
 */
int fibo_tls_read(uintptr_t handle, unsigned char *msg, size_t totalLen, uint32_t timeout_ms, size_t *read_len);






