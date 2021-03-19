/*
 * sarnath.c
 *
 *  Created on: 2019年7月16日
 *      Author: Donal
 */
#include <stdint.h>
#include "net.h"
#include "ql_system.h"
#include "typedef.h"
#include "Debug.h"
#include "CRC.h"
#include "appTask.h"
#include "memory.h"
#include "SysPara.h"
#include "list.h"
#include "device.h"
#include "ESP_Update.h"
#include "modbus.h"
#include "Algorithm.h"
#include "ModbusDevice.h"
#include "Modbus.h"
#include "debug.h"
#include "r_stdlib.h"
#include "Clock.h"
#include "Swap.h"
#include "SysPara.h"
#include "FlashFIFO.h"
#include "FlashEquilibria.h"
#include "crc.h"
#include "FlashHard.h"
#include "Sineng.h"
#include "StateGridData.h"
#include "sarnath.h"


static u8 sPort = 0xFF;
static u16 overtime;
static u16 factorID = 0;
static u16 loggerID = 0;
static StateGridDevice_t *pSarnathDevice;
static StateGridPointTab_t *pSarnathPointTab;


#define OVER_TIME_MIN       (30)
#define OVER_TIME_MAX       (120)

/**
 * @brief Modbus RTU data to ieee645 format
 * @param input
 * @return
 */
static int dataToTEEE645 (float input)
{
    un_dtformconver dtformconver;

    s32 il_temp32;

    dtformconver.ul_temp = input;

    il_temp32 = 0;

    for(int i = 0; i < 4; i++)
    {
        il_temp32 |= (s32)(dtformconver.uc_buf[i] << (i * 8));
    }

    return il_temp32;
}


static void output(Buffer_t *buf)
{
	APP_DEBUG("%s len: %d\r\n", "Sarnth",  buf->lenght);
	if (buf->lenght < 1024)
	{
		char *str = memory_apply(buf->lenght * 3 + 8);
		if (str != null)
		{
			int l = Swap_hexChar(str, buf->payload, buf->lenght, ' ');
			Debug_output(str, l);
			APP_DEBUG("\r\n");
			memory_release(str);
		}
	}
}

static void outputMemory(char * pMemory, u32_t lenght)
{
    u32 displayNum = 0;

	if (lenght < 1024)
	{
		char *str = memory_apply(lenght * 3 + 8);
		if (str != null)
		{
           int l = Swap_hexChar(str, pMemory, lenght, ' ');

            while(l)
            {
                if(l >= 16 * 3)
                {
                    Debug_output(str + displayNum, 16 * 3);
                    l -= 16 * 3;
                    displayNum += 16 * 3;
                }
                else
                {
                    Debug_output(str + displayNum, l);
                    l = 0;
                }
                APP_DEBUG("\r\n");
            }

            memory_release(str);
		}
		else
		{
		    APP_DEBUG("outputMemory malloc failed\r\n");
		}
	}
}

/**
 * @brief  Get the over time of sarnath upload data
 * @return
 */
static int getSarnathUploadOverTime(void)
{
    Buffer_t buf;
    int overtime = OVER_TIME_MAX;
    buf.payload = memory_apply(SYY_PARA_LEN);

    if(buf.payload == null)
    {
        return 0;
    }

	SysPara_Get(SARNATH_UPLOAD_TIME, &buf);
	if(buf.lenght)
	{
	    overtime = Swap_charNum(buf.payload);
	}

	if(overtime < OVER_TIME_MIN || overtime > OVER_TIME_MAX)
	{
	    overtime = OVER_TIME_MAX;
	}

	memory_release(buf.payload);

	return overtime;

}

/**
 * @brief  Get the factory ID and logger ID
 */
static void getSarnathFactorPara(void)
{
    Buffer_t buf;

    SysPara_Get(SARNATH_FACTORY_ID, &buf);

    if(buf.lenght)
    {
        factorID = Swap_charNum(buf.payload);
    }

    memory_release(buf.payload);

    SysPara_Get(SARNATH_LOGGER_ID, &buf);

    if(buf.lenght)
    {
        loggerID = Swap_charNum(buf.payload);
    }

    memory_release(buf.payload);
}

