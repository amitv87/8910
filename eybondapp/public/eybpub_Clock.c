/**********************    COPYRIGHT 2014-2100, eybond    ************************ 
 * @File    : eybpub_Clock.c
 * @Author  : CGQ
 * @Date    : 2017-08-17
 * @Brief   : 
 ******************************************************************************/
#ifdef _PLATFORM_BC25_
#include "ql_time.h"
#include "ql_stdlib.h"
#include "ql_memory.h"
#endif

#ifdef _PLATFORM_L610_
#include "fibo_opencpu.h"
#endif

#include "eyblib_swap.h"
// #include "FlashEquilibria.h"	//mike 20200805
#include "eyblib_r_stdlib.h" // mike 20200828
#include "eyblib_memory.h"   // mike 20200828
#include "eybpub_Debug.h"
#include "eybpub_SysPara_File.h"   // mike 20200826
#include "eybpub_run_log.h"
#include "eybpub_Clock.h"

#include "grid_tool.h"

// #include "FlashHard.h"		//mike 20200805
// #define CLOCK_FALSH_ADDR            (FLASH_SYSPARA_ADDR + FLASH_SYSPARA_SIZE) 
// #define CLOCK_FALSH_SIZE            (0x2000)

//static FlashEquilibria_t  clockHead;	//mike 20200805
static Clock_t  local_clock;

static u8_t leapYear(u16_t year);
static void week(void);

