#ifndef ENCODER_MAP_H
#define ENCODER_MAP_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "settings.h"

#ifdef __cplusplus
extern "C"
{
#endif

void encoderMapSetup(synth_state_t *synth_state);
void encoderMapAssignValue(int8_t value, bool accel);
int16_t encoderMapGetCount();

#ifdef __cplusplus
}
#endif

#endif
