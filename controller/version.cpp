#include "version.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "../common/test.h"

#include "common/globals.h"
#include "sequencer/sequencer.h"
#include "settings.h"
#include "storage.h"
#include "display.h"

// version 0.0.0 data

typedef struct oscillator_shared_params_v0_0_0_t {

    uint8_t portamento;         // 0 - 255
    uint8_t phase_mod;          // 0 - 255
    uint8_t sync;               // 0 - 1

} oscillator_shared_params_v0_0_0_t;

typedef struct synth_parameters_v0_0_0_t {
    oscillator_params_t                     osc[NUMBER_OF_OSCS];
    oscillator_shared_params_v0_0_0_t       shared;
    mixer_params_t                          mixer;
    filter_params_t                         filter;
    vca_params_t                            vca;
    delay_params_t                          delay; 
    envelope_params_t                       env[NUMBER_OF_ENVS];
    lfo_params_t                            lfo[NUMBER_OF_LFOS];
    modulation_params_t                     mod[NUM_MOD_SOURCES];
   //  midi_config_params_t           midi;

} synth_parameters_v0_0_0_t;

typedef struct step_data_v0_0_0_t {
    uint8_t note = 60;
    uint8_t velocity = 100;
    uint8_t length = 0; // steps will always have a length of 1, so 0 == 1
    uint8_t p_lock_param = 0;
    uint8_t p_lock_value = 0;
} step_data_v0_0_0_t;

typedef struct pattern_data_v0_0_0_t {
    uint16_t bpm = 120;
    uint8_t first_step = 0;
    uint8_t last_step = 7;
    uint8_t direction = FORWARD;
    uint8_t swing_amt = SWING_50;
    uint8_t clock_div = CLK_DIV_ONE;
    uint8_t transpose = 12;
    uint8_t probability = 255;
    uint8_t rotation = 0;
    step_data_v0_0_0_t step[NUMBER_OF_SEQUENCER_STEPS];
    bool step_written[NUMBER_OF_SEQUENCER_STEPS] = { false };
    bool step_active[NUMBER_OF_SEQUENCER_STEPS] = { false };

} pattern_data_v0_0_0_t;

typedef struct synth_storable_v0_0_0_t {

    synth_parameters_v0_0_0_t patch;
    pattern_data_v0_0_0_t seq_pattern_data;
    step_data_v0_0_0_t step_data;
    pattern_gen_t pattern_gen;

} synth_storable_v0_0_0_t;

// version 0.1.0 data

typedef struct synth_storable_v0_1_0_t {

    synth_parameters_t patch;
    pattern_data_t seq_pattern_data;
    step_data_t step_data;
    pattern_gen_t pattern_gen;
    assigned_pots_t assigned_pots;

} synth_storable_v0_1_0_t;

// version 0.1.1 data

