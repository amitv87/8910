/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : sineng.c
  *@notes   : 2017.12.29 CGQ   
*******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_system.h"
#include "ql_stdlib.h"
#include "ql_memory.h"
#endif

#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#endif

#include "eyblib_typedef.h"
#include "eyblib_swap.h"
#include "eyblib_list.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_memory.h"
#include "eybpub_run_log.h"
#include "eybpub_utility.h"

#include "Protocol.h"
#include "DeviceIO.h"
#include "Device.h"
#include "Sineng.h"
#include "Modbus.h"
#define SINENG_DEVICE_FLAG			(0x55AA)

static u16_t sinengDevice;
static const SinengPV_t *pvCmd;
static SinengPVScan_t *PVScan;

u16_t dataGetCmd[5] = {0xABCD, 1,}; 
u16_t _3_5KStartCmd[] = {0x0100};
u8_t _3_5Password[] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30};
static const SinengPV_t PV_3_5k = {
	80,
	1904,
	1,
	(u16_t*)&_3_5KStartCmd,
	1358,
	4,
	2,
	5020,
	5,
	(u16_t*)&dataGetCmd,
	8100,
	3,
	(u16_t*)_3_5Password
};

u16_t _EFMpptStartCmd[] = {0x8888, 0x0000, 0x0000};
u8_t _EFMpptPassword[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36};
static const SinengPV_t PV_EFMppt = {
	200,
	1948,
	3,
	(u16_t*)&_EFMpptStartCmd,
	2380,
	4,
	1,
	5020,
	5,
	(u16_t*)&dataGetCmd,
	8100,
	3,
	(u16_t*)_EFMpptPassword
};

static const ModbusGetCmd_t APF[] = {
    {0x03, 8000, 8025},
    {0x03, 1050, 1100},
    {0x03, 2030, 2032}
};

static const ModbusGetCmd_t EPCP[] = {
    {0x03, 8000, 8025},
    {0x03, 1050, 1086},
    {0x03, 2010, 2013}
};

static const ModbusGetCmd_t _3_5K[] = {
    {0x03, 8000, 8030},
    {0x03, 1050, 1063},
    {0x03, 1100, 1104},
    {0x03, 1226, 1244},
    {0x03, 2010, 2011},
};

static const ModbusGetCmd_t _50K[] = {
    {0x03, 8000 ,8045},
    {0x03, 1050 ,1082},
    {0x03, 1100 ,1114},
    {0x03, 1480 ,1485},
    {0x03, 1451 ,1453},
    {0x03, 1800 ,1806},
    {0x03, 2030 ,2032},
    {0x03, 2010 ,2018},
};

static const ModbusGetCmd_t _20K[] = {
    {0x03, 8000, 8025},
    {0x03, 1050, 1100},
    {0x03, 2030, 2032},
};

static const ModbusGetCmd_t EFMppt[] = {
    {0x03, 8000, 8025},
    {0x03, 2000, 2033},
    {0x03, 2151, 2165},
    {0x03, 2220, 2255},
    {0x03, 3001, 3002},
    {0x03, 3009, 3019},
};

static const ModbusGetCmd_t _50K_L[] = {
	{0x03, 0x2328, 0x2354},
    {0x03, 0x23BE, 0x23FC},
    {0x03, 0x2454, 0x2458},
    {0x03, 0x25B2, 0x25B3},
    {0x03, 0x25E4, 0x25E4},
    {0x03, 0x25E6, 0x25E7},
    {0x03, 0x25E8, 0x25E9},
    {0x03, 0x25EA, 0x25EB},
};

static const ModbusGetCmd_t _60K_L[] = {
    {0x03, 8000, 8045},
    {0x03, 1050, 1072},
    {0x03, 1100, 1109},
    {0x03, 1990, 1990},
    {0x03, 2010, 2014},
};

static const ModbusGetCmd_t protocolCmd[] = {
	{0x03, 8000, 8005},
	{0x03, 9000, 9009},
};

