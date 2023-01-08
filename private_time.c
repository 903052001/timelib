/**
  ***************************************************************************
  *           Copyright (C) lance.  2023. All rights reserved.
  * 
  * @file    private_time.c
  * @author  lance
  * @date    2023/1/7
  * @brief   This file implements the private time lib.
  ***************************************************************************
  */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdio.h>
#include <time.h>
#include "private_time.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define NULL                   ((void *)0)

#define MAX_ASCTIME_LENGTH     (32)
#define SECONDS_1_MINUTE       (60)
#define SECONDS_1_HOUR         (60 * SECONDS_1_MINUTE)
#define SECONDS_1_DAY          (24 * SECONDS_1_HOUR)

#define HOURS_1_YEAR           (8760)   // 8760 = 365 * 24
#define HOURS_4_YEAR           (35064)  // 35064 = (365 * 3 + 366) * 24

#define ZONE_EAST_8            (+8 * SECONDS_1_HOUR)
#define ZONE_WEST_8            (-8 * SECONDS_1_HOUR)

#define LOCAL_ZONE             ZONE_EAST_8

/****************************************************************************
 * Private Types
 ****************************************************************************/


/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int is_leap(int year);
static int days_from_0(int year);
static int days_from_1970(int year);
static int days_from_1jan(int year, int month, int day);

/****************************************************************************
 * Private Data
 ****************************************************************************/

