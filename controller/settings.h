#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../synth/patch.h"
#include "../sequencer/sequencer.h"
#include "../sequencer/pattern_generator.h"

typedef enum synth_special_params_t {
    SP_PARAM_TUNE,
    SP_PARAM_OCTAVE,
    SP_PARAM_SYNC,
    SP_PARAM_LFO_SHAPE,
    SP_PARAM_LFO_SYNC,
    SP_PARAM_MOD_DST,

    NUM_SPECIAL_PARAMS
} synth_special_params_t;

typedef enum midi_settings_toggle_type_t {

    MIDI_SETTING_CLOCK,
    MIDI_SETTING_TRANSPORT,
    MIDI_SETTING_PROGRAM_CHANGE,
    MIDI_SETTING_OUT_THRU,

    NUM_TOGGLEABLE_MIDI_SETTINGS

} midi_settings_toggle_type_t;

typedef struct version_t {

    uint8_t major;
    uint8_t minor;
    uint8_t patch;

} version_t;

typedef struct preset_name_t {

    char name_char[8];

} preset_name_t;

typedef struct midi_settings_t {
    uint8_t channel;
    uint8_t pitch_bend_depth;
    bool program_change_rx = true;
    bool clock_rx = true;
    bool transport_rx = true;
    bool out_thru = MIDI_OUTPUT_OUT;
} midi_settings_t;

typedef struct special_settings_t {
    uint8_t seq_octave_limit = 11;
    bool ext_clock_active;
} special_settings_t;

typedef struct assigned_pots_t {

    uint8_t destination[15];

} assigned_pots_t;

typedef struct synth_state_t {
    
    synth_parameters_t patch;
    Sequencer seq;
    step_data_t new_step;
    pattern_gen_t pattern_gen;
    midi_settings_t midi;
    special_settings_t special;
    external_modulation_t ext_mod;
    assigned_pots_t assigned_pots;

} synth_state_t;

typedef struct synth_storable_t {

    preset_name_t preset_name;
    synth_parameters_t patch;
    pattern_data_t seq_pattern_data;
    step_data_t step_data;
    pattern_gen_t pattern_gen;
    assigned_pots_t assigned_pots;

} synth_storable_t;

#ifdef __cplusplus
extern "C"
{
#endif

synth_state_t* settingsGetSynthState();

synth_storable_t settingsGetStorableData();

void settingsSetStoredData(synth_storable_t data);

void settingsSetMidiSettingsAll(midi_settings_t midi);

void settingsToggleMidiSetting(midi_settings_toggle_type_t setting);

bool settingsGetMidiSetting(midi_settings_toggle_type_t setting);

midi_settings_t settingsGetMidiSettingsAll();

external_modulation_t* settingsGetExternalModulation();

void settingsSetClockTickPeriod(uint32_t period);

void settingsSetClockTickEvent();

void settingsSetStartEvent();

#ifdef __cplusplus
}
#endif

#endif
