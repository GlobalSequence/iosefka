#include "encoder_map.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "settings.h"
#include "../controller/view.h"
#include "../common/test.h"

#define MOD_ACCEL 4

typedef struct mode_constants_t {
    view_mode_t mode;
    int16_t min;
    int16_t max;
    bool loop;
    int acceleration_multiplier;
} mode_constants_t;

// NOTE: this is in order of the modes in NUM_VIEW_MODES. 
// any modes added have to follow the order in that enum.
const mode_constants_t rules[NUM_VIEW_MODES] = {
    { MODE_KBD_OCT, MIN_KEYBOARD_OCT, MAX_KEYBOARD_OCT, false, 1},
    { MODE_MOD_DST_ENV_1, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_MOD_DST_ENV_2, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_MOD_DST_ENV_3, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_MOD_DST_LFO_1, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_MOD_DST_LFO_2, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_MOD_DST_LFO_3, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_SEQ_SET_BPM, MIN_BPM, MAX_BPM, false, 4},
    { MODE_SEQ_STEP_NOTE, 0, NUMBER_OF_MIDI_NOTES - 1, false, 12},
    { MODE_SEQ_STEP_LENGTH, 0, MAX_NOTE_LENGTH - 1, false, 4},
    { MODE_SEQ_P_LOCK_DEST, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_SAVE_PRESET_SEL, 0, NUMBER_OF_PRESETS - 1, true, 4},
    { MODE_MIDI_CHANNEL, 0, NUMBER_OF_MIDI_CHANNELS - 1, false, 1}, 
    { MODE_MIDI_VELOCITY_DST, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_MIDI_MOD_WHEEL_DST, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_MIDI_AFTERTOUCH_DST, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_MIDI_PITCHBEND_DEPTH, 0, MIDI_PITCH_BEND_DEPTH_MAX - 1, false, 1},
};

static synth_state_t *synth;

static int16_t count_value;

view_mode_t current_mode;

// ################################
// PRIVATE FUNCTIONS
// ################################

void setModDst(mod_sources_t src, uint8_t value) {

    synth->patch.mod[src].destination = value;

}

mod_destinations_t getModDst(mod_sources_t src) {

    return (mod_destinations_t)synth->patch.mod[src].destination;

}

int16_t getCount(view_mode_t mode) { 

    switch(mode) {
    
        case MODE_KBD_OCT:

            return viewGetKeyboardOctave();

            break;
        
        case MODE_MOD_DST_ENV_1:

            return getModDst(MOD_SRC_ENV_1);

            break;

        case MODE_MOD_DST_ENV_2:

            return getModDst(MOD_SRC_ENV_2);

            break;

        case MODE_MOD_DST_ENV_3:

            return getModDst(MOD_SRC_ENV_3);

            break;

        case MODE_MOD_DST_LFO_1:

            return getModDst(MOD_SRC_LFO_1);

            break;

        case MODE_MOD_DST_LFO_2:

            return getModDst(MOD_SRC_LFO_2);

            break;

        case MODE_MOD_DST_LFO_3:

            return getModDst(MOD_SRC_LFO_3);

            break;

        case MODE_MIDI_CHANNEL:

            return synth->midi.channel;

            break;

        case MODE_MIDI_VELOCITY_DST:

            return getModDst(MOD_SRC_VELOCITY);

            break;

        case MODE_MIDI_MOD_WHEEL_DST:

            return getModDst(MOD_SRC_MOD_WHEEL);

            break;

        case MODE_MIDI_AFTERTOUCH_DST:

            return getModDst(MOD_SRC_AFTERTOUCH);

            break;

        case MODE_SAVE_PRESET_SEL:

            return viewGetPresetNumber();

            break;

        case MODE_SEQ_SET_BPM:

            return synth->seq.getBpm();
        
            break;

        case MODE_MIDI_PITCHBEND_DEPTH:

            return synth->midi.pitch_bend_depth;

            break;
    }

    return 0;

}

// ################################
// PUBLIC FUNCTIONS
// ################################

void encoderMapSetup(synth_state_t *synth_state) {

    synth = synth_state;

    current_mode = viewGetMode();

    count_value = getCount(current_mode);

}



void encoderMapAssignValue(int8_t value, bool accel) {

    view_mode_t mode = viewGetMode();

    if(mode != current_mode) {

        current_mode = mode;

        count_value = getCount(mode);

    }

    bool loop = rules[mode].loop;

    int16_t min = rules[mode].min;

    int16_t max = rules[mode].max;

    if(accel) value *= rules[mode].acceleration_multiplier;

    count_value += value;

    if(loop) {
        if(count_value < min) count_value = max;
        else if (count_value > max) count_value = min;
    } else {
        if(count_value <= min) count_value = min;
        else if(count_value >= max) count_value = max;
    }

    switch (mode) {
        case MODE_KBD_OCT:

            viewSetKeyboardOctave(count_value);

            break;
    
        case MODE_MOD_DST_ENV_1:

            setModDst(MOD_SRC_ENV_1, count_value);

            break;
    
        case MODE_MOD_DST_ENV_2:

            setModDst(MOD_SRC_ENV_2, count_value);

            break;
    
        case MODE_MOD_DST_ENV_3:

            setModDst(MOD_SRC_ENV_3, count_value);

            break;
    
        case MODE_MOD_DST_LFO_1:

            setModDst(MOD_SRC_LFO_1, count_value);

            break;
    
        case MODE_MOD_DST_LFO_2:

            setModDst(MOD_SRC_LFO_2, count_value);

            break;
    
        case MODE_MOD_DST_LFO_3:

            setModDst(MOD_SRC_LFO_3, count_value);

            break;
    
        case MODE_SEQ_SET_BPM:

            synth->seq.setBpm(count_value);

            break;

    
        case MODE_SAVE_PRESET_SEL:

            viewSetPresetNumber(count_value);
        
            break;
    
        case MODE_MIDI_CHANNEL:

            synth->midi.channel = count_value;

            break;
    
        case MODE_MIDI_VELOCITY_DST:

            setModDst(MOD_SRC_VELOCITY, count_value);

            break;
    
        case MODE_MIDI_MOD_WHEEL_DST:

            setModDst(MOD_SRC_MOD_WHEEL, count_value);

            break;
    
        case MODE_MIDI_AFTERTOUCH_DST:

            setModDst(MOD_SRC_AFTERTOUCH, count_value);

            break;
    
        case MODE_MIDI_PITCHBEND_DEPTH:

            synth->midi.pitch_bend_depth = count_value;

            break;

        case MODE_MOD_LAYER_SEL:
        case MODE_SEQ_STEP_NOTE:
        case MODE_SEQ_STEP_LENGTH:
        case MODE_SEQ_P_LOCK_DEST:    
        default:
            break;
    }

}

int16_t encoderMapGetCount() {

    return getCount(current_mode);

}