typedef enum mod_destinations_v0_1_1_t {
    V0_1_1_MOD_DST_NONE,
    V0_1_1_MOD_DST_OSC_1_PITCH,
    V0_1_1_MOD_DST_OSC_1_OCTAVE,
    V0_1_1_MOD_DST_OSC_1_X_MORPH,
    V0_1_1_MOD_DST_OSC_1_Y_MORPH,
    V0_1_1_MOD_DST_OSC_2_PITCH,
    V0_1_1_MOD_DST_OSC_2_OCTAVE,
    V0_1_1_MOD_DST_OSC_2_X_MORPH,
    V0_1_1_MOD_DST_OSC_2_Y_MORPH,
    V0_1_1_MOD_DST_PORTAMENTO,
    V0_1_1_MOD_DST_PHASE_MOD,
    V0_1_1_MOD_DST_SYNC,
    V0_1_1_MOD_DST_MIX_OSC_1_MIX,
    V0_1_1_MOD_DST_MIX_OSC_2_MIX,
    V0_1_1_MOD_DST_MIX_NOISE_MIX,
    V0_1_1_MOD_DST_FILTER_CUTOFF,
    V0_1_1_MOD_DST_FILTER_RESONANCE,
    V0_1_1_MOD_DST_FILTER_ENV_AMT,
    V0_1_1_MOD_DST_FILTER_TYPE,
    V0_1_1_MOD_DST_FILTER_FM,
    V0_1_1_MOD_DST_FILTER_TRACKING,
    V0_1_1_MOD_DST_DELAY_RATE,
    V0_1_1_MOD_DST_DELAY_FEEDBACK,
    V0_1_1_MOD_DST_DELAY_AMOUNT,
    V0_1_1_MOD_DST_DELAY_DIFFUSION,
    V0_1_1_MOD_DST_DELAY_FILTER,
    V0_1_1_MOD_DST_DELAY_STEREO,
    V0_1_1_MOD_DST_ENV_1_ATTACK,
    V0_1_1_MOD_DST_ENV_1_DECAY,
    V0_1_1_MOD_DST_ENV_1_SUSTAIN,
    V0_1_1_MOD_DST_ENV_1_RELEASE,
    V0_1_1_MOD_DST_ENV_1_AMOUNT,
    V0_1_1_MOD_DST_ENV_2_ATTACK,
    V0_1_1_MOD_DST_ENV_2_DECAY,
    V0_1_1_MOD_DST_ENV_2_SUSTAIN,
    V0_1_1_MOD_DST_ENV_2_RELEASE,
    V0_1_1_MOD_DST_ENV_2_AMOUNT,
    V0_1_1_MOD_DST_ENV_3_ATTACK,
    V0_1_1_MOD_DST_ENV_3_DECAY,
    V0_1_1_MOD_DST_ENV_3_SUSTAIN,
    V0_1_1_MOD_DST_ENV_3_RELEASE,
    V0_1_1_MOD_DST_ENV_3_AMOUNT,
    V0_1_1_MOD_DST_LFO_1_RATE,
    V0_1_1_MOD_DST_LFO_1_SHAPE,
    V0_1_1_MOD_DST_LFO_1_PHASE,
    V0_1_1_MOD_DST_LFO_1_SYNC,
    V0_1_1_MOD_DST_LFO_1_AMOUNT,
    V0_1_1_MOD_DST_LFO_2_RATE,
    V0_1_1_MOD_DST_LFO_2_SHAPE,
    V0_1_1_MOD_DST_LFO_2_PHASE,
    V0_1_1_MOD_DST_LFO_2_SYNC,
    V0_1_1_MOD_DST_LFO_2_AMOUNT,
    V0_1_1_MOD_DST_LFO_3_RATE,
    V0_1_1_MOD_DST_LFO_3_SHAPE,
    V0_1_1_MOD_DST_LFO_3_PHASE,
    V0_1_1_MOD_DST_LFO_3_SYNC,
    V0_1_1_MOD_DST_LFO_3_AMOUNT,
    V0_1_1_MOD_DST_VCA, // drone mode uses this parameter

    V0_1_1_MOD_DST_ENV_1_DST,
    V0_1_1_MOD_DST_ENV_2_DST,
    V0_1_1_MOD_DST_ENV_3_DST,
    V0_1_1_MOD_DST_LFO_1_DST,
    V0_1_1_MOD_DST_LFO_2_DST,
    V0_1_1_MOD_DST_LFO_3_DST,

    V0_1_1_MOD_DST_OSC_BOTH_PITCH_FINE,
    V0_1_1_MOD_DST_OSC_BOTH_PITCH,
    V0_1_1_MOD_DST_OSC_BOTH_OCTAVE,
    V0_1_1_MOD_DST_OSC_BOTH_X_MORPH,
    V0_1_1_MOD_DST_OSC_BOTH_Y_MORPH,
    V0_1_1_MOD_DST_OSC_1_PITCH_FINE,
    V0_1_1_MOD_DST_OSC_2_PITCH_FINE,

    V0_1_1_NUM_MOD_DESTINATIONS

} mod_destinations_v0_1_1_t;

// private function declarations

void updateStoredVersionNumber(uint8_t major, uint8_t minor, uint8_t patch);
void versionMigratePresetv0_0_0_to_0_1_0();
void versionMigratePresetv0_1_0_to_0_1_1();
void versionMigratePresetv0_1_1_to_0_1_2();
void saveUpdatedPresetByIndex(synth_storable_t p, uint index);

// public functions

bool versionWriteVersionToCleanStorage() {

    version_t v = storageLoadVersion();

    if(
        v.major == 0xFF && 
        v.minor == 0xFF && 
        v.patch == 0xFF) {
     
        v.major = VERSION_MAJOR;
        v.minor = VERSION_MINOR;
        v.patch = VERSION_PATCH;

        storageSaveVersion(v);
        
        return true;

    }

    return false; 

}

