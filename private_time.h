/**
  ***************************************************************************
  *           Copyright (C) lance.  2023. All rights reserved.
  * 
  * @file    private_time.h
  * @author  lance
  * @date    2023/1/7
  * @brief   This file implements the private time lib.
  ***************************************************************************
  */

#ifndef __PRIVATE_TIME_H__
#define __PRIVATE_TIME_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/


/****************************************************************************
 * Other conditional compilation options
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/


/****************************************************************************
 * Public Types
 ****************************************************************************/

#if 0

typedef int       size_t;
typedef long long clock_t;
typedef long long time_t;

struct tm
{
    /*
     * the number of seconds after the minute, normally in the range
     * 0 to 59, but can be up to 60 to allow for leap seconds
     */
    int tm_sec;
    /* the number of minutes after the hour, in the range 0 to 59*/
    int tm_min;
    /* the number of hours past midnight, in the range 0 to 23 */
    int tm_hour;
    /* the day of the month, in the range 1 to 31 */
    int tm_mday;
    /* the number of months since January, in the range 0 to 11 */
    int tm_mon;
    /* the number of years since 1900 */
    int tm_year;
    /* the number of days since Sunday, in the range 0 to 6 */
    int tm_wday;
    /* the number of days since January 1, in the range 0 to 365 */
    int tm_yday;
    /* >0 used, =0 unused, <0 invalid */
    int tm_isdst;
};

#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

time_t get_time(void);
void   set_time(time_t time);
int    get_local_zone(void);
void   set_local_zone(int zone);

clock_t priv_clock(void);
time_t priv_time(time_t *timer);
char  *priv_ctime(const time_t *timer);
char  *priv_asctime(const struct tm *timeptr);
double priv_difftime(time_t time1, time_t time2);
time_t priv_mktime(struct tm *timeptr);
struct tm *priv_gmtime(const time_t *timer);
struct tm *priv_localtime(const time_t *timer);
size_t priv_strftime(char *str, size_t maxsize, \
	                   const char *format, const struct tm *timeptr);


#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __PRIVATE_TIME_H__ */
