#include "data_collector_parameter_table.h"
#include "struct_type.h"
#include "string.h"
#include "elog.h"
#include "net_task.h"

/*****************************************************************************
                            内部函数
*****************************************************************************/
//设置类型
static INT8 setflag(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setType(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setSerialNum(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setManufacturer(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//写 协议版本
static INT8 setProtocolVer(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//写 固件版本
static INT8 setSoftwareVer(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//写 硬件版本
static INT8 setHardwareVer(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setFactoryTime(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setApplication_Agreement(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//读 在线设备数
static INT8 setDevicesOnline(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setDevicesLimit(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setPollingCycle(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setAttribSet(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setLocalIPType(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setLocalIPAddr(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setLocalIPMask(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setLocalGateway(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setLocalDNS(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//写 本端端口号
static INT8 setLocalPort(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setServer1IPAddr(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setServer2IPAddr(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setServer3IPAddr(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setServerPort(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setSysTimeZone(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setSysTime(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//写 系统告警信息
static INT8 setSysWarning(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//写 系统故障信息
static INT8 setSysFault(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setSysOperate(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setTransmitMode(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setRecommunicateTime(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setSerialBaudRate(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
 
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setcreg(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setcgreg(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setSoftCompileData(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

static INT8 setSoftCompileTime(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}


//写 日志
static INT8 setrunlog(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//写 信号强度
static INT8 setCSQ(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//写 通信卡CCID
static INT8 setEthernetSerial(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//设置APN
static INT8 setAPN(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}


//IMEI CPUID
static INT8 setCPUID(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//写 国网SN 
static INT8 setStateGridSN(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//写 国网 user name
static INT8 setStateGridUserName(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//写 国网SN 
static INT8 setStateGridPassword(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}

//写 国网SN 
static INT8 setStateGridRegisterID(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(buf);
    memcpy(element->a,buf,strlen(buf));
    return 0;
}




//读 类型
static INT8 getflag(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 类型
static INT8 getType(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 序列号
static INT8 getSerialNum(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 生产厂家
static INT8 getManufacturer(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 协议版本
static INT8 getProtocolVer(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 固件版本
static INT8 getSoftwareVer(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 硬件版本
static INT8 getHardwareVer(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 出厂时间
static INT8 getFactoryTime(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

static INT8 getApplication_Agreement(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 在线设备数
static INT8 getDevicesOnline(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 设备监控数量
static INT8 getDevicesLimit(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 设备轮询周期
static INT8 getPollingCycle(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 设备属性集合
static INT8 getAttribSet(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 本端IP分配方式
static INT8 getLocalIPType(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 本端IP地址
static INT8 getLocalIPAddr(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 本端子网掩码
static INT8 getLocalIPMask(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 本端默认网关
static INT8 getLocalGateway(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 本端DNS
static INT8 getLocalDNS(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 本端端口号
static INT8 getLocalPort(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 远端1的IP地址或域名
static INT8 getServer1IPAddr(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 远端2的IP地址或域名
static INT8 getServer2IPAddr(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 远端3的IP地址或域名
static INT8 getServer3IPAddr(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 远端端口号
static INT8 getServerPort(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 系统时区
static INT8 getSysTimeZone(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 系统时间
static INT8 getSysTime(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 系统告警信息
static INT8 getSysWarning(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 系统故障信息
static INT8 getSysFault(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 系统操作
static INT8 getSysOperate(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 数据传输模式
static INT8 getTransmitMode(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 通讯保护时间
static INT8 getRecommunicateTime(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 串口通讯波特率
static INT8 getSerialBaudRate(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

static INT8 getcreg(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

static INT8 getcgreg(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

static INT8 getSoftCompileData(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

static INT8 getSoftCompileTime(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//获取信号强度
static INT8 getrunlog(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//获取信号强度
static INT8 getCSQ(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 通信卡CCID
static INT8 getEthernetSerial(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

static INT8 getAPN(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 
static INT8 getCPUID(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 
static INT8 getStateGridSN(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 
static INT8 getStateGridUserName(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 
static INT8 getStateGridPassword(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}

//读 
static INT8 getStateGridRegisterID(struct _ParamDescripTable_t* element,void *buf, UINT16 *paramLen)
{
    *paramLen       = strlen(element->a);
    memcpy(buf,element->a,strlen(element->a));
    return 0;
}


/* 数采器参数读写接口 */
struct _ParamDescripTable_t PDT[] =
{ 
    {0,   "0",                       setflag,                    getflag},
    //出厂属性
    {1,   "00",                      setType,                    getType},                 /* 类型 */    
    {2,   "00000000000000",          setSerialNum,               getSerialNum },           /* 序列号 */
    {3,   "000",                     setManufacturer,            getManufacturer},         /* 生产厂家 */
    {4,   "1.6",                     setProtocolVer,             getProtocolVer},          /* 协议版本 */
    {5,   "6.0.0.0",                 setSoftwareVer,             getSoftwareVer},          /* 固件版本 */
    {6,   "6.0.0.0",                 setHardwareVer,             getHardwareVer},          /* 硬件版本 */
    {7,   "0000-00-00 00:00:00",     setFactoryTime,             getFactoryTime},          /* 出厂时间 */
    //业务属性
    {8,   "A5",                      setApplication_Agreement,   getApplication_Agreement},/* 应用协议 */
    //9 保留
    //10保留
    {11,  "0",                       setDevicesOnline,           getDevicesOnline},        /* 在线设备数 */
    {12,  "0",                       setDevicesLimit,            getDevicesLimit},         /* 设备监控数量 */
    {13,  "2000",                    setPollingCycle,            getPollingCycle},         /* 设备轮询周期 */
    {14,  "02FF,1,1,#0#",            setAttribSet,               getAttribSet},            /* 设备属性集合 */
    {15,  "0",                       setLocalIPType,             getLocalIPType},          /* 本端IP分配方式 */
    {16,  "0",                       setLocalIPAddr,             getLocalIPAddr},          /* 本端IP地址 */
    {17,  "0",                       setLocalIPMask,             getLocalIPMask},          /* 本端子网掩码 */
    {18,  "0",                       setLocalGateway,            getLocalGateway},         /* 本端默认网关 */
    {19,  "0",                       setLocalDNS,                getLocalDNS},             /* 本端DNS */
    {20,  "0",                       setLocalPort,               getLocalPort},            /* 本端端口号 */
    {21,  "www.shinemonitor.com",    setServer1IPAddr,           getServer1IPAddr},        /* 远端服务器1的IP地址或域名 */
    {22,  "0",                       setServer2IPAddr,           getServer2IPAddr},        /* 远端服务器2的IP地址或域名 */
    {23,  "0",                       setServer3IPAddr,           getServer3IPAddr},        /* 远端服务器3的IP地址或域名 */
    {24,  "502",                     setServerPort,              getServerPort},           /* 远端的端口号 */
    {25,  "20",                      setSysTimeZone,             getSysTimeZone},          /* 系统时区 */
    {26,  "0000-00-00 00:00:00",     setSysTime,                 getSysTime},              /* 系统时间 */
    {27,  "0",                       setSysWarning,              getSysWarning},           /* 系统告警信息 */
    {28,  "0",                       setSysFault,                getSysFault},             /* 系统故障信息 */
    {29,  "0",                       setSysOperate,              getSysOperate},           /* 系统操作 */
    //30保留
    //31保留
    {32,  "1",                       setTransmitMode,            getTransmitMode},         /* 数据传输模式 */
    {33,  "30",                      setRecommunicateTime,       getRecommunicateTime},    /* 通讯保护时间 */
    {34,  "9600-8-1-0",              setSerialBaudRate,          getSerialBaudRate},       /* 串口通讯速率 */
    //35-48
    {49,  "00",                      setcreg,                    getcreg},                  /* 网络注册信息 */
    {50,  "00",                      setcgreg,                   getcgreg},                 /* GPRS网络注册状态 */
    {51,  "0",                       setSoftCompileData,         getSoftCompileData},       /* 固件编译日期 */
    {52,  "0",                       setSoftCompileTime,         getSoftCompileTime},       /* 固件编译时间 */
    //53    运行设备类型及地址
    {54,  "0",                       setrunlog,                  getrunlog},
    {55,  "20",                      setCSQ,                     getCSQ},                   /* 信号强度 */
    {56,  "0",                       setEthernetSerial,          getEthernetSerial},        /* 通信卡CCID */
    {57,  "SYNIVERSE",               setAPN,                     getAPN},
    {58,  "0",                       setCPUID,                   getCPUID},
    //以下为国网参数
    {65,  "1000000000228877",                       setStateGridSN,             getStateGridSN},            /*国网SN*/
    {66,  "pvcloud",                       setStateGridUserName,       getStateGridUserName},      /*国网USER NAME*/
    {67,  "pvcloud",                       setStateGridPassword,       getStateGridPassword},      /*国网PASSWORD*/
    {68,  "1468125152",                       setStateGridRegisterID,     getStateGridRegisterID}    /*国网REGISTER ID*/


};

/*----------------------------------------------------------------------------*
**                             Global Vars                                    *
**----------------------------------------------------------------------------*/
int number_of_array_elements = sizeof(PDT)/sizeof(PDT[0]);