/**
 * @brief Check the validity of the server address
 * @return
 */
static int isServerAddress(void)
{

    return 0;
}

/**
 * @brief Check the factor is sarnath
 * @return
 */
static int isSarnath(void)
{
    return 0;
}

/**
 * @brief Check the TCP link is ok to sarnath server
 * @return
 */
static int isNetOk(void)
{
    return 0;
}

/**
 * @brief Close the TCP connection to the server
 * @return
 */
static int closeNet(void)
{
    return 0;
}

/**
 * @brief Get the sarnath data total size
 * @note  One point of string type has much chars size in state grid point
 * @param pStateGridPointTab
 * @return
 */
static u32 getSarnathDataSize(StateGridPointTab_t * pStateGridPointTab)
{
    u32 dataSize = 0;

    for(int i = 0; i < pStateGridPointTab->count; i++)
    {
//        if(pStateGridPointTab->tab[i].type == 'T')
//        {
//            APP_DEBUG("get the string type parameter\r\n");
//            dataSize += pStateGridPointTab->tab[i].value.lenght;
//        }
//        else
        {
            dataSize += 1;
        }
    }

    return dataSize;
}

/**
 * @brief Copy the data form the State grid to the sarnath
 * @param pStateGridPointTab
 * @param pBuff
 */
static void copyStateToSarnath(StateGridPointTab_t *pStateGridPointTab, Buffer_t *pBuff)
{
    u8_t dataTemp[30] = {0};
    s32_t sarnathTemp = 0;
    s32_t ieee645Temp = 0;
    int cursor = 0;

    for(int i = 0; i < pStateGridPointTab->count; i++)
    {
        r_memset(dataTemp, 0, sizeof(dataTemp));
        r_memcpy(dataTemp, pStateGridPointTab->tab[i].value.payload, pStateGridPointTab->tab[i].value.lenght);

        if(pStateGridPointTab->tab[i].type == 'T')
        {
//            APP_DEBUG("stategrid type T length = %d\r\n", )
//            for(int j = 0; j < pStateGridPointTab->tab[i].value.lenght; j++)
//            {
//                cursor = j;
//                sarnathTemp = pStateGridPointTab->tab[i].value.payload[j];
//                ieee645Temp = dataToTEEE645(((float)sarnathTemp));
//                r_memcpy(pBuff->payload + (i + cursor) * 4, (char *)&ieee645Temp, sizeof(s32_t));
//            }
//            continue;

//            sarnathTemp = pStateGridPointTab->tab[i].value.payload[j];
            sarnathTemp = (u32_t)(dataTemp[0] << 24 | dataTemp[1] << 16 | dataTemp[2] << 8 | dataTemp[3]);
        }
        else if(pStateGridPointTab->tab[i].value.lenght == 0)
        {
            sarnathTemp = 0;
        }
        else if(pStateGridPointTab->tab[i].value.lenght == 1)
        {
            sarnathTemp = (u32_t)dataTemp[0];
        }
        else if(pStateGridPointTab->tab[i].value.lenght == 2)
        {
            sarnathTemp = (u32_t)(dataTemp[0] << 8 | dataTemp[1]);
        }
        else if(pStateGridPointTab->tab[i].value.lenght == 4)
        {
            sarnathTemp = (u32_t)(dataTemp[0] << 24 | dataTemp[1] << 16 | dataTemp[2] << 8 | dataTemp[3]);
        }

        if(pStateGridPointTab->tab[i].value.lenght == 4)
        {
            ieee645Temp = dataToTEEE645(*((float*)&sarnathTemp));
        }
        else
        {
            ieee645Temp = dataToTEEE645(((float)sarnathTemp));
        }
        r_memcpy(pBuff->payload + (i + cursor) * 4, (char *)&ieee645Temp, sizeof(s32_t));
    }
}

