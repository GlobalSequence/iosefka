#ifndef STARTUP_H
#define STARTUP_H

#include <stdio.h>
#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

void startupRun();
void startupRunCore1();

#ifdef __cplusplus
}
#endif

#endif