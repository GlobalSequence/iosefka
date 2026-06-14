// Copyright (C) 2026 Cory Oleson <globeseq@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
