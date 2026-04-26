#ifndef VERSION_DATA_H
#define VERSION_DATA_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "../common/test.h"

#include "common/globals.h"
#include "sequencer/sequencer.h"
#include "controller/settings.h"

// ##########################################################################
// version 1.0.2 data
// ##########################################################################

// added new mod destination MOD_DST_MIX_OSCS

typedef enum mod_destinations_v1_0_2_t {
    
    v1_0_2_MOD_DST_NONE,

    v1_0_2_MOD_DST_OSC_BOTH_PITCH,
    v1_0_2_MOD_DST_OSC_BOTH_PITCH_FINE,
    v1_0_2_MOD_DST_OSC_BOTH_OCTAVE,
    v1_0_2_MOD_DST_OSC_BOTH_X_MORPH,
    v1_0_2_MOD_DST_OSC_BOTH_Y_MORPH,

    v1_0_2_MOD_DST_OSC_1_PITCH,
    v1_0_2_MOD_DST_OSC_1_PITCH_FINE,
    v1_0_2_MOD_DST_OSC_1_OCTAVE,
    v1_0_2_MOD_DST_OSC_1_X_MORPH,
    v1_0_2_MOD_DST_OSC_1_Y_MORPH,

    v1_0_2_MOD_DST_OSC_2_PITCH,
    v1_0_2_MOD_DST_OSC_2_PITCH_FINE,
    v1_0_2_MOD_DST_OSC_2_OCTAVE,
    v1_0_2_MOD_DST_OSC_2_X_MORPH,
    v1_0_2_MOD_DST_OSC_2_Y_MORPH,

    v1_0_2_MOD_DST_PORTAMENTO,
    v1_0_2_MOD_DST_PHASE_MOD,
    v1_0_2_MOD_DST_SYNC,
    v1_0_2_MOD_DST_BITRATE_REDUCE,
    v1_0_2_MOD_DST_SAMPLERATE_REDUCE,

    v1_0_2_MOD_DST_MIX_OSC_1_MIX,
    v1_0_2_MOD_DST_MIX_OSC_2_MIX,
    v1_0_2_MOD_DST_MIX_NOISE_MIX,
    v1_0_2_MOD_DST_MIX_OSCS,             // NEW

    v1_0_2_MOD_DST_FILTER_CUTOFF,
    v1_0_2_MOD_DST_FILTER_RESONANCE,
    v1_0_2_MOD_DST_FILTER_ENV_AMT,
    v1_0_2_MOD_DST_FILTER_TYPE,
    v1_0_2_MOD_DST_FILTER_FM,
    v1_0_2_MOD_DST_FILTER_TRACKING,

    v1_0_2_MOD_DST_VCA, // drone mode uses this parameter

    v1_0_2_MOD_DST_DELAY_RATE,
    v1_0_2_MOD_DST_DELAY_FEEDBACK,
    v1_0_2_MOD_DST_DELAY_AMOUNT,
    v1_0_2_MOD_DST_DELAY_TYPE,
    v1_0_2_MOD_DST_DELAY_INPUT,
    
    v1_0_2_MOD_DST_ENV_1_ATTACK,
    v1_0_2_MOD_DST_ENV_1_DECAY,
    v1_0_2_MOD_DST_ENV_1_SUSTAIN,
    v1_0_2_MOD_DST_ENV_1_RELEASE,
    v1_0_2_MOD_DST_ENV_1_AMOUNT,
    v1_0_2_MOD_DST_ENV_1_LOOP,  
    v1_0_2_MOD_DST_ENV_1_DST,
          
    v1_0_2_MOD_DST_ENV_2_ATTACK,
    v1_0_2_MOD_DST_ENV_2_DECAY,
    v1_0_2_MOD_DST_ENV_2_SUSTAIN,
    v1_0_2_MOD_DST_ENV_2_RELEASE,
    v1_0_2_MOD_DST_ENV_2_AMOUNT,
    v1_0_2_MOD_DST_ENV_2_LOOP,
    v1_0_2_MOD_DST_ENV_2_DST,

    v1_0_2_MOD_DST_ENV_3_ATTACK,
    v1_0_2_MOD_DST_ENV_3_DECAY,
    v1_0_2_MOD_DST_ENV_3_SUSTAIN,
    v1_0_2_MOD_DST_ENV_3_RELEASE,
    v1_0_2_MOD_DST_ENV_3_AMOUNT,
    v1_0_2_MOD_DST_ENV_3_LOOP,
    v1_0_2_MOD_DST_ENV_3_DST,

    v1_0_2_MOD_DST_LFO_1_RATE,
    v1_0_2_MOD_DST_LFO_1_SHAPE,
    v1_0_2_MOD_DST_LFO_1_PHASE,
    v1_0_2_MOD_DST_LFO_1_SYNC,
    v1_0_2_MOD_DST_LFO_1_AMOUNT,
    v1_0_2_MOD_DST_LFO_1_DST,

    v1_0_2_MOD_DST_LFO_2_RATE,
    v1_0_2_MOD_DST_LFO_2_SHAPE,
    v1_0_2_MOD_DST_LFO_2_PHASE,
    v1_0_2_MOD_DST_LFO_2_SYNC,
    v1_0_2_MOD_DST_LFO_2_AMOUNT,
    v1_0_2_MOD_DST_LFO_2_DST,

    v1_0_2_MOD_DST_LFO_3_RATE,
    v1_0_2_MOD_DST_LFO_3_SHAPE,
    v1_0_2_MOD_DST_LFO_3_PHASE,
    v1_0_2_MOD_DST_LFO_3_SYNC,
    v1_0_2_MOD_DST_LFO_3_AMOUNT,
    v1_0_2_MOD_DST_LFO_3_DST, 

    v1_0_2_NUM_MOD_DESTINATIONS

} mod_destinations_v1_0_2_t;

