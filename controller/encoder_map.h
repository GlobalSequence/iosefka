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
