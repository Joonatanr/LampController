/*
 * Lamp.c
 *
 *  Created on: 28. mai 2018
 *      Author: JRE
 */


#include "Lamp.h"
#include "display.h"
#include "timekeeper.h"
#include "buttons.h"
#include "misc.h"
#include "flash.h"

//#define DEBUG_ON
#define HOLD_TIMEOUT 10u

/**************************************** Private function forward declarations  *******************************************/

Private void ChangeState(void);
Private void UpdateDisplayText(void);
Private void handleButtonPressUp(void);
Private void handleButtonPressDown(void);

Private void handleButtonHold(void);

Private void writeTimerToDisplay(void);
Private void resetButtons(void);

Private void handleButtonPressDownSetValue(void);
Private void handleButtonPressUpSetValue(void);

/* Flash writing functions. */
Private void UpdateFlashValues(void);

/**************************************** Private variable definitions  **********************************************/

Private U8 upTime = 12u;  /* Set both up and down time to 12 hours initially. */
Private U8 downTime = 12u;

Private Boolean isOn = FALSE;
Private time_T priv_timekeeper = {0u, 0u, 0u};

Private char timerStr[16];


//Used for the SET menu.
Private Boolean isPaused = FALSE;
Private U16 timeout_counter;


/**************************************** Public function definitions  **********************************************/

/* Called once during startup. */
Public void lamp_init(void)
{
    memset(timerStr, 0u, 16u);
    resetButtons();

    timekeeper_setTimerValue(&priv_timekeeper , upTime, 0u, 0u);

    //Load default values from flash.
    upTime = flash_read_byte(0u);
    downTime = flash_read_byte(1u);

    if (upTime > 24u)
    {
       upTime = 12u;
    }

    if (downTime > 24u)
    {
       downTime = 12u;
    }
}

Private void resetButtons(void)
{
    buttons_unsubscribeAll();

    buttons_subscribeListener(UP_BUTTON, handleButtonPressUp);
    buttons_subscribeListener(DOWN_BUTTON, handleButtonPressDown);

    buttons_setButtonMode(UP_BUTTON, RISING_EDGE);
    buttons_setButtonMode(DOWN_BUTTON, FALLING_EDGE);

    buttons_subscribeHoldDownListener(UP_BUTTON, handleButtonHold);
    buttons_subscribeHoldDownListener(DOWN_BUTTON, handleButtonHold);
}


const time_T timeVal1Second = {.hour = 0u, .minute = 0u, .second = 1u};

/* Called every 1 second. */
Public void lamp_cyclic(void)
{
    static Boolean isFirst = TRUE;

    if (isFirst)
    {
       isFirst = FALSE;
       //UpdateDisplayText();
       ChangeState();
    }


    if(!isPaused)
    {
        if (    priv_timekeeper.hour == 0u &&
                priv_timekeeper.minute == 0u &&
                priv_timekeeper.second == 0u)
        {
            ChangeState();
        }
        else
        {

            /* Update the clock timer. */
            timekeeper_subtractFromTimer(&timeVal1Second, &priv_timekeeper);
        }
        writeTimerToDisplay();

    }
    else
    {
        timeout_counter--;
        if (timeout_counter == 0u)
        {
            isPaused = FALSE;
            resetButtons();
            disp_clear_high();
            disp_clear_low();
            UpdateFlashValues();
            UpdateDisplayText();
        }
    }

}


/**************************************** Private function definitions  **********************************************/

Private void UpdateFlashValues(void)
{
    //Writes down and uptime to flash (or tries to anyway :D )
    flash_erase();
    flash_write(0u, &upTime, 1u);
    flash_write(1u, &downTime, 1u);
}


Private void writeTimerToDisplay(void)
{
    __disable_interrupt();
    timekeeper_getTimerString(&priv_timekeeper, timerStr);
    disp_write_string(timerStr, 0u, DISP_HIGH);
    __enable_interrupt();
}


const time_T hour_value = { .hour = 1u, .minute = 0u, .second = 0u };

Private void handleButtonPressUp(void)
{
    timekeeper_addToTimer(&hour_value, &priv_timekeeper);
    writeTimerToDisplay();
}


Private void handleButtonPressDown(void)
{
    timekeeper_subtractFromTimer(&hour_value, &priv_timekeeper);
    writeTimerToDisplay();
}


Private void handleButtonHold(void)
{
    isPaused = TRUE;
    timeout_counter = HOLD_TIMEOUT;

    //Lets reset all the buttons first.
    buttons_unsubscribeAll();

    buttons_subscribeListener(UP_BUTTON, handleButtonPressUpSetValue);
    buttons_subscribeListener(DOWN_BUTTON, handleButtonPressDownSetValue);

    disp_clear_high();
    disp_clear_low();

    disp_write_string("Up time : ", 0, DISP_HIGH);
    disp_write_string("Down time : ", 0, DISP_LOW);

    disp_write_number(downTime, 12, DISP_LOW, 2);
    disp_write_number(upTime, 12, DISP_HIGH, 2);
}


Private void handleButtonPressDownSetValue(void)
{
    timeout_counter = HOLD_TIMEOUT;
    if(downTime > 1u)
    {
        downTime--;
    }
    else
    {
        downTime = 24u;
    }

    disp_write_number(downTime, 12, DISP_LOW, 2);
}

Private void handleButtonPressUpSetValue(void)
{
    timeout_counter = HOLD_TIMEOUT;
    if(upTime < 24u)
    {
        upTime++;
    }
    else
    {
        upTime = 1u;
    }

    disp_write_number(upTime, 12, DISP_HIGH, 2);
}



Private void ChangeState(void)
{
    /* Switch the current state */
    if (isOn == TRUE)
    {
        isOn = FALSE;
        timekeeper_setTimerValue(&priv_timekeeper, downTime, 0u, 0u);
    }
    else
    {
        isOn = TRUE;
        timekeeper_setTimerValue(&priv_timekeeper, upTime, 0u, 0u);
    }

    UpdateDisplayText();

    /* Currently LED output is connected directly to LED1. */
    set_led1((U8)isOn);
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