// ##########################################################################
// version 1.0.1 data
// ##########################################################################

// _v1_0_1

typedef enum mod_destinations_v1_0_1_t {
    V1_0_1_MOD_DST_NONE,
    V1_0_1_MOD_DST_OSC_1_PITCH,
    V1_0_1_MOD_DST_OSC_1_PITCH_FINE,
    V1_0_1_MOD_DST_OSC_1_OCTAVE,
    V1_0_1_MOD_DST_OSC_1_X_MORPH,
    V1_0_1_MOD_DST_OSC_1_Y_MORPH,
    V1_0_1_MOD_DST_OSC_2_PITCH,
    V1_0_1_MOD_DST_OSC_2_PITCH_FINE,
    V1_0_1_MOD_DST_OSC_2_OCTAVE,
    V1_0_1_MOD_DST_OSC_2_X_MORPH,
    V1_0_1_MOD_DST_OSC_2_Y_MORPH,
    V1_0_1_MOD_DST_PORTAMENTO,
    V1_0_1_MOD_DST_PHASE_MOD,
    V1_0_1_MOD_DST_SYNC,
    V1_0_1_MOD_DST_BITRATE_REDUCE,
    V1_0_1_MOD_DST_SAMPLERATE_REDUCE,
    V1_0_1_MOD_DST_MIX_OSC_1_MIX,
    V1_0_1_MOD_DST_MIX_OSC_2_MIX,
    V1_0_1_MOD_DST_MIX_NOISE_MIX,
    V1_0_1_MOD_DST_FILTER_CUTOFF,
    V1_0_1_MOD_DST_FILTER_RESONANCE,
    V1_0_1_MOD_DST_FILTER_ENV_AMT,
    V1_0_1_MOD_DST_FILTER_TYPE,
    V1_0_1_MOD_DST_FILTER_FM,
    V1_0_1_MOD_DST_FILTER_TRACKING,
    V1_0_1_MOD_DST_DELAY_RATE,
    V1_0_1_MOD_DST_DELAY_FEEDBACK,
    V1_0_1_MOD_DST_DELAY_AMOUNT,
    V1_0_1_MOD_DST_DELAY_TYPE,              // NEW
    V1_0_1_MOD_DST_DELAY_INPUT,             // NEW
    V1_0_1_MOD_DST_ENV_1_ATTACK,
    V1_0_1_MOD_DST_ENV_1_DECAY,
    V1_0_1_MOD_DST_ENV_1_SUSTAIN,
    V1_0_1_MOD_DST_ENV_1_RELEASE,
    V1_0_1_MOD_DST_ENV_1_AMOUNT,
    V1_0_1_MOD_DST_ENV_1_LOOP,             
    V1_0_1_MOD_DST_ENV_2_ATTACK,
    V1_0_1_MOD_DST_ENV_2_DECAY,
    V1_0_1_MOD_DST_ENV_2_SUSTAIN,
    V1_0_1_MOD_DST_ENV_2_RELEASE,
    V1_0_1_MOD_DST_ENV_2_AMOUNT,
    V1_0_1_MOD_DST_ENV_2_LOOP,
    V1_0_1_MOD_DST_ENV_3_ATTACK,
    V1_0_1_MOD_DST_ENV_3_DECAY,
    V1_0_1_MOD_DST_ENV_3_SUSTAIN,
    V1_0_1_MOD_DST_ENV_3_RELEASE,
    V1_0_1_MOD_DST_ENV_3_AMOUNT,
    V1_0_1_MOD_DST_ENV_3_LOOP,
    V1_0_1_MOD_DST_LFO_1_RATE,
    V1_0_1_MOD_DST_LFO_1_SHAPE,
    V1_0_1_MOD_DST_LFO_1_PHASE,
    V1_0_1_MOD_DST_LFO_1_SYNC,
    V1_0_1_MOD_DST_LFO_1_AMOUNT,
    V1_0_1_MOD_DST_LFO_2_RATE,
    V1_0_1_MOD_DST_LFO_2_SHAPE,
    V1_0_1_MOD_DST_LFO_2_PHASE,
    V1_0_1_MOD_DST_LFO_2_SYNC,
    V1_0_1_MOD_DST_LFO_2_AMOUNT,
    V1_0_1_MOD_DST_LFO_3_RATE,
    V1_0_1_MOD_DST_LFO_3_SHAPE,
    V1_0_1_MOD_DST_LFO_3_PHASE,
    V1_0_1_MOD_DST_LFO_3_SYNC,
    V1_0_1_MOD_DST_LFO_3_AMOUNT,
    V1_0_1_MOD_DST_VCA, // drone mode uses this parameter

    V1_0_1_MOD_DST_ENV_1_DST,
    V1_0_1_MOD_DST_ENV_2_DST,
    V1_0_1_MOD_DST_ENV_3_DST,
    V1_0_1_MOD_DST_LFO_1_DST,
    V1_0_1_MOD_DST_LFO_2_DST,
    V1_0_1_MOD_DST_LFO_3_DST,

    V1_0_1_MOD_DST_OSC_BOTH_PITCH,
    V1_0_1_MOD_DST_OSC_BOTH_PITCH_FINE,
    V1_0_1_MOD_DST_OSC_BOTH_OCTAVE,
    V1_0_1_MOD_DST_OSC_BOTH_X_MORPH,
    V1_0_1_MOD_DST_OSC_BOTH_Y_MORPH,

    V1_0_1_NUM_MOD_DESTINATIONS

} mod_destinations_v1_0_1_t;

