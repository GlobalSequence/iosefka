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

} vca_params_t;

typedef struct delay_params_t
{
    uint8_t rate;               // 0 - 255
    uint8_t feedback;           // 0 - 255  
    uint8_t amount;             // 0 - 255
    uint8_t diffusion;          // 0 - 255
    uint8_t filter;             // 0 - 255

} delay_params_t;

typedef struct envelope_params_t
{
    uint8_t attack;             // 0 - 255
    uint8_t decay;              // 0 - 255
    uint8_t sustain;            // 0 - 255
    uint8_t release;            // 0 - 255

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

// typedef struct midi_config_params_t
// {
//     // uint8_t velocity;           // 0 - 255
//     // uint8_t mod_wheel;          // 0 - 255
//     // uint8_t aftertouch;         // 0 - 255


// } midi_config_params_t;

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
    //  midi_config_params_t           midi;

} synth_parameters_t;

typedef struct external_modulation_t {

    uint32_t clock_tick_period;
    volatile uint16_t pitch_bend_amt = 8192;
    volatile uint8_t pitch_bend_depth_send;
    uint8_t mod_wheel_amt;
    uint8_t aftertouch_amt;
    volatile bool clock_tick_occurred;
    volatile bool start_occurred;

} external_modulation_t;

#endif