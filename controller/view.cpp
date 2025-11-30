#include "view.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/utilities.h"
#include "../common/ring_buffer.h"
#include "../common/globals.h"
#include "../common/test.h"

static const view_state_t k_view_state_seq = {PAGE_SEQ, LAYER_SEQ, MODE_SEQ_SET_BPM};
static view_events_t view_events[NUM_VIEW_CHANGES];
static view_change_t last_view_change;
static view_change_t last_synth_2_view_change;
static machine_state_t machine_state;

// misc
static const uint8_t kbd_oct_startup = 2;
static const uint8_t seq_window_startup = 0;
static const uint8_t preset_startup = 0;
static const uint8_t seq_step_note_startup = 0;
static const uint8_t seq_step_octave_startup = 5;

static bool view_change_led = true;
static bool save_state = false;
static bool load_state = false;

static bool encoder_turned_while_held = false;

// page
static view_page_event_t last_page_event;

// layer
static const uint8_t k_syn1_layers = 2;
static view_layer_event_t last_layer_event;
static view_layer_t last_synth_1_layer;
static view_layer_t last_synth_2_layer;
static bool sub_layer_p_lock_ready = false;
static bool sub_layer_p_lock = false;
static bool latch[NUMBER_OF_POTENTIOMETERS];

// mode
static view_mode_event_t last_mode_event;
static view_mode_t mode_context[NUM_MODE_EVENTS];
static const view_mode_t k_mode_env[NUMBER_OF_ENVS] = {MODE_MOD_DST_ENV_1, MODE_MOD_DST_ENV_2, MODE_MOD_DST_ENV_3};
static const view_mode_t k_mode_lfo[NUMBER_OF_LFOS] = {MODE_MOD_DST_LFO_1, MODE_MOD_DST_LFO_2, MODE_MOD_DST_LFO_3};

// ################################
// PRIVATE FUNCTIONS
// ################################

void viewSetPageContext(view_page_event_t event) {

    view_page_t page;
    switch(event) {
        case EVT_PAGE_KEYS:
            page = PAGE_KEYS;
            break;

            case EVT_PAGE_SEQ:
            page = PAGE_SEQ;
            break;

            case EVT_PAGE_SETTINGS:
            page = PAGE_SETTINGS;
            break;
    }
    // machine_state.page = page;
    viewSetPage(page);
    last_page_event = event; 

}

void viewSetLayerContext(view_layer_event_t event) {

    // set default value, in case layer is invalid
    view_layer_t layer = NUM_VIEW_LAYERS;

    switch(event) {
        case EVT_LAYER_SYNTH_1:
            if(last_layer_event == event) {
                layer = machine_state.layer == LAYER_SYNTH_1 ? 
                    LAYER_SYNTH_ENV_SPLIT : LAYER_SYNTH_1;
            } else {
                layer = last_synth_1_layer;
            }
            last_synth_1_layer = layer;
            break;
        case EVT_LAYER_SYNTH_2:
/*             if(last_layer_event == event) {
                layer = machine_state.layer == LAYER_SYNTH_2 ?
                    LAYER_USER_PARAM : LAYER_SYNTH_2;
            } else {
                layer = last_synth_2_layer;
            }
            last_synth_2_layer = layer; */
            layer = LAYER_SYNTH_2;
            break;
        case EVT_LAYER_ENV:
            layer = LAYER_ENV;
            break;
        case EVT_LAYER_LFO:
            layer = LAYER_LFO;
            break;
        case EVT_LAYER_SEQ:
            layer = LAYER_SEQ;
            break;
        case EVT_LAYER_NULL:
        default:
            break;
    }

    if(layer != NUM_VIEW_LAYERS) viewSetLayer(layer);

    last_layer_event = event;

}

void viewSetModeContext(view_mode_event_t event) {

    if(event == last_mode_event && (event == EVT_MODE_ENV || event == EVT_MODE_LFO)) {

        bool cycle = false;
        uint8_t size;
        const view_mode_t *mode;

        if(event == EVT_MODE_ENV) {

            mode = k_mode_env;
            size = NUMBER_OF_ENVS;
        
        } else if (event == EVT_MODE_LFO) {
    
            mode = k_mode_lfo;
            size = NUMBER_OF_LFOS;
    
        }

        for(int i = 0; i < size; i++) {

            if(mode[i] == mode_context[event]) {

                uint8_t next_index = cycleIntAddOrSub(i, 0, size - 1, 1);

                mode_context[event] = mode[next_index];

                // exit the loop
                i = size;

            }

        }

    // this is hacky asf and also relies on the layer switching BEFORE mode switching. sorry not sorry.
    } /* else if (last_layer_event == EVT_LAYER_SYNTH_2 && machine_state.layer == LAYER_USER_PARAM) {

        last_mode_event = event;
        machine_state.mode = MODE_USER_PARAM_DST;
        return;

    } */

    last_mode_event = event;
    machine_state.mode = mode_context[event];
    viewSetMachineStateMode(mode_context[event]);
    // viewSetMode(mode_context[event]);

}