// ##########################################################################
// version 1.0.0 data
// ##########################################################################

// _v1_0_0

typedef enum mod_sources_v1_0_0_t {
    V1_0_0_MOD_SRC_ENV_1,
    V1_0_0_MOD_SRC_ENV_2,
    V1_0_0_MOD_SRC_ENV_3,
    V1_0_0_MOD_SRC_LFO_1,
    V1_0_0_MOD_SRC_LFO_2,
    V1_0_0_MOD_SRC_LFO_3,
    V1_0_0_MOD_SRC_VELOCITY,
    V1_0_0_MOD_SRC_MOD_WHEEL,
    V1_0_0_MOD_SRC_AFTERTOUCH,
    V1_0_0_MOD_SRC_NOTE,
    V1_0_0_MOD_SRC_BREATH,

    V1_0_0_NUM_MOD_SOURCES
} mod_sources_v1_0_0_t;

typedef struct machine_settings_v1_0_0_t {
    // MIDI settings
    uint8_t channel;
    uint8_t pitch_bend_depth;
    bool program_change_rx = true;
    bool clock_rx = true;
    bool transport_rx = true;
    bool out_thru = MIDI_OUTPUT_OUT;
    // view settings
    bool latch = false;
} machine_setting_v1_0_0_t;

typedef struct oscillator_shared_params_v1_0_0_t
{
    uint8_t portamento;         // 0 - 255
    uint8_t phase_mod;          // 0 - 255
    uint8_t sync;               // 0 - 1
    uint8_t bitrate_reduce;     // 11 - 0
    uint8_t samplerate_reduce;  //
    uint8_t legato; 
    uint8_t osc_duo;

} oscillator_shared_params_v1_0_0_t;

typedef struct envelope_params_v1_0_0_t
{
    uint8_t attack;             // 0 - 255
    uint8_t decay;              // 0 - 255
    uint8_t sustain;            // 0 - 255
    uint8_t release;            // 0 - 255
    uint8_t loop;               // 0 - 1

} envelope_params_v1_0_0_t;

typedef struct vca_params_v1_0_0_t 
{
    uint8_t level;
    bool velocity_on;

} vca_params_v1_0_0_t;

