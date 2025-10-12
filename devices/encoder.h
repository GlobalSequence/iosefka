#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"

/*
    // Initialization code setup for the file that includes this library
    void encoderInit() {
        encoderSetup(pio0, 0, PIN_ENCODER_A);
        encoderSetCount(0);
        encoderSetMinMax(0, 255);
    }
*/

#ifdef __cplusplus
extern "C"
{
#endif

void encoderSetup(PIO pio_n, uint sm_n, uint pin);
void encoderIrqHandler();
bool encoderAdvanced();
int encoderGetValue();
int encoderGetCountRaw();

#ifdef __cplusplus
}
#endif


#endif