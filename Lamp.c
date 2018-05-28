/*
 * Lamp.c
 *
 *  Created on: 28. mai 2018
 *      Author: JRE
 */


#include "Lamp.h"
#include "display.h"
#include "register.h"

//#define DEBUG_ON

Private void ChangeState(timekeeper_T * time);
Private void UpdateDisplayText(void);


Private const U8 upTime = 12u;  /* Set both up and down time to 12 hours initially. */
Private const U8 downTime = 12u;

Private Boolean isOn = FALSE;

/* Called every 1 second. */
Public void lamp_cyclic(timekeeper_T * time)
{
    static Boolean isFirst = TRUE;

    if (isFirst)
    {
        isFirst = FALSE;
        ChangeState(time);
    }

    if (isOn)
    {
        /*Lamp is currently on. */
#ifdef DEBUG_ON
        if(time->second >= upTime)
#else
        if (time->hour >= upTime)
#endif
        {
            ChangeState(time);
        }
    }
    else
    {
#ifdef DEBUG_ON
        if (time->second >= downTime)
#else
        /* Lamp is currently off */
        if (time->hour >= downTime)
#endif
        {
            ChangeState(time);
        }
    }
}

Private void ChangeState(timekeeper_T * time)
{
    time->hour = 0u;
    time->minute = 0u;
    time->second = 0u;

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
        disp_write_string("Lamp is on", 0u, 1u);
    }
    else
    {
        disp_write_string("Lamp is off", 0u, 1u);
    }
}


