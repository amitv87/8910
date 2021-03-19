/**************Copyright(C) 2015-2026 Shenzhen Eybond Co., Ltd.***************
  *@brief   : eyblib_HashMD5.h
  *@notes   : 2020.08.04 MWY
*******************************************************************************/
#ifndef __EYBLIB_HASHMD5_H_
#define __EYBLIB_HASHMD5_H_

#include "eyblib_typedef.h"

/* Data structure for MD5 (Message-Digest) computation */
typedef struct {
  u32_t i[2];       /* number of _bits_ handled mod 2^64 */
  u32_t buf[4];     /* scratch buffer */
  u8_t  in[64];     /* input buffer */
  u8_t  digest[16]; /* actual digest after MD5Final call */
} MD5_t;

void Hash_MD5Init  ( MD5_t *mdContext);
void Hash_MD5Update( MD5_t *mdContext, u8_t *inBuf, u16_t inLen);
void Hash_MD5Final (u8_t hash[], MD5_t *mdContext);

#endif //__EYBLIB_HASHMD5_H_