static u8_t protocolCheck(void *load, void *optPoint);
static const ModbusGetCmdTab_t protocolFind = PROTOCOL_TABER(protocolCmd, 0, 0);
static const ModbusGetCmdTab_t sinengAPF = PROTOCOL_TABER(APF, 0, 0x0240);
static const ModbusGetCmdTab_t sinengEPCP = PROTOCOL_TABER(EPCP, 0, 0x0241);
static const ModbusGetCmdTab_t sineng3_5K = PROTOCOL_TABER(_3_5K, 0, 0x0244);
static const ModbusGetCmdTab_t sineng50K = PROTOCOL_TABER(_50K, 0, 0x0243);
static const ModbusGetCmdTab_t sineng20K = PROTOCOL_TABER(_20K, 0, 0x0245);
static const ModbusGetCmdTab_t sinengEFMppt = PROTOCOL_TABER(EFMppt, 0, 0x0503);
static const ModbusGetCmdTab_t sineng50K_L = PROTOCOL_TABER(_50K_L, 1, 0x0227);
static const ModbusGetCmdTab_t sineng60K_L = PROTOCOL_TABER(_60K_L, 0, 0x0267);
const ModbusDeviceHead_t SinengDevice = {
	&UART_9600_N1,
	&protocolCmd[0],
	&protocolFind,
	protocolCheck,
	null,
}; 

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static u8_t protocolCheck(void *load, void *optPoint)
{
	int ret = 0;
	DeviceCmd_t *cmd;

	cmd = (DeviceCmd_t*)load;
	ret = Modbus_CmdCheck(&cmd->cmd, &cmd->ack);
	if (ret == 0)
	{
		char *str = (char*)&cmd->ack.payload[3];
		cmd->ack.payload[cmd->ack.lenght - 1] = '\0';
		if ((cmd->cmd.payload[2]<<8 | cmd->cmd.payload[3]) == 8000)
		{	
			sinengDevice = 0;
			pvCmd = null;
			PVScan = null;
//			if (r_strfind("FPU-", str) >= 0)            // mike 20200828
            if (r_strstr(str, "FPU-") != NULL)
			{
				*((CONVERT_TYPE)optPoint) = &sinengAPF;
			}
//			else if (r_strfind("EP-0500-A", str) >= 0 || r_strfind("EP-0630-A", str) >= 0)
            else if (r_strstr(str, "EP-0500-A") != NULL || r_strstr(str, "EP-0630-A") != NULL)
			{
				*((CONVERT_TYPE)optPoint) = &sinengEPCP;
			}
//			else if (r_strfind("EF-MPPT", str) >= 0)
            else if (r_strstr(str, "EF-MPPT") != NULL)
			{
				*((CONVERT_TYPE)optPoint) = &sinengEFMppt;
				pvCmd = &PV_EFMppt;
				sinengDevice = SINENG_DEVICE_FLAG;
			}
//			else if ((r_strfind("SP-5K", str) >= 0) || (r_strfind("SP-3K", str) >= 0))
            else if ((r_strstr(str, "SP-5K") != NULL) || (r_strstr(str, "SP-3K") != NULL))
			{
				*((CONVERT_TYPE)optPoint) = &sineng3_5K;
				pvCmd = &PV_3_5k;
				sinengDevice = SINENG_DEVICE_FLAG;
			}
//			else if (r_strfind("SP-20K", str) >= 0)
            else if (r_strstr(str, "SP-20K") != NULL)
			{
				*((CONVERT_TYPE)optPoint) = &sineng20K;
			}
//          else if (r_strfind("SP-50K-L", str) >= 0)
            else if (r_strstr(str, "SP-50K-L") != NULL)
            {
                *((CONVERT_TYPE)optPoint) = &sineng50K_L;
            }
//			else if (r_strfind("SP-50K", str) >= 0)
            else if (r_strstr(str, "SP-50K") != NULL)
			{
				*((CONVERT_TYPE)optPoint) = &sineng50K;
			}
//			else if (r_strfind("SP-60K", str) >= 0)
            else if (r_strstr(str, "SP-60K") != NULL)
			{
				*((CONVERT_TYPE)optPoint) = &sineng60K_L;
			}
			else 
			{
				log_save("Sineng protocol find fail: ");
				log_save(str);
				return 1;
			}
			
			return 0; 
		}
		else
		{
//			if (r_strfind("SP-50K", str) >= 0)
            if (r_strstr(str, "SP-50K") != NULL)
			{
				*((CONVERT_TYPE)optPoint) = &sineng50K;
				return 0; 
			}

		}
		
	}

	return 1;
}

