/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#ifndef _DRV_CHARGER_H_
#define _DRV_CHARGER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "fibocom.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    DRV_CHARGER_TYPE_NONE,
    DRV_CHARGER_TYPE_SDP,
    DRV_CHARGER_TYPE_DCP,
    DRV_CHARGER_TYPE_CDP,
    DRV_CHARGER_TYPE_UNKOWN
} drvChargerType_t;
#ifdef CONFIG_FIBOCOM_BASE
typedef enum
{
    // Charge message.
    CHR_CAP_IND = 0x1, // Notify the battery's capacity

    CHR_CHARGE_START_IND, // start the charge process.
    CHR_CHARGE_END_IND,   // the charge ended.

    CHR_WARNING_IND,  // the capacity is low, should charge.
    CHR_SHUTDOWN_IND, // the capacity is very low and must shutdown.

    CHR_CHARGE_FINISH,     // the charge has been completed.
    CHR_CHARGE_DISCONNECT, // the charge be disconnect
    CHR_CHARGE_FAULT,      // the charge fault, maybe the voltage of charge is too low.

    CHR_MSG_MAX_NUM
} CHR_SVR_MSG_SERVICE_E;
#endif
typedef void (*drvChargerPlugCB_t)(void *ctx, bool plugged);

#ifdef CONFIG_FIBOCOM_BASE
typedef void (*drvChargerNoticeCB_t)(CHR_SVR_MSG_SERVICE_E msg);
#else
typedef void (*drvChargerNoticeCB_t)(void);
#endif
void drvChargerInit(void);

void drvChargerSetNoticeCB(drvChargerNoticeCB_t notice_cb);

void drvChargerSetCB(drvChargerPlugCB_t cb, void *ctx);

drvChargerType_t drvChargerGetType(void);

/**
 * @brief return the charger and battery info.
 *
 * @param nBcs  set the channel to measue
 * 0 No charging adapter is connected
 * 1 Charging adapter is connected
 * 2 Charging adapter is connected, charging in progress
 * 3 Charging adapter is connected, charging has finished
 * 4 Charging error, charging is interrupted
 * 5 False charging temperature, charging is interrupted while temperature is beyond allowed range
 * @param nBcl   percent of remaining capacity.
 */

void drvChargerGetInfo(uint8_t *nBcs, uint8_t *nBcl);
#ifdef CONFIG_FIBOCOM_BASE

/**********************************************************/
// note: the all param about vol must below 4200,the unit is mv
//  the shut_vol<shut_vol<rechg_vol<end_vol;
//  the chg_cur can choose 300 350 400 450 500 550 600 650 700 750 800 900 1000 1100 1200 1300mA. 
//  chg_cur test must notice usb_lan standard
/*********************************************************/

typedef struct _chargerinfo
{
	uint32_t end_vol;  //charger end_vol
	uint32_t chg_cur;  //max charger current
	uint32_t rechg_vol;  //re_charger start vol
	uint32_t shut_vol;   // shut_down vol
	uint32_t warn_vol;   // low power warning vol
}chargerinfo_t;
void drvChargerGetCurbatVol(uint32_t *nBct);
bool drvChargerIsWork(void);
bool drvChargerSetConfig(chargerinfo_t * chg_cfg);
uint32_t drvChargerGetChgCurrent(void);
bool drvChargerIsChargeFull(void);
uint32_t drvChargerGetCurBatVolPercent(void);
#endif

void drvChargeEnable(void);

void drvChargeDisable(void);
bool drvChargerOver(void);
uint16_t drvChargerGetVbatRT();

#ifdef __cplusplus
}
#endif
#endif
