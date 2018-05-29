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


extern void lamp_cyclic(void);
extern void lamp_init(void);

extern void handleButtonPress(Button b);

#endif /* LAMP_H_ */