static int PV_scanReady(u8_t addr);
static void PV_scanEnd(void);
static void startAck(DeviceAck_e e);
static void RAMAddrGet(void);
static void RAMAddrAck(DeviceAck_e e);
static void RAMDataAck(DeviceAck_e e);
static void password(void);
static void passwordAck(DeviceAck_e e); 

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
void Sineng_PVData(u8_t addr, u8_t state, u16_t *code, Buffer_t *ack)
{
	ack->lenght = 0;
	ack->payload = null;
	
	if (sinengDevice == SINENG_DEVICE_FLAG && pvCmd != null)
	{
		if (state == 0) //start PV Scan
		{
			ack->size = 1;
			ack->payload = memory_apply(ack->size);
			ack->lenght = 1;

			if (PVScan != null)
			{
				if (PVScan->addr == addr && PVScan->state < PV_RAM_DATA_GET)
				{
					ack->payload[0] = 0x00;
					return ;
				}
				else
				{
					PV_scanEnd();
				}
			}

			if (0 == PV_scanReady(addr))
			{
				ack->payload[0] = 0x00;
			}
			else
			{
				ack->payload[0] = 0x13;
				PV_scanEnd();
			}
		}
		else if (state == 1) //get PV data
		{
			ack->size = 1;
			ack->payload = memory_apply(ack->size);
			ack->lenght = 1;
			
			if (PVScan == null || PVScan->addr != addr)
	        {
	            ack->payload[0] = (0x11);
				PV_scanEnd();
	        }
			else if (PVScan->state == PV_RAM_DATA_UP)
			{
				if (PVScan->upPointCount >= pvCmd->pointCount)
				{
					ack->payload[0] = (0x13);
					PV_scanEnd();
				}
				else
				{
					int i = 0;
					u8_t *p = NULL;

					ack->size = PVScan->buf.lenght + (PVScan->buf.lenght/PVScan->pointSize)*4 + 4;
					memory_release(ack->payload);
					ack->payload = memory_apply(ack->size);
					ack->lenght = 0;
					
					ack->payload[0] = (0x02);
					ack->payload[1] = PVScan->upPointCount;
					ack->payload[2] = PVScan->buf.lenght/PVScan->pointSize;
					ack->payload[3] = pvCmd->pointCount;
					p = &ack->payload[4];

					PVScan->upPointCount += ack->payload[2];
					
					for (i = 0; i < ack->payload[2]; i++) 
					{
						*p++ = 0;
						*p++ = 0;
						*p++ = 0;
						*p++ = (PVScan->upPointCount>>1)-1;
						r_memcpy(p, &PVScan->buf.payload[i*PVScan->pointSize], PVScan->pointSize);
						p += PVScan->pointSize;
					}

					ack->lenght = (int)(p - ack->payload);
	                
	                if (PVScan->upPointCount >= pvCmd->pointCount)
	                {
	                    PV_scanEnd();
	                    log_save("PV Scan complete!");
	                }
					else
					{
						PVScan->buf.lenght = 0;
						PVScan->state = PV_RAM_DATA_WAIT;
#ifdef _PLATFORM_BC25_
						Ql_OS_SendMessage(EYBDEVICE_TASK, DEVICE_PV_GET_ID, 0, 0);
#endif
#ifdef _PLATFORM_L610_
                        Eybpub_UT_SendMessage(EYBDEVICE_TASK, DEVICE_PV_GET_ID, 0, 0);
#endif
					}
	            }
			}
			else
			{
				ack->payload[0] = (0x01);
			}
		}
		else //stop PC scan
		{
			PV_scanEnd();
		}
	}//*/
}


