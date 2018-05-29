/*
 * Lamp.h
 *
 *  Created on: 28. mai 2018
 *      Author: JRE
 */

#ifndef LAMP_H_
#define LAMP_H_

#include "typedefs.h"
#include "register.h"

typedef struct
{
    U8 second;
    U8 minute;
    U8 hour;
} time_obj;

Public void lamp_cyclic(void);
Public void lamp_init(void);

Public void handleButtonPress(Button b);

#endif /* LAMP_H_ */
