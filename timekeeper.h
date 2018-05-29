/*
 * timekeeper.h
 *
 *  Created on: 29. mai 2018
 *      Author: JRE
 */

#ifndef TIMEKEEPER_H_
#define TIMEKEEPER_H_

#include "typedefs.h"

typedef struct
{
    U8 second;
    U8 minute;
    U8 hour;
} time_T;

extern void timekeeper_increment(time_T * timekeeper);
extern void timekeeper_addToTimer(const time_T * value, time_T * dest);
extern void timekeeper_subtractFromTimer(const time_T * value, time_T * dest);
extern void timekeeper_getTimerString(time_T * t, char * dest_str);


#endif /* TIMEKEEPER_H_ */