const static char *g_abbr_wday[7] =
{
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

const static char *g_abbr_months[12] =
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", \
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

const static char *g_name_wday[7] =
{
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

const static char *g_name_months[12] =
{
    "January", "February", "March",     "April",   "May",      "June", \
    "July",    "August",   "September", "October", "November", "December"
};

const static int g_days[3][12] =
{
    { 0, 31, 59, 90, 120,151,181,212,243,273,304,334},
    { 0, 31, 60, 91, 121,152,182,213,244,274,305,335},
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
};

static volatile time_t g_time = 0;
static struct tm g_tm = {0};
static int g_time_zone = 0;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int is_leap(int year)
{
    if (year % 400 == 0)
        return 1;
    if (year % 100 == 0)
        return 0;
    if (year % 4 == 0)
        return 1;
    return 0;
}

static int days_from_0(int year)  //year start from 1
{
    if (--year < 0)  return 0;
    return 365 * year + (year / 400) - (year / 100) + (year / 4);
}

static int days_from_1970(int year)
{
    int days_from_0_to_1970 = days_from_0(1970);
    return days_from_0(year) - days_from_0_to_1970;
}

static int days_from_1jan(int year, int month, int day)
{
    return g_days[is_leap(year)][month - 1] + day - 1;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int get_local_zone(void)
{
    return g_time_zone;
}

void set_local_zone(int zone)
{
    g_time_zone = zone;
}

time_t get_time(void)
{
    return g_time;
}

void set_time(time_t time)
{
    g_time = time;
}

clock_t priv_clock(void)
{
    return 0;
}

char *priv_asctime(const struct tm *timeptr)
{
    static char asctime[MAX_ASCTIME_LENGTH];

    priv_strftime(asctime, sizeof(asctime), "%c", timeptr);

    return asctime;
}

char *priv_ctime(const time_t *timer)
{
    return priv_asctime(priv_localtime(timer));
}

double priv_difftime(time_t time1, time_t time2)
{
    return (double)(time1 - time2);
}

time_t priv_mktime(struct tm *timeptr)
{
    int year   = timeptr->tm_year + 1900;
    int month  = timeptr->tm_mon + 1;
    int day    = timeptr->tm_mday;
    int hour   = timeptr->tm_hour;
    int minute = timeptr->tm_min;
    int second = timeptr->tm_sec;

    int days_of_year = days_from_1jan(year, month, day);
    int days_since_epoch = days_from_1970(year) + days_of_year;

    time_t result = (SECONDS_1_DAY * days_since_epoch) + \
                    (SECONDS_1_HOUR * hour) + (SECONDS_1_MINUTE * minute) + (second);

    return result - get_local_zone();
}

size_t priv_strftime(char *str, size_t maxsize, const char *format, const struct tm *timeptr)
{
    size_t idx = 0;
    char cmd;

    if ((!format) || (!timeptr) || (!str) || (maxsize < 16))
    {
        return -1;
    }

    const int year   = timeptr->tm_year + 1900;
    const int month  = timeptr->tm_mon;
    const int mday   = timeptr->tm_mday;
    const int hour   = timeptr->tm_hour;
    const int minute = timeptr->tm_min;
    const int second = timeptr->tm_sec;
    const int wday   = timeptr->tm_wday;
    const int yday   = timeptr->tm_yday;

    for (int k = 0; format[k]; ++k)
    {
        if (idx >= maxsize)
        {
            return -2;
        }

        cmd = format[k];
        if (cmd != '%')
        {
            str[idx++] = cmd;
            continue;
        }

        cmd = format[++k];
        switch (cmd)
        {
        case 'a':
            {
                for (int i = 0; g_abbr_wday[wday][i]; ++i)
                {
                    str[idx++] = g_abbr_wday[wday][i];
                }
            }break;
        case 'A':
            {
                for (int i = 0; g_name_wday[wday][i]; ++i)
                {
                    str[idx++] = g_name_wday[wday][i];
                }
            }break;
        case 'b':
            {
                for (int i = 0; g_abbr_months[month][i]; ++i)
                {
                    str[idx++] = g_abbr_months[month][i];
                }
            }break;
        case 'B':
            {
                for (int i = 0; g_name_months[month][i]; ++i)
                {
                    str[idx++] = g_name_months[month][i];
                }
            }break;
        case 'c':
            {
                for (int i = 0; g_abbr_wday[wday][i]; ++i)
                {
                    str[idx++] = g_abbr_wday[wday][i];
                }

                str[idx++] = ' ';

                for (int i = 0; g_abbr_months[month][i]; ++i)
                {
                    str[idx++] = g_abbr_months[month][i];
                }

                str[idx++] = ' ';

                if (mday > 9)
                {
                    str[idx++] = '0' + mday / 10;
                    str[idx++] = '0' + mday % 10;
                }
                else
                {
                    str[idx++] = ' ';
                    str[idx++] = '0' + mday % 10;
                }

                str[idx++] = ' ';
                str[idx++] = '0' + hour / 10;
                str[idx++] = '0' + hour % 10;
                str[idx++] = ':';

                str[idx++] = '0' + minute / 10;
                str[idx++] = '0' + minute % 10;
                str[idx++] = ':';

                str[idx++] = '0' + second / 10;
                str[idx++] = '0' + second % 10;
                str[idx++] = ' ';

                str[idx++] = '0' + year / 1000;
                str[idx++] = '0' + year % 1000 / 100;
                str[idx++] = '0' + year % 100 / 10;
                str[idx++] = '0' + year % 10;
            }break;
        case 'd':
            {
                str[idx++] = '0' + mday / 10;
                str[idx++] = '0' + mday % 10;
            }break;
        case 'w':
            {
                str[idx++] = '0' + wday % 10;
            }break;
        case 'H':
            {
                str[idx++] = '0' + hour / 10;
                str[idx++] = '0' + hour % 10;
            }break;
        case 'I':
            {
                int tmp = hour % 12;

                if (tmp == 0)    { tmp = 12; }
                str[idx++] = '0' + tmp / 10;
                str[idx++] = '0' + tmp % 10;
            }break;
        case 'j':
            {
                str[idx++] = '0' + (yday + 1) / 100;
                str[idx++] = '0' + (yday + 1) % 100 / 10;
                str[idx++] = '0' + (yday + 1) % 10;
            }break;
        case 'm':
            {
                str[idx++] = '0' + (month + 1) / 10;
                str[idx++] = '0' + (month + 1) % 10;
            }break;
        case 'M':
            {
                str[idx++] = '0' + minute / 10;
                str[idx++] = '0' + minute % 10;
            }break;
        case 'p':
            {
                if (hour < 12)
                {
                    str[idx++] = 'A';
                    str[idx++] = 'M';
                }
                else
                {
                    str[idx++] = 'P';
                    str[idx++] = 'M';
                }
            }break;
        case 'S':
            {
                str[idx++] = '0' + second / 10;
                str[idx++] = '0' + second % 10;
            }break;
        case 'W':
            {
                int num1 = (yday + 1) / 7;
                int num2 = (yday + 1) % 7;

                if ((0 < wday) && (wday <= num2))
                {
                    num1++;
                }
                str[idx++] = '0' + num1 / 10;
                str[idx++] = '0' + num1 % 10;
            }break;
        case 'U':
            {
                int num1 = (yday + 1) / 7;
                int num2 = (yday + 1) % 7;

                if (wday < num2)   num1++;
                str[idx++] = '0' + num1 / 10;
                str[idx++] = '0' + num1 % 10;
            }break;
        case 'x':
            {
                str[idx++] = '0' + (month + 1) / 10;
                str[idx++] = '0' + (month + 1) % 10;
                str[idx++] = '/';
                str[idx++] = '0' + mday / 10;
                str[idx++] = '0' + mday % 10;
                str[idx++] = '/';
                str[idx++] = '0' + year % 100 / 10;
                str[idx++] = '0' + year % 10;
            }break;
        case 'y':
            {
                str[idx++] = '0' + year % 100 / 10;
                str[idx++] = '0' + year % 10;
            }break;
        case 'Y':
            {
                str[idx++] = '0' + year / 1000;
                str[idx++] = '0' + year % 1000 / 100;
                str[idx++] = '0' + year % 100 / 10;
                str[idx++] = '0' + year % 10;
            }break;
        case 'Z':
            {
                str[idx++] = ' ';
                str[idx++] = ' ';
                str[idx++] = ' ';
            }break;
        default:
            {
                str[idx++] = cmd;
            }break;
        }
    }

    str[idx] = 0;

    return idx;
}

time_t priv_time(time_t *timer)
{
    time_t time = get_time();

    if (timer) *timer = time;

    return time;
}

struct tm *priv_gmtime(const time_t *timer)
{
    time_t timestamp = *timer;
    struct tm *tm_time = &g_tm;
    unsigned int four_year_nums;
    unsigned int one_year_hours;

    tm_time->tm_isdst = 0;

    tm_time->tm_sec = (int)(timestamp % 60);
    timestamp /= 60;

    tm_time->tm_min = (int)(timestamp % 60);
    timestamp /= 60;

    /* January 1, 1970 is Thursday */
    tm_time->tm_wday = (int)(timestamp/24 + 4) % 7;

    four_year_nums = timestamp / HOURS_4_YEAR;

    tm_time->tm_year=(four_year_nums << 2) + 70;

    timestamp %= HOURS_4_YEAR;

    while (1)
    {
        one_year_hours = HOURS_1_YEAR;

        if ((tm_time->tm_year & 3) == 0)
        {
            one_year_hours += 24;  /* leap year */
        }

        if (timestamp < one_year_hours)
        {
            break;
        }

        tm_time->tm_year++;
        timestamp -= one_year_hours;
    }

    tm_time->tm_hour = (int)(timestamp % 24);

    timestamp /= 24;
    timestamp++;

    tm_time->tm_yday = timestamp - 1;

    if ((tm_time->tm_year & 3) == 0)
    {
        if (timestamp > 60)
        {
            timestamp--;
        }
        else if (timestamp == 60) /* Feb 29 */
        {
            tm_time->tm_mon = 1;
            tm_time->tm_mday = 29;

            return tm_time;
        }
    }

    for (tm_time->tm_mon = 0; g_days[2][tm_time->tm_mon] < timestamp; tm_time->tm_mon++)
    {
        timestamp -= g_days[2][tm_time->tm_mon];
    }

    tm_time->tm_mday = (int)(timestamp);

    return tm_time;
}

struct tm *priv_localtime(const time_t *timer)
{
    time_t timestamp = *timer + get_local_zone();

    return priv_gmtime(&timestamp);
}

int main(int argc, char const *argv[])
{
    char buffer[256];
    time_t rawtime = 1; //utc 1970-1-1 00:00:01 equal beijing local time 1970-1-1 08:00:01
    struct tm *info, *info1;

    info = localtime( &rawtime );
    printf("1--%s\n", priv_asctime(info));
    printf("2--%s\n", asctime(info));
    strftime(buffer, 256, "1.%a 2.%A 3.%b 4.%B 5.%c 6.%I 7.%j 8.%p 9.%U 10.%w 11.%W 12.%x 13.%y 14.%Z 15.%Y-%m-%d-%H:%M:%S", info);
    printf("3.格式化的日期 & 时间 : |%s|\n", buffer ); 
    priv_strftime(buffer, 256, "1.%a 2.%A 3.%b 4.%B 5.%c 6.%I 7.%j 8.%p 9.%U 10.%w 11.%W 12.%x 13.%y 14.%Z 15.%Y-%m-%d-%H:%M:%S", info);
    printf("4.格式化的日期 & 时间 : |%s|\n", buffer ); 

    info1 = gmtime( &rawtime );
    printf("5--%s\n", priv_asctime(info1));
    printf("6--%s\n", asctime(info1));
    strftime(buffer, 256, "1.%a 2.%A 3.%b 4.%B 5.%c 6.%I 7.%j 8.%p 9.%U 10.%w 11.%W 12.%x 13.%y 14.%Z 15.%Y-%m-%d-%H:%M:%S", info1);
    printf("7.格式化的日期 & 时间 : |%s|\n", buffer ); 
    priv_strftime(buffer, 256, "1.%a 2.%A 3.%b 4.%B 5.%c 6.%I 7.%j 8.%p 9.%U 10.%w 11.%W 12.%x 13.%y 14.%Z 15.%Y-%m-%d-%H:%M:%S", info1);
    printf("8.格式化的日期 & 时间 : |%s|\n", buffer ); 

    set_local_zone(LOCAL_ZONE);
    info = priv_localtime( &rawtime );
    printf("A--%s\n", priv_asctime(info));
    printf("B--%s\n", asctime(info));
    strftime(buffer, 256, "1.%a 2.%A 3.%b 4.%B 5.%c 6.%I 7.%j 8.%p 9.%U 10.%w 11.%W 12.%x 13.%y 14.%Z 15.%Y-%m-%d-%H:%M:%S", info);
    printf("C.格式化的日期 & 时间 : |%s|\n", buffer ); 
    priv_strftime(buffer, 256, "1.%a 2.%A 3.%b 4.%B 5.%c 6.%I 7.%j 8.%p 9.%U 10.%w 11.%W 12.%x 13.%y 14.%Z 15.%Y-%m-%d-%H:%M:%S", info);
    printf("D.格式化的日期 & 时间 : |%s|\n", buffer ); 

    info1 = priv_gmtime( &rawtime );
    printf("E--%s\n", priv_asctime(info1));
    printf("F--%s\n", asctime(info1));
    strftime(buffer, 256, "1.%a 2.%A 3.%b 4.%B 5.%c 6.%I 7.%j 8.%p 9.%U 10.%w 11.%W 12.%x 13.%y 14.%Z 15.%Y-%m-%d-%H:%M:%S", info1);
    printf("G.格式化的日期 & 时间 : |%s|\n", buffer ); 
    priv_strftime(buffer, 256, "1.%a 2.%A 3.%b 4.%B 5.%c 6.%I 7.%j 8.%p 9.%U 10.%w 11.%W 12.%x 13.%y 14.%Z 15.%Y-%m-%d-%H:%M:%S", info1);
    printf("H.格式化的日期 & 时间 : |%s|\n", buffer ); 

    struct tm tm2 =   //beijing local time 1970-1-1 8:0:0, not UTC
    {
        .tm_sec=0,
        .tm_min=0,
        .tm_hour=0+8,
        .tm_mday=1,
        .tm_mon=1-1,
        .tm_year=1970-1900,
        .tm_wday=4,
        .tm_yday=1-1,
        .tm_isdst=0,
    };

    printf("a--%ld\n", priv_mktime(&tm2));
    printf("b--%ld\n", mktime(&tm2));

    set_time(time(NULL));
    printf("e--%ld\n", priv_time(NULL));
    printf("f--%ld\n", time(NULL));

    return 0;
}
