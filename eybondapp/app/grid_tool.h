/******************************************************************************           
* name:             grid_tool.h           
* introduce:        头文件
* author:           Luee                                     
******************************************************************************/ 
#ifndef _GRID_TOOL_H_
#define _GRID_TOOL_H_

#include "eyblib_typedef.h"
#include "eyblib_memory.h"
#include "fibo_opencpu.h"
#include "eyblib_r_stdlib.h"
#include "eyblib_swap.h"
#include "eybpub_utility.h"
#include "eybpub_Debug.h"
#include "eybpub_SysPara_File.h"
#include "eybpub_data_collector_parameter_table.h"
#include "eybpub_parameter_number_and_value.h"

typedef signed long long   s64;                            
typedef signed long  s32;
typedef signed short s16;
typedef signed char  s8;

typedef const signed long  sc32;  /*!< Read Only */
typedef const signed short sc16;  /*!< Read Only */
typedef const signed char  sc8;   /*!< Read Only */

typedef  volatile signed long   vs32;
typedef  volatile signed short  vs16;
typedef  volatile signed char   vs8;

typedef volatile const signed long  vsc32;  /*!< Read Only */
typedef volatile const signed short vsc16;  /*!< Read Only */
typedef volatile const signed char  vsc8;   /*!< Read Only */

typedef unsigned long long u64;
typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;

typedef const unsigned long  uc32;  /*!< Read Only */
typedef const unsigned short uc16;  /*!< Read Only */
typedef const unsigned char  uc8;   /*!< Read Only */

typedef volatile unsigned long  vu32;
typedef volatile unsigned short vu16;
typedef volatile unsigned char  vu8;

typedef volatile const unsigned long vuc32;  /*!< Read Only */
typedef volatile const unsigned long vuc16;  /*!< Read Only */
typedef volatile const unsigned long vuc8;   /*!< Read Only */

#ifndef bool
#define bool    u8
#endif

//#define memory_release  fibo_free
//#define memory_apply    fibo_malloc

#define log_d APP_DEBUG

void print_buf(u8_t *buf,u16_t lenght);
void SysPara_Get(u8_t num, Buffer_t *databuf);
int SysPara_Set(u8_t num, Buffer_t *buf);

#endif /* _GRID_TOOL_H_ */