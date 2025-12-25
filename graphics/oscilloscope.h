#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include <stdio.h>
#include "pico/stdlib.h"

#define SCOPE_BUFFER_SIZE 256
#define SCOPE_HALF_BUFFER_SIZE 128
#define SCOPE_READ_BUFFER_SIZE 256
#define NUMBER_OF_SCOPE_DIVS 4

#ifdef __cplusplus
extern "C"
{
#endif

void oscilloscopeProcess(uint16_t* buf_ptr, bool* reset_ptr);
void oscilloscopeSetTimeDivCtrl(uint8_t div);

uint8_t* oscilloscopeGetActiveBuffer();
uint8_t oscilloscopeGetTimeDivCtrl();
uint8_t oscilloscopeGetTimeDiv();

#ifdef __cplusplus
}
#endif

#endif
