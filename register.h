/*
 * register.h
 *
 *  Created on: 22. märts 2018
 *      Author: localpcadmin
 */

#ifndef REGISTER_H_
#define REGISTER_H_

#include <msp430g2553.h>
#include "typedefs.h"

#define SPECIAL_TASK_INTERVAL 5u

extern U8 bReady;

typedef void (*timer_callback)(void);


void register_init (timer_callback timer10msec, timer_callback timer1sec);
void timer_init (void);
void ports_init (void);
void wait_msec (U16 m_secs);

void set_backlight(U8 b);

void set_si(U8 b);
void set_clk(U8 b);
void set_rs(U8 b);
void set_cs(U8 b);

void set_led1       (U8 b);
void set_led2      (U8 b);


#endif /* REGISTER_H_ */
