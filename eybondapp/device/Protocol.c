/**************Copyright(C)，2015-2026，Shenzhen Eybond Co., Ltd.***************
  *@brief   : ModbusProtocol
  *@notes   : 2017.12.18 CGQ
*******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_stdlib.h"
#include "ql_memory.h"
#endif

#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#endif

#include "eybpub_Debug.h"
#include "eybpub_SysPara_File.h"

#include "eyblib_typedef.h"
#include "eyblib_swap.h"
#include "eyblib_algorithm.h"
#include "eyblib_memory.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_list.h"

#include "Device.h"
#include "TransDevice.h"

#include "Protocol.h"
#include "MOSO.h"
#include "Common.h"

#include "ModbusDevice.h"
#include "Litto.h"
#include "Sineng.h"
#include "Huawei.h"
#include "Sofar.h"
#include "MustSolar.h"
#include "growatt.h"
#include "GreatWall.h"
#include "InnovPower.h"
#include "East.h"
#include "SunGrow.h"
#include "SREN.h"
#include "Suntrio.h"
#include "AOTAI.h"
#include "Chunhui.h"
#include "EybondSmartMeter.h"
#include "Changling.h"
#include "KLIE.h"
#include "Hopewind.h"

typedef struct {
  u16_t code;
  u16_t type;
  const void *protocol;
} DeviceProtocol_t;

const ST_UARTDCB UART_2400_N1   = {2400,  HAL_UART_DATA_BITS_8, HAL_UART_STOP_BITS_1, HAL_UART_NO_PARITY, FC_NONE};
const ST_UARTDCB UART_4800_N1   = {4800,  HAL_UART_DATA_BITS_8, HAL_UART_STOP_BITS_1, HAL_UART_NO_PARITY, FC_NONE};
const ST_UARTDCB UART_9600_N1   = {9600,  HAL_UART_DATA_BITS_8, HAL_UART_STOP_BITS_1, HAL_UART_NO_PARITY, FC_NONE};
const ST_UARTDCB UART_9600_E1   = {9600,  HAL_UART_DATA_BITS_8, HAL_UART_STOP_BITS_1, HAL_UART_EVEN_PARITY, FC_NONE};
const ST_UARTDCB UART_19200_N1  = {19200,   HAL_UART_DATA_BITS_8, HAL_UART_STOP_BITS_1, HAL_UART_NO_PARITY, FC_NONE};
const ST_UARTDCB UART_38400_N1  = {38400,   HAL_UART_DATA_BITS_8, HAL_UART_STOP_BITS_1, HAL_UART_NO_PARITY, FC_NONE};
const ST_UARTDCB UART_115200_N1 = {115200,  HAL_UART_DATA_BITS_8, HAL_UART_STOP_BITS_1, HAL_UART_NO_PARITY, FC_NONE};

const DeviceProtocol_t protocolTab[] = {
  {0x0200, DEVICE_MOBUS, &EybondDevice}, // Eybond
  {0x0201, DEVICE_MOBUS, &_0201Device},
  {0x0202, DEVICE_MOBUS, &_0202Device},
  {0x0204, DEVICE_MOBUS, &_0204Device},
  {0x0205, DEVICE_MOBUS, &_0205Device},
  {0x0206, DEVICE_MOBUS, &_0206Device},
  {0x0209, DEVICE_MOBUS, &_0209Device},
  {0x020E, DEVICE_MOBUS, &_020EDevice},
  {0x0210, DEVICE_MOBUS, &MOSODevice},   //  Moso
  {0x0211, DEVICE_MOBUS, &MOSODevice},   //  Moso
  {0x0212, DEVICE_MOBUS, &MOSODevice},   //  Moso
  {0x0213, DEVICE_MOBUS, &_0213Device},
  {0x0214, DEVICE_MOBUS, &_0214Device},
  {0x0215, DEVICE_MOBUS, &ChunHuiDevice},
  {0x0216, DEVICE_MOBUS, &_0216Device},
  {0x0217, DEVICE_MOBUS, &_0217Device},
  {0x0218, DEVICE_MOBUS, &_0218Device},
  {0x021A, DEVICE_MOBUS, &_021ADevice},
  {0x021B, DEVICE_MOBUS, &_021BDevice},
  {0x021C, DEVICE_MOBUS, &_021CDevice},
  {0x021D, DEVICE_MOBUS, &_021DDevice},
  {0x021E, DEVICE_MOBUS, &_021EDevice},
  {0x021F, DEVICE_MOBUS, &_021FDevice},
  {0x0222, DEVICE_MOBUS, &ClouDevice},   //  Clou
  {0x0224, DEVICE_MOBUS, &_0224Device},
  {0x0226, DEVICE_MOBUS, &_0226Device},
  {0x0227, DEVICE_MOBUS, &SinengDevice},
  {0x0228, DEVICE_MOBUS, &ChangLingDevice},
  {0x0229, DEVICE_MOBUS, &_0229Device},
  {0x022A, DEVICE_MOBUS, &Sungrow022ADevice},
  {0x022B, DEVICE_MOBUS, &_022BDevice},
  {0x022C, DEVICE_MOBUS, &Tbea022CDevice},
  {0x022D, DEVICE_MOBUS, &_022DDevice},
  {0x0230, DEVICE_MOBUS, &InvtDevice},   //  Invt
  {0x0231, DEVICE_MOBUS, &HopewindDevice},
  {0x0232, DEVICE_MOBUS, &_0232Device},
  {0x0233, DEVICE_MOBUS, &_0233Device},
  {0x0234, DEVICE_MOBUS, &_0234Device},
  {0x0235, DEVICE_MOBUS, &_0235Device},
  {0x0236, DEVICE_MOBUS, &_0236Device},
  {0x0237, DEVICE_MOBUS, &HopewindDevice},
  {0x0238, DEVICE_MOBUS, &_0238Device},
  {0x0240, DEVICE_MOBUS, &SinengDevice}, // Sineng SP
  {0X0241, DEVICE_MOBUS, &SinengDevice}, //  Sineng EP CP
  {0x0242, DEVICE_MOBUS, &_0242Device},
  {0x0243, DEVICE_MOBUS, &SinengDevice}, //  Sineng 50K
  {0x0244, DEVICE_MOBUS, &SinengDevice}, //  Sineng 3_5K
  {0x0245, DEVICE_MOBUS, &SinengDevice}, //  Sineng 20K
  {0x0248, DEVICE_MOBUS, &_0248Device},
  {0x0249, DEVICE_MOBUS, &_0249Device},
  {0x024A, DEVICE_MOBUS, &_024ADevice},
  {0x024B, DEVICE_MOBUS, &_024BDevice},
  {0x024D, DEVICE_MOBUS, &_024DDevice},
  {0x024E, DEVICE_MOBUS, &_024EDevice},
  {0x024F, DEVICE_MOBUS, &_024FDevice},
  {0x0250, DEVICE_MOBUS, &EastDevice},  // 易事特
  {0x0251, DEVICE_MOBUS, &EastDevice},
  {0x0252, DEVICE_MOBUS, &EastDevice},
  {0x0253, DEVICE_MOBUS, &_0253Device},
  {0x0254, DEVICE_MOBUS, &_0254Device},
  {0x0255, DEVICE_MOBUS, &_0255Device},
  {0x0256, DEVICE_MOBUS, &_0256Device},
  {0x0257, DEVICE_MOBUS, &_0257Device},
  {0x0258, DEVICE_MOBUS, &_0258Device},
  {0x0259, DEVICE_MOBUS, &_0259Device},
  {0x025A, DEVICE_MOBUS, &_025ADevice},
  {0x025B, DEVICE_MOBUS, &_025BDevice},
  {0x025D, DEVICE_MOBUS, &_025DDevice},
  {0x025E, DEVICE_MOBUS, &HuaweiDevice},
  {0x025F, DEVICE_MOBUS, &_025FDevice},
  {0x0260, DEVICE_MOBUS, &LittoDevice}, //  Litto
  {0x0262, DEVICE_MOBUS, &_0262Device},
  {0x0263, DEVICE_MOBUS, &_0263Device},
  {0x0264, DEVICE_MOBUS, &_0264Device},
  {0x0265, DEVICE_MOBUS, &_0265Device},
  {0x0266, DEVICE_MOBUS, &_0266Device},
  {0x0267, DEVICE_MOBUS, &SinengDevice},
  {0x0268, DEVICE_MOBUS, &_0268Device},
  {0x0269, DEVICE_MOBUS, &HuaweiDevice},
  {0x026A, DEVICE_MOBUS, &_026ADevice},
  {0x026B, DEVICE_MOBUS, &_026BDevice},
  {0x026C, DEVICE_MOBUS, &_026CDevice},
  {0x026F, DEVICE_MOBUS, &_026FDevice},
  {0x0270, DEVICE_MOBUS, &Kstar2Device},  //  Kstar
  {0x0272, DEVICE_MOBUS, &_0272Device},
  {0x0273, DEVICE_MOBUS, &_0273Device}, //  Kstar
  {0x0274, DEVICE_MOBUS, &_0274Device},
  {0x0275, DEVICE_MOBUS, &_0275Device},
  {0x0276, DEVICE_MOBUS, &_0276Device},
  {0x0278, DEVICE_MOBUS, &_0278Device},
  {0x0279, DEVICE_MOBUS, &_0279Device},
  {0x027A, DEVICE_MOBUS, &_027ADevice},
  {0x027B, DEVICE_MOBUS, &_027BDevice},
  {0x027C, DEVICE_MOBUS, &_027CDevice},
  {0x027D, DEVICE_MOBUS, &_027DDevice},
  {0x027E, DEVICE_MOBUS, &_027EDevice},
  {0x027F, DEVICE_MOBUS, &_027FDevice},
  {0x0281, DEVICE_MOBUS, &ChunHuiDevice},
  {0x0283, DEVICE_MOBUS, &_0283Device},
  {0x0284, DEVICE_MOBUS, &HuaweiDevice},
  {0x0285, DEVICE_MOBUS, &Tbea4Device},
  {0x0287, DEVICE_MOBUS, &Sungrow0287Device},
  {0x0288, DEVICE_MOBUS, &HopewindDevice},
  {0x0289, DEVICE_MOBUS, &InovanceDevice},
  {0x028C, DEVICE_MOBUS, &_028CDevice},
  {0x028F, DEVICE_MOBUS, &_028FDevice},
  {0x0290, DEVICE_MOBUS, &GrowattDevice}, //  Growatt
  {0x0291, DEVICE_MOBUS, &KeHuaDevice},
  {0x0292, DEVICE_MOBUS, &Suntrio0292Device},
  {0x0293, DEVICE_MOBUS, &ChunHuiDevice},
  {0x0294, DEVICE_MOBUS, &ChunHuiDevice},
  {0x0295, DEVICE_MOBUS, &YanXuDevice},
  {0x0296, DEVICE_MOBUS, &DeltaDevice},
  {0x0297, DEVICE_MOBUS, &MYSolarDevice},
  {0x0298, DEVICE_MOBUS, &MixedDevice},
  {0x0299, DEVICE_MOBUS, &HuaweiDevice},
  {0x029C, DEVICE_MOBUS, &CRRCDevice},   // MODBUS
  {0x029D, DEVICE_MOBUS, &Innovpower9DDevice}, // 250K
  {0x029E, DEVICE_MOBUS, &Sungrow029EDevice},    // С
  {0x029F, DEVICE_MOBUS, &SofarDevice},   // 50-70K
  {0x02A0, DEVICE_MOBUS, &SofarDevice},     //
  {0x02A1, DEVICE_MOBUS, &MustsolarDevice},
  {0x02A2, DEVICE_MOBUS, &Suntrio02A2Device},
  {0x02A3, DEVICE_MOBUS, &Suntrio02A3Device},
  {0x02A4, DEVICE_MOBUS, &Poweroak1Device},
  {0x02A7, DEVICE_MOBUS, &HuaweiDevice},    // 8~28k
  {0x02A8, DEVICE_MOBUS, &HuaweiDevice},    // 33~40k
  {0x02A9, DEVICE_MOBUS, &HuaweiDevice},      // 2000
  {0x02AA, DEVICE_MOBUS, &TsolDevice},
  {0x02AB, DEVICE_MOBUS, &Poweroak2Device},
  {0x02AC, DEVICE_MOBUS, &TsinergyDevice},
  {0x02AD, DEVICE_MOBUS, &Sungrow02ADDevice},   //
  {0x02AE, DEVICE_MOBUS, &GreatWallDevice},     //
  {0x02AF, DEVICE_MOBUS, &InnovpowerAFDevice},  //
  {0x02B0, DEVICE_MOBUS, &GreatWallOldDevice},  //
  {0x02B1, DEVICE_MOBUS, &SolarlakeDevice},
  {0x02B2, DEVICE_MOBUS, &Tbea1Device},
  {0x02B3, DEVICE_MOBUS, &Jxznet1Device},
  {0x02B4, DEVICE_MOBUS, &Tbea2Device},
  {0x02B5, DEVICE_MOBUS, &Kstar1Device},
  {0x02B6, DEVICE_MOBUS, &Jxznet2Device},
  {0x02B7, DEVICE_MOBUS, &AthertonDevice},
  {0x02B8, DEVICE_MOBUS, &DeliDevice},
  {0x02B9, DEVICE_MOBUS, &MustSolarDevice2},
  {0x02BA, DEVICE_MOBUS, &GinlongDevice},
  {0x02BB, DEVICE_MOBUS, &MustSolarDevice1},
  {0x02BC, DEVICE_MOBUS, &ZeversolarDevice},
  {0x02BD, DEVICE_MOBUS, &GoodweDevice},
  {0x02BE, DEVICE_MOBUS, &Deli02BEDevice},
  {0x02BF, DEVICE_MOBUS, &ZealinkDevice},
  {0x02C0, DEVICE_MOBUS, &ChintDevice},
  {0x02C1, DEVICE_MOBUS, &MustSolarDevice1},
  {0x02C4, DEVICE_MOBUS, &KostalDevice},
  {0x02C5, DEVICE_MOBUS, &SrneDevice},
  {0x02C6, DEVICE_MOBUS, &HuaweiDevice},    // 20K
  {0x02C7, DEVICE_MOBUS, &AtoaiDevice},
  {0x02C8, DEVICE_MOBUS, &AtoaiDevice},
  {0x02C9, DEVICE_MOBUS, &AtoaiDevice},
  {0x02CA, DEVICE_MOBUS, &KLNEDevice},
  {0x02CD, DEVICE_MOBUS, &GreatWallEQMX1Device},  //
  {0x02CE, DEVICE_MOBUS, &GreatWallEQMX2Device},
  {0x02CF, DEVICE_MOBUS, &GoldwindDevice},
  {0x02D0, DEVICE_MOBUS, &XjgcDevice},
  {0x02D1, DEVICE_MOBUS, &_02D1Device},  // Growatt
  {0x02D2, DEVICE_MOBUS, &GreatWallEQMX3Device},  //
  {0x02D3, DEVICE_MOBUS, &HuaweiDevice},    // SUN2000
  {0x02D5, DEVICE_MOBUS, &Sungrow02D5Device},
  {0x02D7, DEVICE_MOBUS, &SenErgyDevice},
  {0x02D8, DEVICE_MOBUS, &ClenErgyDevice},    //
  {0x02D9, DEVICE_MOBUS, &_02D9Device},   // Growatt CP & 100
  {0x02DA, DEVICE_MOBUS, &NOGODevice},  //
  {0x02DB, DEVICE_MOBUS, &Tbea3Device},  // 50kw
  {0x02DC, DEVICE_MOBUS, &_02DCDevice},
  {0x02DD, DEVICE_MOBUS, &HuaweiDevice},    // SUN2000L60K
  {0x02DE, DEVICE_MOBUS, &HuaweiDevice},    // SUN2000L60K
  {0x02E1, DEVICE_MOBUS, &_02E1Device},
  {0x02FF, DEVICE_TRANS,  null},
  {0x0300, DEVICE_MOBUS, &EybondHDevice},
  {0x0301, DEVICE_MOBUS, &Whxph1Device},
  {0x0303, DEVICE_MOBUS, &Whxph2Device},
  {0x0305, DEVICE_MOBUS, &JinZhouDevice},
  {0x0306, DEVICE_MOBUS, &Whxph3Device},
  {0x0307, DEVICE_MOBUS, &BVGDevice},
  {0x0308, DEVICE_MOBUS, &_0308Device},
  {0x0309, DEVICE_MOBUS, &_0309Device},
  {0x030A, DEVICE_MOBUS, &_030ADevice},
  {0x030B, DEVICE_MOBUS, &_030BDevice},
  {0x030C, DEVICE_MOBUS, &_030CDevice},
  {0x030D, DEVICE_MOBUS, &_030DDevice},
  {0x030E, DEVICE_MOBUS, &_030EDevice},
  {0x0400, DEVICE_MOBUS, &Eybond4Device},
  {0x0401, DEVICE_MOBUS, &ChlykjDevice},
  {0x0402, DEVICE_MOBUS, &Ha0402Device},
  {0x0403, DEVICE_MOBUS, &Ha0403Device},
  {0x0404, DEVICE_MOBUS, &YgDevice},
  {0x0405, DEVICE_MOBUS, &AcrelDevice},
  {0x0406, DEVICE_MOBUS, &ChintMetetDevice},
  {0x0407, DEVICE_MOBUS, &_0407Device},
  {0x0408, DEVICE_MOBUS, &_0408Device},
  {0x0409, DEVICE_MOBUS, &_0409Device},
  {0x040A, DEVICE_MOBUS, &_040ADevice},
  {0x040B, DEVICE_MOBUS, &_040BDevice},
  {0x040C, DEVICE_MOBUS, &_040CDevice},
  {0x040D, DEVICE_MOBUS, &_040DDevice},
  {0x0412, DEVICE_MOBUS, &_0412Device},
  {0x0501, DEVICE_TRANS, &Litto2Device},
  {0x0502, DEVICE_TRANS, &Kstar3Device},
  {0x0503, DEVICE_MOBUS, &SinengDevice},  //
  {0x0505, DEVICE_MOBUS, &TeiHeDevice},
  {0x0508, DEVICE_MOBUS, &GreatWall2Device},
  {0x0509, DEVICE_MOBUS, &FygDevice},
  {0x050B, DEVICE_MOBUS, &_050BDevice},
  {0x050C, DEVICE_MOBUS, &_050CDevice},
  {0x0510, DEVICE_MOBUS, &XintronDevice},
  {0x051B, DEVICE_MOBUS, &_051BDevice},
  {0x0701, DEVICE_MOBUS, &PoweroakDevice3},
  {0x0702, DEVICE_MOBUS, &GreatWall3Device},
  {0x0707, DEVICE_MOBUS, &_0707Device},
  {0x0708, DEVICE_MOBUS, &_0708Device},
  {0x0801, DEVICE_MOBUS, &PoweroakDevice4},
  {0x0802, DEVICE_MOBUS, &MustSolarDevice3},
  {0x0803, DEVICE_MOBUS, &MOSOBatter},
  {0x0804, DEVICE_MOBUS, &_0804Device},
  {0x0902, DEVICE_MOBUS, &MustSolarDevice4},
  {0x0903, DEVICE_MOBUS, &MustSolarDevice5},
  {0x0904, DEVICE_MOBUS, &VoltronicDevice},
  {0x0905, DEVICE_MOBUS, &NicestDevice},
  {0x0906, DEVICE_MOBUS, &VoltronicDevice2},
  {0x0907, DEVICE_MOBUS, &VoltronicDevice3},
  {0x0908, DEVICE_MOBUS, &Kstar4Device},
  {0x0909, DEVICE_MOBUS, &KelongDevice},
  {0x090D, DEVICE_MOBUS, &SRENDevice},
  {0x090F, DEVICE_MOBUS, &_090FDevice},
  {0x0910, DEVICE_MOBUS, &CanroonDevice},
  {0x0912, DEVICE_MOBUS, &Nicest0912Device},
  {0x0915, DEVICE_MOBUS, &_0915Device},
  {0x0919, DEVICE_MOBUS, &_0919Device},
  {0x091A, DEVICE_MOBUS, &_091ADevice},
  {0x091B, DEVICE_MOBUS, &_091BDevice}, //  Kstar
  {0x091C, DEVICE_MOBUS, &_091CDevice}, //  Kstar
  {0x091D, DEVICE_MOBUS, &_091DDevice},
  {0x091F, DEVICE_MOBUS, &Srne091FDevice},
  {0x0920, DEVICE_MOBUS, &Srne0920Device},
  {0x0921, DEVICE_MOBUS, &ChuangChiDevice},
  {0x0923, DEVICE_MOBUS, &Invt0923Device},
  {0x0924, DEVICE_MOBUS, &_0924Device},
  {0x0925, DEVICE_MOBUS, &Nicest0925Device},
  {0x0927, DEVICE_MOBUS, &DEYE0927Device},
  {0x0928, DEVICE_MOBUS, &BOS0928Device},
  {0x092A, DEVICE_MOBUS, &_092ADevice},
  {0x092D, DEVICE_MOBUS, &_092DDevice},
  {0x0935, DEVICE_MOBUS, &_0935Device},
  {0x0936, DEVICE_MOBUS, &_0936Device},
  {0x0937, DEVICE_MOBUS, &_0937Device},
  {0x0938, DEVICE_MOBUS, &_0938Device},
  {0x0939, DEVICE_MOBUS, &_0939Device},
  {0x093A, DEVICE_MOBUS, &_093ADevice},
  {0x093B, DEVICE_MOBUS, &_093BDevice},
  {0x093C, DEVICE_MOBUS, &_093CDevice},
  {0x093D, DEVICE_MOBUS, &_093DDevice},
  {0x0940, DEVICE_MOBUS, &_0940Device},
  {0x0941, DEVICE_MOBUS, &_0941Device},
  {0x0942, DEVICE_MOBUS, &_0942Device},
  {0x0943, DEVICE_MOBUS, &_0943Device},
  {0x0944, DEVICE_MOBUS, &_0944Device},
  {0x0945, DEVICE_MOBUS, &_0945Device},
  {0x094B, DEVICE_MOBUS, &_094BDevice},
  {0x094C, DEVICE_MOBUS, &_094CDevice},
  {0x094E, DEVICE_MOBUS, &_094EDevice},
  {0x094F, DEVICE_MOBUS, &_094FDevice},
  {0x0965, DEVICE_MOBUS, &_0965Device},
  {0x0971, DEVICE_MOBUS, &_0971Device},
  {0x0B01, DEVICE_MOBUS, &_0B01Device},
  {0x0c01, DEVICE_MOBUS, &EybondSmartMeter},
  {0x0C05, DEVICE_MOBUS, &_0C05Device},
  {0x0C0E, DEVICE_MOBUS, &_0C0EDevice},
  {0x0E01, DEVICE_MOBUS, &_0E01Device},
  {0x0E0E, DEVICE_MOBUS, &_0E0EDevice},
  {0x1001, DEVICE_MOBUS, &_1001Device},
};

static u8_t addrTab[68];
static const char trans_mode[]  = "1";
static const char modbus_mode[] = "0";
static const char noFind[] = "\0";

ADDDevice_t MeterDevice;
ModbusDevice_t ModbusDevice;

static int monitorCountGet(int *num);

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void Protocol_init(void) {
  ProtocolAttr_t attr;
  DeviceProtocol_t *pro;
  int i = 0;
  u8_t *p;
  Buffer_t buf;

  ModbusDevice.head = null;
  ModbusDevice.cfg = NULL;  // 已在Protocol_clean中释放
  ModbusDevice.addrTab  = addrTab;

  MeterDevice.startAddr   = 0;
  MeterDevice.endAddr   = 0;
  MeterDevice.cfg     = null;   // 已在Protocol_clean中释放
  MeterDevice.head    = null;
  r_memset(addrTab, 0, sizeof(addrTab));
  p = addrTab;

  /* 获取监控设备数量 */
  if (monitorCountGet((int *)&ModbusDevice.monitorCount) != 0) {
    goto END;
  }

  APP_DEBUG("ModbusDevice count %d\r\n", ModbusDevice.monitorCount);
  r_memset(&attr, 0, sizeof(ProtocolAttr_t));
  /* 电表相关操作 */
  if (protocolAttrGet(METER_PROTOCOL, &attr) == 0) {
    pro = (DeviceProtocol_t *)ALG_binaryFind(attr.code, BINARY_FIND(protocolTab));

    if (pro != null) {
      APP_DEBUG("Meter protocol code %x, %d. \r\n", pro->code, attr.startAddr);
      MeterDevice.startAddr = attr.startAddr;
      MeterDevice.endAddr = attr.endAddr;
      MeterDevice.cfg = memory_apply(sizeof(ST_UARTDCB));
//      MeterDevice.cfg = attr.cfg;
      if (attr.cfg.baudrate == 0) {
        APP_DEBUG("Meter Device UART setting config is null\r\n");
        MeterDevice.cfg->baudrate = UART_9600_N1.baudrate;
        MeterDevice.cfg->dataBits = UART_9600_N1.dataBits;
        MeterDevice.cfg->stopBits = UART_9600_N1.stopBits;
        MeterDevice.cfg->parity = UART_9600_N1.parity;
        MeterDevice.cfg->flowCtrl = UART_9600_N1.flowCtrl;
      } else {
        MeterDevice.cfg->baudrate = attr.cfg.baudrate;
        MeterDevice.cfg->dataBits = attr.cfg.dataBits;
        MeterDevice.cfg->stopBits = attr.cfg.stopBits;
        MeterDevice.cfg->parity = attr.cfg.parity;
        MeterDevice.cfg->flowCtrl = attr.cfg.flowCtrl;
      }
      MeterDevice.head = pro->protocol;
      i = MeterDevice.endAddr - MeterDevice.startAddr;
      for (*p++ = attr.startAddr; i > 0 && p < (u8_t *)(&addrTab + 1); i--, p++) {
        *p = *(p - 1) + 1;
      }
    }
  }

 // r_memset(&attr, 0, sizeof(ProtocolAttr_t));
  if (protocolAttrGet(DEVICE_PROTOCOL, &attr) != 0) {
    goto END;
  }

  APP_DEBUG("Device protocol code 0x%04x, %d - %d \r\n", attr.code, attr.startAddr, attr.endAddr);

  /* 从协议列表中提取目标协议 */
  pro = (DeviceProtocol_t *)ALG_binaryFind(attr.code, BINARY_FIND(protocolTab));

  buf.lenght = 2;
  buf.payload = (u8_t *)noFind;
  if (pro != null) {
    i = MAX(attr.endAddr - attr.startAddr, ModbusDevice.monitorCount - 1);  // 如果为监控1台，此值为0
    APP_DEBUG("Device protocol code %x, %d - %d. \r\n", attr.code, attr.startAddr, i + 1);
    APP_DEBUG("protocol code %04x, type %04x\r\n", pro->code, pro->type);

    /* 地址列表中写入实际所需监控的设备地址 */
    for (*p++ = attr.startAddr; i > 0 && p < (u8_t *)(&addrTab + 1); i--, p++) {
      *p = *(p - 1) + 1;
    }

    if (MeterDevice.head != null) {
      *(int *)&ModbusDevice.monitorCount += 1;
    }

    buf.lenght = 2;
    if (pro->type == DEVICE_MOBUS) {
      ModbusDevice.head = (ModbusDeviceHead_t *)pro->protocol;
      ModbusDevice.cfg = memory_apply(sizeof(ST_UARTDCB));
      if (attr.cfg.baudrate == 0) {
        APP_DEBUG("ModbusDevice UART setting config is null\r\n");
        ModbusDevice.cfg->baudrate = UART_9600_N1.baudrate;
        ModbusDevice.cfg->dataBits = UART_9600_N1.dataBits;
        ModbusDevice.cfg->stopBits = UART_9600_N1.stopBits;
        ModbusDevice.cfg->parity = UART_9600_N1.parity;
        ModbusDevice.cfg->flowCtrl = UART_9600_N1.flowCtrl;
      } else {
        APP_DEBUG("ModbusDevice UART setting config is not null\r\n");
        ModbusDevice.cfg->baudrate = attr.cfg.baudrate;
        ModbusDevice.cfg->dataBits = attr.cfg.dataBits;
        ModbusDevice.cfg->stopBits = attr.cfg.stopBits;
        ModbusDevice.cfg->parity = attr.cfg.parity;
        ModbusDevice.cfg->flowCtrl = attr.cfg.flowCtrl;
      }
      APP_DEBUG("ModbusDevice UART baudrate = %ld\r\n", ModbusDevice.cfg->baudrate);
      buf.payload = (u8_t *)modbus_mode;
    } else if (pro->type == DEVICE_TRANS) {
      parametr_get(DEVICE_VENDER, &buf);
      ST_UARTDCB *Transdev_uartcfg = memory_apply(sizeof(ST_UARTDCB));
      if (attr.cfg.baudrate == 0) {
        APP_DEBUG("TransDevice UART setting config is null\r\n");
        Transdev_uartcfg->baudrate = UART_9600_N1.baudrate;
        Transdev_uartcfg->dataBits = UART_9600_N1.dataBits;
        Transdev_uartcfg->stopBits = UART_9600_N1.stopBits;
        Transdev_uartcfg->parity = UART_9600_N1.parity;
        Transdev_uartcfg->flowCtrl = UART_9600_N1.flowCtrl;
      } else {
        APP_DEBUG("TransDevice UART setting config is not null\r\n");
        Transdev_uartcfg->baudrate = attr.cfg.baudrate;
        Transdev_uartcfg->dataBits = attr.cfg.dataBits;
        Transdev_uartcfg->stopBits = attr.cfg.stopBits;
        Transdev_uartcfg->parity = attr.cfg.parity;
        Transdev_uartcfg->flowCtrl = attr.cfg.flowCtrl;
      }
      APP_DEBUG("TransDevice UART baudrate = %ld\r\n", Transdev_uartcfg->baudrate);
      TransDevice_init((char *)buf.payload, Transdev_uartcfg);
      memory_release(buf.payload);
      buf.payload = (u8_t *)trans_mode;
    }
  }

  parametr_set(32, &buf);
