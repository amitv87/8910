/*
 * sarnath.h
 *
 *  Created on: 2019年7月16日
 *      Author: Donal
 */

#ifndef APP_SERVER_SARNATH_SARNATH_H_
#define APP_SERVER_SARNATH_SARNATH_H_

#include <stdio.h>
#include <stdint.h>
#include "typedef.h"
#define SARNATH_TASK            subtask7_id
#define SARNATH_DATA_PROCESS    (0x011002)
#define SARNATH_DATA_UPLOAD     (0x011003)


typedef struct _Sarnath {
    u16_t packetHeader;
    u16_t  packetLenght;
    u16_t   factoryID;
    u16_t loggerAddr;
    u8_t  frameID;
    u8_t  functionNum;
    u8_t  deviceType;
    u8_t  bodyAddr[3];
    u8_t dataPoint[];

}__attribute__((packed)) xSarnath_t;


typedef union
{
    float ul_temp;
    u8 uc_buf[4];
}un_dtformconver;

#endif /* APP_SERVER_SARNATH_SARNATH_H_ */
