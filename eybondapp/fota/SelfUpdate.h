/**********************    COPYRIGHT 2014-2100,  Eybond ************************ 
 * @File    : SelfUpdate.h
 * @Author  : CGQ
 * @Date    : 2017-12-01
 * @Brief   : 
 ******************************************************************************/
#ifndef __SELF_UPDATE_H__
#define __SELF_UPDATE_H__

#ifdef _PLATFORM_M26_
#include "ql_type.h"
#include "File.h"
#endif

#ifdef _PLATFORM_L610_
#include "eybpub_File.h"
#endif

int Update_Self(File_t *file);

#endif //__SELF_UPDATE_H__
/******************************************************************************/