// ################################
// PUBLIC FUNCTIONS
// ################################

void viewSetup() {

    view_events[VIEW_CHANGE_KBD_SY1] = (view_events_t){
        EVT_PAGE_KEYS, EVT_LAYER_SYNTH_1, EVT_MODE_KEYS
    };
    view_events[VIEW_CHANGE_KBD_SY2] = (view_events_t){
        EVT_PAGE_KEYS, EVT_LAYER_SYNTH_2, EVT_MODE_KEYS
    };
    view_events[VIEW_CHANGE_KBD_ENV] = (view_events_t){
        EVT_PAGE_KEYS, EVT_LAYER_ENV, EVT_MODE_ENV
    };
    view_events[VIEW_CHANGE_KBD_LFO] = (view_events_t){
        EVT_PAGE_KEYS, EVT_LAYER_LFO, EVT_MODE_LFO
    };
    view_events[VIEW_CHANGE_KBD_SEQ] = (view_events_t){
        EVT_PAGE_KEYS, EVT_LAYER_SEQ, EVT_MODE_SEQ
    };
    view_events[VIEW_CHANGE_SEQ_SEQ] = (view_events_t){
        EVT_PAGE_SEQ, EVT_LAYER_SEQ, EVT_MODE_SEQ
    };
    view_events[VIEW_CHANGE_SEQ_SY1] = (view_events_t){
        EVT_PAGE_SEQ, EVT_LAYER_SYNTH_1, EVT_MODE_SEQ
    };
    view_events[VIEW_CHANGE_SEQ_SY2] = (view_events_t){
        EVT_PAGE_SEQ, EVT_LAYER_SYNTH_2, EVT_MODE_SEQ
    };
    view_events[VIEW_CHANGE_SEQ_ENV] = (view_events_t){
        EVT_PAGE_SEQ, EVT_LAYER_ENV, EVT_MODE_SEQ
    };
    view_events[VIEW_CHANGE_SEQ_LFO] = (view_events_t){
        EVT_PAGE_SEQ, EVT_LAYER_LFO, EVT_MODE_SEQ
    };
    view_events[VIEW_CHANGE_SETTINGS] = (view_events_t){
        EVT_PAGE_SETTINGS, EVT_LAYER_NULL, EVT_MODE_SETTINGS
    };


    mode_context[EVT_MODE_KEYS] = MODE_KBD_OCT;
    mode_context[EVT_MODE_ENV] = MODE_MOD_DST_ENV_1;
    mode_context[EVT_MODE_LFO] = MODE_MOD_DST_LFO_1;
    mode_context[EVT_MODE_SEQ] = MODE_SEQ_SET_BPM;
    // mode_context[EVT_MODE_SETTINGS] = MODE_MIDI_CHANNEL;
    // mode_context[EVT_MODE_SETTINGS] = MODE_SAVE_PRESET_SEL;
    mode_context[EVT_MODE_SETTINGS] = MODE_SET_SCROLL;
    mode_context[EVT_MODE_USER_PARAM] = MODE_USER_PARAM_DST;

    machine_state.keyboard_octave = kbd_oct_startup;
    machine_state.sequencer_window = seq_window_startup;
    machine_state.preset_number = preset_startup;
    machine_state.seq_step_note = seq_step_note_startup;
    machine_state.seq_step_octave = seq_step_octave_startup;

    last_synth_1_layer = LAYER_SYNTH_1;
    last_synth_2_layer = LAYER_SYNTH_2;

}

// ################################
// SET FUNCTIONS
// ################################

void viewChange(view_change_t event) {

    viewSetPageContext(view_events[event].p);
    viewSetLayerContext(view_events[event].l);
    viewSetModeContext(view_events[event].m);

    // view_change_led = true;
    last_view_change = event;
}

void viewSetPage(view_page_t page) {

    machine_state.page = page;
    view_change_led = true;

}

void viewSetLayer(view_layer_t layer) {

    machine_state.layer = layer;
    view_change_led = true;
    viewSetLatchAll();

}

