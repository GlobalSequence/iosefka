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

// page
static view_page_event_t last_page_event;

// layer
static const uint8_t k_syn1_layers = 2;
static view_layer_event_t last_layer_event;
static view_layer_t last_synth_1_layer;
static bool sub_layer_p_lock_ready = false;
static bool sub_layer_p_lock = false;

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
    machine_state.page = page;
    last_page_event = event; 

}

void viewSetLayerContext(view_layer_event_t event) {

    switch(event) {
        case EVT_LAYER_SYNTH_1:
            if(last_layer_event == event) {
                machine_state.layer = machine_state.layer == LAYER_SYNTH_1 ? 
                    LAYER_SYNTH_ENV_SPLIT : LAYER_SYNTH_1;
            } else {
                machine_state.layer = last_synth_1_layer;
            }
            last_synth_1_layer = machine_state.layer;
            break;
        case EVT_LAYER_SYNTH_2:
            machine_state.layer = LAYER_SYNTH_2;
            break;
        case EVT_LAYER_ENV:
            machine_state.layer = LAYER_ENV;
            break;
        case EVT_LAYER_LFO:
            machine_state.layer = LAYER_LFO;
            break;
        case EVT_LAYER_SEQ:
            machine_state.layer = LAYER_SEQ;
            break;
        case EVT_LAYER_NULL:
        default:
            break;
    }

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

    }

    last_mode_event = event;
    machine_state.mode = mode_context[event];

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
    mode_context[EVT_MODE_SETTINGS] = MODE_SAVE_PRESET_SEL;

    machine_state.keyboard_octave = kbd_oct_startup;
    machine_state.sequencer_window = seq_window_startup;
    machine_state.preset_number = preset_startup;
    machine_state.seq_step_note = seq_step_note_startup;
    machine_state.seq_step_octave = seq_step_octave_startup;

}



// ################################
// SET FUNCTIONS
// ################################

void viewChange(view_change_t event) {

    viewSetPageContext(view_events[event].p);
    viewSetLayerContext(view_events[event].l);
    viewSetModeContext(view_events[event].m);

    view_change_led = true;
}

void viewSetPage(view_page_t page) {

    machine_state.page = page;

}

void viewSetLayer(view_layer_t layer) {

    machine_state.layer = layer;

}

void viewSetMode(view_mode_t mode) {

    machine_state.mode = mode_context[last_mode_event] = mode;

}

void viewSetKeyboardOctave(uint8_t octave) {

    machine_state.keyboard_octave = octave;

}

void viewSetSequencerWindow(bool increment) {

    int8_t counter = increment? 1 : -1;

    machine_state.sequencer_window = cycleIntAddOrSub(machine_state.sequencer_window, 0, NUMBER_OF_SEQUENCER_WINDOWS - 1, counter);

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

// void viewSetSaveState(bool enter) {

//     save_state = enter;

// }

// void viewSetLoadState(bool enter) {

//     load_state = enter;

// }

// ################################
// GET FUNCTIONS
// ################################

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

// bool viewInSaveState() { return save_state; }

// bool viewInLoadState() { return load_state; }