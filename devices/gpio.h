#ifndef GPIO_H
#define GPIO_H

#include <stdio.h>
#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

void gpioSetup(int a, int b);
uint8_t* gpioGetStateValPointer();
void gpioSetTwo();

#ifdef __cplusplus
}
#endif

#endif
