#include "encoder_map.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "settings.h"
#include "../controller/view.h"
#include "../common/test.h"
#include "../graphics/graphics.h"
#include "../graphics/oscilloscope.h"

#define MOD_ACCEL 1

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
    { MODE_MIDI_BREATH_DST, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_MIDI_NOTE_DST, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_SET_SCROLL, 0, NUM_SET_ROWS - 1, false, NUM_SET_ROWS - 1},
    { MODE_SET_MOD_SRC_SEL, 0, NUM_SET_MOD_SRC - 1, true, 1},
    { MODE_SET_MOD_DST_SEL, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_SET_PRESET_CHAR_INDEX, 0, PRESET_NAME_LENGTH - 1, true, 1},    
    { MODE_SET_PRESET_CHAR_SEL, 0, NUM_CHAR_IDS - 1, true, 4},
    { MODE_USER_PARAM_SEL, 0, NUMBER_OF_POTENTIOMETERS - 1, true, 1},
    { MODE_USER_PARAM_DST, 0, NUM_MOD_DESTINATIONS - 1, true, MOD_ACCEL},
    { MODE_SEQ_WINDOW_SEL, 0, NUMBER_OF_SEQUENCER_WINDOWS - 1, true, 1},
    { MODE_SCOPE_DIV, 0, NUMBER_OF_SCOPE_DIVS - 1, false, 1},

};

static uint8_t mod_dst_skip_index_size;

