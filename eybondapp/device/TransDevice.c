/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : TransDevice.c  Eybond transparent transmission monitor
  *@notes   : 2017.12.28 CGQ
*******************************************************************************/
#include "eybpub_Debug.h"

#include "eyblib_r_stdlib.h"
#include "eyblib_list.h"

#include "TransDevice.h"
#include "Device.h"
#include "Protocol.h"

static const TransCfgTab_t TransCfgTab[] = {    // 对于07,A3客户码强制绑定9600波特率
  {"07", (ST_UARTDCB *)& UART_9600_N1},
  {"A3", (ST_UARTDCB *)& UART_9600_E1},
};

static u8_t Transcallback(Device_t *dev);

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void TransDevice_init(char *str, ST_UARTDCB *cfg) {
  int i = 0;
  Device_t *dev = NULL;

  for (i = 0; i < sizeof(TransCfgTab) / sizeof(TransCfgTab[0]); i++) {
  // if (r_strfind(str, TransCfgTab[i].narration) >= 0)  // mike 20200828
  // APP_DEBUG("%s %s\r\n", str, TransCfgTab[i].narration);
    if (r_strstr(TransCfgTab[i].narration, str) != NULL) {
      break;
    }
  }  //

  if (i >= sizeof(TransCfgTab) / sizeof(TransCfgTab[0])) {
    i = 0;
  }
  APP_DEBUG("%d %ld\r\n", i, cfg->baudrate);

  dev = list_nodeApply(sizeof(Device_t));
  if (dev == NULL) {
    APP_DEBUG("dev node list is null\r\n");
  }
  dev->type = DEVICE_TRANS;
  if (cfg == null) {
    dev->cfg = TransCfgTab[i].cfg;
  } else {
    dev->cfg = cfg;
  }
  dev->callBack = Transcallback;
  dev->explain = null;
  list_init(&dev->cmdList);

  Device_add(dev);
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void TransDevice_clear(void) {

}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static u8_t  Transcallback(Device_t *dev) {
  APP_DEBUG("Transcallback\r\n");
  return 0;
}

/******************************************************************************/

