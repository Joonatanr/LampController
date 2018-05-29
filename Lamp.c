/*
 * Lamp.c
 *
 *  Created on: 28. mai 2018
 *      Author: JRE
 */


#include "Lamp.h"
#include "display.h"
#include "timekeeper.h"

//#define DEBUG_ON

/**************************************** Private function forward declarations  *******************************************/

Private void ChangeState(void);
Private void UpdateDisplayText(void);


/**************************************** Private variable definitions  **********************************************/

Private const U8 upTime = 12u;  /* Set both up and down time to 12 hours initially. */
Private const U8 downTime = 12u;

Private Boolean isOn = FALSE;
Private time_T priv_timekeeper = {0u, 0u, 0u};

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
    timekeeper_increment(&priv_timekeeper);
    timekeeper_getTimerString(&priv_timekeeper, timerStr);
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