typedef struct synth_parameter_v1_0_0_t {
     oscillator_params_t                    osc[NUMBER_OF_OSCS];
     oscillator_shared_params_v1_0_0_t      shared;
     mixer_params_t                         mixer;
     filter_params_t                        filter;
     vca_params_v1_0_0_t                    vca;
     delay_params_t                         delay; 
     envelope_params_v1_0_0_t               env[NUMBER_OF_ENVS];
     lfo_params_t                           lfo[NUMBER_OF_LFOS];
     modulation_params_t                    mod[V1_0_0_NUM_MOD_SOURCES];

} synth_parameters_v1_0_0_t;

typedef struct synth_storable_v1_0_0_t {

    preset_name_t preset_name;
    synth_parameters_v1_0_0_t patch;
    pattern_data_t seq_pattern_data;
    step_data_t step_data;
    pattern_gen_t pattern_gen;
    user_param_t user_param;

} synth_storable_v1_0_0_t;

typedef enum mod_destinations_v1_0_0_t {
    V1_0_0_MOD_DST_NONE,
    V1_0_0_MOD_DST_OSC_1_PITCH,
    V1_0_0_MOD_DST_OSC_1_PITCH_FINE,
    V1_0_0_MOD_DST_OSC_1_OCTAVE,
    V1_0_0_MOD_DST_OSC_1_X_MORPH,
    V1_0_0_MOD_DST_OSC_1_Y_MORPH,
    V1_0_0_MOD_DST_OSC_2_PITCH,
    V1_0_0_MOD_DST_OSC_2_PITCH_FINE,
    V1_0_0_MOD_DST_OSC_2_OCTAVE,
    V1_0_0_MOD_DST_OSC_2_X_MORPH,
    V1_0_0_MOD_DST_OSC_2_Y_MORPH,
    V1_0_0_MOD_DST_PORTAMENTO,
    V1_0_0_MOD_DST_PHASE_MOD,
    V1_0_0_MOD_DST_SYNC,
    V1_0_0_MOD_DST_BITRATE_REDUCE,          // NEW
    V1_0_0_MOD_DST_SAMPLERATE_REDUCE,       // NEW
    V1_0_0_MOD_DST_MIX_OSC_1_MIX,
    V1_0_0_MOD_DST_MIX_OSC_2_MIX,
    V1_0_0_MOD_DST_MIX_NOISE_MIX,
    V1_0_0_MOD_DST_FILTER_CUTOFF,
    V1_0_0_MOD_DST_FILTER_RESONANCE,
    V1_0_0_MOD_DST_FILTER_ENV_AMT,
    V1_0_0_MOD_DST_FILTER_TYPE,
    V1_0_0_MOD_DST_FILTER_FM,
    V1_0_0_MOD_DST_FILTER_TRACKING,
    V1_0_0_MOD_DST_DELAY_RATE,
    V1_0_0_MOD_DST_DELAY_FEEDBACK,
    V1_0_0_MOD_DST_DELAY_AMOUNT,
    // V1_0_0_MOD_DST_DELAY_DIFFUSION,
    // V1_0_0_MOD_DST_DELAY_FILTER,
    // V1_0_0_MOD_DST_DELAY_STEREO,
    V1_0_0_MOD_DST_ENV_1_ATTACK,
    V1_0_0_MOD_DST_ENV_1_DECAY,
    V1_0_0_MOD_DST_ENV_1_SUSTAIN,
    V1_0_0_MOD_DST_ENV_1_RELEASE,
    V1_0_0_MOD_DST_ENV_1_AMOUNT,
    V1_0_0_MOD_DST_ENV_1_LOOP,              // NEW
    V1_0_0_MOD_DST_ENV_2_ATTACK,
    V1_0_0_MOD_DST_ENV_2_DECAY,
    V1_0_0_MOD_DST_ENV_2_SUSTAIN,
    V1_0_0_MOD_DST_ENV_2_RELEASE,
    V1_0_0_MOD_DST_ENV_2_AMOUNT,
    V1_0_0_MOD_DST_ENV_2_LOOP,              // NEW
    V1_0_0_MOD_DST_ENV_3_ATTACK,
    V1_0_0_MOD_DST_ENV_3_DECAY,
    V1_0_0_MOD_DST_ENV_3_SUSTAIN,
    V1_0_0_MOD_DST_ENV_3_RELEASE,
    V1_0_0_MOD_DST_ENV_3_AMOUNT,
    V1_0_0_MOD_DST_ENV_3_LOOP,              // NEW
    V1_0_0_MOD_DST_LFO_1_RATE,
    V1_0_0_MOD_DST_LFO_1_SHAPE,
    V1_0_0_MOD_DST_LFO_1_PHASE,
    V1_0_0_MOD_DST_LFO_1_SYNC,
    V1_0_0_MOD_DST_LFO_1_AMOUNT,
    V1_0_0_MOD_DST_LFO_2_RATE,
    V1_0_0_MOD_DST_LFO_2_SHAPE,
    V1_0_0_MOD_DST_LFO_2_PHASE,
    V1_0_0_MOD_DST_LFO_2_SYNC,
    V1_0_0_MOD_DST_LFO_2_AMOUNT,
    V1_0_0_MOD_DST_LFO_3_RATE,
    V1_0_0_MOD_DST_LFO_3_SHAPE,
    V1_0_0_MOD_DST_LFO_3_PHASE,
    V1_0_0_MOD_DST_LFO_3_SYNC,
    V1_0_0_MOD_DST_LFO_3_AMOUNT,
    V1_0_0_MOD_DST_VCA, // drone mode uses this parameter

    V1_0_0_MOD_DST_ENV_1_DST,
    V1_0_0_MOD_DST_ENV_2_DST,
    V1_0_0_MOD_DST_ENV_3_DST,
    V1_0_0_MOD_DST_LFO_1_DST,
    V1_0_0_MOD_DST_LFO_2_DST,
    V1_0_0_MOD_DST_LFO_3_DST,

    V1_0_0_MOD_DST_OSC_BOTH_PITCH,
    V1_0_0_MOD_DST_OSC_BOTH_PITCH_FINE,
    V1_0_0_MOD_DST_OSC_BOTH_OCTAVE,
    V1_0_0_MOD_DST_OSC_BOTH_X_MORPH,
    V1_0_0_MOD_DST_OSC_BOTH_Y_MORPH,

    V1_0_0_NUM_MOD_DESTINATIONS

} mod_destinations_v1_0_0_t;



