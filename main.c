
#include "display.h"
#include "typedefs.h"
#include "register.h"


/**
 * main.c
 */

typedef struct
{
    U8 second;
    U8 minute;
    U8 hour;
} timekeeper_T;

Private void timer_10msec(void);
Private void timer_1sec(void);
Private void incrementTimeKeeper(void);
Private void convertTimerString(timekeeper_T * t, char * dest_str);

Private char timerStr[16];
Private volatile U8 timer_flag = 0u;
Private timekeeper_T priv_timekeeper = {0u, 0u, 0u};

Private disp_config_struct priv_disp_conf =
{
     .port_clk = set_clk,
     .port_csb = set_cs,
     .port_rs  = set_rs,
     .port_si  = set_si,
     .delay_func = wait_msec
};

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	memset(timerStr, 0u, 16u);

	register_init(timer_10msec, timer_1sec);

	disp_set_conf(&priv_disp_conf);

	disp_init();

	set_backlight(1u);

	while(1)
	{
	    if (timer_flag)
	    {
	        timer_flag = 0u;

	        /* Should probably disable interrupts just to be correct here, but realistically
	         * the 1 second interval will be more than enough time to finish writing to display. */
	        convertTimerString(&priv_timekeeper, timerStr);
	        disp_write_string(timerStr, 0u, DISP_HIGH);
	    }
	}

	return 0;
}


Private void timer_10msec(void)
{
    /* TODO : Add buttons handling here. */
}

Private void timer_1sec(void)
{
    if (bReady)
    {
        incrementTimeKeeper();
        timer_flag = 1u;
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

Private void convertTimerString(timekeeper_T * t, char * dest_str)
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


