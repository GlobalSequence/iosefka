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

static switchEventDisplayFunc dispFunc;

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

uint8_t last_switch_pressed;
bool wait_for_sy1_release = false;
bool wait_for_sy2_release = false;
bool view_switch_pressed[NUM_VIEW_SWITCHES];

// ################################
// PRIVATE FUNCTIONS
// ################################

void toggleSetting(bool *state) {

    *state = !*state;

}

void pageKeyboard(switch_id_t sw, bool pressed, bool shift) {

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

    uint8_t note, velocity;

    if(pressed) {

        // option to reset oscillator 
        if(shift) synth->ext_mod.osc_reset = true;

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
        
            // if(pressed) setSequencerWindow(false);
            if(pressed) {
                if(!shift) setSequencerWindow(false);
                else synth->seq.shiftByPatternLength(false);
            }
        
            break;
        
        case SW_T1:
        
            // if(pressed) setSequencerWindow(true);
            if(pressed) {
                if(!shift) setSequencerWindow(true);
                else synth->seq.shiftByPatternLength(true);
            }
        
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
    // TEST_PRINT_ONE_NL(sizeof(synth->settings))
    // TEST_PRINT_TEXT("NEW STEP: ")
    // TEST_PRINT_ONE_NL(sizeof(synth->new_step))

    uint save_data_size = sizeof(synth->patch) + sizeof(*synth->seq.getPatternDataPtr()) + sizeof(synth->pattern_gen) + sizeof(synth->settings) + sizeof(synth->new_step);

    // TEST_PRINT_TEXT("SAVE SIZE: ")
    // TEST_PRINT_ONE_NL(save_data_size)

    storage_type_t current_type = storageGetType();
    storage_state_t state = storageGetState();

    storageSetBlockIndex(viewGetPresetNumberSelected());

    if(type != current_type) {

        storageSetType(type);
        storageSetState(STORAGE_STATE_READY);

    } else {

        if(shift) {

            storageSetState(STORAGE_STATE_ACTIVE);
            viewSetPresetNumberLoaded(viewGetPresetNumberSelected());

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
        
            if(pressed) {
                viewSetMode(MODE_MIDI_CHANNEL);
                viewSetSettingsRow(SET_ROW_MIDI_CHAN);
            }
        
            break;
        
        case SW_B1:
        
            if(pressed) {
                
                viewSetMode(MODE_MIDI_VELOCITY_DST);
                viewSetSettingsModSource(SET_MOD_SRC_VELOCITY);
                viewSetSettingsRow(SET_ROW_MOD_DST);

                if(last_switch_pressed == SW_B1) {
                    toggleSetting(&synth->patch.vca.velocity_on);
                    viewSetSettingsRow(SET_ROW_VCA_VELOCITY);
                }

            }

            break;
        
        case SW_B2:
        
            if(pressed) {
                viewSetMode(MODE_MIDI_MOD_WHEEL_DST);
                viewSetSettingsModSource(SET_MOD_SRC_MOD_WHEEL);
                viewSetSettingsRow(SET_ROW_MOD_DST);
            }

            break;

        case SW_B3:
        
            if(pressed) {
                viewSetMode(MODE_MIDI_AFTERTOUCH_DST);
                viewSetSettingsModSource(SET_MOD_SRC_AFTERTOUCH);
                viewSetSettingsRow(SET_ROW_MOD_DST);
            }

            break;

        case SW_B4:
        
            if(pressed) {
                viewSetMode(MODE_SET_SCROLL);
                toggleSetting(&synth->settings.clock_rx);
                viewSetSettingsRow(SET_ROW_MIDI_CLOCK);
            }

            break;

        case SW_B5:
        
            if(pressed) {
                viewSetMode(MODE_SET_SCROLL);
                toggleSetting(&synth->settings.transport_rx);
                viewSetSettingsRow(SET_ROW_MIDI_TRANS);
            }

            break;
        
        case SW_B6:

            if(pressed){ 
                viewSetMode(MODE_MIDI_PITCHBEND_DEPTH);
                viewSetSettingsRow(SET_ROW_PB_DEPTH);
            }

            break;
        
        case SW_B7:
        
            // if(pressed) toggleSetting(&synth->settings.out_thru);
            if(pressed) {
                viewSetMode(MODE_SET_SCROLL);
                toggleSetting(&synth->settings.latch);
                viewSetSettingsRow(SET_ROW_POT_LATCH);
            }        

            break;
                
        case SW_T0:
        
            if(pressed) setStorageState(STORAGE_TYPE_SAVE, shift);

            break;
        
        case SW_T1:
        
            if(pressed) setStorageState(STORAGE_TYPE_LOAD, shift);
        
            break;
        
        case SW_T2:
        
            if(pressed) {
                viewSetMode(MODE_SAVE_PRESET_SEL);
                viewSetSettingsRow(SET_ROW_PRESET_NUM);
            }
        
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

            pageKeyboard(sw, pressed, shift);

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

bool checkViewSwitchesMinusCurrent(view_switch_t sw) {

    for(int i = 0; i < NUM_VIEW_SWITCHES; i++) {

        if(i != sw && view_switch_pressed[i]) {
            
            // TEST_PRINT_ONE_NL(i);   
            return true;
        }

    }

    return false;

}

void handleViewSwitch(view_switch_t sw, bool state) {

    view_switch_pressed[sw] = state;

    if(sw != VIEW_SWITCH_SY1) wait_for_sy1_release = false;
    if(sw != VIEW_SWITCH_SY2) wait_for_sy2_release = false;

}

// ################################
// PUBLIC FUNCTIONS
// ################################

void switchActionsSetup(synth_state_t *synth_state, switchEventDisplayFunc func) {

    synth = synth_state;
    dispFunc = func;

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
    view_layer_t layer = viewGetLayer();
    view_mode_t mode = viewGetMode();

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

        case SW_SYNTH_1_VIEW:

            // if(pressed) setViewCheckSeq(page, VIEW_CHANGE_KBD_SY1, VIEW_CHANGE_SEQ_SY1, shift);

            handleViewSwitch(VIEW_SWITCH_SY1, pressed);

            if(pressed) {

                if(checkViewSwitchesMinusCurrent(VIEW_SWITCH_SY1)) {

                    viewSetPage(PAGE_KEYS);
                    viewSetMachineStateMode(MODE_KBD_OCT);

                } else if (layer == LAYER_SYNTH_1 || layer == LAYER_SYNTH_ENV_SPLIT) {

                    wait_for_sy1_release = true;

                } else {

                    // viewChange(VIEW_CHANGE_KBD_SY1);
                    setView(VIEW_CHANGE_KBD_SY1);
                }

            } else {

                if(wait_for_sy1_release) {

                    wait_for_sy1_release = false;
                    // viewChange(VIEW_CHANGE_KBD_SY1);
                    setView(VIEW_CHANGE_KBD_SY1);
                }
            
            }

            break;

        case SW_MOD_ENV_VIEW:

            // if(pressed) setViewCheckSeq(page, VIEW_CHANGE_KBD_ENV, VIEW_CHANGE_SEQ_ENV, shift);
            handleViewSwitch(VIEW_SWITCH_ENV, pressed);
            if(pressed) setView(VIEW_CHANGE_KBD_ENV); /* viewChange(VIEW_CHANGE_KBD_ENV); */ 

            break;

        case SW_SEQ_VIEW:

            // if(pressed)shift? setView(VIEW_CHANGE_SETTINGS) : setView(VIEW_CHANGE_SEQ_SEQ);
            handleViewSwitch(VIEW_SWITCH_SEQ, pressed);
            if(pressed) {

                if(checkViewSwitchesMinusCurrent(VIEW_SWITCH_SEQ)) {

                    viewSetPage(PAGE_SEQ);
                    viewSetMachineStateMode(MODE_SEQ_SET_BPM);

                } else if(shift) {

                    // viewChange(VIEW_CHANGE_SETTINGS);
                    setView(VIEW_CHANGE_SETTINGS);

                } else {

                    // viewChange(VIEW_CHANGE_SEQ_SEQ);
                    setView(VIEW_CHANGE_SEQ_SEQ);
                }

            }

            
            break;

        case SW_PLAY:

            // if(pressed) synth->seq.isRunning()? synth->seq.stop() : synth->seq.run();
            if(pressed) playPressed();

            break;

        case SW_SHIFT:

            // if(pressed && viewGetMode() == MODE_USER_PARAM_DST) viewSetMachineStateMode(MODE_USER_PARAM_SEL);
            // else if(!pressed && viewGetMode() == MODE_USER_PARAM_SEL) viewSetMachineStateMode(MODE_USER_PARAM_DST);
            // if(layer == LAYER_USER_PARAM) {

            //     if(pressed) viewSetMode(MODE_USER_PARAM_SEL);
            //     else viewSetMachineStateMode(MODE_USER_PARAM_DST);

            // } else 
            switch(mode) {

                // selecting the user parameter
                case MODE_USER_PARAM_DST:
                    if(pressed) viewSetMachineStateMode(MODE_USER_PARAM_SEL);
                    break;
                
                case MODE_USER_PARAM_SEL:
                    if(!pressed) viewSetMachineStateMode(MODE_USER_PARAM_DST);
                    break;

                // setting the preset name character selection
                case MODE_SET_PRESET_CHAR_SEL:
                    if(pressed) viewSetMachineStateMode(MODE_SET_PRESET_CHAR_INDEX);
                    break;

                case MODE_SET_PRESET_CHAR_INDEX:
                    if(!pressed) viewSetMachineStateMode(MODE_SET_PRESET_CHAR_SEL);
                    break;

            }

            break;

        case SW_SYNTH_2_VIEW:

            // if(pressed) setViewCheckSeq(page, VIEW_CHANGE_KBD_SY2, VIEW_CHANGE_SEQ_SY2, shift);
            handleViewSwitch(VIEW_SWITCH_SY2, pressed);

            // TEST_PRINT_TEXT("SY2: ")
            // TEST_PRINT_ONE_NL(pressed)

            if(pressed) {

                if(layer == LAYER_USER_PARAM && !shift) { 
                    
                    wait_for_sy2_release = true;
                
                } else {

                    // viewChange(VIEW_CHANGE_KBD_SY2);
                    // setView(VIEW_CHANGE_KBD_SY2);
                    if(shift) {
                        // crazy hack because "User Param" isn't a 'view'. 
                        // it's because the view change process is a bit convoluted
                        viewChange(VIEW_CHANGE_KBD_SY2);
                        viewSetLayer(LAYER_USER_PARAM);
                        viewSetMachineStateMode(MODE_USER_PARAM_DST);
                        view_changed = true;

                    } else {

                        setView(VIEW_CHANGE_KBD_SY2);

                    }

                }

            } else {

                if(wait_for_sy2_release == true) {

                    wait_for_sy2_release = false;
                    // viewChange(VIEW_CHANGE_KBD_SY2);
                    setView(VIEW_CHANGE_KBD_SY2);
                }
                

            }
            TEST_PRINT_ONE_NL(viewGetLayer())

            break;

        case SW_MOD_LFO_VIEW:

            // if(pressed) setViewCheckSeq(page, VIEW_CHANGE_KBD_LFO, VIEW_CHANGE_SEQ_LFO, shift);
            handleViewSwitch(VIEW_SWITCH_LFO, pressed);
            if(pressed) setView(VIEW_CHANGE_KBD_LFO); /* viewChange(VIEW_CHANGE_KBD_LFO); */

            break;

        case SW_ENCODER:
            
            if(!pressed && !viewGetEncoderTurnedWhileHeld()) { // TODO: ADD ENCODER TURN EXCEPTION
                    
                if(page == PAGE_SETTINGS) {
                
                    if(mode == MODE_SET_SCROLL) {

                        switch(viewGetSettingsRow()) {
                            
                            // SETTINGS VALUES SELECTED BY ENCODER MODE
                            case SET_ROW_PRESET_NUM: viewSetMode(MODE_SAVE_PRESET_SEL); break;
                            case SET_ROW_PRESET_NAME: viewSetMode(MODE_SET_PRESET_CHAR_SEL); break;
                            case SET_ROW_MIDI_CHAN: viewSetMode(MODE_MIDI_CHANNEL); break;
                            case SET_ROW_PB_DEPTH: viewSetMode(MODE_MIDI_PITCHBEND_DEPTH); break;
                            case SET_ROW_MOD_SRC: viewSetMode(MODE_SET_MOD_SRC_SEL); break;
                            case SET_ROW_MOD_DST: viewSetMode(MODE_SET_MOD_DST_SEL); break;

                            // SETTINGS VALUES TOGGLED
                            case SET_ROW_MIDI_CLOCK: toggleSetting(&synth->settings.clock_rx); break;
                            case SET_ROW_MIDI_TRANS: toggleSetting(&synth->settings.transport_rx); break;
                            case SET_ROW_MIDI_PGM: toggleSetting(&synth->settings.program_change_rx); break;
                            // needs to be toggled manually because it's not a boolean
                            case SET_ROW_LEGATO: synth->patch.shared.legato = synth->patch.shared.legato? 0 : 1;  break; 
                            case SET_ROW_OSC_DUO: break; // TODO: IMPLEMENT OSC DUO
                            case SET_ROW_VCA_VELOCITY: toggleSetting(&synth->patch.vca.velocity_on); break;
                            case SET_ROW_POT_LATCH: toggleSetting(&synth->settings.latch); break;
                            case SET_ROW_INIT_PATCH: 
                                if(shift) {
                                    settingsSetInitPatch();
                                    dispFunc(DISP_EVT_INIT_PATCH, true);
                                }
                                break;

                            default: break;
                            // case SET_ROW_: viewSetMode(MODE_); break;
                            // case SET_ROW_: toggleSetting(synth->); break;

                        }

                    } else {

                        viewSetMode(MODE_SET_SCROLL);

                    }


                }
            
            }

            break;

        default:
            break;
    }

    // TODO: if page is changing, wait. If page is the same, keep truckin
    executeNewView(num_all_keys_pressed);
    
    if(pressed) last_switch_pressed = sw;

}

bool switchActionsViewChanged() {

    if(view_changed) {
        view_changed = false;
        return true;
    }
    return false;
}