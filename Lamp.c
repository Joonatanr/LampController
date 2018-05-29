/*
 * Lamp.c
 *
 *  Created on: 28. mai 2018
 *      Author: JRE
 */


#include "Lamp.h"
#include "display.h"

//#define DEBUG_ON

/**************************************** Private function forward declarations  *******************************************/

Private void ChangeState(void);
Private void UpdateDisplayText(void);

Private void addToTimer(const time_obj * value, time_obj * dest);
Private void incrementTimeKeeper(void);

Private void convertTimerString(time_obj * t, char * dest_str);
Private void subtractFromTimer(const time_obj * value, time_obj * dest);
Private void addToTimer(const time_obj * value, time_obj * dest);


/**************************************** Private variable definitions  **********************************************/

Private const U8 upTime = 12u;  /* Set both up and down time to 12 hours initially. */
Private const U8 downTime = 12u;

Private Boolean isOn = FALSE;
Private time_obj priv_timekeeper = {0u, 0u, 0u};

Private char timerStr[16];



/**************************************** Public function definitions  **********************************************/

/* Called once during startup. */
Public void lamp_init(void)
{
    memset(timerStr, 0u, 16u);
}


/* Called every 1 second. */
Public void lamp_cyclic(void)
{
    static Boolean isFirst = TRUE;

    /* Update the clock timer. */
    incrementTimeKeeper();
    convertTimerString(&priv_timekeeper, timerStr);
    disp_write_string(timerStr, 0u, DISP_HIGH);

    if (isFirst)
    {
        isFirst = FALSE;
        ChangeState();
    }

    if (isOn)
    {
        /*Lamp is currently on. */
#ifdef DEBUG_ON
        if(priv_timekeeper.second >= upTime)
#else
        if (priv_timekeeper.hour >= upTime)
#endif
        {
            ChangeState();
        }
    }
    else
    {
#ifdef DEBUG_ON
        if (priv_timekeeper.second >= downTime)
#else
        /* Lamp is currently off */
        if (priv_timekeeper.hour >= downTime)
#endif
        {
            ChangeState();
        }
    }
}


Public void handleButtonPress(Button b)
{
    /* TODO : Implement this. */
}


/**************************************** Private function definitions  **********************************************/
Private void ChangeState(void)
{
    priv_timekeeper.hour = 0u;
    priv_timekeeper.minute = 0u;
    priv_timekeeper.second = 0u;

    /* Switch the current state */
    if (isOn == TRUE)
    {
        isOn = FALSE;
    }
    else
    {
        isOn = TRUE;
    }

    UpdateDisplayText();

    /* TODO : Drive digital input accordingly.*/
    set_led((U8)isOn);
}

Private void UpdateDisplayText(void)
{
    disp_clear_low();

    if(isOn)
    {
        disp_write_string("Lamp is on", 0u, DISP_LOW);
    }
    else
    {
        disp_write_string("Lamp is off", 0u, DISP_LOW);
    }
}


Private void incrementTimeKeeper(void)
{
    priv_timekeeper.second++;
    if (priv_timekeeper.second >= 60u)
    {
        priv_timekeeper.second = 0u;
        priv_timekeeper.minute++;

        if (priv_timekeeper.minute >= 60u)
        {
            priv_timekeeper.minute = 0u;
            priv_timekeeper.hour++;

            if (priv_timekeeper.hour >= 24u)
            {
                priv_timekeeper.hour = 0u;
            }
        }
    }
}


Private void addToTimer(const time_obj * value, time_obj * dest)
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


Private void subtractFromTimer(const time_obj * value, time_obj * dest)
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


Private void convertTimerString(time_obj * t, char * dest_str)
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