void versionUpdate() {

    version_t version = storageLoadVersion();

    // version 0.1.0 update
    if(
        version.major == 0xFF && 
        version.minor == 0xFF && 
        version.patch == 0x00) {

        updateStoredVersionNumber(0, 1, 0);

        versionMigratePresetv0_0_0_to_0_1_0();
    
        version = storageLoadVersion();
    } 
    // version 0.1.1 update
    if(
        version.major == 0x00 && 
        version.minor == 0x01 && 
        version.patch == 0x00) {

        updateStoredVersionNumber(0, 1, 1);

        versionMigratePresetv0_1_0_to_0_1_1();

        version = storageLoadVersion();

    }
    // version 0.1.2 update
    if(
        version.major == 0x00 && 
        version.minor == 0x01 && 
        version.patch == 0x01) {

        updateStoredVersionNumber(0, 1, 2);

        versionMigratePresetv0_1_1_to_0_1_2();

        version = storageLoadVersion();
    }

}

// private functions

void updateStoredVersionNumber(uint8_t major, uint8_t minor, uint8_t patch) {

    version_t v;

    v.major = major;
    v.minor = minor;
    v.patch = patch;

    storageSaveVersion(v);

}

// put this inside a function that recursively runs for every version behind that a model is
void versionMigratePresetv0_0_0_to_0_1_0() {

    // p_l - loaded preset
    synth_storable_v0_0_0_t p_l;
    // p_s - preset to be saved back into storage
    synth_storable_t p_s;
    step_data_v0_0_0_t d;

    // loop through all presets
    for(int i = 0; i < NUMBER_OF_PRESETS; i++) {

        // load preset from storage which is in old format
        // p_l = storageLoadPreset(i);
        synth_storable_t temp = storageLoadPreset(i);
        memcpy(&p_l, &temp, sizeof(synth_storable_v0_0_0_t));

        // copy the parameters that haven't changed
        // p_s.patch = p_l.patch;
        p_s.patch.osc[0] = p_l.patch.osc[0];
        p_s.patch.osc[1] = p_l.patch.osc[1];
        // p_s.patch.shared = p_l.patch.shared;
        p_s.patch.mixer = p_l.patch.mixer;
        p_s.patch.filter = p_l.patch.filter;
        p_s.patch.vca = p_l.patch.vca;
        p_s.patch.delay = p_l.patch.delay;
        
        for(int j = 0; j < NUMBER_OF_ENVS; j++) {

            p_s.patch.env[j] = p_l.patch.env[j];

        } 

        for(int j = 0; j < NUMBER_OF_LFOS; j++) {

            p_s.patch.lfo[j] = p_l.patch.lfo[j];

        } 

        for(int j = 0; j < NUM_MOD_SOURCES; j++) {

            p_s.patch.mod[j] = p_l.patch.mod[j];

        } 

        // struct with new parameters
        p_s.patch.shared.portamento = p_l.patch.shared.portamento;
        p_s.patch.shared.phase_mod = p_l.patch.shared.phase_mod;
        p_s.patch.shared.sync = p_l.patch.shared.sync;
        
        p_s.patch.shared.bitrate_reduce = 0xFF;
        p_s.patch.shared.samplerate_reduce = 0xFF;
        
        p_s.pattern_gen = p_l.pattern_gen;

        p_s.seq_pattern_data.bpm = p_l.seq_pattern_data.bpm;
        p_s.seq_pattern_data.first_step = p_l.seq_pattern_data.first_step;
        p_s.seq_pattern_data.last_step = p_l.seq_pattern_data.last_step;
        p_s.seq_pattern_data.direction = p_l.seq_pattern_data.direction;
        p_s.seq_pattern_data.swing_amt = p_l.seq_pattern_data.swing_amt;
        p_s.seq_pattern_data.clock_div = p_l.seq_pattern_data.clock_div;
        p_s.seq_pattern_data.transpose = p_l.seq_pattern_data.transpose;
        p_s.seq_pattern_data.probability = p_l.seq_pattern_data.probability;
        p_s.seq_pattern_data.rotation = p_l.seq_pattern_data.rotation;

        for(int j = 0; j < NUMBER_OF_SEQUENCER_STEPS; j++) {

            p_s.seq_pattern_data.step_written[j] = p_l.seq_pattern_data.step_written[j];
            p_s.seq_pattern_data.step_active[j] = p_l.seq_pattern_data.step_active[j];

        }

        // copy loaded step_data to the old format 
        // (this is the format the loaded storage is currently in)
        memcpy(&d, &p_l.step_data, sizeof(step_data_v0_0_0_t));

        // load up the preset to be saved
        p_s.step_data.note = d.note;
        p_s.step_data.length = d.length;
        p_s.step_data.velocity = d.velocity;
        p_s.step_data.p_lock_param = d.p_lock_param;
        p_s.step_data.p_lock_value = d.p_lock_value;
        //p_s.step_data.step_prob = 0x00; // probability always set to '0', turning per step probability off


        for(int j = 0; j < NUMBER_OF_SEQUENCER_STEPS; j++) {
            memcpy(&d, &p_l.seq_pattern_data.step[j], sizeof(step_data_v0_0_0_t));

            p_s.seq_pattern_data.step[j].length = d.length;
            p_s.seq_pattern_data.step[j].note = d.note;
            p_s.seq_pattern_data.step[j].p_lock_param = d.p_lock_param;
            p_s.seq_pattern_data.step[j].p_lock_value = d.p_lock_value;
            p_s.seq_pattern_data.step[j].velocity= d.velocity;
            p_s.step_data.step_prob = 0x00; // the default value of per step probability

            /*             
            if(i == 0 || i == 63) {

                if(j==0)TEST_PRINT_TEXT("step notes\n")

                TEST_PRINT_ONE_NL(p_s.seq_pattern_data.step[j].note)

            } */
            
        } 

        for(int j = 0; j < NUMBER_OF_POTENTIOMETERS; j++) {

            p_s.assigned_pots.destination[j] = 0x00;

        }
        
        /*
        if(i == 0 || i == 63) {
            
            TEST_PRINT_TEXT("note and noise: ")
            TEST_PRINT_TWO_NL(p_s.pattern_gen.base_note, p_s.patch.mixer.noise_mix)

        }

        if(i == 0)settingsSetStoredData(p_s);

        storageSave(i); 
        */

/*         settingsSetStoredData(p_s);

        displayShowUpdatingPresets(i);

        sleep_ms(10);

        storageSave(i); */
        saveUpdatedPresetByIndex(p_s, i);

    }

}