/**
 * @brief Format the state grid to the sarnath data format
 * @param pBuff
 */
static int sarnathDataFormat(Buffer_t *pBuff)
{
    int pointNum = 0;

    if(pBuff == null)
    {
        return -1;
    }

    pointNum = getSarnathDataSize(pSarnathPointTab);

    if(pointNum)
    {
       pBuff->payload = memory_apply(sizeof(u32) * pointNum);

       if(pBuff->payload == null)
       {
           APP_DEBUG("pBuff->payload malloc failed\r\n");
           return -1;
       }

       pBuff->lenght = 4 * pointNum;
       r_memset(pBuff->payload, 0, pBuff->lenght);
    }

    APP_DEBUG("sarnath buff size = %d, tabnum = %d pBuff->lenght = %d!!!!!!!!!!!!!!!!&&&&&&&&&&\r\n",
            pointNum, pSarnathPointTab->count, pBuff->lenght);

    copyStateToSarnath(pSarnathPointTab, pBuff);

    return 0;

}

/**
 * @brief Get the device data from the subordinate equipment
 * @return
 */
static int getDataFromDevice(void)
{
    int ret = -1;
    if(StateGrid_dataStatus() == 1)
    {
        ret = getGridDataHandle(&pSarnathDevice, &pSarnathPointTab);
    }
    else
    {
        APP_DEBUG("The StateGrid data Status is failed!!!!!!!!!!!!!!!!!!!!!\r\n");
        ret = -1;
    }

    return ret;
}

/**
 * @brief Process the device dataes to upload sarnath server
 * @return
 */
static void sarnathCallback(u8 port, Buffer_t *buf)
{
    overtime = 0;
    sPort = port;
}

/**
 * @brief Upload the device data to sarnath server
 * @return
 */
