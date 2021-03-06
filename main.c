

#include "display.h"
#include "typedefs.h"
#include "register.h"
#include "Lamp.h"
#include "flash.h"
#include "buttons.h"

/**
 * main.c
 */

Private void timer_10msec(void);
Private void timer_1sec(void);
Private volatile U8 timer_flag = 0u;

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

	register_init(timer_10msec, timer_1sec);

	disp_set_conf(&priv_disp_conf);

	disp_init();

	set_backlight(1u);

	buttons_init();

	lamp_init();

	while(1)
	{
	    /* Called every 1 second. */
	    if (timer_flag)
	    {
	        timer_flag = 0u;
	        lamp_cyclic();
	    }
	}

	return 0;
}


Private void timer_10msec(void)
{
    static U8 msec_100_cnt = 0u;
    buttons_cyclic10msec();

    msec_100_cnt++;

    if (msec_100_cnt >= 10u)
    {
        msec_100_cnt = 0u;
        buttons_cyclic100msec();
    }
}

Private void timer_1sec(void)
{
    if (bReady)
    {
        /*incrementTimeKeeper();*/
        timer_flag = 1u;
    }
}