static const mod_destinations_t mod_dst_skip_index[] = {
    MOD_DST_NONE, 
    MOD_DST_OSC_BOTH_PITCH,
    MOD_DST_OSC_1_PITCH, 
    MOD_DST_OSC_2_PITCH,
    MOD_DST_PORTAMENTO,
    MOD_DST_MIX_OSC_1_MIX,
    MOD_DST_FILTER_CUTOFF,
    MOD_DST_VCA,
    MOD_DST_DELAY_RATE,
    MOD_DST_ENV_1_ATTACK,
    MOD_DST_ENV_2_ATTACK,
    MOD_DST_ENV_3_ATTACK,
    MOD_DST_LFO_1_RATE,
    MOD_DST_LFO_2_RATE,
    MOD_DST_LFO_3_RATE,

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

void setModDstJump(mod_sources_t src, uint8_t value, int8_t direction) {

    // VERY stupid workaround to get the most updated version of this value because of the stupid way the
    // encoderMapAssignValue function is set up
    current_mode = NUM_VIEW_MODES;
    // also a stupid workaround because the value is already incremented/decremented in the encoderMapAssignValue
    // function LMAO
    if(direction == -1 && value > MOD_DST_LFO_3_RATE) value = 0;
    else value -= direction;

    int8_t index = 0;

    if (value >= MOD_DST_NONE && value < MOD_DST_OSC_BOTH_PITCH)    
    {
        index = 0;
    }
    else if (value >= MOD_DST_OSC_BOTH_PITCH && value < MOD_DST_OSC_1_PITCH)
    {
        index = 1;
    }
    else if (value >= MOD_DST_OSC_1_PITCH && value < MOD_DST_OSC_2_PITCH)
    {
        index = 2;
    }
    else if (value >= MOD_DST_OSC_2_PITCH && value < MOD_DST_PORTAMENTO)
    {
        index = 3;
    }
    else if (value >= MOD_DST_PORTAMENTO && value < MOD_DST_MIX_OSC_1_MIX)
    {
        index = 4;
    }
    else if (value >= MOD_DST_MIX_OSC_1_MIX && value < MOD_DST_FILTER_CUTOFF)
    {
        index = 5;
    }
    else if (value >= MOD_DST_FILTER_CUTOFF && value < MOD_DST_VCA)
    {
        index = 6;
    }
    else if (value >= MOD_DST_VCA && value < MOD_DST_DELAY_RATE)
    {
        index = 7;
    }
    else if (value >= MOD_DST_DELAY_RATE && value < MOD_DST_ENV_1_ATTACK)
    {
        index = 8;
    }
    else if (value >= MOD_DST_ENV_1_ATTACK && value < MOD_DST_ENV_2_ATTACK)
    {
        index = 9;
    }
    else if (value >= MOD_DST_ENV_2_ATTACK && value < MOD_DST_ENV_3_ATTACK)
    {
        index = 10;
    }
    else if (value >= MOD_DST_ENV_3_ATTACK && value < MOD_DST_LFO_1_RATE)
    {
        index = 11;
    }
    else if (value >= MOD_DST_LFO_1_RATE && value < MOD_DST_LFO_2_RATE)
    {
        index = 12;
    }
    else if (value >= MOD_DST_LFO_2_RATE && value < MOD_DST_LFO_3_RATE)
    {
        index = 13;
    }
    else if (value >= MOD_DST_LFO_3_RATE)
    {
        index = 14;
    }

    index += direction;

    if(index < 0) index = 14;
    else if (index > 14) index = 0;

    setModDst(src, mod_dst_skip_index[index]);

}

mod_destinations_t getModDst(mod_sources_t src) {

    return (mod_destinations_t)synth->patch.mod[src].destination;

}

mod_sources_t getSettingsModSource() {

    return (mod_sources_t) viewGetModSourceFromSettings();

}

int16_t getCount(view_mode_t mode) { 

    switch(mode) {
    
        case MODE_KBD_OCT: return viewGetKeyboardOctave();
        
        case MODE_MOD_DST_ENV_1: return getModDst(MOD_SRC_ENV_1);

        case MODE_MOD_DST_ENV_2: return getModDst(MOD_SRC_ENV_2);

        case MODE_MOD_DST_ENV_3: return getModDst(MOD_SRC_ENV_3);

        case MODE_MOD_DST_LFO_1: return getModDst(MOD_SRC_LFO_1);

        case MODE_MOD_DST_LFO_2: return getModDst(MOD_SRC_LFO_2);

        case MODE_MOD_DST_LFO_3: return getModDst(MOD_SRC_LFO_3);

        case MODE_MIDI_CHANNEL: return synth->settings.channel;

        case MODE_MIDI_VELOCITY_DST: return getModDst(MOD_SRC_VELOCITY);

        case MODE_MIDI_MOD_WHEEL_DST: return getModDst(MOD_SRC_MOD_WHEEL);

        case MODE_MIDI_AFTERTOUCH_DST: return getModDst(MOD_SRC_AFTERTOUCH);

        case MODE_SAVE_PRESET_SEL: return viewGetPresetNumberSelected();

        case MODE_SEQ_SET_BPM: return synth->seq.getBpm();

        case MODE_MIDI_PITCHBEND_DEPTH: return synth->settings.pitch_bend_depth;

        case MODE_SET_SCROLL: return viewGetSettingsRow();

        case MODE_SET_MOD_SRC_SEL: return viewGetSettingsModSource();

        case MODE_SET_MOD_DST_SEL: return getModDst(getSettingsModSource());

        case MODE_USER_PARAM_SEL: return viewGetCurrentUserParameter();

        case MODE_USER_PARAM_DST: return synth->user_param.destination[viewGetCurrentUserParameter()];

        case MODE_SEQ_WINDOW_SEL: return viewGetSequencerWindow();

        case MODE_SET_PRESET_CHAR_INDEX: return viewGetPresetCharIndex();

        case MODE_SET_PRESET_CHAR_SEL: return viewGetPresetCharSelected();

        case MODE_SCOPE_DIV: return oscilloscopeGetTimeDivCtrl();

        default:
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

    mod_dst_skip_index_size = sizeof(mod_dst_skip_index) / sizeof(mod_dst_skip_index[0]);

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

            accel? setModDstJump(MOD_SRC_ENV_1, count_value, value) : setModDst(MOD_SRC_ENV_1, count_value);

            break;
    
        case MODE_MOD_DST_ENV_2:

            accel? setModDstJump(MOD_SRC_ENV_2, count_value, value) : setModDst(MOD_SRC_ENV_2, count_value);

            break;
    
        case MODE_MOD_DST_ENV_3:

            accel? setModDstJump(MOD_SRC_ENV_3, count_value, value) : setModDst(MOD_SRC_ENV_3, count_value);

            break;
    
        case MODE_MOD_DST_LFO_1:

            accel? setModDstJump(MOD_SRC_LFO_1, count_value, value) : setModDst(MOD_SRC_LFO_1, count_value);

            break;
    
        case MODE_MOD_DST_LFO_2:

            accel? setModDstJump(MOD_SRC_LFO_2, count_value, value) : setModDst(MOD_SRC_LFO_2, count_value);

            break;
    
        case MODE_MOD_DST_LFO_3:

            accel? setModDstJump(MOD_SRC_LFO_3, count_value, value) : setModDst(MOD_SRC_LFO_3, count_value);

            break;
    
        case MODE_SEQ_SET_BPM:

            synth->seq.setBpm(count_value);

            break;

    
        case MODE_SAVE_PRESET_SEL:

            viewSetPresetNumberSelected(count_value);
        
            break;
    
        case MODE_MIDI_CHANNEL:

            synth->settings.channel = count_value;

            break;
    
        case MODE_MIDI_VELOCITY_DST:

            accel? setModDstJump(MOD_SRC_VELOCITY, count_value, value) : setModDst(MOD_SRC_VELOCITY, count_value);

            break;
    
        case MODE_MIDI_MOD_WHEEL_DST:

            accel? setModDstJump(MOD_SRC_MOD_WHEEL, count_value, value) : setModDst(MOD_SRC_MOD_WHEEL, count_value);

            break;
    
        case MODE_MIDI_AFTERTOUCH_DST:

            accel? setModDstJump(MOD_SRC_AFTERTOUCH, count_value, value) : setModDst(MOD_SRC_AFTERTOUCH, count_value);

            break;
    
        case MODE_MIDI_PITCHBEND_DEPTH:

            synth->settings.pitch_bend_depth = count_value;

            break;

        case MODE_SET_SCROLL:

            viewSetSettingsRow(count_value);

            break;

        case MODE_SET_MOD_SRC_SEL:

            viewSetSettingsModSource(count_value);

            break;

        case MODE_SET_MOD_DST_SEL:

            accel? setModDstJump(getSettingsModSource(), count_value, value) : setModDst(getSettingsModSource(), count_value);

            break;

        case MODE_USER_PARAM_SEL:

            viewSetCurrentUserParameter(count_value);

            break;

        case MODE_USER_PARAM_DST:

            synth->user_param.destination[viewGetCurrentUserParameter()] = count_value;

            break;

        case MODE_SEQ_WINDOW_SEL:

            viewSetSequencerWindowByValue(count_value);

            break;

        case MODE_SET_PRESET_CHAR_INDEX:

            viewSetPresetCharIndex(count_value);

            break;

        case MODE_SET_PRESET_CHAR_SEL:

            settingsSetPresetNameChar((char)count_value, viewGetPresetCharIndex(), viewGetPresetNumberSelected());

            break;

        case MODE_SCOPE_DIV:

            oscilloscopeSetTimeDivCtrl(count_value);

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