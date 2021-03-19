/**********************    COPYRIGHT 2014-2100,  Eybond ************************
 * @File    : eyblib_r_stdlib.h
 * @Author  : CGQ
 * @Date    : 2015-06-20 Unarty establish
 * @Brief   :
 ******************************************************************************/
#ifndef __EYBLIB_R_STDLIB_H_
#define __EYBLIB_R_STDLIB_H_

#include "eyblib_typedef.h"

void *r_memcpy(void *trg, const void *src, u32_t num);  // 内存考备
u32_t r_memcmp(void *trg,  const void *src, u32_t num);  // 内存比较
void *r_memmove(void *trg,  const void *src, u32_t num);  // 内存移动
void r_memset(void *trg, u8_t const val, u32_t num);  // 内存设置
void *r_memfind(void *trg, u32_t trgLen, const void *src, u32_t srcLen);

u32_t r_strlen(const char *src);
// u32_t r_strcpy(char *trg, const char *src);
char *r_strcpy(char *trg, const char *src);
u32_t r_strcpy_len(char *trg, const char *src);
char *r_strncpy(char *trg, const char *src, u32_t srcLen);
u32_t r_stradd(char *trg, const char *src);
char *r_strcat(char *trg, const char *src);
char *r_strncat(char *trg, const char *src, u32_t srcLen);
u32_t r_strcmp(const char *trg, const char *src);
u32_t r_strncmp(const char *trg, const char *src, u32_t srcLen); // 定长字符串比较
u32_t r_strcmpformat(const char *trg, const char *format);  //**:**:**
int r_strfind(const char *trg, const char *src);
char *r_strstr(const char *trg, const char *src);   // 字符串查找
void r_strSub(char *trg, const char *src, char c);

u16_t r_htons(u16_t n);
u32_t r_htonl(u32_t n);
#endif //__EYBLIB_R_STDLIB_H_
/*******************************Copyright Eybond*********************************/

