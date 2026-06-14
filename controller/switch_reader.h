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

#ifndef SWITCH_READER_H
#define SWITCH_READER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "common/globals.h"

typedef struct switch_reader_t {

    bool state[NUMBER_OF_SWITCHES_TOTAL]            = { 0 };
    bool timeout[NUMBER_OF_SWITCHES_TOTAL]          = { 0 };
    bool state_bottom[NUMBER_OF_BOTTOM_SWITCHES]    = { 0 };
    bool shift                                      = false;
    bool encoder_switch                             = false;
    uint8_t num_all_pressed                         = 0;
    uint8_t num_keys_pressed                        = 0;
    uint8_t num_bottom_keys_pressed                 = 0;

} switch_reader_t;

#ifdef __cplusplus
extern "C"
{
#endif

void switchReaderSubmit(switch_id_t sw, bool pressed);

switch_reader_t* switchReaderGetStructPtr();

#ifdef __cplusplus
}
#endif

#endif
