#include "switch_actions.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "storage.h"
#include "view.h"
#include "../common/test.h"
#include "../common/globals.h"
#include "settings.h"
#include "note_queue.h"
#include "midi.h"
#include "../sequencer/pattern_generator.h"
#include "../sequencer/p_lock.h"

static const switch_id_t save_switch = SW_T0;
static const switch_id_t load_switch = SW_T1;

static synth_state_t *synth;
static bool ignore_release_action[NUMBER_OF_SWITCHES_TOTAL];
static int8_t stored_pressed_note[NUMBER_OF_KEY_SWITCHES] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

view_change_t selected_view;
bool view_change_pending = false;
bool p_lock_exception;
bool view_changed = false;
bool selected_window_direction;
bool seq_window_change_pending = false;

// ################################
// PRIVATE FUNCTIONS
// ################################

void toggleSetting(bool *state) {

    *state = !*state;

}

void pageKeyboard(switch_id_t sw, bool pressed) {

    uint8_t key;

    switch(sw) {

        case SW_B7: key = 12; break;
        case SW_B6: key = 11; break;
        case SW_B5: key = 9; break;
        case SW_B4: key = 7; break;
        case SW_B3: key = 5; break;
        case SW_B2: key = 4; break;
        case SW_B1: key = 2; break;
        case SW_B0: key = 0; break;
        case SW_T0: key = 1; break;
        case SW_T1: key = 3; break;
        case SW_T2: key = 6; break;
        case SW_T3: key = 8; break;
        case SW_T4: key = 10; break;

        default: break;

    }

    // uint8_t octave = viewGetKeyboardOctave();

    // uint8_t note = (uint8_t) key + (octave * NUMBER_OF_NOTES_PER_OCTAVE);

    // uint8_t velocity = pressed? MAX_PARAM_VALUE : 0;


    // noteQueueSubmit(note, velocity, pressed);

    // if(pressed) stored_pressed_note[key] = note;

    uint8_t note, velocity;

    if(pressed) {

        uint8_t octave = viewGetKeyboardOctave();

        note = (uint8_t) key + (octave * NUMBER_OF_NOTES_PER_OCTAVE);

        stored_pressed_note[key] = note;

        velocity = MAX_PARAM_VALUE;

    } else {

        note = stored_pressed_note[key];

        velocity = 0;

        stored_pressed_note[key] = -1;

    }

    noteQueueSubmit(note, velocity, pressed);

}

void seqSwitchAction(
    switch_id_t sw, 
    bool pressed, 
    bool timeout, 
    bool shift, 
    uint8_t num_bottom_keys_pressed) {

    uint8_t sw_num;

    switch(sw) {
        case SW_B0: sw_num = 0; break;
        case SW_B1: sw_num = 1; break;
        case SW_B2: sw_num = 2; break;
        case SW_B3: sw_num = 3; break;
        case SW_B4: sw_num = 4; break;
        case SW_B5: sw_num = 5; break;
        case SW_B6: sw_num = 6; break;
        case SW_B7: sw_num = 7; break;
        default: break;
    }

    uint8_t step_base = viewGetSequencerWindowStepBase();
    uint8_t step = sw_num + step_base;

    if(pressed) {

        // pressing shift and a seq switch resets the step to 'unwritten' state
        if(shift) {

            synth->seq.resetActiveStep(step);
            synth->seq.resetWrittenStep(step);

        // if the step is active, prepare for p lock entry otherwise set the step
        } else {

            if(!synth->seq.isStepActive(step)) {

                if(!synth->seq.isStepWritten(step)) {

                    synth->seq.setStepData(step, synth->new_step);
                    synth->seq.setWrittenStep(step);

                }

                // synth->seq.setStepData(step, synth->new_step);
                synth->seq.setActiveStep(step);
                // synth->seq.setWrittenStep(step);
    
                ignore_release_action[sw_num] = true;

            }

            uint8_t note = synth->seq.getStepNote(step);
            pLockEnable(true);
            pLockSetOrder(sw_num);
            pLockSetNote(sw_num, note);

        }

    } else {

        bool ignore_release = ignore_release_action[sw_num];

        bool p_lock_happened = pLockIsEngaged();

        if(!timeout && !ignore_release && !p_lock_happened) {

            synth->seq.resetActiveStep(step);

        } 

        if(!num_bottom_keys_pressed) {

            pLockEnable(false);
            pLockEngage(false);

        }

        ignore_release_action[sw_num] = false;
        pLockResetOrder(sw_num);

    }

    // TEST_PRINT_TEXT("P LOCK ORDER: ")

    // for(int i = 0; i < NUMBER_OF_BOTTOM_SWITCHES; i++) {
    
    //     // TEST_PRINT_ONE_TAB(viewGetPLockOrder(i))
    //     TEST_PRINT_ONE_TAB(pLockGetOrder(i))
        
    // }
    // TEST_PRINT_NL

}

