#include "settings.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "../synth/patch.h"
#include "../common/test.h"

synth_state_t synth_state;

synth_state_t* settingsGetSynthState() {

    return &synth_state;

}

synth_storable_t settingsGetStorableData() {

    synth_storable_t s;

    s.patch = synth_state.patch;
    s.seq_pattern_data = *synth_state.seq.getPatternDataPtr();
    s.step_data = synth_state.new_step;
    s.pattern_gen = synth_state.pattern_gen;

    return s;

}

void settingsSetStoredData(synth_storable_t data) {

    synth_state.patch = data.patch;
    synth_state.seq.setPatternData(data.seq_pattern_data);
    synth_state.new_step = data.step_data;
    synth_state.pattern_gen = data.pattern_gen;

}

void settingsSetMidiSettingsAll(midi_settings_t midi) {

    synth_state.midi = midi;

}

void settingsToggleMidiSetting(midi_settings_toggle_type_t setting) {

    bool *val_ptr;

    switch(setting) {

        case MIDI_SETTING_CLOCK:

            val_ptr = &synth_state.midi.clock_rx;

            break;

        case MIDI_SETTING_TRANSPORT:

            val_ptr = &synth_state.midi.transport_rx;
        
            break;


        case MIDI_SETTING_PROGRAM_CHANGE:

            val_ptr = &synth_state.midi.program_change_rx;
        
            break;

        case MIDI_SETTING_OUT_THRU:

            val_ptr = &synth_state.midi.out_thru;
        
            break;

    }

    *val_ptr = !*val_ptr;

}

bool settingsGetMidiSetting(midi_settings_toggle_type_t setting) {

    bool *val_ptr;

    switch(setting) {

        case MIDI_SETTING_CLOCK:

            return synth_state.midi.clock_rx;

            break;

        case MIDI_SETTING_TRANSPORT:

            return synth_state.midi.transport_rx;
        
            break;


        case MIDI_SETTING_PROGRAM_CHANGE:

            return synth_state.midi.program_change_rx;
        
            break;

        case MIDI_SETTING_OUT_THRU:

            return synth_state.midi.out_thru;
        
            break;

        default:
            return 0;

    }

}

midi_settings_t settingsGetMidiSettingsAll() {

    return synth_state.midi;

}

external_modulation_t* settingsGetExternalModulation() {

    return &synth_state.ext_mod;

}

void settingsSetClockTickPeriod(uint32_t period) {

    synth_state.ext_mod.clock_tick_period = period;

}

void settingsSetClockTickEvent() {

    synth_state.ext_mod.clock_tick_occurred = true;

}

void settingsSetStartEvent() {

    synth_state.ext_mod.start_occurred = true;

}