// ##########################################################################
// version 0.1.2 data
// ##########################################################################

// _v0_1_2

typedef enum mod_sources_v0_1_2_t {
    V0_1_2_MOD_SRC_ENV_1,
    V0_1_2_MOD_SRC_ENV_2,
    V0_1_2_MOD_SRC_ENV_3,
    V0_1_2_MOD_SRC_LFO_1,
    V0_1_2_MOD_SRC_LFO_2,
    V0_1_2_MOD_SRC_LFO_3,
    V0_1_2_MOD_SRC_VELOCITY,
    V0_1_2_MOD_SRC_MOD_WHEEL,
    V0_1_2_MOD_SRC_AFTERTOUCH,

    V0_1_2_NUM_MOD_SOURCES
} mod_sources_v0_1_2_t;

typedef struct machine_settings_v0_1_2_t {
    uint8_t channel;
    uint8_t pitch_bend_depth;
    bool program_change_rx = true;
    bool clock_rx = true;
    bool transport_rx = true;
    bool out_thru = MIDI_OUTPUT_OUT;
} machine_settings_v0_1_2_t;

typedef struct oscillator_shared_params_v0_1_2_t
{
    uint8_t portamento;         // 0 - 255
    uint8_t phase_mod;          // 0 - 255
    uint8_t sync;               // 0 - 1
    uint8_t bitrate_reduce;     // 11 - 0
    uint8_t samplerate_reduce;  //

} oscillator_shared_params_v0_1_2_t;

typedef struct envelope_params_v0_1_2_t
{
    uint8_t attack;             // 0 - 255
    uint8_t decay;              // 0 - 255
    uint8_t sustain;            // 0 - 255
    uint8_t release;            // 0 - 255

} envelope_params_v0_1_2_t;

/* typedef struct lfo_params_v0_1_2_t
{
    uint8_t rate;               // 0 - 255
    uint8_t shape;              // 0 - 4
    uint8_t phase;              // 0 - 255
    uint8_t sync;               // 0 - 3

} lfo_params_v0_1_2_t; */

typedef struct vca_params_v0_1_2_t 
{
    uint8_t level;

} vca_params_v0_1_2_t;

typedef struct synth_parameters_v0_1_2_t {
     oscillator_params_t                    osc[NUMBER_OF_OSCS];
     oscillator_shared_params_v0_1_2_t      shared;
     mixer_params_t                         mixer;
     filter_params_t                        filter;
     vca_params_v0_1_2_t                    vca;
     delay_params_t                         delay; 
     envelope_params_v0_1_2_t               env[NUMBER_OF_ENVS];
     lfo_params_t                           lfo[NUMBER_OF_LFOS];
     modulation_params_t                    mod[V0_1_2_NUM_MOD_SOURCES];

} synth_parameters_v0_1_2_t;