void setSequencerWindow(bool window_sel) {

    selected_window_direction = window_sel;
    seq_window_change_pending = true;

}

void pageSequencer(
    switch_id_t sw, 
    bool pressed, 
    bool timeout, bool shift, 
    uint8_t num_bottom_keys_pressed
) {

    switch (sw) {

        case SW_B0:        
        case SW_B1: 
        case SW_B2:
        case SW_B3:
        case SW_B4:
        case SW_B5:
        case SW_B6:
        case SW_B7:
        
            seqSwitchAction(sw, pressed, timeout, shift, num_bottom_keys_pressed);
        
            break;
                
        case SW_T0:
        
            if(pressed) setSequencerWindow(false);
        
            break;
        
        case SW_T1:
        
            if(pressed) setSequencerWindow(true);
        
            break;
        
        case SW_T2:
        
            if(pressed) {
                toggleSetting(&synth->pattern_gen.on);
                synth->seq.resetLoopCount();
            }
        
            break;

        case SW_T3:
        
            if(pressed) patternGenExecute();
        
            break;
        
        case SW_T4:



            break;
    
    }

    if(seq_window_change_pending && !num_bottom_keys_pressed) {

        seq_window_change_pending = false;
        viewSetSequencerWindow(selected_window_direction);

    }

}

void setStorageState(storage_type_t type, bool shift) {

    // TEST_PRINT_TEXT("ALL DATA SIZE: ")
    // TEST_PRINT_ONE_NL(sizeof(*synth))
    // TEST_PRINT_TEXT("PATCH SIZE: ")
    // TEST_PRINT_ONE_NL(sizeof(synth->patch))
    // TEST_PRINT_TEXT("SEQ SIZE: ")
    // TEST_PRINT_ONE_NL(sizeof(*synth->seq.getPatternDataPtr()))
    // TEST_PRINT_TEXT("PATTERN GEN: ")
    // TEST_PRINT_ONE_NL(sizeof(synth->pattern_gen))
    // TEST_PRINT_TEXT("MIDI SIZE: ")
    // TEST_PRINT_ONE_NL(sizeof(synth->midi))
    // TEST_PRINT_TEXT("NEW STEP: ")
    // TEST_PRINT_ONE_NL(sizeof(synth->new_step))

    uint save_data_size = sizeof(synth->patch) + sizeof(*synth->seq.getPatternDataPtr()) + sizeof(synth->pattern_gen) + sizeof(synth->midi) + sizeof(synth->new_step);

    // TEST_PRINT_TEXT("SAVE SIZE: ")
    // TEST_PRINT_ONE_NL(save_data_size)

    storage_type_t current_type = storageGetType();
    storage_state_t state = storageGetState();

    storageSetBlockIndex(viewGetPresetNumber());

    if(type != current_type) {

        storageSetType(type);
        storageSetState(STORAGE_STATE_READY);

    } else {

        if(shift) {

            storageSetState(STORAGE_STATE_ACTIVE);

        }

    }

}

void checkForExitStorageState(switch_id_t sw) {

    if(sw == SW_SHIFT) return;

    storage_type_t type = storageGetType();

    if((type == STORAGE_TYPE_SAVE && sw != save_switch) || (type == STORAGE_TYPE_LOAD && sw != load_switch)){

        storageDisableTypeAndState();

    }

}

