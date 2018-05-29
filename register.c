/*
 * register.c
 *
 *  Created on: 22. märts 2018
 *      Author: localpcadmin
 */


#include <msp430.h>
#include "register.h"


#define MCU_CLOCK           1000000

static U16 count_1sec = 0;

static timer_callback callback10msec;
static timer_callback callback1sec;

U8 bReady = 0;

void register_init(timer_callback timer10msec, timer_callback timer1sec)
{
    /* Disable watchdog timer */
    WDTCTL = WDTPW + WDTHOLD;

    /* Set up internal clock */
    /* Check if 8MHz Calibration is present */
    if (CALBC1_8MHZ != 0xFF)
    {
        DCOCTL = 0; // Select lowest DCOx and MODx
        BCSCTL1 = CALBC1_8MHZ; // Set range
        DCOCTL = CALDCO_8MHZ; // Set DCO step + modulation
    }

    /* Set port directions. */
    ports_init ();

    /* Set internal resistor for buttons */
    SETBIT(P1REN, BIT_5);
    SETBIT(P2REN, BIT_0);

    /* Initialize main timer. */
    /*Set callback function pointers. */
    callback10msec = timer10msec;
    callback1sec = timer1sec;

    //Initialize timekeeping timer (timer A0).
    timer_init();

    bReady = 1;
    /* Enable interrupts */
    _enable_interrupt();
}


void ports_init (void)
{
    /* P1.0 --- Indicator LED           */

    /* P1.1 --- UART RxD                */
    /* P1.2 --- UART TxD                */

    /* P1.3 --- Display Backlight       */
    /* P1.5 --- Display Register Select */
    /* P1.6 --- Indicator LED2 --- Currently also Lamp output         */
    /* P2.0 --- Display Chip Select     */
    /* P2.1 --- Display Clock           */
    /* P2.2 --- Display SI              */

    /* Initialize LED pin */
    P1DIR |= BIT0;
    P1DIR |= BIT6;
    /* LCD needs 4 control outputs... */
    P1DIR |= BIT5;                /*  1.5  */
    P2DIR |= BIT0 + BIT1 + BIT2;  /*  2.0  2.1  2.2 */
    P1DIR |= BIT3;

    P3DIR = 0u;
}


void timer_init (void)
{
    TA0CCTL0 = CM_0 + CCIE; //Enable Timer A0 interrupts, bit 4=1
    TA0CCR0 = 2000;     //1000 Hz
    TA0CTL = TASSEL_2 + MC_1 + ID_2;        // Timer A0 with ACLK,
}

static U8 count_10msec = 0;

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
    if (++count_10msec < 10u)
    {
        return;
    }

    count_10msec = 0u;
    callback10msec();

    if (++count_1sec < 100u)
    {
        return;
    }

    callback1sec();
    count_1sec = 0;
}

void wait_msec (U16 ms)
{
    U16 t;
    for (t = ms; t > 0; t--)
    {
        /* For 8MHz calibration */
        __delay_cycles (8000);
    }
}

void set_led1      (U8 b)   {if(b){SETBIT(P1OUT, BIT0);} else {CLRBIT(P1OUT,  BIT0);}}
void set_led2      (U8 b)   {if(b){SETBIT(P1OUT, BIT6);} else {CLRBIT(P1OUT,  BIT6);}}

void set_backlight (U8 b)   {if(b){SETBIT(P1OUT, BIT3);} else {CLRBIT(P1OUT,  BIT3);}}
void set_rs        (U8 b)   {if(b){SETBIT(P1OUT, BIT5);} else {CLRBIT(P1OUT,  BIT5);}}
void set_cs        (U8 b)   {if(b){SETBIT(P2OUT, BIT0);} else {CLRBIT(P2OUT,  BIT0);}}
void set_clk       (U8 b)   {if(b){SETBIT(P2OUT, BIT1);} else {CLRBIT(P2OUT,  BIT1);}}
void set_si        (U8 b)   {if(b){SETBIT(P2OUT, BIT2);} else {CLRBIT(P2OUT,  BIT2);}}

U8 isBtnOne     (void){if (ISBIT(P2IN, BIT_4)){return 0u;} else {return 1u;}}
U8 isBtnTwo     (void){if (ISBIT(P2IN, BIT_5)){return 0u;} else {return 1u;}}