typedef struct synth_storable_v0_1_2_t {

    preset_name_t preset_name;
    synth_parameters_v0_1_2_t patch;
    pattern_data_t seq_pattern_data;
    step_data_t step_data;
    pattern_gen_t pattern_gen;
    user_param_t user_param;

} synth_storable_v0_1_2_t;

typedef enum mod_destinations_v0_1_2_t {
    V0_1_2_MOD_DST_NONE,
    V0_1_2_MOD_DST_OSC_1_PITCH,
    V0_1_2_MOD_DST_OSC_1_PITCH_FINE,
    V0_1_2_MOD_DST_OSC_1_OCTAVE,
    V0_1_2_MOD_DST_OSC_1_X_MORPH,
    V0_1_2_MOD_DST_OSC_1_Y_MORPH,
    V0_1_2_MOD_DST_OSC_2_PITCH,
    V0_1_2_MOD_DST_OSC_2_PITCH_FINE,
    V0_1_2_MOD_DST_OSC_2_OCTAVE,
    V0_1_2_MOD_DST_OSC_2_X_MORPH,
    V0_1_2_MOD_DST_OSC_2_Y_MORPH,
    V0_1_2_MOD_DST_PORTAMENTO,
    V0_1_2_MOD_DST_PHASE_MOD,
    V0_1_2_MOD_DST_SYNC,
    V0_1_2_MOD_DST_MIX_OSC_1_MIX,
    V0_1_2_MOD_DST_MIX_OSC_2_MIX,
    V0_1_2_MOD_DST_MIX_NOISE_MIX,
    V0_1_2_MOD_DST_FILTER_CUTOFF,
    V0_1_2_MOD_DST_FILTER_RESONANCE,
    V0_1_2_MOD_DST_FILTER_ENV_AMT,
    V0_1_2_MOD_DST_FILTER_TYPE,
    V0_1_2_MOD_DST_FILTER_FM,
    V0_1_2_MOD_DST_FILTER_TRACKING,
    V0_1_2_MOD_DST_DELAY_RATE,
    V0_1_2_MOD_DST_DELAY_FEEDBACK,
    V0_1_2_MOD_DST_DELAY_AMOUNT,
    V0_1_2_MOD_DST_DELAY_DIFFUSION,
    V0_1_2_MOD_DST_DELAY_FILTER,
    V0_1_2_MOD_DST_DELAY_STEREO,
    V0_1_2_MOD_DST_ENV_1_ATTACK,
    V0_1_2_MOD_DST_ENV_1_DECAY,
    V0_1_2_MOD_DST_ENV_1_SUSTAIN,
    V0_1_2_MOD_DST_ENV_1_RELEASE,
    V0_1_2_MOD_DST_ENV_1_AMOUNT,
    V0_1_2_MOD_DST_ENV_2_ATTACK,
    V0_1_2_MOD_DST_ENV_2_DECAY,
    V0_1_2_MOD_DST_ENV_2_SUSTAIN,
    V0_1_2_MOD_DST_ENV_2_RELEASE,
    V0_1_2_MOD_DST_ENV_2_AMOUNT,
    V0_1_2_MOD_DST_ENV_3_ATTACK,
    V0_1_2_MOD_DST_ENV_3_DECAY,
    V0_1_2_MOD_DST_ENV_3_SUSTAIN,
    V0_1_2_MOD_DST_ENV_3_RELEASE,
    V0_1_2_MOD_DST_ENV_3_AMOUNT,
    V0_1_2_MOD_DST_LFO_1_RATE,
    V0_1_2_MOD_DST_LFO_1_SHAPE,
    V0_1_2_MOD_DST_LFO_1_PHASE,
    V0_1_2_MOD_DST_LFO_1_SYNC,
    V0_1_2_MOD_DST_LFO_1_AMOUNT,
    V0_1_2_MOD_DST_LFO_2_RATE,
    V0_1_2_MOD_DST_LFO_2_SHAPE,
    V0_1_2_MOD_DST_LFO_2_PHASE,
    V0_1_2_MOD_DST_LFO_2_SYNC,
    V0_1_2_MOD_DST_LFO_2_AMOUNT,
    V0_1_2_MOD_DST_LFO_3_RATE,
    V0_1_2_MOD_DST_LFO_3_SHAPE,
    V0_1_2_MOD_DST_LFO_3_PHASE,
    V0_1_2_MOD_DST_LFO_3_SYNC,
    V0_1_2_MOD_DST_LFO_3_AMOUNT,
    V0_1_2_MOD_DST_VCA, // drone mode uses this parameter

    V0_1_2_MOD_DST_ENV_1_DST,
    V0_1_2_MOD_DST_ENV_2_DST,
    V0_1_2_MOD_DST_ENV_3_DST,
    V0_1_2_MOD_DST_LFO_1_DST,
    V0_1_2_MOD_DST_LFO_2_DST,
    V0_1_2_MOD_DST_LFO_3_DST,

    V0_1_2_MOD_DST_OSC_BOTH_PITCH,
    V0_1_2_MOD_DST_OSC_BOTH_PITCH_FINE,
    V0_1_2_MOD_DST_OSC_BOTH_OCTAVE,
    V0_1_2_MOD_DST_OSC_BOTH_X_MORPH,
    V0_1_2_MOD_DST_OSC_BOTH_Y_MORPH,

    V0_1_2_NUM_MOD_DESTINATIONS

} mod_destinations_v0_1_2_t;


