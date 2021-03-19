/**********************    COPYRIGHT 2014-2100, eybond  ************************ 
 * @File    : eybpub_Clock.h
 * @Author  : CGQ
 * @Date    : 2017-08-23
 * @Brief   : 
 ******************************************************************************/
#ifndef __EYBPUB_CLOCK_H
#define __EYBPUB_CLOCK_H

#include "eyblib_typedef.h"

#define CLOCK_MIN_YEAR              (2015)
#define CLOCK_MAX_YEAR              (2225)


typedef struct {
    u8_t   week;               // [0,6]
    u8_t   secs;                // [0,59]
    u8_t   min;                //[0,59]
    u8_t   hour;               //[0,23]
    u8_t   day;                // [1,31]
    u8_t   month;              //[1,12]
    u16_t  year;               // [cMinYear,cMaxYear] 
}Clock_t;

/*
typedef struct
{
    uint8_t sec;   ///< Second
    uint8_t min;   ///< Minute
    uint8_t hour;  ///< Hour
    uint8_t day;   ///< Day
    uint8_t month; ///< Month
    uint16_t year;  ///< Year
    uint8_t wDay;  ///< Week Day
} hal_rtc_time_t;
*/


void Clock_init(void);
void Clock_Add(void);
Clock_t *Clock_get(void);
void Clock_Set(Clock_t *str);  
void Clock_timeZone(Clock_t * time);
void Clock_save(void);
u8_t runTimeCheck(u8_t start, u8_t end);

void get_GMT(Clock_t * time,u8_t dir);

#endif //__EYBPUB_CLOCK_H
/******************************************************************************/