void pageSettings(switch_id_t sw, bool pressed, bool shift) {


    switch (sw) {

        case SW_B0:
        
            if(pressed) viewSetMode(MODE_MIDI_CHANNEL);
        
            break;
        
        case SW_B1:
        
            if(pressed) viewSetMode(MODE_MIDI_VELOCITY_DST);

            break;
        
        case SW_B2:
        
            if(pressed) viewSetMode(MODE_MIDI_MOD_WHEEL_DST);

            break;

        case SW_B3:
        
            if(pressed) viewSetMode(MODE_MIDI_AFTERTOUCH_DST);

            break;

        case SW_B4:
        
            if(pressed) toggleSetting(&synth->midi.clock_rx);

            break;

        case SW_B5:
        
            if(pressed) toggleSetting(&synth->midi.transport_rx);

            break;
        
        case SW_B6:
        
            // if(pressed) toggleSetting(&synth->midi.program_change_rx);
            if(pressed) viewSetMode(MODE_MIDI_PITCHBEND_DEPTH);

            break;
        
        case SW_B7:
        
            if(pressed) toggleSetting(&synth->midi.out_thru);
        
            break;
                
        case SW_T0:
        
            if(pressed) setStorageState(STORAGE_TYPE_SAVE, shift);
        
            break;
        
        case SW_T1:
        
            if(pressed) setStorageState(STORAGE_TYPE_LOAD, shift);
        
            break;
        
        case SW_T2:
        
            if(pressed) viewSetMode(MODE_SAVE_PRESET_SEL);
        
            break;

        case SW_T3:
        
        
        
            break;
        
        case SW_T4:


            break;
    
    }

}

void keyHandler(
    view_page_t page,
    switch_id_t sw, 
    bool pressed, 
    bool shift, 
    bool encoder_switch,
    bool timeout, 
    uint8_t num_all_switches_pressed, 
    uint8_t num_all_keys_pressed, 
    uint8_t num_bottom_keys_pressed
) {

    switch(page) {

        case PAGE_KEYS:

            pageKeyboard(sw, pressed);

            break;

        case PAGE_SEQ:
        
            pageSequencer(sw, pressed, timeout, shift, num_bottom_keys_pressed);

            break;

        case PAGE_SETTINGS:

            pageSettings(sw, pressed, shift);

            break;
    }

}

void setView(view_change_t view) {

        selected_view = view;
        view_change_pending = true;

}

void setViewCheckSeq(
    view_page_t page, 
    view_change_t norm_view, 
    view_change_t alt_view, 
    bool shift
) {

    bool p_lock_switch_held = pLockIsEnabled();
    
    // If we're on the seq page and press shift, or a p_lock switch is held 
    view_change_t view;

    if((page == PAGE_SEQ && shift) || p_lock_switch_held) {

        view = alt_view;

    } else {

        view = norm_view;
    }

    p_lock_exception = p_lock_switch_held;

    setView(view);

}

void executeNewView(uint8_t num_all_keys_pressed) {

    if(view_change_pending && (!num_all_keys_pressed || p_lock_exception)) {

        view_change_pending = false;
        p_lock_exception = false;

        viewChange(selected_view);

        view_changed = true;
    }

}

void playPressed() {

    if(!synth->seq.isRunning()) {

        synth->seq.run();
        midiEnqueueRealtime(MIDI_START);

    } else {

        synth->seq.stop();
        midiEnqueueRealtime(MIDI_STOP);

    }

}

// ################################
// PUBLIC FUNCTIONS
// ################################

void switchActionsSetup(synth_state_t *synth_state) {

    synth = synth_state;

}