// ##########################################################################
// version 0.1.1 data
// ##########################################################################

typedef enum mod_destinations_v0_1_1_t {
    V0_1_1_MOD_DST_NONE,
    V0_1_1_MOD_DST_OSC_1_PITCH,
    V0_1_1_MOD_DST_OSC_1_OCTAVE,
    V0_1_1_MOD_DST_OSC_1_X_MORPH,
    V0_1_1_MOD_DST_OSC_1_Y_MORPH,
    V0_1_1_MOD_DST_OSC_2_PITCH,
    V0_1_1_MOD_DST_OSC_2_OCTAVE,
    V0_1_1_MOD_DST_OSC_2_X_MORPH,
    V0_1_1_MOD_DST_OSC_2_Y_MORPH,
    V0_1_1_MOD_DST_PORTAMENTO,
    V0_1_1_MOD_DST_PHASE_MOD,
    V0_1_1_MOD_DST_SYNC,
    V0_1_1_MOD_DST_MIX_OSC_1_MIX,
    V0_1_1_MOD_DST_MIX_OSC_2_MIX,
    V0_1_1_MOD_DST_MIX_NOISE_MIX,
    V0_1_1_MOD_DST_FILTER_CUTOFF,
    V0_1_1_MOD_DST_FILTER_RESONANCE,
    V0_1_1_MOD_DST_FILTER_ENV_AMT,
    V0_1_1_MOD_DST_FILTER_TYPE,
    V0_1_1_MOD_DST_FILTER_FM,
    V0_1_1_MOD_DST_FILTER_TRACKING,
    V0_1_1_MOD_DST_DELAY_RATE,
    V0_1_1_MOD_DST_DELAY_FEEDBACK,
    V0_1_1_MOD_DST_DELAY_AMOUNT,
    V0_1_1_MOD_DST_DELAY_DIFFUSION,
    V0_1_1_MOD_DST_DELAY_FILTER,
    V0_1_1_MOD_DST_DELAY_STEREO,
    V0_1_1_MOD_DST_ENV_1_ATTACK,
    V0_1_1_MOD_DST_ENV_1_DECAY,
    V0_1_1_MOD_DST_ENV_1_SUSTAIN,
    V0_1_1_MOD_DST_ENV_1_RELEASE,
    V0_1_1_MOD_DST_ENV_1_AMOUNT,
    V0_1_1_MOD_DST_ENV_2_ATTACK,
    V0_1_1_MOD_DST_ENV_2_DECAY,
    V0_1_1_MOD_DST_ENV_2_SUSTAIN,
    V0_1_1_MOD_DST_ENV_2_RELEASE,
    V0_1_1_MOD_DST_ENV_2_AMOUNT,
    V0_1_1_MOD_DST_ENV_3_ATTACK,
    V0_1_1_MOD_DST_ENV_3_DECAY,
    V0_1_1_MOD_DST_ENV_3_SUSTAIN,
    V0_1_1_MOD_DST_ENV_3_RELEASE,
    V0_1_1_MOD_DST_ENV_3_AMOUNT,
    V0_1_1_MOD_DST_LFO_1_RATE,
    V0_1_1_MOD_DST_LFO_1_SHAPE,
    V0_1_1_MOD_DST_LFO_1_PHASE,
    V0_1_1_MOD_DST_LFO_1_SYNC,
    V0_1_1_MOD_DST_LFO_1_AMOUNT,
    V0_1_1_MOD_DST_LFO_2_RATE,
    V0_1_1_MOD_DST_LFO_2_SHAPE,
    V0_1_1_MOD_DST_LFO_2_PHASE,
    V0_1_1_MOD_DST_LFO_2_SYNC,
    V0_1_1_MOD_DST_LFO_2_AMOUNT,
    V0_1_1_MOD_DST_LFO_3_RATE,
    V0_1_1_MOD_DST_LFO_3_SHAPE,
    V0_1_1_MOD_DST_LFO_3_PHASE,
    V0_1_1_MOD_DST_LFO_3_SYNC,
    V0_1_1_MOD_DST_LFO_3_AMOUNT,
    V0_1_1_MOD_DST_VCA, // drone mode uses this parameter

    V0_1_1_MOD_DST_ENV_1_DST,
    V0_1_1_MOD_DST_ENV_2_DST,
    V0_1_1_MOD_DST_ENV_3_DST,
    V0_1_1_MOD_DST_LFO_1_DST,
    V0_1_1_MOD_DST_LFO_2_DST,
    V0_1_1_MOD_DST_LFO_3_DST,

    V0_1_1_MOD_DST_OSC_BOTH_PITCH_FINE,
    V0_1_1_MOD_DST_OSC_BOTH_PITCH,
    V0_1_1_MOD_DST_OSC_BOTH_OCTAVE,
    V0_1_1_MOD_DST_OSC_BOTH_X_MORPH,
    V0_1_1_MOD_DST_OSC_BOTH_Y_MORPH,
    V0_1_1_MOD_DST_OSC_1_PITCH_FINE,
    V0_1_1_MOD_DST_OSC_2_PITCH_FINE,

    V0_1_1_NUM_MOD_DESTINATIONS

} mod_destinations_v0_1_1_t;

