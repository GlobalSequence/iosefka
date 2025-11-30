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

void settingsSetMachineSettingsAll(machine_settings_t settings) {

    synth_state.settings = settings;

}



machine_settings_t settingsGetMachineSettingsAll() {

    return synth_state.settings;

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

void settingsSetInitPatch() {

    settingsSetStoredData(settingsGetInitPatch());

}

void settingsSetInitMachineSettings() {

    settingsSetMachineSettingsAll(settingsGetInitMachineSettings());

}

synth_storable_t settingsGetInitPatch() {

    synth_parameters_t p;

    pattern_data_t pd;

    step_data_t sd;

    pattern_gen_t pg;

    synth_storable_t st;

    // machine_settings_t ms;

    p.osc[0].scale          = 0x80;
    p.osc[0].octave         = 0x80;
    p.osc[0].x_morph        = 0x00;
    p.osc[0].y_morph        = 0x00;

    p.osc[1].scale          = 0x80;
    p.osc[1].octave         = 0x80;
    p.osc[1].x_morph        = 0x00;
    p.osc[1].y_morph        = 0x00;

    p.shared.phase_mod      = 0x00;
    p.shared.portamento     = 0x00;
    p.shared.sync           = 0x00;

    p.mixer.osc_1_mix       = 0xFF;
    p.mixer.osc_2_mix       = 0x00;
    p.mixer.noise_mix       = 0x00;

    p.filter.cutoff         = 0xFF;
    p.filter.resonance      = 0x00;
    p.filter.env_amt        = 0x80;
    p.filter.type           = 0x40;
    p.filter.fm             = 0x00;
    p.filter.tracking       = 0xFF;

    p.vca.level             = 0x00;
    p.vca.velocity_on       = 0x01;
    
    p.delay.amount          = 0x00;
    p.delay.rate            = 0x80;
    p.delay.diffusion       = 0x00;
    p.delay.feedback        = 0x00;
    p.delay.filter          = 0x00;

    p.env[0].attack         = 0x00;
    p.env[0].decay          = 0x80;
    p.env[0].sustain        = 0x80;
    p.env[0].release        = 0x80;
    p.env[0].loop           = 0x00;

    p.env[1].attack         = 0x00;
    p.env[1].decay          = 0x80;
    p.env[1].sustain        = 0x80;
    p.env[1].release        = 0x80;
    p.env[1].loop           = 0x00;

    p.env[2].attack         = 0x00;
    p.env[2].decay          = 0x80;
    p.env[2].sustain        = 0x80;
    p.env[2].release        = 0x80;
    p.env[2].loop           = 0x00;

    p.lfo[0].rate           = 0x00;
    p.lfo[0].shape          = 0x00;
    p.lfo[0].sync           = 0x00;
    p.lfo[0].phase          = 0x00;

    p.lfo[1].rate           = 0x00;
    p.lfo[1].shape          = 0x00;
    p.lfo[1].sync           = 0x00;
    p.lfo[1].phase          = 0x00;

    p.lfo[2].rate           = 0x00;
    p.lfo[2].shape          = 0x00;
    p.lfo[2].sync           = 0x00;
    p.lfo[2].phase          = 0x00;

    p.mod[0].destination    = 0x00;
    p.mod[0].amount         = 0x80;

    p.mod[1].destination    = 0x00;
    p.mod[1].amount         = 0x80;

    p.mod[2].destination    = 0x00;
    p.mod[2].amount         = 0x80;

    p.mod[3].destination    = 0x00;
    p.mod[3].amount         = 0x80;

    p.mod[4].destination    = 0x00;
    p.mod[4].amount         = 0x80;

    p.mod[5].destination    = 0x00;
    p.mod[5].amount         = 0x80;

    p.mod[6].destination    = 0x00;
    p.mod[6].amount         = 0x80;

    p.mod[7].destination    = 0x00;
    p.mod[7].amount         = 0x80;

    p.mod[8].destination    = 0x00;
    p.mod[8].amount         = 0x80;

    p.mod[9].destination    = 0x00;
    p.mod[9].amount         = 0x80;

    // step data;

    sd.note                 = 0x24;
    sd.length               = 0x00;
    sd.velocity             = 0xFF;
    sd.p_lock_param         = 0x00;
    sd.p_lock_value         = 0x00;

    // pattern data

    pd.bpm                  = 0x78; // 120 bpm
    pd.clock_div            = 0x04; // NUMBER_OF_CLOCK_DIVS - 1
    pd.direction            = 0x00; // forward
    pd.first_step           = 0x00; 
    pd.last_step            = 0x07;
    pd.probability          = 0xFF;
    pd.swing_amt            = 0x00;
    pd.transpose            = 0x0C; // 0 transposition

    // pattern generator

    pg.chance               = pd.probability;
    pg.base_note            = sd.note;
    pg.scale                = 0x00;
    pg.density              = 0x00;
    pg.repeat               = 0x01;
    pg.octave_range         = 0x03;
    pg.countdown            = 0x00;
    pg.on                   = true;

    // prepare storable data

    st.patch                = p;
    st.seq_pattern_data     = pd;
    st.step_data            = sd;
    st.pattern_gen          = pg;

    return st;

}

machine_settings_t settingsGetInitMachineSettings() {

    machine_setting_t ms;

    // machine settings

    ms.channel              = 0x0B;
    ms.pitch_bend_depth     = 0x01;
    ms.clock_rx             = true;
    ms.transport_rx         = true;
    ms.program_change_rx    = true;
    ms.out_thru             = false;
    ms.latch                = false;

    return ms;

}

// deprecated

/* void settingsToggleMidiSetting(midi_settings_toggle_type_t setting) {

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

} */

/* bool settingsGetMidiSetting(midi_settings_toggle_type_t setting) {

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

} */