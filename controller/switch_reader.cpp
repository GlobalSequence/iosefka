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

#include "switch_reader.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/test.h"
#include "common/globals.h"

static switch_reader_t sr_struct;
static const unsigned long long k_switch_held_timeout = 1000000;
static unsigned long long time_switch_was_pressed[NUMBER_OF_SWITCHES_TOTAL];


void switchReaderSubmit(switch_id_t sw, bool pressed) {

    sr_struct.state[sw] = pressed;

    for(int i = 0; i < NUMBER_OF_SWITCHES_TOTAL; i++) {

        bool sw_held = sr_struct.state[i];
        uint8_t switches_all, keys_all, keys_bottom;

        if(sw_held) {

            switches_all++;

            if(
                i == SW_B0 ||
                i == SW_B1 ||
                i == SW_B2 ||
                i == SW_B3 ||
                i == SW_B4 ||
                i == SW_B5 ||
                i == SW_B6 ||
                i == SW_B7 ||
                i == SW_T0 ||
                i == SW_T1 ||
                i == SW_T2 ||
                i == SW_T3 ||
                i == SW_T4 
            ) keys_all++;

            if(
                i == SW_B0 ||
                i == SW_B1 ||
                i == SW_B2 ||
                i == SW_B3 ||
                i == SW_B4 ||
                i == SW_B5 ||
                i == SW_B6 ||
                i == SW_B7 
            ) keys_bottom++;

        }

        sr_struct.num_all_pressed = switches_all;
        sr_struct.num_keys_pressed = keys_all;
        sr_struct.num_bottom_keys_pressed = keys_bottom;

    }

    if(sw == SW_B0) sr_struct.state_bottom[0] = pressed;
    if(sw == SW_B1) sr_struct.state_bottom[1] = pressed;
    if(sw == SW_B2) sr_struct.state_bottom[2] = pressed;
    if(sw == SW_B3) sr_struct.state_bottom[3] = pressed;
    if(sw == SW_B4) sr_struct.state_bottom[4] = pressed;
    if(sw == SW_B5) sr_struct.state_bottom[5] = pressed;
    if(sw == SW_B6) sr_struct.state_bottom[6] = pressed;
    if(sw == SW_B7) sr_struct.state_bottom[7] = pressed;

    if(sw == SW_SHIFT) sr_struct.shift = pressed;
    if(sw == SW_ENCODER) sr_struct.encoder_switch = pressed;

    unsigned long long now = time_us_64();

    if(pressed) {
        
        sr_struct.timeout[sw] = false;

        time_switch_was_pressed[sw] = now;

    } else {

        if(now - time_switch_was_pressed[sw] >= k_switch_held_timeout) {

            sr_struct.timeout[sw] = true;

        }

    }

}

switch_reader_t* switchReaderGetStructPtr() {

    return &sr_struct;

}