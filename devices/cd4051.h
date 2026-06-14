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

#ifndef CD4051_H
#define CD4051_H

#include <stdio.h>
#include "pico/stdlib.h"

#define NUM_MUX_INPUTS 8

typedef enum {
    MUX_CTRL_A,
    MUX_CTRL_B,
    MUX_CTRL_C,

    NUM_MUX_CTRL_PINS
} mux_ctrl_pins_t;

typedef enum {
    MUX_COM_1,
    MUX_COM_2,

    NUM_MUXS    
} mux_com_pins_t;

const uint8_t k_num_mux_inputs_total = NUM_MUXS * NUM_MUX_INPUTS;

#ifdef __cplusplus
extern "C"
{
#endif

void cd4051Setup(int mux_a, int mux_b, int mux_c, int mux_com_1, int mux_com_2);
uint16_t cd4051SelectChannelAndRead(uint8_t adc_in_sel, uint8_t mux_in_sel);
void cd4051SelectAdcAndChannel(uint8_t adc_in_sel, uint8_t mux_in_sel);
uint16_t cd4051ReadCurrentAdc();
uint16_t cd4051ReadSelectedAdc(uint8_t input_sel);

#ifdef __cplusplus
}
#endif

#endif