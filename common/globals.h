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

#ifndef GLOBALS_H
#define GLOBALS_H

// ################################################################
// 
// HARDWARE GLOBALS
//
// ################################################################

#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_PATCH 0

// ################################
// SWITCHES
// ################################

// Switches 0 - 12 are 'key switches' 
#define NUMBER_OF_KEY_SWITCHES 13
#define NUMBER_OF_BOTTOM_SWITCHES 8
#define NUMBER_OF_TOP_SWITCHES 5
#define LAST_BOTTOM_SWITCH 7
#define LAST_TOP_SWITCH 12

typedef enum switch_id_t {

    SW_B7,
    SW_B6,
    SW_B5,
    SW_B4,
    SW_B3,
    SW_B2,
    SW_B1,
    SW_B0,
    SW_MOD_LFO_VIEW,
    SW_MOD_ENV_VIEW,
    SW_SEQ_VIEW,
    SW_T0,
    SW_T1,
    SW_T2,
    SW_T3,
    SW_T4,
    SW_SHIFT,
    SW_PLAY,
    SW_SYNTH_2_VIEW,
    SW_SYNTH_1_VIEW,
    SW_ENCODER,

    NUMBER_OF_SWITCHES_TOTAL,
} switch_id_t;

typedef enum view_switch_t {

    VIEW_SWITCH_SY1,
    VIEW_SWITCH_SY2,
    VIEW_SWITCH_ENV,
    VIEW_SWITCH_LFO,
    VIEW_SWITCH_SEQ,

    NUM_VIEW_SWITCHES

} view_switch_t;

// ################################
// POTENTIOMETERS
// ################################
typedef enum {

    POT_2,
    POT_1,
    POT_0,
    POT_3,
    POT_5,
    POT_4,
    POT_6,
    POT_7,
    POT_12,
    POT_11,
    POT_10,
    POT_8,
    POT_13,
    POT_9,
    POT_14,
   
    // POT_15,

    NUMBER_OF_POTENTIOMETERS
} pot_index_t;

typedef enum led_index_t {

    LED_LAYER_SYNTH_1,
    LED_LAYER_SYNTH_2,
    LED_PAGE_KBD,
    LED_PAGE_SEQ,
    LED_LAYER_ENV,
    LED_LAYER_LFO,
    LED_PAGE_SET,
    LED_LAYER_SEQ,

    NUMBER_OF_LEDS

} led_index_t;

// ################################################################
// 
// SOFTWARE GLOBALS
//
// ################################################################

// ################################
// SAVE PRESETS
// ################################

#define NUMBER_OF_PRESETS 64
#define PRESET_NAME_LENGTH 8

// ################################
// SYNTH
// ################################

#define NUMBER_OF_OSCS 2
#define NUMBER_OF_ENVS 3
#define NUMBER_OF_LFOS 3
#define SIZE_NOTE_BUFFER 48

typedef enum note_state_t {
    NOTE_OFF,
    NOTE_ON
} note_state_t;

// ################################
// AUDIO BUFFERS
// ################################

#define AUDIO_BLOCK_SIZE 128
#define DAC_SAMPLES_PER_AUDIO_BLOCK AUDIO_BLOCK_SIZE * 2
#define DAC_OUTPUT_BUF_SIZE DAC_SAMPLES_PER_AUDIO_BLOCK * 2

#define DAC_ADC_UPDATE_RATE_KHZ 96000
#define AUDIO_SAMPLE_RATE_KHZ DAC_ADC_UPDATE_RATE_KHZ / 2
#define AUDIO_RENDER_RATE_KHZ AUDIO_SAMPLE_RATE_KHZ / AUDIO_BLOCK_SIZE

// #define ADC_INPUT_BUF_SIZE 32768
#define DELAY_BUF_SIZE 32768
#define ADC_INPUT_BUF_SIZE AUDIO_BLOCK_SIZE * 2
#define SCOPE_BUF_SIZE AUDIO_BLOCK_SIZE * 2


// ################################
// KEYBOARD
// ################################

#define MIN_KEYBOARD_OCT 2
#define MAX_KEYBOARD_OCT 7
#define NUMBER_OF_NOTES_PER_OCTAVE 12
#define SEMITONES_PER_OCTAVE NUMBER_OF_NOTES_PER_OCTAVE

// ################################
// SEQUENCER
// ################################

#define MIN_BPM 30
#define MAX_BPM 300
#define MAX_SEQUENCER_STEPS 32
#define MAX_NOTE_LENGTH MAX_SEQUENCER_STEPS
#define NUMBER_OF_SEQ_STEPS_PER_WINDOW 8

