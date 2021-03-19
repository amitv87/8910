/******************************************************************************           
* name:                     
* introduce:        头文件
* author:           Luee                                     
******************************************************************************/ 
#ifndef __L610_NET_SSL_H_
#define __L610_NET_SSL_H_

#include "eyblib_typedef.h"
#include "L610Net_TCP_EYB.h"
#include "grid_tool.h"

#define CA_FILE_FLAG            (0xAA550099)
#define CA_FILE_NAME            "/ca_file.dat"              // ca文件名
#define CA_STATUSFILE_NAME      "/ca_file.status"           // ca文件名

extern s32 sslsock;
extern L610Net_t *SSLNet;

void SSL_init(void);
s8  SSL_Open(L610Net_t *net);
void SSLHandler_TCPConnect(const char* strURC, void* reserved);
int  SSL_Send(s32_t socketId, u8_t* pData, s32_t dataLen);
void SSLHandler_DataRcve(const char* strURC, void* reserved);

s32 ssl_rec(void);

#define TEST_CA_FILE "-----BEGIN CERTIFICATE-----\r\n"                                      \
                     "MIID9TCCAt2gAwIBAgIJAOEIwHHcR9K7MA0GCSqGSIb3DQEBBQUAMIGPMQswCQYD\r\n" \
                     "VQQGEwJDTjEPMA0GA1UECAwGc2hhbnhpMQ0wCwYDVQQHDAR4aWFuMRAwDgYDVQQK\r\n" \
                     "DAdmaWJvY29tMRAwDgYDVQQLDAdmaWJvY29tMRYwFAYDVQQDDA00Ny4xMTAuMjM0\r\n" \
                     "LjM2MSQwIgYJKoZIhvcNAQkBFhV2YW5zLndhbmdAZmlib2NvbS5jb20wIBcNMTkw\r\n" \
                     "ODMwMDY1MjUwWhgPMjExOTA4MDYwNjUyNTBaMIGPMQswCQYDVQQGEwJDTjEPMA0G\r\n" \
                     "A1UECAwGc2hhbnhpMQ0wCwYDVQQHDAR4aWFuMRAwDgYDVQQKDAdmaWJvY29tMRAw\r\n" \
                     "DgYDVQQLDAdmaWJvY29tMRYwFAYDVQQDDA00Ny4xMTAuMjM0LjM2MSQwIgYJKoZI\r\n" \
                     "hvcNAQkBFhV2YW5zLndhbmdAZmlib2NvbS5jb20wggEiMA0GCSqGSIb3DQEBAQUA\r\n" \
                     "A4IBDwAwggEKAoIBAQC6UMQfxHL0oW9pY1cGvq5QPdw8OU7dX2YsCbPdEiXePKce\r\n" \
                     "E6AN3IKqOuZhEd1iIypXG2AywzIu9bd5w1d4COjjSC/Tpf2AKYw+jqfxHsQAvSKt\r\n" \
                     "Rvwp1wrS5IvWy8yEG9lNpyVJHBUWlVpU/tUf02MYYU5xUBS+mJE9Tc10j7kd/uV7\r\n" \
                     "aEfM0pYhm7VmHPWDHXeXj3LKYigjttNxUgpDh2UVpq6ejzzHA5T/k2+XtKtWu7Pb\r\n" \
                     "ag6lONzz6Zxya9htVLBy7I4uTFrcRPxNgc/KF2BuwEVc4rqGUZ4vpVdwmCyKGIua\r\n" \
                     "fvit1nsvnhvYMu01HhWuK6e3IO6hOpeyR1wk75ofAgMBAAGjUDBOMB0GA1UdDgQW\r\n" \
                     "BBTT9RodyqsY/C2WS/7k8GFWidQrlTAfBgNVHSMEGDAWgBTT9RodyqsY/C2WS/7k\r\n" \
                     "8GFWidQrlTAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBBQUAA4IBAQCkg9dUYBRs\r\n" \
                     "uqCz71Q75B2n768icIeMfQmf969fNVK/mwaUnFxjqq+4Xw3zADdUdnTZ8FEfjAt2\r\n" \
                     "LQaxmsiGlM3KmUhXy/k1xKypIu2KecxEX/NqdG02SYcBmrIAP6ZxOxyyJZXbPRBt\r\n" \
                     "11W3e9+MsRFjRNDxvi5xPcBTu7padUXf7gZp/U8RTc9r0RzsTJu0oFx1Vl6B9m9Z\r\n" \
                     "4Ae7EshqUrGbnQMJ9XinPVMhuPB4UTc5H9F9ZEswkd/8fK1kXE2aD9LOUD3ITpfH\r\n" \
                     "h4UBb/UX3VY2eoLC6T5FzPggAcyxU/S2svZaq2+fSWvA7WpEYmTvzQTeT+y1BaUW\r\n" \
                     "9SoOHidKUkQe\r\n"                                                     \
                     "-----END CERTIFICATE-----\r\n"

/*
#define STATEGRID_CA_FILE   "-----BEGIN CERTIFICATE-----\r\n"                                      \
                            "MIICuDCCAiGgAwIBAgIJAP+L+/yzpCHgMA0GCSqGSIb3DQEBBQUAMHQxCzAJBgNV\r\n" \
                            "BAYTAnpoMQswCQYDVQQIDAJkbDELMAkGA1UEBwwCZGwxDTALBgNVBAoMBHNvZnQx\r\n" \
                            "CzAJBgNVBAsMAmRsMREwDwYDVQQDDAhjanNlcnZlcjEcMBoGCSqGSIb3DQEJARYN\r\n" \
                            "Y2pAc2VydmVyLmNvbTAgFw0xNjA4MzEwOTE0MjhaGA8yMTE2MDgwNzA5MTQyOFow\r\n" \
                            "dDELMAkGA1UEBhMCemgxCzAJBgNVBAgMAmRsMQswCQYDVQQHDAJkbDENMAsGA1UE\r\n" \
                            "CgwEc29mdDELMAkGA1UECwwCZGwxETAPBgNVBAMMCGNqc2VydmVyMRwwGgYJKoZI\r\n" \
                            "hvcNAQkBFg1jakBzZXJ2ZXIuY29tMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKB\r\n" \
                            "gQDF8M2VsIjtwOOn2mNEqJ+yL7Lz2SeRVfnx/mmYs5fqYN//mz/LQtX6DLhuUIg3\r\n" \
                            "nehDa7sX1VQeFd7YuVCv7aKoUfHLllfy5MWq5leM+F2UOkH1IF6BSl+PRxgIwAEQ\r\n" \
                            "X2M69VhSCQva6p/dZs0pdn1GW2bGrp1WjNdwfH5qor+zLwIDAQABo1AwTjAdBgNV\r\n" \
                            "HQ4EFgQUxrYkWFqCIsmasiWXptWzhcDJABkwHwYDVR0jBBgwFoAUxrYkWFqCIsma\r\n" \
                            "siWXptWzhcDJABkwDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQUFAAOBgQAurlSk\r\n" \
                            "YBsorMV+4zPOH1unbdph5K1W0soUtLNfejWUWDEJQRoYA2UxPPbVYkzdEsTsbgnt\r\n" \
                            "ZeHF6gk1V9VmDhmVKFAy141iYlLjIJxNHVnhGZDot06oPsF+gZ1ymAzX6RB+I9GD\r\n" \
                            "N8BeBxfUcpriXykKg5MraSlMsjyeD1XhwZfJ/w==\r\n"                         \
                            "-----END CERTIFICATE-----\r\n"
*/
#define TEST_CLIENT_CRT_FILE "-----BEGIN CERTIFICATE-----\r\n"                                      \
                             "MIID9TCCAt2gAwIBAgIJAOEIwHHcR9K7MA0GCSqGSIb3DQEBBQUAMIGPMQswCQYD\r\n" \
                             "VQQGEwJDTjEPMA0GA1UECAwGc2hhbnhpMQ0wCwYDVQQHDAR4aWFuMRAwDgYDVQQK\r\n" \
                             "DAdmaWJvY29tMRAwDgYDVQQLDAdmaWJvY29tMRYwFAYDVQQDDA00Ny4xMTAuMjM0\r\n" \
                             "LjM2MSQwIgYJKoZIhvcNAQkBFhV2YW5zLndhbmdAZmlib2NvbS5jb20wIBcNMTkw\r\n" \
                             "ODMwMDY1MjUwWhgPMjExOTA4MDYwNjUyNTBaMIGPMQswCQYDVQQGEwJDTjEPMA0G\r\n" \
                             "A1UECAwGc2hhbnhpMQ0wCwYDVQQHDAR4aWFuMRAwDgYDVQQKDAdmaWJvY29tMRAw\r\n" \
                             "DgYDVQQLDAdmaWJvY29tMRYwFAYDVQQDDA00Ny4xMTAuMjM0LjM2MSQwIgYJKoZI\r\n" \
                             "hvcNAQkBFhV2YW5zLndhbmdAZmlib2NvbS5jb20wggEiMA0GCSqGSIb3DQEBAQUA\r\n" \
                             "A4IBDwAwggEKAoIBAQC6UMQfxHL0oW9pY1cGvq5QPdw8OU7dX2YsCbPdEiXePKce\r\n" \
                             "E6AN3IKqOuZhEd1iIypXG2AywzIu9bd5w1d4COjjSC/Tpf2AKYw+jqfxHsQAvSKt\r\n" \
                             "Rvwp1wrS5IvWy8yEG9lNpyVJHBUWlVpU/tUf02MYYU5xUBS+mJE9Tc10j7kd/uV7\r\n" \
                             "aEfM0pYhm7VmHPWDHXeXj3LKYigjttNxUgpDh2UVpq6ejzzHA5T/k2+XtKtWu7Pb\r\n" \
                             "ag6lONzz6Zxya9htVLBy7I4uTFrcRPxNgc/KF2BuwEVc4rqGUZ4vpVdwmCyKGIua\r\n" \
                             "fvit1nsvnhvYMu01HhWuK6e3IO6hOpeyR1wk75ofAgMBAAGjUDBOMB0GA1UdDgQW\r\n" \
                             "BBTT9RodyqsY/C2WS/7k8GFWidQrlTAfBgNVHSMEGDAWgBTT9RodyqsY/C2WS/7k\r\n" \
                             "8GFWidQrlTAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBBQUAA4IBAQCkg9dUYBRs\r\n" \
                             "uqCz71Q75B2n768icIeMfQmf969fNVK/mwaUnFxjqq+4Xw3zADdUdnTZ8FEfjAt2\r\n" \
                             "LQaxmsiGlM3KmUhXy/k1xKypIu2KecxEX/NqdG02SYcBmrIAP6ZxOxyyJZXbPRBt\r\n" \
                             "11W3e9+MsRFjRNDxvi5xPcBTu7padUXf7gZp/U8RTc9r0RzsTJu0oFx1Vl6B9m9Z\r\n" \
                             "4Ae7EshqUrGbnQMJ9XinPVMhuPB4UTc5H9F9ZEswkd/8fK1kXE2aD9LOUD3ITpfH\r\n" \
                             "h4UBb/UX3VY2eoLC6T5FzPggAcyxU/S2svZaq2+fSWvA7WpEYmTvzQTeT+y1BaUW\r\n" \
                             "9SoOHidKUkQe\r\n"                                                     \
                             "-----END CERTIFICATE-----\r\n"

#define TEST_CLIENT_KEY_FILE "-----BEGIN CERTIFICATE-----\r\n"                                      \
                             "MIID9TCCAt2gAwIBAgIJAOEIwHHcR9K7MA0GCSqGSIb3DQEBBQUAMIGPMQswCQYD\r\n" \
                             "VQQGEwJDTjEPMA0GA1UECAwGc2hhbnhpMQ0wCwYDVQQHDAR4aWFuMRAwDgYDVQQK\r\n" \
                             "DAdmaWJvY29tMRAwDgYDVQQLDAdmaWJvY29tMRYwFAYDVQQDDA00Ny4xMTAuMjM0\r\n" \
                             "LjM2MSQwIgYJKoZIhvcNAQkBFhV2YW5zLndhbmdAZmlib2NvbS5jb20wIBcNMTkw\r\n" \
                             "ODMwMDY1MjUwWhgPMjExOTA4MDYwNjUyNTBaMIGPMQswCQYDVQQGEwJDTjEPMA0G\r\n" \
                             "A1UECAwGc2hhbnhpMQ0wCwYDVQQHDAR4aWFuMRAwDgYDVQQKDAdmaWJvY29tMRAw\r\n" \
                             "DgYDVQQLDAdmaWJvY29tMRYwFAYDVQQDDA00Ny4xMTAuMjM0LjM2MSQwIgYJKoZI\r\n" \
                             "hvcNAQkBFhV2YW5zLndhbmdAZmlib2NvbS5jb20wggEiMA0GCSqGSIb3DQEBAQUA\r\n" \
                             "A4IBDwAwggEKAoIBAQC6UMQfxHL0oW9pY1cGvq5QPdw8OU7dX2YsCbPdEiXePKce\r\n" \
                             "E6AN3IKqOuZhEd1iIypXG2AywzIu9bd5w1d4COjjSC/Tpf2AKYw+jqfxHsQAvSKt\r\n" \
                             "Rvwp1wrS5IvWy8yEG9lNpyVJHBUWlVpU/tUf02MYYU5xUBS+mJE9Tc10j7kd/uV7\r\n" \
                             "aEfM0pYhm7VmHPWDHXeXj3LKYigjttNxUgpDh2UVpq6ejzzHA5T/k2+XtKtWu7Pb\r\n" \
                             "ag6lONzz6Zxya9htVLBy7I4uTFrcRPxNgc/KF2BuwEVc4rqGUZ4vpVdwmCyKGIua\r\n" \
                             "fvit1nsvnhvYMu01HhWuK6e3IO6hOpeyR1wk75ofAgMBAAGjUDBOMB0GA1UdDgQW\r\n" \
                             "BBTT9RodyqsY/C2WS/7k8GFWidQrlTAfBgNVHSMEGDAWgBTT9RodyqsY/C2WS/7k\r\n" \
                             "8GFWidQrlTAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBBQUAA4IBAQCkg9dUYBRs\r\n" \
                             "uqCz71Q75B2n768icIeMfQmf969fNVK/mwaUnFxjqq+4Xw3zADdUdnTZ8FEfjAt2\r\n" \
                             "LQaxmsiGlM3KmUhXy/k1xKypIu2KecxEX/NqdG02SYcBmrIAP6ZxOxyyJZXbPRBt\r\n" \
                             "11W3e9+MsRFjRNDxvi5xPcBTu7padUXf7gZp/U8RTc9r0RzsTJu0oFx1Vl6B9m9Z\r\n" \
                             "4Ae7EshqUrGbnQMJ9XinPVMhuPB4UTc5H9F9ZEswkd/8fK1kXE2aD9LOUD3ITpfH\r\n" \
                             "h4UBb/UX3VY2eoLC6T5FzPggAcyxU/S2svZaq2+fSWvA7WpEYmTvzQTeT+y1BaUW\r\n" \
                             "9SoOHidKUkQe\r\n"                                                     \
                             "-----END CERTIFICATE-----\r\n"




#endif //__L610_NET_SSL_H_
/******************************************************************************/