#ifdef _PLATFORM_BC25_
/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
void Clock_init(void)
{
/*    FlashEquilibria_init(&clockHead, CLOCK_FALSH_ADDR, CLOCK_FALSH_SIZE, sizeof(Clock_t), &clock);		//mike 20200826
  if (clock.year < CLOCK_MIN_YEAR || clock.year > CLOCK_MAX_YEAR) {
    clock.year = 2015;
    clock.month = 1;
    clock.day = 1;
    clock.hour = 0;
    clock.min = 0;
    clock.secs = 0;
    week();
    Clock_save();
    log_save("System clock init...");
  }*/

  ST_Time time;
  if ((Ql_GetRealLocalTime(&time)) != NULL) {
    local_clock.year = time.year;
    local_clock.month = time.month;
    local_clock.day = time.day;
    local_clock.hour = time.hour;
    local_clock.min = time.minute;
    local_clock.secs = time.second;
  } else {
    local_clock.year = 2015;
    local_clock.month = 1;
    local_clock.day = 1;
    local_clock.hour = 0;
    local_clock.min = 0;
    local_clock.secs = 0;    
  }

  week();
  Clock_save();
  APP_DEBUG("System clock init(%d.%02d.%02d %02d:%02d:%02d timezone=%02d)\r\n",
    local_clock.year, local_clock.month, local_clock.day, local_clock.hour, local_clock.min, local_clock.secs, time.timezone);
  log_save("System clock init...");
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
 *******************************************************************************/
void Clock_timeZone(Clock_t * time)
{
	Buffer_t buf;
    r_memset(&buf, 0, sizeof(Buffer_t));    // mike 20200828
    ST_Time st_time;
    if ((Ql_GetRealLocalTime(&st_time)) != NULL) {
      if (st_time.year >= 2020) {   // BC25不返回2017年的时间就判定是实际时间
        local_clock.year = st_time.year;
        local_clock.month = st_time.month;
        local_clock.day = st_time.day;
        local_clock.hour = st_time.hour;
        local_clock.min = st_time.minute;
        local_clock.secs = st_time.second;
      }
    } else {
      local_clock.year = 2015;
      local_clock.month = 1;
      local_clock.day = 1;
      local_clock.hour = 0;
      local_clock.min = 0;
      local_clock.secs = 0;
    }
    r_memcpy(time, &local_clock, sizeof(Clock_t));
//	SysPara_Get(TIME_ZONE_ADDR, &buf);
    parametr_get(TIME_ZONE_ADDR, &buf);

    if (buf.payload != NULL && buf.lenght > 0) {
      int timeZone = 0;
      timeZone = Swap_charNum((char *)buf.payload);
      if (timeZone >= 0 && timeZone <= 24) {
        timeZone -= 12;
      }
    }
/*	if (buf.payload != null && buf.lenght > 0)
	{
		int timeZone = 0;
		timeZone = Swap_charNum((char*)buf.payload);
		if (timeZone >= 0 && timeZone <= 24)
		{
			int hour = time->hour;;
			timeZone -= 12;
			hour += timeZone;

			if (hour < 0)
			{
				hour += 24;
				if (time->day == 1)
				{
					if (time->month == 1)
					{
						time->year--;
						time->month = 12;
						time->day = 31;
					}
					else 
					{
						time->month--;
						if (time->month == 2)
						{
							if (0 == leapYear(time->year))
							{
								time->day = 29;
							}
							else
							{
								time->day = 28;
							}
						}
						else if (((time->month < 8) && ((time->month&0x01) == 0x01))
									|| ((time->month > 7)&& ((time->month&0x01) == 0x00))
									)
						{
							
							time->day = 31;
						}
						else 
						{
							time->day = 30;
						}
					}
				}
			}
			else if (hour > 23)
			{
				hour -= 24;
				if ((time->day < 28) 
					|| ((time->month != 2) 
                    	&& (time->day < (30 + (time->month > 7 ? (time->month - 7)&0x01 : time->month &0x01))))
              		|| (time->day < 29 && 0 == leapYear(time->year))
	            )
	   			{
					time->day++;
				}
				else
				{
					time->day = 1;
					if (time->month < 12)
					{
						time->month++;
					}
					else
					{
						time->year++;
						time->month = 1;
					}
				}
			}
			time->hour = hour;
		}
	} */
	memory_release(buf.payload);
    buf.lenght = 0;
    buf.size = 0;
}
/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
void Clock_Set(Clock_t *clk)
{
    if (clk->year < CLOCK_MIN_YEAR || clk->year > CLOCK_MAX_YEAR
        || clk->month > 12 || clk->day > 31 || clk->hour > 24)
    {
        return;
    }
    //FlashEquilibria_write(&clockHead, &clock);    // mike 20200805
    r_memcpy(&local_clock, clk, sizeof(Clock_t));
    week();
}
#endif

#ifdef _PLATFORM_L610_
void Clock_init(void) {
  hal_rtc_time_t time;
  s32_t ret = 0;
  ret = fibo_getRTC(&time);
  if (ret < 0) {
    time.year = 2015;
    time.month = 1;
    time.day = 1;
    time.hour = 0;
    time.min = 0;
    time.sec = 0;
    time.wDay = 4;
  }

  local_clock.year = time.year;
  local_clock.month = time.month;
  local_clock.day = time.day;
  local_clock.hour = time.hour;
  local_clock.min = time.min;
  local_clock.secs = time.sec;

  week();
  Clock_save();
  APP_DEBUG("System clock init(%d.%02d.%02d %02d:%02d:%02d week:%02d)\r\n",
    local_clock.year, local_clock.month, local_clock.day, local_clock.hour, local_clock.min, local_clock.secs, local_clock.week);
  log_save("System clock init...");
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
 *******************************************************************************/
/*
void Clock_timeZone(Clock_t * time) {
  Buffer_t buf;
  r_memset(&buf, 0, sizeof(Buffer_t));    // mike 20200828
  hal_rtc_time_t st_time;
  s32_t ret = 0;
  ret = fibo_getRTC(&st_time);
  if (ret >= 0) {
    if (st_time.year >= 2020) {   // BC25不返回2017年的时间就判定是实际时间
      local_clock.year = st_time.year;
      local_clock.month = st_time.month;
      local_clock.day = st_time.day;
      local_clock.hour = st_time.hour;
      local_clock.min = st_time.min;
      local_clock.secs = st_time.sec;
    }
  } else {
      local_clock.year = 2015;
      local_clock.month = 1;
      local_clock.day = 1;
      local_clock.hour = 0;
      local_clock.min = 0;
      local_clock.secs = 0;
  }
  r_memcpy(time, &local_clock, sizeof(Clock_t));
  parametr_get(TIME_ZONE_ADDR, &buf);

  if (buf.payload != NULL && buf.lenght > 0) {
    int timeZone = 0;
    timeZone = Swap_charNum((char *)buf.payload);
    if (timeZone >= 0 && timeZone <= 24) {
      timeZone -= 12;
    }
  }

  memory_release(buf.payload);
  buf.lenght = 0;
  buf.size = 0;
}
*/


void Clock_timeZone(Clock_t * time)
{
	Buffer_t buf;

	r_memcpy(time, &local_clock, sizeof(Clock_t));
	SysPara_Get(TIME_ZONE_ADDR, &buf);
	if (buf.payload != null && buf.lenght > 0)
	{
		int timeZone;
		timeZone = Swap_charNum((char*)buf.payload);
		if (timeZone >= 0 && timeZone <= 24)
		{
			int hour = time->hour;;
			timeZone -= 12;
			hour += timeZone;

			if (hour < 0)
			{
				hour += 24;
				if (time->day == 1)
				{
					if (time->month == 1)
					{
						time->year--;
						time->month = 12;
						time->day = 31;
					}
					else 
					{
						time->month--;
						if (time->month == 2)
						{
							if (0 == leapYear(time->year))
							{
								time->day = 29;
							}
							else
							{
								time->day = 28;
							}
						}
						else if (((time->month < 8) && ((time->month&0x01) == 0x01))
									|| ((time->month > 7)&& ((time->month&0x01) == 0x00))
									)
						{
							
							time->day = 31;
						}
						else 
						{
							time->day = 30;
						}
					}
				}
			}
			else if (hour > 23)
			{
				hour -= 24;
				if ((time->day < 28) 
					|| ((time->month != 2) 
                    	&& (time->day < (30 + (time->month > 7 ? (time->month - 7)&0x01 : time->month &0x01))))
              		|| (time->day < 29 && 0 == leapYear(time->year))
	            )
	   			{
					time->day++;
				}
				else
				{
					time->day = 1;
					if (time->month < 12)
					{
						time->month++;
					}
					else
					{
						time->year++;
						time->month = 1;
					}
				}
			}
			time->hour = hour;
		}
	}
	memory_release(buf.payload);
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
void Clock_Set(Clock_t *clk)
{
	//从益邦云得到是两位数年GMT如：(21.01.22 06:03:27)
	APP_DEBUG("\r\n-->gmt_clock(%04d.%02d.%02d %02d:%02d:%02d)\r\n",
      clk->year, clk->month, clk->day, clk->hour, clk->min, clk->secs);
	clk->year+=2000; 
	r_memcpy(&local_clock, clk, sizeof(Clock_t));		//local_clock用于发国网，国网要的是UNIX时钟，由4位数年GMT转换,如：(2021.01.22 06:03:27)
	week();
	APP_DEBUG("-->local_clock(%04d.%02d.%02d %02d:%02d:%02d week:%02d)\r\n",
      local_clock.year, local_clock.month, local_clock.day, local_clock.hour, local_clock.min, local_clock.secs, local_clock.week);
 
    if (clk->year < CLOCK_MIN_YEAR || clk->year > CLOCK_MAX_YEAR
        || clk->month > 12 || clk->day > 31 || clk->hour > 24)
    {
        return;
    }
    //FlashEquilibria_write(&clockHead, &clock);    // mike 20200805
    //r_memcpy(&local_clock, clk, sizeof(Clock_t));
    //week();
	
	
	//get_GMT(clk,0);    //gmt-->bj
	
	Clock_timeZone(clk);

    hal_rtc_time_t current = {
		.year 		= clk->year,
		.month 		= clk->month,
		.day 		= clk->day,
		.hour		= clk->hour,
		.min	    = clk->min,
		.sec		= clk->secs,
	};
	APP_DEBUG("\r\n-->g2b_clock(%04d.%02d.%02d %02d:%02d:%02d)\r\n",
      current.year, current.month, current.day, current.hour, current.min, current.sec);    

    s32_t current_flag = fibo_setRTC(&current);             //设置本地时间 参数：time 时间结构体
    APP_DEBUG("current_flag is %ld\r\n",current_flag);
    hal_rtc_time_t local_time;
    s32_t local_time_flag = fibo_getRTC(&local_time);       //获取本地时间
    APP_DEBUG("local_time_flag is %ld\r\n",local_time_flag);
	local_time.year+=2000;
    char current_char[30]={0};
    snprintf(current_char, 30, "%04d-%02d-%02d %02d:%02d:%02d",current.year,current.month,current.day,current.hour,current.min,current.sec);
    Buffer_t buf;
    buf.size = 64;
    buf.lenght = r_strlen(current_char);
    buf.payload = (u8_t *)current_char;
    parametr_set(LOCAL_TIME, &buf);    
    APP_DEBUG("Clock_Sett(%04d.%02d.%02d %02d:%02d:%02d week:%02d)\r\n",
      local_time.year, local_time.month, local_time.day, local_time.hour, local_time.min, local_time.sec, local_time.wDay);
}
#endif

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/

Clock_t *Clock_get(void)
{
    return &local_clock;
}

/*
Clock_t *Clock_get(void)
{
  Clock_t *gmtclock;
  get_GMT(gmtclock,1);

    return gmtclock;
}
*/

/*******************************************************************************
 Brief    : 格林威治时间GMT与本地时间转换
 Parameter: dir=1,本地时间转GMT，=0 GMT转本地时间 
 return   : 
 *******************************************************************************/
void get_GMT(Clock_t * time,u8_t dir)
{
	Buffer_t buf;

	r_memcpy(time, &local_clock, sizeof(Clock_t));
	SysPara_Get(TIME_ZONE_ADDR, &buf);
	if (buf.payload != null && buf.lenght > 0)
	{
		int timeZone;
		timeZone = Swap_charNum((char*)buf.payload);
		if (timeZone >= 0 && timeZone <= 24)
		{
			int hour = time->hour;
			timeZone -= 12;
			if(dir)
				hour -= timeZone;
			else
				hour += timeZone;
			
			if (hour < 0)
			{
				hour += 24;
				if (time->day == 1)
				{
					if (time->month == 1)
					{
						time->year--;
						time->month = 12;
						time->day = 31;
					}
					else 
					{
						time->month--;
						if (time->month == 2)
						{
							if (0 == leapYear(time->year))
							{
								time->day = 29;
							}
							else
							{
								time->day = 28;
							}
						}
						else if (((time->month < 8) && ((time->month&0x01) == 0x01))
									|| ((time->month > 7)&& ((time->month&0x01) == 0x00))
									)
						{
							
							time->day = 31;
						}
						else 
						{
							time->day = 30;
						}
					}
				}
			}
			else if (hour > 23)
			{
				hour -= 24;
				if ((time->day < 28) 
					|| ((time->month != 2) 
                    	&& (time->day < (30 + (time->month > 7 ? (time->month - 7)&0x01 : time->month &0x01))))
              		|| (time->day < 29 && 0 == leapYear(time->year))
	            )
	   			{
					time->day++;
				}
				else
				{
					time->day = 1;
					if (time->month < 12)
					{
						time->month++;
					}
					else
					{
						time->year++;
						time->month = 1;
					}
				}
			}
			time->hour = hour;
		}
	}
	memory_release(buf.payload);
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
void Clock_Add(void)
{
    if (local_clock.secs < 59)
    {
        local_clock.secs++;
    }
    else if (local_clock.min < 59)
    {
        if (local_clock.min%10 == 0)
        {
            Clock_save();
        }
        local_clock.secs = 0;
        local_clock.min++;
    }
    else if (local_clock.hour < 23)
    {
        local_clock.secs = 0;
        local_clock.min = 0;
        local_clock.hour++;
    }
    else if ((local_clock.day < 28)
              || ((local_clock.month != 2) 
                    && (local_clock.day < (30 + (local_clock.month > 7 ? (local_clock.month - 7)&0x01 : local_clock.month &0x01))))
              || ( local_clock.day < 29 && 0 == leapYear(local_clock.year))
            )
    {
        local_clock.secs = 0;
        local_clock.min = 0;
        local_clock.hour = 0;
        local_clock.day++;
    }
    else if (local_clock.month < 12)
    {
        local_clock.secs = 0;
        local_clock.min = 0;
        local_clock.hour = 0;
        local_clock.day = 1;
        local_clock.month++;
    }
    else 
    {
        local_clock.secs = 0;
        local_clock.min = 0;
        local_clock.hour = 0;
        local_clock.day = 1;
        local_clock.month = 1;
        local_clock.year++;
    }
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
void Clock_save(void)
{
//	FlashEquilibria_write(&clockHead, &clock);	// mike 20200805
}

/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
static u8_t leapYear(u16_t year)
{
    if((year%400) == 0 || (((year&0x03) == 0) && ((year%100) != 0)))
    {
        return 0;
    }
    else
    {
        return 1;
    } 
}
/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
u8_t runTimeCheck(u8_t start, u8_t end)
{
	u8_t i = 0;

	if (start <= end)
	{
		Clock_t c;
		
		Clock_timeZone(&c);
//        APP_PRINT("System clock (%d.%02d.%02d %02d:%02d:%02d)\r\n",c.year, c.month, c.day, c.hour, c.min, c.secs);
		
		if (c.hour < start || c.hour > end)
		{
			i = 1;
		}
	}

	return i;
}


/*******************************************************************************
 Brief    : void
 Parameter: 
 return   : 
*******************************************************************************/
static void week(void)
{
    u16_t year = local_clock.year;
    u16_t month = local_clock.month;
    
    if(local_clock.month == 1 || local_clock.month == 2) 
    {
        month += 12;
        year--;
    }
    
    local_clock.week = (local_clock.day + 2*month + 3*(month + 1)/5 + year + year/4 - year/100 + year/400)%7;
}
/******************************************************************************/