// ################################
// GRAPHICS
// ################################

#define NUMBER_OF_SEQUENCER_WINDOWS MAX_SEQUENCER_STEPS / NUMBER_OF_SEQ_STEPS_PER_WINDOW

// ################################
// MIDI
// ################################

#define NUMBER_OF_MIDI_NOTES 128
#define NUMBER_OF_MIDI_CHANNELS 16
#define MIDI_RX_BUFFER_SIZE 64
#define MIDI_TX_BUFFER_SIZE 128

#define MIDI_PITCH_BEND_DEPTH_MAX 12

typedef enum midi_output_t {
    MIDI_OUTPUT_THRU,
    MIDI_OUTPUT_OUT
} midi_output_t;

// ################################
// MISC PARAM VALUES
// ################################

// the 2 octave + 1 semitone range of scale parameter
#define NOTE_SCALE_RANGE 25
#define NUMBER_OF_PARAMETERS_PER_LAYER NUMBER_OF_POTENTIOMETERS - 1
#define NUMBER_OF_USER_PARAMETERS NUMBER_OF_POTENTIOMETERS

// ################################
// MODULATION
// ################################

#define MAX_PARAM_VALUE 0xFF
#define MAX_VELOCITY_VALUE 0X80

typedef enum mod_destinations_t {
    
    MOD_DST_NONE,

    MOD_DST_OSC_BOTH_PITCH,
    MOD_DST_OSC_BOTH_PITCH_FINE,
    MOD_DST_OSC_BOTH_OCTAVE,
    MOD_DST_OSC_BOTH_X_MORPH,
    MOD_DST_OSC_BOTH_Y_MORPH,

    MOD_DST_OSC_1_PITCH,
    MOD_DST_OSC_1_PITCH_FINE,
    MOD_DST_OSC_1_OCTAVE,
    MOD_DST_OSC_1_X_MORPH,
    MOD_DST_OSC_1_Y_MORPH,

    MOD_DST_OSC_2_PITCH,
    MOD_DST_OSC_2_PITCH_FINE,
    MOD_DST_OSC_2_OCTAVE,
    MOD_DST_OSC_2_X_MORPH,
    MOD_DST_OSC_2_Y_MORPH,

    MOD_DST_PORTAMENTO,
    MOD_DST_PHASE_MOD,
    MOD_DST_SYNC,
    MOD_DST_BITRATE_REDUCE,
    MOD_DST_SAMPLERATE_REDUCE,

    MOD_DST_MIX_OSC_1_MIX,
    MOD_DST_MIX_OSC_2_MIX,
    MOD_DST_MIX_NOISE_MIX,
    MOD_DST_MIX_OSCS,             // NEW

    MOD_DST_FILTER_CUTOFF,
    MOD_DST_FILTER_RESONANCE,
    MOD_DST_FILTER_ENV_AMT,
    MOD_DST_FILTER_TYPE,
    MOD_DST_FILTER_FM,
    MOD_DST_FILTER_TRACKING,

    MOD_DST_VCA, // drone mode uses this parameter

    MOD_DST_DELAY_RATE,
    MOD_DST_DELAY_FEEDBACK,
    MOD_DST_DELAY_AMOUNT,
    MOD_DST_DELAY_TYPE,
    MOD_DST_DELAY_INPUT,
    
    MOD_DST_ENV_1_ATTACK,
    MOD_DST_ENV_1_DECAY,
    MOD_DST_ENV_1_SUSTAIN,
    MOD_DST_ENV_1_RELEASE,
    MOD_DST_ENV_1_AMOUNT,
    MOD_DST_ENV_1_LOOP,  
    MOD_DST_ENV_1_DST,
          
    MOD_DST_ENV_2_ATTACK,
    MOD_DST_ENV_2_DECAY,
    MOD_DST_ENV_2_SUSTAIN,
    MOD_DST_ENV_2_RELEASE,
    MOD_DST_ENV_2_AMOUNT,
    MOD_DST_ENV_2_LOOP,
    MOD_DST_ENV_2_DST,

    MOD_DST_ENV_3_ATTACK,
    MOD_DST_ENV_3_DECAY,
    MOD_DST_ENV_3_SUSTAIN,
    MOD_DST_ENV_3_RELEASE,
    MOD_DST_ENV_3_AMOUNT,
    MOD_DST_ENV_3_LOOP,
    MOD_DST_ENV_3_DST,

    MOD_DST_LFO_1_RATE,
    MOD_DST_LFO_1_SHAPE,
    MOD_DST_LFO_1_PHASE,
    MOD_DST_LFO_1_SYNC,
    MOD_DST_LFO_1_AMOUNT,
    MOD_DST_LFO_1_DST,

    MOD_DST_LFO_2_RATE,
    MOD_DST_LFO_2_SHAPE,
    MOD_DST_LFO_2_PHASE,
    MOD_DST_LFO_2_SYNC,
    MOD_DST_LFO_2_AMOUNT,
    MOD_DST_LFO_2_DST,

    MOD_DST_LFO_3_RATE,
    MOD_DST_LFO_3_SHAPE,
    MOD_DST_LFO_3_PHASE,
    MOD_DST_LFO_3_SYNC,
    MOD_DST_LFO_3_AMOUNT,
    MOD_DST_LFO_3_DST, 

    NUM_MOD_DESTINATIONS

} mod_destinations_t;

