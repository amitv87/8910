/********************** COPYRIGHT 2014-2100, eybond ***************************
 * @File    : eybpub_data_collector_parameter_table.c
 * @Author  :
 * @Date    : 2020-08-20
 * @Brief   :
 ******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_stdlib.h"
#include "ql_memory.h"
#endif

#include "eyblib_typedef.h"
#include "eyblib_r_stdlib.h"

#include "eybpub_data_collector_parameter_table.h"
#include "eybpub_struct_type.h"

/*****************************************************************************
                            内部函数
*****************************************************************************/
// 设置类型
static u8_t setflag(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setType(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setSerialNum(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setManufacturer(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 写 协议版本
static u8_t setProtocolVer(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 写 固件版本
static u8_t setSoftwareVer(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 写 硬件版本
static u8_t setHardwareVer(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setFactoryTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setApplication_Agreement(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 写 在线设备数
static u8_t setDevicesOnline(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setDevicesLimit(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setPollingCycle(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setAttribSet(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setLocalIPType(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setLocalIPAddr(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setLocalIPMask(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setLocalGateway(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setLocalDNS(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 写 本端端口号
static u8_t setLocalPort(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setServer1IPAddr(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setServer2IPAddr(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setServer3IPAddr(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setServerPort(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setSysTimeZone(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setSysTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 写 系统告警信息
static u8_t setSysWarning(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 写 系统故障信息
static u8_t setSysFault(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setSysOperate(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setTransmitMode(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setRecommunicateTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setSerialBaudRate(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setcreg(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setcgreg(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 写 编译日期
static u8_t setSoftCompileData(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 写 编译时间
static u8_t setSoftCompileTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 写 日志
static u8_t setrunlog(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 写 信号强度
static u8_t setCSQ(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen  = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 写 通信卡CCID
static u8_t setEthernetSerial(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 设置APN
static u8_t setAPN(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// IMEI CPUID
static u8_t setCPUID(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// Report Time
static u8_t setReportTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// ProdectKey
static u8_t setProdectKey(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// DevName
static u8_t setDevName(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// DevSecret
static u8_t setDevSecret(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// setPubTopic
static u8_t setPubTopic(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// setSubTopic
static u8_t setSubTopic(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setTotoalWorkTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setCurWorkTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setCollectorVersion(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setStateGridSN(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setStateGridUsername(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setStateGridPassword(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setStateGridRID(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

static u8_t setBuzzer(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(buf);
  r_memcpy(element->a, buf, r_strlen(buf));
  return 0;
}

// 读 类型
static u8_t getflag(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 类型
static u8_t getType(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 序列号
static u8_t getSerialNum(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 生产厂家
static u8_t getManufacturer(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 协议版本
static u8_t getProtocolVer(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 固件版本
static u8_t getSoftwareVer(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 硬件版本
static u8_t getHardwareVer(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 出厂时间
static u8_t getFactoryTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getApplication_Agreement(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 在线设备数
static u8_t getDevicesOnline(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 设备监控数量
static u8_t getDevicesLimit(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 设备轮询周期
static u8_t getPollingCycle(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 设备属性集合
static u8_t getAttribSet(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 本端IP分配方式
static u8_t getLocalIPType(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 本端IP地址
static u8_t getLocalIPAddr(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 本端子网掩码
static u8_t getLocalIPMask(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 本端默认网关
static u8_t getLocalGateway(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 本端DNS
static u8_t getLocalDNS(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 本端端口号
static u8_t getLocalPort(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 远端1的IP地址或域名
static u8_t getServer1IPAddr(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 远端2的IP地址或域名
static u8_t getServer2IPAddr(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 远端3的IP地址或域名
static u8_t getServer3IPAddr(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 远端端口号
static u8_t getServerPort(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 系统时区
static u8_t getSysTimeZone(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 系统时间
static u8_t getSysTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 系统告警信息
static u8_t getSysWarning(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 系统故障信息
static u8_t getSysFault(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 系统操作
static u8_t getSysOperate(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 数据传输模式
static u8_t getTransmitMode(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 通讯保护时间
static u8_t getRecommunicateTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 串口通讯波特率
static u8_t getSerialBaudRate(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getcreg(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getcgreg(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen       = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getSoftCompileData(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getSoftCompileTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 获取信号强度
static u8_t getrunlog(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 获取信号强度
static u8_t getCSQ(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读 通信卡CCID
static u8_t getEthernetSerial(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getAPN(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读
static u8_t getCPUID(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

// 读
static u8_t getReportTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getProdectKey(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getDevName(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getDevSecret(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getPubTopic(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getSubTopic(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getTotoalWorkTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getCurWorkTime(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getCollectorVersion(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getStateGridSN(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getStateGridUsername(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getStateGridPassword(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getStateGridRID(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

static u8_t getBuzzer(struct _ParamDescripTable_t *element, void *buf, u16_t *paramLen) {
  *paramLen = r_strlen(element->a);
  r_memcpy(buf, element->a, r_strlen(element->a));
  return 0;
}

#define JIN_LANG_COLL_VER  "3A010000"  // 锦浪采集器版本
const DefaultPara_List_t defaultPara[] = {  // ""选项恢复默认值时会忽略
  {0,   "",                       },
  // 出厂属性
  {1,   "",                       },         /* 类型 */
  {2,   "",                       },         /* 序列号 */
  {3,   "",                       },         /* 生产厂家 */
  {4,   "1.6",                    },         /* 协议版本 */
  {5,   __APPVERSION__,           },         /* 固件版本 */
  {6,   "6.0.0.0",                },         /* 硬件版本 */
  {7,   "",                       },         /* 出厂时间 */
  // 业务属性
  {8,   "",                       },         /* 应用协议 */
  // 9 保留
  // 10保留
  {11,  "1",                      },         /* 在线设备数 */
  {12,  "1",                      },         /* 设备监控数量 */
  {13,  "2000",                   },         /* 设备轮询周期 */
 // {14,  "02FF,1,1,#9600-8-1-0#",  },         /* 设备属性集合 */
  {14,  "02A9,1,1,#9600-8-1-0#",  },         /* 设备属性集合 */
  {15,  "1",                      },         /* 本端IP分配方式 */   /*电表协议*/
  {16,  "",                       },         /* 本端IP地址 */
  {17,  "",                       },         /* 本端子网掩码 */
  {18,  "",                       },         /* 本端默认网关 */
  {19,  "",                       },         /* 本端DNS */
  {20,  "",                       },         /* 本端端口号 */
  {21,  "www.shinemonitor.com",   },         /* 远端服务器1的IP地址或域名 */
  {22,  "",                       },         /* 远端服务器2的IP地址或域名 */
  {23,  "gfyfront.esgcc.com.cn:19020:SSL",                       },         /* 远端服务器3的IP地址或域名 */
  {24,  "502",                    },         /* 远端的端口号 */
  {25,  "20",                     },         /* 系统时区 */
  {26,  "",                       },         /* 系统时间 */
  {27,  "",                       },         /* 系统告警信息 */
  {28,  "",                       },         /* 系统故障信息 */
  {29,  "",                       },         /* 系统操作 */
  // 30保留
  // 31保留
  {32,  "",                       },         /* 数据传输模式 */
  {33,  "30",                     },         /* 通讯保护时间 */
  {34,  "",                       },         /* 串口通讯速率 */
  // 35-47
  {48,  "",                       },         /* Buzzer enable */
  {49,  "",                       },         /* 网络注册信息 */
  {50,  "",                       },         /* GPRS网络注册状态 */
  {51,  MAKE_DATE,                },         /* 固件编译日期 */
  {52,  MAKE_TIME2,               },         /* 固件编译时间 */
  // 53 运行设备类型及地址
  {54,  ""                        },         /* 运行日志 */
  {55,  "",                       },         /* 信号强度 */
  {56,  "",                       },         /* 通信卡CCID */
  {57,  "",                       },
  {58,  "",                       },
  {65,  "",                       },         /* 国网SN */
  {66,  "",                       },         /* 国网Username */
  {67,  "",                       },         /* 国网Password */
  {68,  "",                       },         /* 国网register ID */
  {82,  "300",                    },         /* 主动上报间隔时间 */
  // 阿里MQTT
  {92,  ""                        },         /*product_key*/
  {93,  ""                        },         /*device_name*/
  {94,  ""                        },         /*device_secret*/
  {95,  ""                        },         /*pub_topic*/
  {96,  ""                        },         /*sub_topic*/
  {97,  "0"                       },         /*total_working_time 累计工作时间*/
 // {98,  JIN_LANG_COLL_VER         },         /*锦浪采集器版本*/
 {98,  "0"         },         /*锦浪采集器版本*/
};

/* 数采器参数读写接口 */
struct _ParamDescripTable_t PDT[] = {
  {0,   "0",                       setflag,                    getflag},
  // 出厂属性
  {1,   "00",                      setType,                    getType},                 /* 类型 */
  {2,   "00000000000000",          setSerialNum,               getSerialNum },           /* 序列号 */
  {3,   "000",                     setManufacturer,            getManufacturer},         /* 生产厂家 */
  {4,   "1.6",                     setProtocolVer,             getProtocolVer},          /* 协议版本 */
  {5,   "6.0.0.0",                 setSoftwareVer,             getSoftwareVer},          /* 固件版本 */
  {6,   "6.0.0.0",                 setHardwareVer,             getHardwareVer},          /* 硬件版本 */
  {7,   "0000-00-00 00:00:00",     setFactoryTime,             getFactoryTime},          /* 出厂时间 */
  // 业务属性
  {8,   "0",                       setApplication_Agreement,   getApplication_Agreement},/* 应用协议 */
  // 9 保留
  // 10保留
  {11,  "1",                       setDevicesOnline,           getDevicesOnline},        /* 在线设备数 */
  {12,  "1",                       setDevicesLimit,            getDevicesLimit},         /* 设备监控数量 */
  {13,  "2000",                    setPollingCycle,            getPollingCycle},         /* 设备轮询周期 */
  {14,  "0206,1,1,#9600-8-1-0#",   setAttribSet,               getAttribSet},            /* 设备属性集合 */
  {15,  "1",                       setLocalIPType,             getLocalIPType},          /* 本端IP分配方式 */
  {16,  "0",                       setLocalIPAddr,             getLocalIPAddr},          /* 本端IP地址 */
  {17,  "0",                       setLocalIPMask,             getLocalIPMask},          /* 本端子网掩码 */
  {18,  "0",                       setLocalGateway,            getLocalGateway},         /* 本端默认网关 */
  {19,  "0",                       setLocalDNS,                getLocalDNS},             /* 本端DNS */
  {20,  "0",                       setLocalPort,               getLocalPort},            /* 本端端口号 */
  {21,  "www.shinemonitor.com",    setServer1IPAddr,           getServer1IPAddr},        /* 远端服务器1的IP地址或域名 */
  {22,  "0",                       setServer2IPAddr,           getServer2IPAddr},        /* 远端服务器2的IP地址或域名 */
  {23,  "gfyfront.esgcc.com.cn:19020:SSL",                       setServer3IPAddr,           getServer3IPAddr},        /* 远端服务器3的IP地址或域名 */
  {24,  "502",                     setServerPort,              getServerPort},           /* 远端的端口号 */
  {25,  "20",                      setSysTimeZone,             getSysTimeZone},          /* 系统时区 */
  {26,  "0000-00-00 00:00:00",     setSysTime,                 getSysTime},              /* 系统时间 */
  {27,  "0",                       setSysWarning,              getSysWarning},           /* 系统告警信息 */
  {28,  "0",                       setSysFault,                getSysFault},             /* 系统故障信息 */
  {29,  "0",                       setSysOperate,              getSysOperate},           /* 系统操作 */
  // 30保留
  // 31保留
  {32,  "1",                       setTransmitMode,            getTransmitMode},         /* 数据传输模式 */
  {33,  "30",                      setRecommunicateTime,       getRecommunicateTime},    /* 通讯保护时间 */
  {34,  "9600",                    setSerialBaudRate,          getSerialBaudRate},       /* 串口通讯速率 */
  // 35-47
  {48,  "0",                       setBuzzer,                  getBuzzer},                /* Buzzer enable */
  {49,  "00",                      setcreg,                    getcreg},                  /* 网络注册信息 */
  {50,  "00",                      setcgreg,                   getcgreg},                 /* GPRS网络注册状态 */
  {51,  "0",                       setSoftCompileData,         getSoftCompileData},       /* 固件编译日期 */
  {52,  "0",                       setSoftCompileTime,         getSoftCompileTime},       /* 固件编译时间 */
  // 53 运行设备类型及地址
  {54,  "0",                       setrunlog,                  getrunlog},
  {55,  "20",                      setCSQ,                     getCSQ},                   /* 信号强度 */
  {56,  "0",                       setEthernetSerial,          getEthernetSerial},        /* 通信卡CCID */
  {57,  "apn:user,pwd",            setAPN,                     getAPN},
  {58,  "0",                       setCPUID,                   getCPUID},
  // 国网
  {65,  "0",                       setStateGridSN,             getStateGridSN},           /* SN */
  {66,  "0",                       setStateGridUsername,       getStateGridUsername},     /* user name */
  {67,  "0",                       setStateGridPassword,       getStateGridPassword},     /* paddword  */
  {68,  "0",                       setStateGridRID,            getStateGridRID},          /* registeer ID */
  {82,  "300",                     setReportTime,              getReportTime},
  // 阿里MQTT
  {92,  "0",                       setProdectKey,              getProdectKey },         /*product_key*/
  {93,  "0",                       setDevName,                 getDevName    },         /*device_name*/
  {94,  "0",                       setDevSecret,               getDevSecret  },         /*device_secret*/
  {95,  "0",                       setPubTopic,                getPubTopic   },         /*pub_topic*/
  {96,  "0",                       setSubTopic,                getSubTopic   },         /*sub_topic*/
  {97,  "0",                       setTotoalWorkTime,          getTotoalWorkTime },     /*total_working_time 累计工作时间*/
  {98,  "00000000",                setCollectorVersion,        getCollectorVersion},    /*采集器版本*/
};
/*----------------------------------------------------------------------------*
**                             Global Vars                                    *
**----------------------------------------------------------------------------*/
int number_of_array_elements = sizeof(PDT) / sizeof(PDT[0]);
