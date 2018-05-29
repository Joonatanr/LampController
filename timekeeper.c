/*
 * timekeeper.c
 *
 *  Created on: 29. mai 2018
 *      Author: JRE
 */

#include "timekeeper.h"

Public void timekeeper_increment(time_T * timekeeper)
{
    __disable_interrupt();
    timekeeper->second++;
    if (timekeeper->second >= 60u)
    {
        timekeeper->second = 0u;
        timekeeper->minute++;

        if (timekeeper->minute >= 60u)
        {
            timekeeper->minute = 0u;
            timekeeper->hour++;

            if (timekeeper->hour >= 24u)
            {
                timekeeper->hour = 0u;
            }
        }
    }
    __enable_interrupt();
}


Public void timekeeper_addToTimer(const time_T * value, time_T * dest)
{
    U8 hour_val = 0u;
    U8 minute_val = 0u;
    U8 second_val;

    /* Critical section. */
    __disable_interrupt();
    second_val = dest->second + value->second;

    if (second_val >= 60u)
    {
        minute_val++;
        second_val = second_val % 60u;
    }

    minute_val += (value->minute + dest->minute);

    if (minute_val >= 60u)
    {
        hour_val++;
        minute_val = minute_val % 60u;
    }

    hour_val += (value->hour + dest->hour);

    dest->hour = hour_val;
    dest->minute = minute_val;
    dest->second = second_val;

    __enable_interrupt();
}


Public void timekeeper_subtractFromTimer(const time_T * value, time_T * dest)
{
    int second_value;
    int minute_value;
    int hour_value;

    __disable_interrupt();

    second_value    = dest->second;
    minute_value    = dest->minute;
    hour_value      = dest->hour;

    second_value -= value->second;
    if (second_value < 0)
    {
        second_value += 60;
        minute_value--;
    }

    minute_value -= value->minute;
    if (minute_value < 0)
    {
        minute_value += 60;
        hour_value--;
    }

    hour_value -= value->hour;
    if (hour_value < 0)
    {
        second_value = 0;
        minute_value = 0;
    }

    dest->hour =    (U8)hour_value;
    dest->minute =  (U8)minute_value;
    dest->second =  (U8)second_value;

    __enable_interrupt();
}


Public void timekeeper_getTimerString(time_T * t, char * dest_str)
{
    dest_str[0] = '0' + (t->hour / 10u);
    dest_str[1] = '0' + (t->hour % 10u);
    dest_str[2] = ':';
    dest_str[3] = '0' + (t->minute / 10u);
    dest_str[4] = '0' + (t->minute % 10u);
    dest_str[5] = ':';
    dest_str[6] = '0' + (t->second / 10u);
    dest_str[7] = '0' + (t->second % 10u);
    dest_str[8] = 0;
}
