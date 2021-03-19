/**********************    COPYRIGHT 2014-2100,  Eybond ************************
 * @File    : eyblib_typedef.h
 * @Author  : CGQ
 * @Date    : 2015-06-28
              2016.04.10 Unarty Change User 8/32bit MCU
 * @Brief   :
 ******************************************************************************/
#ifndef __EYBLIB_TYPEDEF_H_
#define __EYBLIB_TYPEDEF_H_

#ifndef null
#define null        ((void*)0)
#endif
#ifndef NULL
#define NULL        ((void*)0)
#endif

#ifndef _PLATFORM_L610_  // 与L610 SDK冲突
#define MAX(a, b)     ((a) > (b) ? (a) : (b))
#define MIN(a, b)     ((a) > (b) ? (b) : (a))
#endif
#define ABS(a, b)     (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))

#define SIZE(n)         (sizeof(n)/sizeof(n[0]))

/*  */
#define ERRR(conf, ret)     do\
                            {\
                                if (conf)\
                                {\
                                    ret;\
                                }\
                            } while(0)

typedef signed long long   s64_t;
typedef signed long  s32_t;
typedef signed short s16_t;
typedef signed char  s8_t;

typedef const signed long  sc32_t;  /*!< Read Only */
typedef const signed short sc16_t;  /*!< Read Only */
typedef const signed char  sc8_t;   /*!< Read Only */

typedef  volatile signed long   vs32_t;
typedef  volatile signed short  vs16_t;
typedef  volatile signed char   vs8_t;

typedef volatile const signed long  vsc32_t;  /*!< Read Only */
typedef volatile const signed short vsc16_t;  /*!< Read Only */
typedef volatile const signed char  vsc8_t;   /*!< Read Only */

typedef unsigned long long u64_t;
typedef unsigned long  u32_t;
typedef unsigned short u16_t;
typedef unsigned char  u8_t;

typedef const unsigned long  uc32_t;  /*!< Read Only */
typedef const unsigned short uc16_t;  /*!< Read Only */
typedef const unsigned char  uc8_t;   /*!< Read Only */

typedef volatile unsigned long  vu32_t;
typedef volatile unsigned short vu16_t;
typedef volatile unsigned char  vu8_t;

typedef volatile const unsigned long vuc32_t;  /*!< Read Only */
typedef volatile const unsigned long vuc16_t;  /*!< Read Only */
typedef volatile const unsigned long vuc8_t;   /*!< Read Only */

typedef struct {
  u16_t size;
  u16_t lenght;
  u8_t *payload;
} Buffer_t;

typedef void (*void_fun_void)(void);
typedef u8_t (*u8_fun_void)(void);
typedef u16_t (*u16_fun_void)(void);
typedef void (*void_fun_u8)(u8_t val);
typedef void (*void_fun_u8p_u16)(u8_t *val, u16_t len);
typedef void (*void_fun_bufp)(Buffer_t *buf);

typedef void (*void_fun_u32_u8p_u32)(u32_t src, u8_t *data, u32_t len);

#define mcu_t       u32_t   //mcu
#define TEST_RF   (1)

#endif //__EYBLIB_TYPEDEF_H_
/******************************************************************************/

