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
    LAYER_USER_PARAM,

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
    MODE_MIDI_BREATH_DST,
    MODE_MIDI_NOTE_DST,
    MODE_SET_SCROLL,
    MODE_SET_MOD_SRC_SEL,
    MODE_SET_MOD_DST_SEL,
    MODE_SET_PRESET_CHAR_INDEX,
    MODE_SET_PRESET_CHAR_SEL,
    MODE_USER_PARAM_SEL,
    MODE_USER_PARAM_DST,
    MODE_SEQ_WINDOW_SEL,
    MODE_SCOPE_DIV,
    MODE_MOD_LAYER_SEL,

    NUM_VIEW_MODES
} view_mode_t;

typedef enum view_mode_event_t {
    EVT_MODE_KEYS,
    EVT_MODE_ENV,
    EVT_MODE_LFO,
    EVT_MODE_SEQ,
    EVT_MODE_SETTINGS,
    EVT_MODE_USER_PARAM,

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
    uint8_t preset_number_selected;
    uint8_t preset_number_loaded;
    uint8_t preset_char_index;
    uint8_t preset_char_selected;
    uint8_t settings_row;
    uint8_t settings_mod_source;
    uint8_t current_user_param;
    // uint8_t* scope_div;
    bool scope_on;

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
void viewSetMachineStateMode(view_mode_t mode);
void viewResetMachineStateOnLoad();
// void viewSetScopeDivPtr(uint8_t* ptr);

void viewSetKeyboardOctave(uint8_t octave);
void viewSetSequencerWindow(bool increment);
void viewSetSequencerWindowByValue(uint8_t value);
void viewSetPresetNumberSelected(uint8_t preset);
void viewSetPresetNumberLoaded(uint8_t preset);
void viewSetPresetNumberSelectedAndLoaded(uint preset);
void viewSetSeqParamStepNote(uint8_t note);
void viewSetSeqParamStepOctave(uint8_t octave);
void viewSetSettingsRow(uint8_t row);
void viewSetSettingsModSource(uint8_t index);
void viewSetCurrentUserParameter(uint8_t index);
void viewSetEncoderTurnedWhileHeld();
void viewSetPresetCharIndex(uint8_t index);
void viewSetPresetCharSelected(uint8_t index);
// void viewSetScopeDiv(uint8_t value);
void viewSetScopeOn(bool on);

view_change_t viewGetLastViewChange();
view_page_t viewGetPage();
view_layer_t viewGetLayer();
view_mode_t viewGetMode();
uint8_t viewGetKeyboardOctave();
uint8_t viewGetSequencerWindow();
uint8_t viewGetSequencerWindowStepBase();
uint8_t viewGetPresetNumberSelected();
uint8_t viewGetPresetNumberLoaded();
bool viewIsSubLayerPLockReady();
// bool viewDidPLockHappen();
uint8_t viewGetSeqParamStepNote();
uint8_t viewGetSeqParamStepOctave();
bool viewGetViewChangeFlagLed();
uint8_t viewGetSettingsRow();
uint8_t viewGetSettingsModSource();
uint8_t viewGetModSourceFromSettings(); // convert settings specific mod sources to mod_sources_t
uint8_t viewGetCurrentUserParameter();
bool viewGetEncoderTurnedWhileHeld();
uint8_t viewGetPresetCharIndex();
uint8_t viewGetPresetCharSelected();
bool viewPresetSelectedIsPresetLoaded();
// uint8_t viewGetScopeDiv(); 
bool viewGetScopeOn();

void viewSetLatchSingle(uint8_t index);
void viewResetLatchSingle(uint8_t index);
void viewSetLatchAll();
bool viewGetLatchSingle(uint8_t index);

bool viewWillPageChangeOnViewChange(view_change_t event);

#ifdef __cplusplus
}
#endif

#endif