typedef enum mod_sources_t {
    MOD_SRC_ENV_1,
    MOD_SRC_ENV_2,
    MOD_SRC_ENV_3,
    MOD_SRC_LFO_1,
    MOD_SRC_LFO_2,
    MOD_SRC_LFO_3,
    MOD_SRC_VELOCITY,
    MOD_SRC_MOD_WHEEL,
    MOD_SRC_AFTERTOUCH,
    MOD_SRC_NOTE,
    MOD_SRC_BREATH,

    NUM_MOD_SOURCES
} mod_sources_t;

typedef enum seq_params_t {
    SEQ_PARAM_STEP_NOTE,
    SEQ_PARAM_STEP_OCTAVE,
    SEQ_PARAM_STEP_LENGTH,
    SEQ_PARAM_VELOCITY,
    SEQ_PARAM_TRANSPOSE,
    SEQ_PARAM_DIRECTION,
    SEQ_PARAM_SWING,
    SEQ_PARAM_CLOCK_DIV,
    SEQ_PARAM_FIRST_STEP,
    SEQ_PARAM_LAST_STEP,
    SEQ_PARAM_PROBABILITY,
    SEQ_PARAM_SCALE,
    SEQ_PARAM_DENSITY,
    SEQ_PARAM_REPEAT,
    SEQ_PARAM_OCTAVE_RANGE,
    SEQ_PARAM_CHANCE,

    NUM_SEQ_PARAMS
} seq_params_t;

// ################################
// SETTINGS
// ################################

typedef enum settings_row_t {
    SET_ROW_PRESET_NUM,
    SET_ROW_PRESET_NAME,
    SET_ROW_MIDI_CHAN,
    SET_ROW_MOD_SRC,
    SET_ROW_MOD_DST,
    SET_ROW_LEGATO,
    SET_ROW_OSC_DUO,
    SET_ROW_VCA_VELOCITY,
    SET_ROW_MIDI_CLOCK,
    SET_ROW_MIDI_TRANS,
    SET_ROW_MIDI_PGM,
    SET_ROW_POT_LATCH,
    SET_ROW_PB_DEPTH,
    SET_ROW_RECORD_CLICK,
    SET_ROW_SCOPE,
    SET_ROW_INIT_PATCH,

    NUM_SET_ROWS

} settings_row_t;

typedef enum settings_mod_sources_t {
    SET_MOD_SRC_VELOCITY,
    SET_MOD_SRC_MOD_WHEEL,
    SET_MOD_SRC_AFTERTOUCH,
    SET_MOD_SRC_NOTE, 
    SET_MOD_SRC_BREATH,

    NUM_SET_MOD_SRC

} settings_mod_sources_t;

// ################################
// DISPLAYABLE EVENTS
// ################################

typedef enum displayable_events_t {

    //DISP_EVT
    DISP_EVT_NONE,
    DISP_EVT_MODE_MOD_ENV,
    DISP_EVT_MODE_MOD_LFO,
    DISP_EVT_MODE_KEYBOARD,
    DISP_EVT_MODE_BPM,
    DISP_EVT_PAGE_KBD,
    DISP_EVT_PAGE_SEQ,
    DISP_EVT_INIT_PATCH,
    DISP_EVT_CLEAR_P_LOCK_PARAM,
    DISP_EVT_SAVE_PREPARE,
    DISP_EVT_SAVE_PROGRESS,
    DISP_EVT_SAVE_COMPLETE,
    DISP_EVT_LOAD_PREPARE,
    DISP_EVT_LOAD_PROGRESS,
    DISP_EVT_LOAD_COMPLETE,
    DISP_EVT_SCOPE,

    NUM_DISPLAYABLE_EVENTS

} displayable_events_t;

#endif