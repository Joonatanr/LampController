

#include "display.h"
#include "typedefs.h"
#include "register.h"
#include "Lamp.h"
#include "flash.h"

/**
 * main.c
 */

Private void timer_10msec(void);
Private void timer_1sec(void);
Private volatile U8 timer_flag = 0u;

Boolean isBtn1Pressed = FALSE;
Boolean isBtn2Pressed = FALSE;

Private disp_config_struct priv_disp_conf =
{
     .port_clk = set_clk,
     .port_csb = set_cs,
     .port_rs  = set_rs,
     .port_si  = set_si,
     .delay_func = wait_msec
};

U8 test_data[10u];

volatile U8 test_byte1;
volatile U8 test_byte2;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	register_init(timer_10msec, timer_1sec);

	disp_set_conf(&priv_disp_conf);

	disp_init();

	set_backlight(1u);

	lamp_init();

	/* Test our flash writing function */
	memset(test_data, 0xDAu, sizeof(test_data));

	flash_erase();
	flash_write(0u, test_data, 10u);

	test_byte1 = flash_read_byte(0u);
	test_byte2 = flash_read_byte(1u);
	/* End of test. */

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
    /* We catch the rising edge of a button press. */
    if (isBtnOne() && !isBtn1Pressed)
    {
        isBtn1Pressed = TRUE;
        handleButtonPress(BTN_ONE);
    }
    else if(!isBtnOne())
    {
        isBtn1Pressed = FALSE;
    }


    if (isBtnTwo() && !isBtn2Pressed)
    {
        isBtn2Pressed = TRUE;
        handleButtonPress(BTN_TWO);
    }
    else if(!isBtnTwo())
    {
        isBtn2Pressed = FALSE;
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


