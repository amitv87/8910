/**********************    COPYRIGHT 2014-2100, eybond ************************
 * @File    : eybpub_UnixTime.c
 * @Author  : CGQ
 * @Date    : 2018-11-01
 * @Brief   :
 ******************************************************************************/
#include "eyblib_r_stdlib.h"
#include "eybpub_UnixTime.h"
#include "eybpub_Clock.h"
#include "eybpub_Debug.h"

enum TIME_DEF {
  SEC = 1,
  MIN = SEC * 60,
  HOUR = MIN * 60,
  DAY = HOUR * 24,
  YEAR = DAY * 365,
};

static u16_t mon_yday[2][12] = {
  {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
  {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
};

static int isleap(int year) {
  return (year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0);
}

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/
/*
void UnixTime_get(u8_t *time) {
  int i;
  unsigned long secs;
  u32_t msecH;
  u32_t msecL;

  Clock_t *clock = Clock_get();

  // 以平年时间计算的秒数
  secs = (clock->year - 1970) * YEAR
         + (mon_yday[isleap(clock->year)][clock->month - 1] + clock->day - 1) * DAY
         + clock->hour * HOUR + clock->min * MIN +  clock->secs;
  // 加上闰年的秒数
  for (int i = 1970; i < clock->year; i++) {
    if (isleap(i)) {
      secs += DAY;
    }
  }


  msecH = secs >> 22;
  msecL = secs << 10;

  for (i = 0; i < 24; i++) {
    if (msecL < secs) {
      msecH--;
    }
    msecL -= secs;
  }
  r_memcpy(time, &msecL, sizeof(msecL));
  r_memcpy(&time[4], &msecH, 4);
}
*/

/*******************************************************************************
  * @note   None
  * @param  None
  * @retval None
*******************************************************************************/

void UnixTime_get(u8_t *time)
{
	int i;
	unsigned long secs;
	u32_t msecH;
	u32_t msecL;

/*
	Clock_t *clock;
	Clock_t *clock_bj = Clock_get();
	clock->year=clock_bj->year;
	clock->month=clock_bj->month;
	clock->day=clock_bj->day;
	clock->hour=clock_bj->hour;
	clock->min=clock_bj->min;
	clock->secs=clock_bj->secs;
	log_d("Clock_get() clock is :%04d-%02d-%02d %02d:%02d:%02d\r\n",clock->year,clock->month,clock->day,clock->hour,clock->min,clock->secs);
	get_GMT(clock);			//得到格林威治时间，Luee新加
*/

	Clock_t *clock = Clock_get();	
	//APP_DEBUG("\r\nBJclock is :%04d-%02d-%02d %02d:%02d:%02d\r\n",clock->year,clock->month,clock->day,clock->hour,clock->min,clock->secs);
	//get_GMT(clock,1);			//得到格林威治时间，Luee新加
	APP_DEBUG("\r\n GMT clock is :%04d-%02d-%02d %02d:%02d:%02d\r\n",clock->year,clock->month,clock->day,clock->hour,clock->min,clock->secs);
	// 以平年时间计算的秒数
	secs = (clock->year - 1970) * YEAR 
			 + (mon_yday[isleap(clock->year)][clock->month - 1] + clock->day - 1) * DAY
			 + clock->hour * HOUR + clock->min * MIN +  clock->secs;
	// 加上闰年的秒数
	for(int i=1970; i < clock->year; i++)
	{
		if(isleap(i))
		{
			secs += DAY;
		}
	}
	
	/*x * 1000 = x * 1024(x<<10) - x* 24*/

	msecH = secs>>22;
	msecL = secs<<10;

	for (i = 0; i < 24; i++)
	{
		if (msecL < secs)
		{
			msecH--;
		}
		msecL -= secs;
	}
	r_memcpy(time, &msecL, sizeof(msecL));
	r_memcpy(&time[4], &msecH, 4);
	//get_GMT(clock,0);	//将GMT转为本地时间
	//APP_DEBUG("\r\n BJ2GMT clock is :%04d-%02d-%02d %02d:%02d:%02d\r\n",clock->year,clock->month,clock->day,clock->hour,clock->min,clock->secs);
}


/******************************************************************************/
