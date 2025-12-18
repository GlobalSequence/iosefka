#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include <stdio.h>
#include "pico/stdlib.h"

#define SCOPE_BUFFER_SIZE 256
#define SCOPE_HALF_BUFFER_SIZE 128
#define SCOPE_READ_BUFFER_SIZE 256

#ifdef __cplusplus
extern "C"
{
#endif

void oscilloscopeProcess(uint16_t* buf_ptr, bool* reset_ptr);
void oscilloscopeSetTimeDiv(uint8_t div);
void oscilloscopeSetModulo(uint8_t mod);

uint8_t* oscilloscopeGetActiveBuffer();

#ifdef __cplusplus
}
#endif

#endif