void switchActionsTakeInput(
    switch_id_t sw, 
    bool pressed, 
    bool shift, 
    bool timeout,
    bool encoder_switch, 
    uint8_t num_all_switches_pressed, 
    uint8_t num_all_keys_pressed, 
    uint8_t num_bottom_keys_pressed
) {

    checkForExitStorageState(sw);

    view_page_t page = viewGetPage();

    switch (sw) {
        case SW_B0:
        case SW_B1:
        case SW_B2:
        case SW_B3:
        case SW_B4:
        case SW_B5:
        case SW_B6:
        case SW_B7:
        case SW_T0:
        case SW_T1:
        case SW_T2:
        case SW_T3:
        case SW_T4:

            keyHandler(
                page, 
                sw, 
                pressed, 
                shift, 
                encoder_switch, 
                timeout, 
                num_all_switches_pressed, 
                num_all_keys_pressed, 
                num_bottom_keys_pressed
            );

            break;

        case SW_SYNTH_1_LAYER:

            if(pressed) setViewCheckSeq(page, VIEW_CHANGE_KBD_SY1, VIEW_CHANGE_SEQ_SY1, shift);

            break;

        case SW_MOD_ENV_LAYER:

            if(pressed) setViewCheckSeq(page, VIEW_CHANGE_KBD_ENV, VIEW_CHANGE_SEQ_ENV, shift);

            break;

        case SW_SEQ:

            if(pressed)shift? setView(VIEW_CHANGE_SETTINGS) : setView(VIEW_CHANGE_SEQ_SEQ);
            
            break;

        case SW_PLAY:

            // if(pressed) synth->seq.isRunning()? synth->seq.stop() : synth->seq.run();
            if(pressed) playPressed();

            break;

        case SW_SHIFT:

            break;

        case SW_SYNTH_2_LAYER:

            if(pressed) setViewCheckSeq(page, VIEW_CHANGE_KBD_SY2, VIEW_CHANGE_SEQ_SY2, shift);

            break;

        case SW_MOD_LFO_LAYER:

            if(pressed) setViewCheckSeq(page, VIEW_CHANGE_KBD_LFO, VIEW_CHANGE_SEQ_LFO, shift);

            break;

        case SW_ENCODER:

            break;

        default:
            break;
    }

    executeNewView(num_all_keys_pressed);
    
        // TEST_PRINT_TEXT("PAGE: ")
        // TEST_PRINT_ONE_TAB(viewGetPage())
        // TEST_PRINT_TEXT("LAYER: ")
        // TEST_PRINT_ONE_TAB(viewGetLayer())
        // TEST_PRINT_TEXT("MODE: ")
        // TEST_PRINT_ONE_TAB(viewGetMode())
        // TEST_PRINT_TEXT("save: ")
        // TEST_PRINT_ONE_TAB(viewInSaveState())
        // TEST_PRINT_TEXT("load: ")
        // TEST_PRINT_ONE_TAB(viewInLoadState())
        // TEST_PRINT_TEXT("shift: ")
        // TEST_PRINT_ONE_TAB(shift)
        // TEST_PRINT_TEXT("switch: ")
        // TEST_PRINT_ONE_TAB(sw)
        // TEST_PRINT_TEXT("timeout: ")
        // TEST_PRINT_ONE_TAB(timeout)
        // TEST_PRINT_NL
        // TEST_PRINT_TEXT("SEQ WINDOW: ")
        // TEST_PRINT_ONE_TAB(viewGetSequencerWindow())
        // TEST_PRINT_TEXT("MIDI CLK: ")
        // TEST_PRINT_ONE_TAB(synth->midi.clock_rx)
        // TEST_PRINT_TEXT("PAT GEN: ")
        // TEST_PRINT_ONE_TAB(synth->pattern_gen.on)
        // TEST_PRINT_NL

    // TEST_PRINT_TEXT("ACTIVE:  ")
    // for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

    //     TEST_PRINT_ONE(synth->seq.isStepActive(i))

    // }

    // TEST_PRINT_NL

    // TEST_PRINT_TEXT("WRITTEN: ")
    // for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

    //     TEST_PRINT_ONE(synth->seq.isStepWritten(i))

    // }
    // TEST_PRINT_NL
    // TEST_PRINT_TEXT("P LOCK READY: ")
    // TEST_PRINT_ONE_NL(viewIsSubLayerPLockReady())

    // TEST_PRINT_TEXT("TIMEOUT: ")
    // TEST_PRINT_ONE_NL(timeout)

}

bool switchActionsViewChanged() {

    if(view_changed) {
        view_changed = false;
        return true;
    }
    return false;
}