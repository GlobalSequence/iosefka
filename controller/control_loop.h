#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H

#include <stdio.h>
#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

void controlLoopUpdateByFlag(bool flag);

#ifdef __cplusplus
}
#endif

#endif