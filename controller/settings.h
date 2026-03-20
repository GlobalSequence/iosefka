#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/globals.h"
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

typedef struct version_t {

    uint8_t major;
    uint8_t minor;
    uint8_t patch;

} version_t;

typedef struct preset_name_t {

    char name_char[PRESET_NAME_LENGTH];

} preset_name_t;

typedef struct machine_settings_t {
    // MIDI settings
    uint8_t channel;
    uint8_t pitch_bend_depth;
    bool program_change_rx = true;
    bool clock_rx = true;
    bool transport_rx = true;
    bool out_thru = MIDI_OUTPUT_OUT;
    // view settings
    bool latch = false;
} machine_setting_t;

typedef struct special_settings_t {
    uint8_t seq_octave_limit = 11;
    bool ext_clock_active;
} special_settings_t;

typedef struct user_param_t {

    uint8_t destination[NUMBER_OF_POTENTIOMETERS];

} user_param_t;

typedef struct synth_state_t {
    
    synth_parameters_t patch;
    Sequencer seq;
    step_data_t new_step;
    pattern_gen_t pattern_gen;
    machine_settings_t settings;
    special_settings_t special;
    external_modulation_t ext_mod;
    user_param_t user_param;

} synth_state_t;

typedef struct synth_storable_t {

    preset_name_t preset_name;
    synth_parameters_t patch;
    pattern_data_t seq_pattern_data;
    step_data_t step_data;
    pattern_gen_t pattern_gen;
    user_param_t user_param;

} synth_storable_t;

#ifdef __cplusplus
extern "C"
{
#endif

synth_state_t* settingsGetSynthState();

synth_storable_t settingsGetStorableData();

void settingsSetStoredData(synth_storable_t data);

void settingsSetMachineSettingsAll(machine_settings_t settings);

machine_settings_t settingsGetMachineSettingsAll();

external_modulation_t* settingsGetExternalModulation();

void settingsSetClockTickPeriod(uint32_t period);

void settingsSetClockTickEvent();

void settingsSetStartEvent();

void settingsSetInitPatch();

void settingsSetInitMachineSettings();

synth_storable_t settingsGetInitPatch();

machine_settings_t settingsGetInitMachineSettings(); 

void settingsSetLoadedPresetNameByIndex(preset_name_t name, uint8_t index);

void settingsSetBlankLoadedPresetNameByIndex(uint8_t index);

void settingsSetPresetNameChar(char c, uint8_t index, uint8_t preset); 

preset_name_t settingsGetLoadedPresetNameByIndex(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif
