/**
  ********************************************************************************
  *@Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.
  *@file    : modbus_md5.h
  *@version : V1.0
  *@arthor  : Donal Dong
  *@date    : May 12, 20175:48:22 PM
  *@brief   : 
  ********************************************************************************
  *@notes   :
  ********************************************************************************
  */
#ifndef __MODBUS_MD5_H__
#define __MODBUS_MD5_H__
#include "stdint.h"
/**********************************Includes************************************/
/*******************************Private typedef********************************/
/*******************************Private define*********************************/
/*************************Global variables prototypes**************************/
/*******************************Global variables*******************************/
/********************************Private macro ********************************/
/******************************Private variables*******************************/
/***************************Private function prototypes ***********************/
/******************************Private functions**  ***************************/
// #include "common.h"
/* Data structure for MD5 (Message-Digest) computation */
typedef  struct {
  uint32_t i[2];                /* number of _bits_ handled mod 2^64 */ //处理的位数2 ^ 64的位数
  uint32_t buf[4];              /* scratch buffer */                    //暂存缓冲区
  uint8_t  in[64];              /* input buffer */                      //输入缓冲区
  uint8_t  digest[16];          /* actual digest after MD5Final call */ //MD5Final调用后的实际摘要
} hash_md5_ctx_t;

void copy_buff(uint8_t* pDest, uint8_t* pSrc, uint16_t wSize);
void modbus_md5_init( hash_md5_ctx_t *mdContext);
void modbus_md5_update( hash_md5_ctx_t *mdContext, uint8_t *inBuf, uint16_t inLen);
void modbus_md5_final( uint8_t hash[], hash_md5_ctx_t *mdContext);


#endif /* __MODBUS_MD5_H__ */