void versionMigratePresetv0_1_0_to_0_1_1() {

    synth_storable_t p_s;
    synth_storable_v0_1_0_t p_l;

    for(int i = 0; i < NUMBER_OF_PRESETS; i++) {

        synth_storable_t temp = storageLoadPreset(i);
        memcpy(&p_l, &temp, sizeof(synth_storable_v0_1_0_t));

        p_s.patch = p_l.patch;
        p_s.seq_pattern_data = p_l.seq_pattern_data;
        p_s.pattern_gen = p_l.pattern_gen;
        p_s.step_data = p_l.step_data;
        p_s.assigned_pots = p_l.assigned_pots;

        for(int j = 0; j < 8; j++) {

            p_s.preset_name.name_char[j] = 0x00;

        }
        
        saveUpdatedPresetByIndex(p_s, i);

    }

}

void versionMigratePresetv0_1_1_to_0_1_2() {

    for(int i = 0; i < NUMBER_OF_PRESETS; i++) {

        synth_storable_t p_s = storageLoadPreset(i);

        for(int j = 0; j < NUM_MOD_SOURCES; j++) {

            uint8_t *d = &p_s.patch.mod[j].destination;

            if(*d >= MOD_DST_OSC_1_PITCH_FINE && *d < MOD_DST_OSC_2_PITCH_FINE) {

                *d += 1;

            } else if(*d >= MOD_DST_OSC_2_PITCH_FINE && *d < MOD_DST_OSC_BOTH_Y_MORPH) {

                *d += 2;

            } else if(*d == V0_1_1_MOD_DST_OSC_1_PITCH_FINE) {

                *d = MOD_DST_OSC_1_PITCH_FINE;

            } else if(*d == V0_1_1_MOD_DST_OSC_2_PITCH_FINE) {

                *d = MOD_DST_OSC_2_PITCH_FINE;
                
            }

        }

        saveUpdatedPresetByIndex(p_s, i);

    }

}

void saveUpdatedPresetByIndex(synth_storable_t p, uint index) {

        settingsSetStoredData(p);

        displayShowUpdatingPresets(index);

        sleep_ms(10);

        storageSave(index);

}