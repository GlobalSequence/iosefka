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
// ---
// Much of the code in this file is borrowed from Emilie Gillet's Shruthi-1. Thanks Emilie!

#ifndef PATCH_H
#define PATCH_H

// #include "Arduino.h"
// #include "defs.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/globals.h"

typedef struct oscillator_params_t
{
    uint8_t scale;              // 0 - 255
    uint8_t octave = 2;         // 0 - 5
    uint8_t x_morph;            // 0 - 255
    uint8_t y_morph;            // 0 - 255

} oscillator_params_t;

typedef struct oscillator_shared_params_t
{
    uint8_t portamento;         // 0 - 255
    uint8_t phase_mod;          // 0 - 255
    uint8_t sync;               // 0 - 1
    uint8_t bitrate_reduce;     // 11 - 0
    uint8_t samplerate_reduce;  //
    uint8_t legato; 
    uint8_t osc_duo;

} oscillator_shared_params_t;

typedef struct mixer_params_t
{
    uint8_t osc_1_mix;          // 0 - 255
    uint8_t osc_2_mix;          // 0 - 255
    uint8_t noise_mix;          // 0 - 255

} mixer_params_t;

typedef struct filter_params_t
{
    uint8_t cutoff;             // 0 - 255
    uint8_t resonance;          // 0 - 255
    uint8_t env_amt;            // 0 - 255
    uint8_t type;               // 0 - 3
    uint8_t fm;                 // 0 - 255
    uint8_t tracking;           // 0 - 255

} filter_params_t;

typedef struct vca_params_t 
{
    uint8_t level;
    bool velocity_on;

} vca_params_t;

typedef struct delay_params_t
{
    uint8_t rate;               // 0 - 255
    uint8_t feedback;           // 0 - 255  
    uint8_t amount;             // 0 - 255
    uint8_t type;               // 0 - 3
    uint8_t input;              // 0 - 255

} delay_params_t;

typedef struct envelope_params_t
{
    uint8_t attack;             // 0 - 255
    uint8_t decay;              // 0 - 255
    uint8_t sustain;            // 0 - 255
    uint8_t release;            // 0 - 255
    uint8_t loop;               // 0 - 1

} envelope_params_t;

typedef struct lfo_params_t
{
    uint8_t rate;               // 0 - 255
    uint8_t shape;              // 0 - 4
    uint8_t phase;              // 0 - 255
    uint8_t sync;               // 0 - 3

} lfo_params_t;

typedef struct modulation_params_t
{
    uint8_t destination;        // 0 - 54
    uint8_t amount;             // 0 - 255

} modulation_params_t;

typedef struct synth_parameters_t {
     oscillator_params_t            osc[NUMBER_OF_OSCS];
     oscillator_shared_params_t     shared;
     mixer_params_t                 mixer;
     filter_params_t                filter;
     vca_params_t                   vca;
     delay_params_t                 delay; 
     envelope_params_t              env[NUMBER_OF_ENVS];
     lfo_params_t                   lfo[NUMBER_OF_LFOS];
     modulation_params_t            mod[NUM_MOD_SOURCES];

} synth_parameters_t;

typedef struct external_modulation_t {

    uint32_t clock_tick_period;
    volatile uint16_t pitch_bend_amt = 8192;
    volatile uint8_t pitch_bend_depth_send;
    uint8_t mod_wheel_amt;
    uint8_t aftertouch_amt;
    uint8_t breath_amt;
    volatile bool clock_tick_occurred;
    volatile bool start_occurred;
    volatile uint8_t p_lock_dst;
    volatile uint8_t p_lock_amt;
    volatile bool osc_reset;

} external_modulation_t;

#endif