static int uploadDataToServer(Buffer_t * pBuff)
{
    int  count = 0;
    int  curtor = pBuff->payload;
    u16_t crc = 0;
    static u32_t i = 0;
    u32_t bodyaddr = 0x4001;
    xSarnath_t * sarnthData = null;
    u16_t packetLength = 0;

    sarnthData = memory_apply(888);

    if(sarnthData == null)
    {
        APP_DEBUG("sarnthData malloc failed\r\n");
        return 0;
    }

    APP_DEBUG("bodyaddr = %d, i = %d\r\n", bodyaddr, i);

    if(i >= pBuff->lenght >> 2)
    {
        i = 0;
    }

    if (i < pBuff->lenght >> 2)
    {
        r_memset(sarnthData, 0, 888);
        packetLength = sizeof(xSarnath_t) + 2;

        sarnthData->packetHeader = 0x6969;
        sarnthData->factoryID = factorID;
        sarnthData->loggerAddr = loggerID;
        sarnthData->frameID = 0;
        sarnthData->functionNum = 0x07;
        sarnthData->deviceType = 0x01;
        bodyaddr += i;
        r_memcpy(sarnthData->bodyAddr, &bodyaddr, 3);

        for(int j = 0; j < 888 - sizeof(xSarnath_t); j += 4)
        {
            r_memcpy(sarnthData->dataPoint + j, pBuff->payload + i * 4, 4);
            i++;
            packetLength += 4;
            bodyaddr++;

            if(i >= pBuff->lenght >> 2 || packetLength > 880)
            {
                break;
            }
        }

        sarnthData->packetLenght = packetLength;
        crc = crc16_standard(CRC_RTU,(const unsigned char *)sarnthData, packetLength - 2);
        APP_DEBUG("pBuff.lenght = %d\r\n", pBuff->lenght);

        r_memcpy((u8_t *)sarnthData + (packetLength - 2), &crc, sizeof(u16_t));

        APP_DEBUG("crc = %02X\r\n", crc);

        if(crc16_standard(CRC_RTU, (const unsigned char *)sarnthData, packetLength) == 0)
        {
            APP_DEBUG("Sarnath Data Send OK, packetLend = %d\r\n", packetLength);
            Net_send(sPort,(u8_t *)sarnthData , packetLength);
        }
        else
        {
            APP_DEBUG("Sarnath Data CRC faile,0x%02X, lenght = %d\r\n", crc, packetLength);
        }

    }

    memory_release(sarnthData);

    if(i < pBuff->lenght >> 2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


/**
 * @brief The Snarth server entry
 * @param taskId
 */
void proc_sarnath_task(s32 taskId)
{
	ST_MSG msg;
	u16 uploadCount = 0;//(ms)
	u16 uploadOverTime = OVER_TIME_MAX;
	u16 relinkTime = 0;
	u16 timeCount = 0;
	bool serverSwitch = TRUE;
	int uploadFlag = 0;


	int ret = 0xFF;
	Buffer_t uploadBuff;


	APP_DEBUG("Sarnath Task run!!!!!");


	isSarnath();
	isServerAddress();
    while(1)
    {
        Ql_OS_GetMessage(&msg);

        switch(msg.message)
        {
			case SYS_PARA_CHANGE:
				Net_close(sPort);
				sPort = 0xff;//*/
				serverSwitch = TRUE;
            case USER_TIMER_ID:
                if(serverSwitch)
                {
                    ret = Net_status(sPort);
                    if(uploadCount++ >= uploadOverTime && !ret)
                    {
                        uploadCount = 0;
                        APP_DEBUG("Send sarnath data flag**************************************\r\n");
                        Ql_OS_SendMessage(SARNATH_TASK, SARNATH_DATA_PROCESS, 0, 0);
                    }

                    if (ret == 0xFF && relinkTime++ > 100)
                    {
//                        relinkTime = 0;
                        ServerAddr_t *sarnethServer = ServerAdrrGet(SARNATH_SERVER_ADDR);

                        if(sarnethServer == NULL)
                        {
                            serverSwitch = FALSE;
                            break;
                        }

                        getSarnathFactorPara();
                        uploadOverTime = getSarnathUploadOverTime();


                        if (sarnethServer != NULL)
                        {
                            APP_DEBUG("uploadOvertime = %d\r\nFactorID = %d\r\nLoggerID = %d\r\n",
                                    uploadOverTime, factorID, loggerID);
                            APP_DEBUG("sarnethServer = %s!!!!!!!!!!!!!!!!\r\n", sarnethServer->addr);
                            APP_DEBUG("sarnethPort = %d!!!!!!!!!!!!!!!!!!\r\n", sarnethServer->port);
                              overtime = 0;
                            sPort = Net_connect(1, sarnethServer->addr, sarnethServer->port, sarnathCallback);
                            memory_release(sarnethServer);
                        }
                        else
                        {
                            serverSwitch = FALSE;
                        }

                    }
                    else if (ret != 1)
                    {
                        if (overtime++ >= uploadOverTime + 60) //萨纳斯的服务器超时时间为30s
                        {
                            overtime = 0;
                            Net_close(sPort);
                        }
                    }
                }
                else
                {
                    uploadFlag = 0;
                }

                break;

            case SARNATH_DATA_PROCESS:
                overtime = 0;
                if(getDataFromDevice())
                {
                    APP_DEBUG("point parameter get failed@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
                    break;
                }

                if(sarnathDataFormat(&uploadBuff))
                {
                    break;
                }
                APP_DEBUG("sarnath data format\r\n");

            case SARNATH_DATA_UPLOAD:
                APP_DEBUG("sarnath data upload\r\n");
                uploadFlag = uploadDataToServer(&uploadBuff);

                if(!uploadFlag)
                {
                    memory_release(uploadBuff.payload);
                }
                else
                {
                    Ql_OS_SendMessage(SARNATH_TASK, SARNATH_DATA_UPLOAD, 0, 0);
                }

                break;

            default:
                break;

        }
    }
}
