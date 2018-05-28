/*
 * Lamp.h
 *
 *  Created on: 28. mai 2018
 *      Author: JRE
 */

#ifndef LAMP_H_
#define LAMP_H_

#include "typedefs.h"

typedef struct
{
    U8 second;
    U8 minute;
    U8 hour;
} timekeeper_T;

Public void lamp_cyclic(timekeeper_T * time);



#endif /* LAMP_H_ */
