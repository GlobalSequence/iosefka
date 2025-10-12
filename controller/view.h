#ifndef VIEW_H
#define VIEW_H

#include <stdio.h>
#include "pico/stdlib.h"

// ################################
// VIEW ENUMS
// ################################

typedef enum view_change_t {
    VIEW_CHANGE_KBD_SY1,
    VIEW_CHANGE_KBD_SY2,
    VIEW_CHANGE_KBD_ENV,
    VIEW_CHANGE_KBD_LFO,
    VIEW_CHANGE_KBD_SEQ,
    VIEW_CHANGE_SEQ_SEQ,
    VIEW_CHANGE_SEQ_SY1,
    VIEW_CHANGE_SEQ_SY2,
    VIEW_CHANGE_SEQ_ENV,
    VIEW_CHANGE_SEQ_LFO,
    VIEW_CHANGE_SETTINGS,

    NUM_VIEW_CHANGES

} view_change_t;

// ################################
// PAGE ENUMS
// ################################

typedef enum view_page_t {
	PAGE_KEYS,
	PAGE_SEQ,
    PAGE_P_LOCK,
	PAGE_SETTINGS,

	NUM_VIEW_PAGES
} view_page_t;

typedef enum view_page_event_t {
    EVT_PAGE_KEYS,
    EVT_PAGE_SEQ,
    EVT_PAGE_P_LOCK,
    EVT_PAGE_SETTINGS,

    NUM_PAGE_EVENTS
} view_page_event_t;

// ################################
// LAYER ENUMS
// ################################

typedef enum view_layer_t {
	LAYER_SYNTH_1,
	LAYER_SYNTH_2,
	LAYER_ENV,
	LAYER_LFO,
    LAYER_SYNTH_ENV_SPLIT,
    LAYER_SEQ,

	NUM_VIEW_LAYERS	
} view_layer_t;

typedef enum view_layer_event_t {
	EVT_LAYER_SYNTH_1,
	EVT_LAYER_SYNTH_2,
	EVT_LAYER_ENV,
	EVT_LAYER_LFO,
    EVT_LAYER_SEQ,
    EVT_LAYER_NULL,

	NUM_LAYER_EVENTS	
} view_layer_event_t;

// ################################
// MODE ENUMS
// ################################

typedef enum view_mode_t{
    MODE_KBD_OCT,
    MODE_MOD_DST_ENV_1,
    MODE_MOD_DST_ENV_2,
    MODE_MOD_DST_ENV_3,
    MODE_MOD_DST_LFO_1,
    MODE_MOD_DST_LFO_2,
    MODE_MOD_DST_LFO_3,
    MODE_SEQ_SET_BPM,
    MODE_SEQ_STEP_NOTE,
    MODE_SEQ_STEP_LENGTH,
    MODE_SEQ_P_LOCK_DEST,
    MODE_SAVE_PRESET_SEL,
    MODE_MIDI_CHANNEL,
    MODE_MIDI_VELOCITY_DST,
    MODE_MIDI_MOD_WHEEL_DST,
    MODE_MIDI_AFTERTOUCH_DST,
    MODE_MIDI_PITCHBEND_DEPTH,
    MODE_MOD_LAYER_SEL,
    

    NUM_VIEW_MODES
} view_mode_t;

typedef enum view_mode_event_t {
    EVT_MODE_KEYS,
    EVT_MODE_ENV,
    EVT_MODE_LFO,
    EVT_MODE_SEQ,
    EVT_MODE_SETTINGS,

    NUM_MODE_EVENTS
} view_mode_event_t;

// ################################
// MACHINE STATE STRUCTS
// ################################

typedef struct view_events_t {
    view_page_event_t    p;
    view_layer_event_t   l;
    view_mode_event_t    m;
} view_events_t;

typedef struct view_state_t {

    view_page_t page;
    view_layer_t layer;
    view_mode_t mode;

} view_state_t;

typedef struct machine_state_t {

    view_page_t page;
    view_layer_t layer;
    view_mode_t mode;
    uint8_t keyboard_octave;
    uint8_t sequencer_window;
    uint8_t seq_step_note;
    uint8_t seq_step_octave;
    uint8_t preset_number;

} machine_state_t;

#ifdef __cplusplus
extern "C"
{
#endif

void viewSetup();
void viewChange(view_change_t event);
void viewSetPage(view_page_t page);
void viewSetLayer(view_layer_t layer);
void viewSetMode(view_mode_t mode);
void viewSetKeyboardOctave(uint8_t octave);
void viewSetSequencerWindow(bool increment);
void viewSetPresetNumber(uint8_t preset);
void viewSetSeqParamStepNote(uint8_t note);
void viewSetSeqParamStepOctave(uint8_t octave);

view_page_t viewGetPage();
view_layer_t viewGetLayer();
view_mode_t viewGetMode();
uint8_t viewGetKeyboardOctave();
uint8_t viewGetSequencerWindow();
uint8_t viewGetSequencerWindowStepBase();
uint8_t viewGetPresetNumber();
bool viewIsSubLayerPLockReady();
bool viewDidPLockHappen();
uint8_t viewGetSeqParamStepNote();
uint8_t viewGetSeqParamStepOctave();
bool viewGetViewChangeFlagLed();


#ifdef __cplusplus
}
#endif

#endif