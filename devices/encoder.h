#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"

#ifdef __cplusplus
extern "C"
{
#endif

void encoderSetup(PIO pio_n, uint sm_n, uint pin);
void encoderIrqHandler();
bool encoderAdvanced();
int encoderGetValue();
int encoderGetCountRaw();

void encoderGetPins(uint8_t pin_a);
void encoderCallback(uint gpio, uint32_t events);
bool encoderDidEventOccur();
int encoderGetDirectionValue();

#ifdef __cplusplus
}
#endif


#endif