// ##########################################################################
// version 0.1.0 data
// ##########################################################################

typedef struct synth_storable_v0_1_0_t {

    synth_parameters_t patch;
    pattern_data_t seq_pattern_data;
    step_data_t step_data;
    pattern_gen_t pattern_gen;
    user_param_t user_param;

} synth_storable_v0_1_0_t;

// ##########################################################################
// version 0.0.0 data
// ##########################################################################

typedef struct oscillator_shared_params_v0_0_0_t {

    uint8_t portamento;         // 0 - 255
    uint8_t phase_mod;          // 0 - 255
    uint8_t sync;               // 0 - 1

} oscillator_shared_params_v0_0_0_t;

typedef struct synth_parameters_v0_0_0_t {
    oscillator_params_t                     osc[NUMBER_OF_OSCS];
    oscillator_shared_params_v0_0_0_t       shared;
    mixer_params_t                          mixer;
    filter_params_t                         filter;
    vca_params_t                            vca;
    delay_params_t                          delay; 
    envelope_params_t                       env[NUMBER_OF_ENVS];
    lfo_params_t                            lfo[NUMBER_OF_LFOS];
    modulation_params_t                     mod[NUM_MOD_SOURCES];
   //  midi_config_params_t           midi;

} synth_parameters_v0_0_0_t;

typedef struct step_data_v0_0_0_t {
    uint8_t note = 60;
    uint8_t velocity = 100;
    uint8_t length = 0; // steps will always have a length of 1, so 0 == 1
    uint8_t p_lock_param = 0;
    uint8_t p_lock_value = 0;
} step_data_v0_0_0_t;

typedef struct pattern_data_v0_0_0_t {
    uint16_t bpm = 120;
    uint8_t first_step = 0;
    uint8_t last_step = 7;
    uint8_t direction = FORWARD;
    uint8_t swing_amt = SWING_50;
    uint8_t clock_div = CLK_DIV_ONE;
    uint8_t transpose = 12;
    uint8_t probability = 255;
    uint8_t rotation = 0;
    step_data_v0_0_0_t step[NUMBER_OF_SEQUENCER_STEPS];
    bool step_written[NUMBER_OF_SEQUENCER_STEPS] = { false };
    bool step_active[NUMBER_OF_SEQUENCER_STEPS] = { false };

} pattern_data_v0_0_0_t;

typedef struct synth_storable_v0_0_0_t {

    synth_parameters_v0_0_0_t patch;
    pattern_data_v0_0_0_t seq_pattern_data;
    step_data_v0_0_0_t step_data;
    pattern_gen_t pattern_gen;

} synth_storable_v0_0_0_t;

#endif