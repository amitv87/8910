/**************Copyright(C)��2015-2026��QIYI Temp *****************************
  *@brief   : ParaSave.c 
  *@notes   : 2017.07.15  Unarty establish define Item powerDown save Para
*******************************************************************************/
#include "eybpub_run_log.h"
#include "ParaSave.h"
#include "FlashReliablec.h"
#include "FlashHard.h"

FlashReliable_t s_paraHead;

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
int Para_init(void)
{
    if (0 != FlashReliable_init(&s_paraHead, FLASH_SYSPARA_ADDR, FLASH_SYSPARA_SIZE)
		|| s_paraHead.userAddr == 0)
    {
      
      log_save("Para save area err!");
      return 1;
    }

	return 0;
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
int Para_read(u16_t offset, Buffer_t *buf)
{
	
    return FlashReliable_read(&s_paraHead, offset, buf);
}

/*******************************************************************************
  * @brief  
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
int Para_write(u16_t offset, Buffer_t *buf)
{
    return FlashReliable_write(&s_paraHead, offset, buf);
}
/******************************************************************************/