END:
  ModbusDevice_init();
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
int protocolAttrGet(u8_t num, ProtocolAttr_t *attr) {
  Buffer_t buf;
  int ret = -1;

  r_memset(&buf, 0, sizeof(Buffer_t));   // mike 20200828
  parametr_get(num, &buf);
  if (buf.payload != null && buf.lenght > 0) {
    char *str = NULL;
    str = memory_apply(sizeof(char) * 64);
    if (str == NULL) {
      APP_DEBUG("MEM Alloc Error\r\n");
      memory_release(buf.payload);
      return ret;
    }
    r_memset(str, 0, sizeof(str));
    r_strncpy(str, (char *)buf.payload, r_strlen((char *)buf.payload));
    ListHandler_t attrStr;
    r_strsplit(&attrStr, str, ',');
    APP_DEBUG("%d\r\n", attrStr.count);
    if (attrStr.count == 4) {   // str must fix XXX,X,X,XXXX mode
      attr->code = Swap_HexCharNum((char *) * (int *)attrStr.node->payload);
      attr->startAddr = Swap_charNum((char *) * (int *)attrStr.node->next->payload);
      attr->endAddr = Swap_charNum((char *) * (int *)attrStr.node->next->next->payload);

      ListHandler_t attr4Str;
      r_strsplit(&attr4Str, (char *) * (int *)attrStr.node->next->next->next->payload, '#');
      if (attr4Str.count == 3) {  // XXXX must fix #....# mode
        ListHandler_t cfgStr;
        r_strsplit(&cfgStr, (char *) * (int *)attr4Str.node->next->payload, '-');
        if (cfgStr.count == 4) {  // .... must fix xxxx-x-x-x mode
          Node_t *node = cfgStr.node;
          int tab[4] = {0, 0, 0, 0};
          int i = 0;
          do {
            tab[i++] = Swap_charNum((char *) * (int *)node->payload);
            APP_DEBUG("Band %d, %d.\r\n", i, tab[i - 1]);
            node = node->next;
          } while (node != cfgStr.node && i < 4);

          if ((tab[0] > 2000 && tab[0] < 1000000) //bandrate
              && (tab[1] > 4 && tab[1] < 9) //data bit
              && (tab[2] > 0 && tab[2] < 4)  //stop bit
              && (tab[3] >= 0 && tab[3] < 5)) {
            attr->cfg.baudrate = tab[0];
            attr->cfg.dataBits = tab[1];
            attr->cfg.stopBits = tab[2];
            attr->cfg.parity = tab[3];
            attr->cfg.flowCtrl = FC_NONE;
          } else {
            attr->cfg.baudrate = 0;
            attr->cfg.dataBits = 0;
            attr->cfg.stopBits = 0;
            attr->cfg.parity = 0;
            attr->cfg.flowCtrl = 0;
          }
        } else {    // reset all unfix setting to #9600-8-1-0#
          attr->cfg.baudrate = 0;
          attr->cfg.dataBits = 0;
          attr->cfg.stopBits = 0;
          attr->cfg.parity = 0;
          attr->cfg.flowCtrl = 0;
        }
        list_delete(&cfgStr);
      } else {
        attr->cfg.baudrate = 0;
        attr->cfg.dataBits = 0;
        attr->cfg.stopBits = 0;
        attr->cfg.parity = 0;
        attr->cfg.flowCtrl = 0;
      }
      list_delete(&attr4Str);
      ret = 0;
    } else if (attrStr.count == 1) {
      ret = -1;
    } else {
      attr->code = 0x2FF;
      attr->startAddr = 1;
      attr->endAddr = 1;
      attr->cfg.baudrate = 0;
      attr->cfg.dataBits = 0;
      attr->cfg.stopBits = 0;
      attr->cfg.parity = 0;
      attr->cfg.flowCtrl = 0;
      ret = 0;
    }

    if (attr->startAddr == 0) {
      attr->startAddr = 1;
    }

    if (attr->endAddr == 0 || attr->endAddr < attr->startAddr) {
      attr->endAddr = attr->startAddr;
    }
    list_delete(&attrStr);
  }

  memory_release(buf.payload);
  return ret;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
static int monitorCountGet(int *num) {
  Buffer_t buf;
  int ret = -1;

  *num = 0;
//  r_memset(&buf, 0, sizeof(Buffer_t));   // mike 20200828
  parametr_get(DEVICE_MONITOR_NUM, &buf);
  if (buf.payload != null && buf.lenght > 0) {
    *num = Swap_charNum((char *)buf.payload);
    if (*num == 0 || *num > 64) {
      *num = 1;
    }
    ret = 0;
  }

  memory_release(buf.payload);
  return ret;
}

/*******************************************************************************
  * @brief
  * @param  None
  * @retval None
*******************************************************************************/
void Protocol_clean(void) {
  ModbusDevice_clear();
  TransDevice_clear();
  if (ModbusDevice.cfg != NULL) {   // mike 20201228
    memory_release(ModbusDevice.cfg);
  }
  if (MeterDevice.cfg != NULL) {
    memory_release(MeterDevice.cfg);
  }
}

/******************************************************************************/

