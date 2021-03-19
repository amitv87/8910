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

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'Y', 'A', 'P')

#include "fibo_opencpu.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "eybpub_Debug.h"
#include "L610_conn_ali_net.h"
#include "ali_data_packet.h"

#include "eybpub_utility.h"
#include "eybapp_appTask.h"

/*
  * set the value to execute different process
  * 1 : test app update
  * 2 : test fimware update
  * 3 : test app download and update
  * 4 : test firmware download and update
  */
//#define TEST_TYPE             1
#define TEST_TYPE               3

#define def_max_ServerAdrLen    256
#define def_max_VersionLen      50

typedef struct
{
    bool download_flag;
    uint8_t ServerAdrLen;
    uint8_t ServerAdr[def_max_ServerAdrLen];
    uint8_t VersionLen;
    uint8_t Version[def_max_VersionLen];
    uint8_t tag[def_max_VersionLen];
    uint8_t md5[33];
} OtaParaTypeDef;

struct pdphandle
{
    UINT32 sem;
    char success;
};

OtaParaTypeDef otaData;
struct pdphandle g_pdp_handle;


void app_download_test()
{
    int ret;
    char *url = http_url;
    APP_PRINT("app_download_test start \r\n");
    APP_PRINT("app update url = %s \r\n",url);

    ret = fibo_app_dl(0, (UINT8 *)url, NULL, NULL, NULL);
    if(ret < 0)
    {
        APP_PRINT("fibo_app_dl fail.\r\n");
        goto fexit;
    }
    APP_PRINT("fibo_app_dl is OK.\r\n");

    for (int n = 0; n < 5; n++)
    {
        APP_PRINT("hello world %d\r\n",n);
        fibo_taskSleep(1000);
    }

    ret = fibo_app_update();
    if(ret < 0)
    {
        APP_PRINT("fibo_app_update fail.\r\n");
        goto fexit;
    }
    APP_PRINT("fibo_app_update is OK.\r\n");
fexit:
    return;
}

void firmware_download_test()
{
    int ret;
    char *url = "http://xatest.fibocom.com:8000/httpfota/test1/lobin/fota.bin";

    ret = fibo_firmware_dl(0, (UINT8 *)url, NULL, NULL, NULL);
    if(ret < 0)
    {
        APP_PRINT("fibo_firmware_dl fail.");
        goto fexit;
    }
    APP_PRINT("fibo_firmware_dl is OK.");

    for (int n = 0; n < 5; n++)
    {
        OSI_LOGI(0, "hello world %d", n);
        fibo_taskSleep(1000);
    }

    ret = fibo_firmware_update();
    if(ret < 0)
    {
        APP_PRINT("fibo_firmware_update fail.");
        goto fexit;
    }
    APP_PRINT("fibo_firmware_update is OK.");
fexit:
    return;
}

void app_update_test()
{
    int ret;
//  char *url = "http://xatest.fibocom.com:8000/httpfota/test1/lobin/hello_flash.img";
    char *url = "http://ginlong-dev.oss-cn-shanghai.aliyuncs.com/collectorupdate/update.img?Expires=1608198110&OSSAccessKeyId=LTAIrFOb5NRUYCNh&Signature=ozztATsb4is9SL7FhSteKhZEd94%3D";

    APP_PRINT("fibo_open_ota start.\r\n")
    ret = fibo_open_ota(0, (UINT8 *)url, NULL, NULL, NULL);
    if(ret < 0)
    {
        APP_PRINT("fibo_open_ota fail.\r\n");
        goto fexit;
    }
    APP_PRINT("fibo_open_ota is OK.\r\n");

    for (int n = 0; n < 10; n++)
    {
        APP_PRINT("hello world \r\n");
        fibo_taskSleep(1000);
    }

fexit:
    return;
}

void firmware_update_test()
{
    int ret;
    char *url = "http://xatest.fibocom.com:8000/httpfota/test1/lobin/fota.bin";

    ret = fibo_firmware_ota(0, (UINT8 *)url, NULL, NULL, NULL);
    if(ret < 0)
    {
        APP_PRINT("fibo_firmware_ota fail.");
        goto fexit;
    }
    APP_PRINT("fibo_firmware_ota is OK.");

    for (int n = 0; n < 10; n++)
    {
        OSI_LOGI(0, "hello world %d", n);
        fibo_taskSleep(1000);
    }

fexit:
    return;
}



void otaThreadEntry_firmware(void *param)
{
    if(TEST_TYPE == 1)
        app_update_test();
    else if(TEST_TYPE == 2)
        firmware_update_test();
    else if(TEST_TYPE == 3)
        app_download_test();
    else if(TEST_TYPE == 4)
        firmware_download_test();
    else
        APP_PRINT("TEST_TYPE value is setting error, please set again!");

    fibo_thread_delete();
}


void proc_http_fota_task(void *param)
{
    int             ret = 0;
    int             test = 1;
    reg_info_t      reg_info;
    UINT8           ip[50];
    UINT8           cid_status;

    INT8            cid = 1;
    CFW_SIM_ID      sim_id = CFW_SIM_0;
    APP_PRINT("http fota task run...\r\n");

    memset(&ip, 0, sizeof(ip));
    memset(&g_pdp_handle, 0, sizeof(g_pdp_handle));
    memset(&otaData, 0, sizeof(otaData));
    ST_MSG msg;
    r_memset(&msg, 0, sizeof(ST_MSG));
    while (1)
    {
        fibo_queue_get(FOTA_TASK, (void *)&msg, 0);
        switch (msg.message)
        {
            case FIRMWARE_UPDATE :              

                while(1)
                {
                    fibo_PDPStatus(cid, ip, &cid_status, sim_id);
                    if(1 == cid_status)
                    {
                        APP_PRINT("http pdp active is ok");
                        break;
                    }
                    APP_PRINT("http pdp active is not");
                    fibo_taskSleep(2000);
                }
                APP_PRINT("http pdp active while is ok");
                app_download_test();
                break;
            case APP_MSG_TIMER_ID:
                break;
            default :
                break;
        }
    }
    fibo_thread_delete();
}