/*******************************************************************************
  * @brief  
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t cmdClear(void *payload, void *point)
{
  DeviceCmd_t * cmd = (DeviceCmd_t*)payload;
  memory_release(cmd->ack.payload);
  memory_release(cmd->cmd.payload);
  return 1;
}
static void ListCmdclear(void)
{
	if (PVScan != null)
	{
	    list_trans(&PVScan->cmdList, cmdClear, null);
	    list_delete(&PVScan->cmdList);
	}
}   

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static int PV_scanReady(u8_t addr)
{ 
	PVScan = memory_apply(sizeof(SinengPVScan_t));

	if (pvCmd != null && PVScan != null)
	{
		DeviceCmd_t *cmd;

		r_memset(PVScan, 0, sizeof(SinengPVScan_t));
		list_init(&PVScan->cmdList);
		PVScan->addr = addr;
		Modbus_16SetCmd(&PVScan->cmdList, (u8_t)PVScan->addr, pvCmd->startAddr,\
						pvCmd->startAddr + pvCmd->startRegNum - 1, (u8_t*)pvCmd->startCmd);
		cmd = list_nextData(&PVScan->cmdList, null);
		if (cmd != null)
		{
			PVScan->deviceHead.buf = &cmd->ack;
			PVScan->deviceHead.callback = startAck;
			PVScan->deviceHead.waitTime = 2000;
			DeviceIO_lock(&PVScan->deviceHead);
#ifdef _PLATFORM_BC25_
            Ql_OS_SendMessage(EYBDEVICE_TASK, DEVICE_PV_SCAN_ID, 0, 0);
#endif
#ifdef _PLATFORM_L610_
            Eybpub_UT_SendMessage(EYBDEVICE_TASK, DEVICE_PV_SCAN_ID, 0, 0);
#endif            
			return 0;
		}	
	}

	return -1;
}

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static void PV_scanEnd(void)
{
	DeviceIO_unlock();
	ListCmdclear();
	memory_release(PVScan);
	PVScan = null;
}

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
void PV_Scan(void)
{
	DeviceCmd_t *cmd;

	cmd = list_nextData(&PVScan->cmdList, null);

	if (cmd != null)
	{
		int	ret = DeviceIO_write(&PVScan->deviceHead, cmd->cmd.payload, cmd->cmd.lenght);
		if (ret != 0)
		{
#ifdef _PLATFORM_BC25_
		    Ql_OS_SendMessage(EYBDEVICE_TASK, DEVICE_PV_SCAN_ID, 0, 0);
#endif
#ifdef _PLATFORM_L610_
            Eybpub_UT_SendMessage(EYBDEVICE_TASK, DEVICE_PV_SCAN_ID, 0, 0);
#endif            
		}	
	}
	else
	{
		PV_scanEnd();
	}
}


/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static void startAck(DeviceAck_e e)
{
	DeviceCmd_t *cmd;
	int i = 0;

	cmd = list_nextData(&PVScan->cmdList, null);
	if ((e == DEVICE_ACK_FINISH) 
		&& (cmd != null)
		&& (0 == (i = Modbus_CmdCheck(&cmd->cmd, &cmd->ack)))
		)
	{
		ListCmdclear();
		ModbusCmd_0304(&PVScan->cmdList, PVScan->addr, pvCmd->readyAddr, 0x04,\
						pvCmd->readyAddr + pvCmd->readyRAMSize - 1);
		cmd = list_nextData(&PVScan->cmdList, null);
		if (cmd != null)
		{
			PVScan->deviceHead.buf = &cmd->ack;
			PVScan->deviceHead.callback = RAMAddrAck;
			PVScan->deviceHead.waitTime = 2000;
			RAMAddrGet();
			PVScan->tryCnt = 0;
			PVScan->state = PV_READY;
		}
		else
		{
			PV_scanEnd();
		}
	}
	else if (++PVScan->tryCnt > 10 || i > 0)
	{
		PV_scanEnd();
	}
	else
	{
		PV_Scan();
	}
}

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static void RAMAddrGet(void)
{
	DeviceCmd_t *cmd;

	cmd = list_nextData(&PVScan->cmdList, null);

	if (cmd != null)
	{
		int	ret = DeviceIO_write(&PVScan->deviceHead, cmd->cmd.payload, cmd->cmd.lenght);
		if (ret == 0)
		{
			return ;
		}
	}

	PV_scanEnd();
}

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static void RAMAddrAck(DeviceAck_e e)
{
	DeviceCmd_t *cmd;
	int i = 0;

	cmd = list_nextData(&PVScan->cmdList, null);
	if ((e == DEVICE_ACK_FINISH) 
		&& (cmd != null)
		&& (0 == (i = Modbus_CmdCheck(&cmd->cmd, &cmd->ack)))
		)
	{
        r_memcpy(&PVScan->RAM, &cmd->ack.payload[3], sizeof(PVScan->RAM));  
		PVScan->RAM.addr = ENDIAN_BIG_LITTLE_32(PVScan->RAM.addr);
		PVScan->RAM.size = ENDIAN_BIG_LITTLE_16(PVScan->RAM.size);
		PVScan->RAM.user = ENDIAN_BIG_LITTLE_16(PVScan->RAM.user);

		PVScan->deviceHead.buf = &cmd->ack;
		PVScan->deviceHead.callback = RAMDataAck;
		PVScan->deviceHead.waitTime = 2000;
		PVScan->pointSize = (PVScan->RAM.size/pvCmd->pointCount)<<1;
		PVScan->buf.size = 880 - 880%PVScan->pointSize; //point buffer complete
		PVScan->buf.lenght = 0;
		PVScan->buf.payload = memory_apply(PVScan->buf.size);
		switch (PVScan->pointSize)
		{
			case 4:
	            PVScan->code = 0x0506;
		        break;
	        case 8:
	            PVScan->code = 0x0507;
	        	break;
	        default:
	            PVScan->code = 0x0504;
	        break;
		}
		PV_dataGet();
		PVScan->tryCnt = 0;
		PVScan->state = PV_RAM_ADDR_GET;
	}
	else if (++PVScan->tryCnt > 10 || i > 0)
	{
		PV_scanEnd();
	}
	else
	{
		RAMAddrGet();
	}
}

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
void PV_dataGet(void)
{
	DeviceCmd_t *cmd;
	
    ListCmdclear();

	pvCmd->dataCmd[2] = ((PVScan->RAM.addr>>24)&0xFF) | ((PVScan->RAM.addr>>16)&0xFF);
	pvCmd->dataCmd[3] = ((PVScan->RAM.addr>>8)&0xFF) | ((PVScan->RAM.addr)&0xFF);
	pvCmd->dataCmd[4] = ENDIAN_BIG_LITTLE_16(PVScan->RAM.size > 64 ? 64 : PVScan->RAM.size);
	Modbus_16SetCmd(&PVScan->cmdList, (u8_t)PVScan->addr, pvCmd->dataAddr,\
						pvCmd->dataAddr + pvCmd->dataRegNum - 1, (u8_t*)pvCmd->dataCmd);
	cmd = list_nextData(&PVScan->cmdList, null);

	if (cmd != null)
	{
		int	ret = DeviceIO_write(&PVScan->deviceHead, cmd->cmd.payload, cmd->cmd.lenght);
        PVScan->deviceHead.callback = RAMDataAck;
		if (ret == 0)
		{
			return ;
		}
	}

	PV_scanEnd();
}

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static void RAMDataAck(DeviceAck_e e)
{
	DeviceCmd_t *cmd;
	int i =0, j = 0;    // mike 20200914

	cmd = list_nextData(&PVScan->cmdList, null);
    j = Modbus_CmdCheck(&cmd->cmd, &cmd->ack);
	if ((e == DEVICE_ACK_FINISH) && (cmd != null) && (0 == j)) {
		i = PVScan->buf.size - PVScan->buf.lenght;

		cmd->cmd.lenght -= 5;
		
		i = i > (cmd->cmd.lenght) ? (cmd->cmd.lenght) : i;
		
		PVScan->RAM.addr += i/2;
		PVScan->RAM.size -= i/2;
        r_memcpy((&PVScan->buf.payload[PVScan->buf.lenght]), &cmd->ack.payload[2], i);
		PVScan->buf.lenght += i;
 
		if (PVScan->buf.lenght == PVScan->buf.size || PVScan->RAM.size == 0)
		{
			PVScan->tryCnt = 0;
			PVScan->state = PV_RAM_DATA_UP;
		}
		else
		{
			PVScan->tryCnt = 0;
			PVScan->state = PV_RAM_DATA_GET;
			PV_dataGet();
		}
	}
	else if (j > 0)
	{
		password();
	}
	else if (++PVScan->tryCnt > 10)
	{
		PV_scanEnd();
	}
	else
	{
		PV_dataGet();
	}
}

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static void password(void)
{
	DeviceCmd_t *cmd;

	ListCmdclear();
	Modbus_16SetCmd(&PVScan->cmdList, (u8_t)PVScan->addr, pvCmd->passwordAddr,\
						pvCmd->passwordAddr + pvCmd->passwordRegNum - 1, (u8_t*)pvCmd->passwordCmd);
	cmd = list_nextData(&PVScan->cmdList, null);

	if (cmd != null)
	{
		int	ret = DeviceIO_write(&PVScan->deviceHead, cmd->cmd.payload, cmd->cmd.lenght);
        PVScan->deviceHead.callback = passwordAck;
		if (ret == 0)
		{
			return;
		}
	}

	PV_scanEnd();
}

/*******************************************************************************
  * @brief	
  * @param	None
  * @retval None
*******************************************************************************/
static void passwordAck(DeviceAck_e e)
{
	DeviceCmd_t *cmd;
	int i = 0;

	cmd = list_nextData(&PVScan->cmdList, null);
	if ((e == DEVICE_ACK_FINISH) 
		&& (cmd != null)
		&& (0 == (i = Modbus_CmdCheck(&cmd->cmd, &cmd->ack)))
		)
	{
		PVScan->tryCnt = 0;
		PV_dataGet();
	}
	else if (i > 0)
	{
		log_save("PV password error!!");
	}
	else if (++PVScan->tryCnt > 3)
	{
		PV_scanEnd();
	}
	else
	{
		password();
	}
}

/******************************************************************************/

