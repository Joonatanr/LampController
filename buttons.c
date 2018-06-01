/*
 * buttons.c
 *
 *  Created on: Sep 7, 2017
 *      Author: Joonatan
 */

//#define MSP432_BUTTONS
#define MSP430_BUTTONS

#include "buttons.h"
#ifdef MSP432_BUTTONS
#include <driverlib.h>
#endif


#ifdef MSP432_BUTTONS
#define ENTER_CRITICAL_SECTION()    Interrupt_disableMaster()
#define EXIT_CRITICAL_SECTION()     Interrupt_enableMaster
#endif

#ifdef MSP430_BUTTONS
#define ENTER_CRITICAL_SECTION()    __disable_interrupt();
#define EXIT_CRITICAL_SECTION()     __enable_interrupt();
#endif


#define BUTTON_HOLD_TIME 3000u  /* Set this threshold to 3 seconds. */

typedef Boolean (*buttonFunction)(void);

typedef struct
{
#ifdef MSP432_BUTTONS
    const U8            port;
    const U8            pin;
#endif
#ifdef MSP430_BUTTONS
    U16 *                 port;
    U16 *                  dir;
    const U8               pin;
#endif
} ButtonConf;

typedef struct
{
    const ButtonConf * conf;

    ButtonMode     mode;                  //Rising or falling edge.
    buttonListener listener_press_func;   //Can be subscribed to. > Triggers when button is pressed
    buttonListener listener_hold_func;    //Can be subscribed to. > Triggers when button is held down for a period of time.

    Boolean        button_hold; /* Used for falling edge detection.     */
    U16            hold_cnt;    /* Used for button held-down detection  */

    Boolean        pressed;     /* Flag is triggered when a button press has been detected.         */
    Boolean        held_down;   /* Flag is triggered when a button has been held down for a time    */
} ButtonState;


/* TODO : Review this. */
#ifdef MSP430_BUTTONS
#define P2IN  0x0028
#define P2DIR 0x002A
#endif

Private const ButtonConf priv_button_config[NUMBER_OF_BUTTONS] =
{
     {.port = (U16 *)P2IN, .dir = (U16 *)P2DIR, .pin = BIT_4 },
     {.port = (U16 *)P2IN, .dir = (U16 *)P2DIR, .pin = BIT_5 },
};


Private ButtonState priv_button_state[NUMBER_OF_BUTTONS];


Public void buttons_init(void)
{
    U8 ix;

    for (ix = 0u; ix < NUMBER_OF_BUTTONS; ix++)
    {
        priv_button_state[ix].conf = &priv_button_config[ix];

        priv_button_state[ix].listener_hold_func = NULL;
        priv_button_state[ix].listener_press_func = NULL;

        priv_button_state[ix].held_down =   FALSE;
        priv_button_state[ix].pressed =     FALSE;
        priv_button_state[ix].button_hold = FALSE;
        priv_button_state[ix].mode = FALLING_EDGE;

        //Set up buttons as inputs.
#ifdef MSP432_BUTTONS
        //For MSP432
        GPIO_setAsInputPin(priv_button_config[ix].port, priv_button_config[ix].pin);
#endif

#ifdef MSP430_BUTTONS
        //For MSP430
        CLRBIT(*(priv_button_state[ix].conf->dir), priv_button_state[ix].conf->pin);
#endif
    }
}


//Hi priority task.
Public void buttons_cyclic10msec(void)
{
    U8 ix;
    Boolean state;
    ButtonState * btn_ptr;
    const ButtonConf  * conf_ptr;

    for (ix = 0u; ix < NUMBER_OF_BUTTONS; ix++)
    {
        btn_ptr = &priv_button_state[ix];
        conf_ptr = btn_ptr->conf;
#ifdef MSP432_BUTTONS
        state = (GPIO_getInputPinValue(conf_ptr->port, conf_ptr->pin) == 1u) ? FALSE : TRUE;
#endif

#ifdef MSP430_BUTTONS
      state = ((ISBIT(*conf_ptr->port, conf_ptr->pin)) > 0) ? FALSE : TRUE;
#endif

        /* Handle button press detection. */
        if (state && (btn_ptr->button_hold == FALSE))
        {
            btn_ptr->button_hold = TRUE;
            if (btn_ptr->mode == RISING_EDGE)
            {
                btn_ptr->pressed = TRUE;
            }
        }
        else if (!state && (btn_ptr->button_hold == TRUE))
        {
            btn_ptr->button_hold = FALSE;
            if (btn_ptr->mode == FALLING_EDGE)
            {
                btn_ptr->pressed = TRUE;
            }
        }
        else
        {
            /* Do nothing. */
        }

        /* Handle button hold-down detection */
        if (state)
        {
            btn_ptr->hold_cnt += 10u;
        }
        else
        {
            btn_ptr->hold_cnt = 0u;
        }

        if (btn_ptr->hold_cnt >= BUTTON_HOLD_TIME)
        {
            btn_ptr->held_down = TRUE;
        }
    }
}


//Low priority task.
Public void buttons_cyclic100msec(void)
{
    U8 ix;
    for (ix = 0u; ix < NUMBER_OF_BUTTONS; ix++)
    {
        if (priv_button_state[ix].pressed)
        {
            priv_button_state[ix].pressed = FALSE;
            if (priv_button_state[ix].listener_press_func != NULL)
            {
                priv_button_state[ix].listener_press_func();
            }
        }

        if (priv_button_state[ix].held_down)
        {
            priv_button_state[ix].held_down = FALSE;
            if (priv_button_state[ix].listener_hold_func != NULL)
            {
                priv_button_state[ix].listener_hold_func();
            }
        }
    }
}

//Note that the listeners are called from lo prio context.
//This function subscribes a listener function to a button.
Public void buttons_subscribeListener(ButtonType btn, buttonListener listener)
{
    ENTER_CRITICAL_SECTION();
    //Critical section.
    if (btn < NUMBER_OF_BUTTONS)
    {
        priv_button_state[btn].listener_press_func = listener;
    }
    EXIT_CRITICAL_SECTION();
}


Public void buttons_setButtonMode(ButtonType btn, ButtonMode mode)
{
    if ((btn < NUMBER_OF_BUTTONS) && (mode < NUMBER_OF_BUTTON_MODES))
    {
        priv_button_state[btn].mode = mode;
    }
}


/* TODO : Test this. */
Public void buttons_subscribeHoldDownListener(ButtonType btn, buttonListener listener)
{
    ENTER_CRITICAL_SECTION();
    //Critical section.
    if (btn < NUMBER_OF_BUTTONS)
    {
        priv_button_state[btn].listener_hold_func = listener;
    }
    EXIT_CRITICAL_SECTION();
}


Public void buttons_unsubscribeAll(void)
{
    U8 ix;

    ENTER_CRITICAL_SECTION();
    for (ix = 0u; ix < NUMBER_OF_BUTTONS; ix++)
    {
        priv_button_state[ix].listener_press_func = NULL;
        priv_button_state[ix].listener_hold_func = NULL;
    }
    EXIT_CRITICAL_SECTION();
}


Public Boolean isButton(ButtonType btn)
{
    Boolean res = FALSE;

    if(btn < NUMBER_OF_BUTTONS)
    {
#ifdef MSP432_BUTTONS
        res = (GPIO_getInputPinValue(priv_button_config[btn].port, priv_button_config[btn].pin) == 1u) ? FALSE : TRUE;
#endif

#ifdef MSP430_BUTTONS
      res = (ISBIT(*priv_button_config[btn].port, priv_button_config[btn].pin)) > 0 ? FALSE : TRUE;
#endif
    }

    return res;
}


