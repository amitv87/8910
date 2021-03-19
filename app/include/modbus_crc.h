/**
  ********************************************************************************
  *@Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.
  *@file    : modbus_crc.h
  *@version : V1.0
  *@arthor  : Donal Dong
  *@date    : May 31, 201711:40:25 AM
  *@brief   : 
  ********************************************************************************
  *@notes   :
  ********************************************************************************
  */
// #ifndef COMPONENTS_CHECK_CRC16_MODBUS_CRC_H_
// #define COMPONENTS_CHECK_CRC16_MODBUS_CRC_H_
#ifndef __MODBUS_CRC_H__
#define __MODBUS_CRC_H_
//#include "includes.h"
/**********************************Includes************************************/
/*******************************Private typedef********************************/
/*******************************Private define*********************************/
/*************************Global variables prototypes**************************/
/*******************************Global variables*******************************/
/********************************Private macro ********************************/
/******************************Private variables*******************************/
/***************************Private function prototypes ***********************/
/******************************Private functions**  ***************************/
uint16_t rtu_crc16(uint8_t *pucframe, uint16_t usdatalen);
uint16_t crc16rtu( uint8_t * crcbuf, uint16_t len);
uint16_t crc16ccit_xmoderm( uint8_t * crcbuf, uint16_t len);

// #endif /* COMPONENTS_CHECK_CRC16_MODBUS_CRC_H_ */
#endif /* __MODBUS_CRC_H_ */