void viewSetMode(view_mode_t mode) {

    // machine_state.mode = mode_context[last_mode_event] = mode;
    
    mode_context[last_mode_event] = mode;
    viewSetMachineStateMode(mode);

}

void viewSetMachineStateMode(view_mode_t mode) {

    machine_state.mode = mode;
    view_change_led = true;

}

void viewSetKeyboardOctave(uint8_t octave) {

    machine_state.keyboard_octave = octave;

}

void viewSetSequencerWindow(bool increment) {

    int8_t counter = increment? 1 : -1;

    uint8_t val = cycleIntAddOrSub(machine_state.sequencer_window, 0, NUMBER_OF_SEQUENCER_WINDOWS - 1, counter);

    viewSetSequencerWindowByValue(val);

    // machine_state.sequencer_window = cycleIntAddOrSub(machine_state.sequencer_window, 0, NUMBER_OF_SEQUENCER_WINDOWS - 1, counter);

}

void viewSetSequencerWindowByValue(uint8_t value) {

    machine_state.sequencer_window = value;

}

void viewSetPresetNumber(uint8_t preset) {

    machine_state.preset_number = preset;

}

void viewReadySubLayerPLock(bool set) {

    sub_layer_p_lock_ready = set;

}

void viewPLockIsHappening(bool set) {

    sub_layer_p_lock = set;

}

void viewSetSeqParamStepNote(uint8_t note) {

    machine_state.seq_step_note = note;

}

void viewSetSeqParamStepOctave(uint8_t octave) {

    machine_state.seq_step_octave = octave;

}

void viewSetLatchSingle(uint8_t index) {

    latch[index] = true;

}

void viewResetLatchSingle(uint8_t index) {

    latch[index] = false;

}

void viewSetLatchAll() {


    for(int i = 0; i < NUMBER_OF_POTENTIOMETERS; i++) {

        viewSetLatchSingle(i);

    }

}

void viewSetSettingsRow(uint8_t row) {

    machine_state.settings_row = row;

}

void viewSetSettingsModSource(uint8_t index) {

    machine_state.settings_mod_source = index;

}

void viewSetCurrentUserParameter(uint8_t index) {

    machine_state.current_user_param = index;

}

void viewSetEncoderTurnedWhileHeld() {

    encoder_turned_while_held = true;

}

// ################################
// GET FUNCTIONS
// ################################

view_change_t viewGetLastViewChangeEvent() { return last_view_change; }

view_page_t viewGetPage() { return machine_state.page; }

view_layer_t viewGetLayer() { return machine_state.layer; }

view_mode_t viewGetMode() { return machine_state.mode; }

uint8_t viewGetKeyboardOctave() { return machine_state.keyboard_octave; }

uint8_t viewGetSequencerWindow() { return machine_state.sequencer_window; }

uint8_t viewGetSequencerWindowStepBase() {

    return machine_state.sequencer_window * NUMBER_OF_SEQ_STEPS_PER_WINDOW;

}

uint8_t viewGetPresetNumber() { return machine_state.preset_number; }

uint8_t viewGetSeqParamStepNote() { return machine_state.seq_step_note; }

uint8_t viewGetSeqParamStepOctave() { return machine_state.seq_step_octave; }

bool viewGetViewChangeFlagLed() {

    bool out = view_change_led;
    
    if(out) {

        view_change_led = false;

    }

    return out;

}

bool viewGetLatchSingle(uint8_t index) {

    return latch[index];

}

uint8_t viewGetSettingsRow() { return machine_state.settings_row; }

uint8_t viewGetSettingsModSource() { return machine_state.settings_mod_source; }

uint8_t viewGetModSourceFromSettings() {

    uint8_t a = machine_state.settings_mod_source;

    switch(a) {

        case SET_MOD_SRC_MOD_WHEEL: return MOD_SRC_MOD_WHEEL; break;
        case SET_MOD_SRC_AFTERTOUCH: return MOD_SRC_AFTERTOUCH; break;
        // case SET_MOD_SRC_NOTE: return MOD_SRC_NOTE; break;
        // case SET_MOD_SRC_BR_CTRL: return MOD_SRC_BR_CTRL; break;
        default: return MOD_SRC_VELOCITY; break;
    }

    return MOD_SRC_VELOCITY;

}

uint8_t viewGetCurrentUserParameter() { return machine_state.current_user_param; }

bool viewGetEncoderTurnedWhileHeld() { 

    if(encoder_turned_while_held) {

        encoder_turned_while_held = false;
        return true;

    }

    return false;

}