// Copyright (C) 2026 Cory Oleson <globeseq@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "version.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "../common/test.h"

#include "version_data.h"
#include "controller/storage.h"
#include "controller/display.h"


// private function declarations

void updateStoredVersionNumber(uint8_t major, uint8_t minor, uint8_t patch);
void versionMigratePresetv0_1_2_to_1_0_0();
void saveUpdatedPresetByIndex(synth_storable_t p, uint index);

void testNewSettings();

// public functions

// DEPRECATED
bool versionWriteVersionToCleanStorage() {

    // version_t v = storageLoadVersion();

    // if(
    //     v.major == 0xFF && 
    //     v.minor == 0xFF && 
    //     v.patch == 0xFF) {
     
    //     v.major = VERSION_MAJOR;
    //     v.minor = VERSION_MINOR;
    //     v.patch = VERSION_PATCH;

    //     storageSaveVersion(v);
        
    //     return true;

    // }

    return false; 

}


void versionTest() {

    testNewSettings();

}


// private functions

void updateStoredVersionNumber(uint8_t major, uint8_t minor, uint8_t patch) {

    version_t v;

    v.major = major;
    v.minor = minor;
    v.patch = patch;

    storageSaveVersion(v);

}


void saveUpdatedPresetByIndex(synth_storable_t p, uint index) {

        settingsSetStoredData(p);

        displayShowUpdatingPresets(index);

        sleep_ms(10);

        storageSave(index);

}


// version update functions

// DEPRECATED
void versionUpdate() {

    // version_t version = storageLoadVersion();

    // // version 1.0.0 update
    // if(
    //     version.major == 0x00 && 
    //     version.minor == 0x01 && 
    //     version.patch == 0x02) {

    //     updateStoredVersionNumber(1, 0, 0);

    //     versionMigratePresetv0_1_2_to_1_0_0();

    //     version = storageLoadVersion();
    // } // copy this and adapt for subsequent versions

}

void versionMigratePresetv0_1_2_to_1_0_0() {

    synth_storable_v0_1_2_t o;
    synth_storable_t n;
    machine_settings_v0_1_2_t so;
    machine_settings_t sn;

    for(int i = 0; i < NUMBER_OF_PRESETS; i++) {

        uint block = i;
        uint addr = storageGetAddressFromBlockNum(block);

        storageLoadToPointer((uint8_t*)&o, addr, sizeof(o));

        n.preset_name = o.preset_name;
        n.patch.osc[0] = o.patch.osc[0];
        n.patch.osc[1] = o.patch.osc[1];
        // n.patch.shared = o.patch.shared;

        n.patch.shared.portamento = o.patch.shared.portamento;
        n.patch.shared.phase_mod = o.patch.shared.phase_mod;
        n.patch.shared.sync = o.patch.shared.sync;
        // n.patch.shared.bitrate_reduce = o.patch.shared.bitrate_reduce;
        // n.patch.shared.samplerate_reduce = o.patch.shared.samplerate_reduce;
        n.patch.shared.bitrate_reduce = 0xFF;
        n.patch.shared.samplerate_reduce = 0xFF;
        n.patch.shared.legato = 0xFF;           // new param
        n.patch.shared.osc_duo = 0x00;          // new param

        n.patch.mixer = o.patch.mixer;
        n.patch.filter = o.patch.filter;
        n.patch.vca.level = o.patch.vca.level;
        n.patch.vca.velocity_on = true;         // new param
        n.patch.delay = o.patch.delay;
        for(int i = 0; i < NUMBER_OF_ENVS; i++) {
            n.patch.env[i].attack = o.patch.env[i].attack;
            n.patch.env[i].decay = o.patch.env[i].decay;
            n.patch.env[i].sustain = o.patch.env[i].sustain;
            n.patch.env[i].release = o.patch.env[i].release;
            n.patch.env[i].loop = false;        // new param
        }
        for(int i = 0; i < NUMBER_OF_LFOS; i++) {
            n.patch.lfo[i] = o.patch.lfo[i];
        }
        // for(int i = 0; i < NUM_MOD_SOURCES; i++) {
        //     n.patch.mod[i] = o.patch.mod[i];
        // }
        for(int i = 0; i < V0_1_2_NUM_MOD_SOURCES; i++) {
            n.patch.mod[i] = o.patch.mod[i];
        }
        n.patch.mod[MOD_SRC_NOTE].amount = 0xFF;        // new param
        n.patch.mod[MOD_SRC_NOTE].destination = 0x00;   // new param
        n.patch.mod[MOD_SRC_BREATH].amount = 0xFF;      // new param
        n.patch.mod[MOD_SRC_BREATH].destination = 0x00; // new param

        n.seq_pattern_data = o.seq_pattern_data;
        for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

            n.seq_pattern_data.step[i].step_prob = 0x00;

        }
        n.step_data = o.step_data;
        n.step_data.step_prob = 0x00;
        n.pattern_gen = o.pattern_gen;
        n.pattern_gen.on = false;
        // n.user_param = o.user_param;
        user_param_t new_user_param;
        for(int i = 0; i < NUMBER_OF_POTENTIOMETERS; i++) {
            if(i == 13) {
                new_user_param.destination[i] = MOD_DST_BITRATE_REDUCE;
            } else if (i == 14) {
                new_user_param.destination[i] = MOD_DST_SAMPLERATE_REDUCE;
            } else {
                new_user_param.destination[i] = MOD_DST_NONE;
            }

        }
        n.user_param = new_user_param;

        for(int i = 0; i < NUM_MOD_SOURCES; i++) {

            uint8_t d = n.patch.mod[i].destination;

            if(d > V1_0_0_MOD_DST_SYNC)             d += 2;
            if(d >= V1_0_0_MOD_DST_ENV_1_ATTACK)    d -= 3;
            if(d > V1_0_0_MOD_DST_ENV_1_AMOUNT)     d += 1;
            if(d > V1_0_0_MOD_DST_ENV_2_AMOUNT)     d += 1;
            if(d > V1_0_0_MOD_DST_ENV_3_AMOUNT)     d += 1;

            n.patch.mod[i].destination = d;
        }

        for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

            uint8_t d = n.seq_pattern_data.step[i].p_lock_param;

            if(d > V1_0_0_MOD_DST_SYNC)             d += 2;
            if(d >= V1_0_0_MOD_DST_ENV_1_ATTACK)    d -= 3;
            if(d > V1_0_0_MOD_DST_ENV_1_AMOUNT)     d += 1;
            if(d > V1_0_0_MOD_DST_ENV_2_AMOUNT)     d += 1;
            if(d > V1_0_0_MOD_DST_ENV_3_AMOUNT)     d += 1;

            n.seq_pattern_data.step[i].p_lock_param = d;
        }

        saveUpdatedPresetByIndex(n, i);

    }

    storageLoadToPointer((uint8_t*)&so, SETTINGS_ADDRESS, sizeof(so));

    sn.channel = so.channel;
    sn.clock_rx = so.clock_rx;
    sn.transport_rx = so.transport_rx;
    sn.pitch_bend_depth = so.pitch_bend_depth;
    sn.program_change_rx = so.program_change_rx;
    sn.out_thru = so.out_thru;
    sn.latch = false;                               // new

    storageSaveSettings(sn);

}

void versionParameterHotfix() {

    machine_settings_t s = storageLoadSettings();

    s.record_click = 0;

    storageSaveSettings(s);

    // for(int i = 0; i < NUMBER_OF_PRESETS; i++) {

    //     synth_storable_t s = storageLoadPreset(i);

    //     // s.patch.shared.bitrate_reduce = 0xFF;
    //     // s.patch.shared.samplerate_reduce = 0xFF;
    //     // s.pattern_gen.on = false;
    //     user_param_t new_user_param;
    //     // for(int i = 0; i < NUMBER_OF_POTENTIOMETERS; i++) {
    //     //     if(i == 13) {
    //     //         new_user_param.destination[i] = MOD_DST_BITRATE_REDUCE;
    //     //     } else if (i == 14) {
    //     //         new_user_param.destination[i] = MOD_DST_SAMPLERATE_REDUCE;
    //     //     } else {
    //     //         new_user_param.destination[i] = MOD_DST_NONE;
    //     //     }

    //     // }

    //     for(int i = 0; i < NUMBER_OF_POTENTIOMETERS; i++) {
    //         if(i == 11) {
    //             new_user_param.destination[i] = MOD_DST_ENV_1_LOOP;
    //         } else if (i == 12) {
    //             new_user_param.destination[i] = MOD_DST_ENV_2_LOOP;
    //         } else if(i == 13) {
    //             new_user_param.destination[i] = MOD_DST_ENV_3_LOOP;
    //         } else if (i == 14) {
    //             new_user_param.destination[i] = MOD_DST_VCA;
    //         } else {
    //             new_user_param.destination[i] = MOD_DST_NONE;
    //         }

    //     }
    //     s.user_param = new_user_param;

    //     storageSavePreset(s, i);

    //     displayShowUpdatingPresets(i);

    //     sleep_ms(10);

    // }

}

/* void versionStorageTest() {

    version_t v = storageLoadVersion();
    if(
        v.major == VERSION_MAJOR &&
        v.minor == VERSION_MINOR &&
        v.patch == VERSION_PATCH
    ) {
        return;
    } else {

        v.major = VERSION_MAJOR;
        v.minor = VERSION_MINOR;
        v.patch = VERSION_PATCH;

        storageSaveVersion(v);

    }

#ifdef VERSION_TEST

    synth_storable_v0_1_2_t o;
    synth_storable_t n;
    machine_settings_v0_1_2_t so;
    machine_settings_t sn;

    for(int i = 0; i < NUMBER_OF_PRESETS; i++) {

        uint block = i;
        uint addr = storageGetAddressFromBlockNum(block);

        storageLoadToPointer((uint8_t*)&o, addr, sizeof(o));

        n.preset_name = o.preset_name;
        n.patch.osc[0] = o.patch.osc[0];
        n.patch.osc[1] = o.patch.osc[1];
        n.patch.shared = o.patch.shared;
        n.patch.mixer = o.patch.mixer;
        n.patch.filter = o.patch.filter;
        n.patch.vca.level = o.patch.vca.level;
        n.patch.vca.velocity_on = true;         // new param
        n.patch.delay = o.patch.delay;
        for(int i = 0; i < NUMBER_OF_ENVS; i++) {
            n.patch.env[i].attack = o.patch.env[i].attack;
            n.patch.env[i].decay = o.patch.env[i].decay;
            n.patch.env[i].sustain = o.patch.env[i].sustain;
            n.patch.env[i].release = o.patch.env[i].release;
            n.patch.env[i].loop = false;        // new param
        }
        for(int i = 0; i < NUMBER_OF_LFOS; i++) {
            n.patch.lfo[i] = o.patch.lfo[i];
        }
        for(int i = 0; i < NUM_MOD_SOURCES; i++) {
            n.patch.mod[i] = o.patch.mod[i];
        }
        n.seq_pattern_data = o.seq_pattern_data;
        n.step_data = o.step_data;
        n.pattern_gen = o.pattern_gen;
        n.user_param = o.user_param;

        for(int i = 0; i < NUM_MOD_SOURCES; i++) {

            uint8_t d = n.patch.mod[i].destination;

            if(d > V1_0_0_MOD_DST_SYNC)             d += 2;
            if(d >= V1_0_0_MOD_DST_ENV_1_ATTACK)    d -= 3;
            if(d > V1_0_0_MOD_DST_ENV_1_AMOUNT)     d += 1;
            if(d > V1_0_0_MOD_DST_ENV_2_AMOUNT)     d += 1;
            if(d > V1_0_0_MOD_DST_ENV_3_AMOUNT)     d += 1;

            n.patch.mod[i].destination = d;
        }

        for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

            uint8_t d = n.seq_pattern_data.step[i].p_lock_param;

            if(d > V1_0_0_MOD_DST_SYNC)             d += 2;
            if(d >= V1_0_0_MOD_DST_ENV_1_ATTACK)    d -= 3;
            if(d > V1_0_0_MOD_DST_ENV_1_AMOUNT)     d += 1;
            if(d > V1_0_0_MOD_DST_ENV_2_AMOUNT)     d += 1;
            if(d > V1_0_0_MOD_DST_ENV_3_AMOUNT)     d += 1;

            n.seq_pattern_data.step[i].p_lock_param = d;
        }

        saveUpdatedPresetByIndex(n, i);

    }

    storageLoadToPointer((uint8_t*)&so, SETTINGS_ADDRESS, sizeof(so));

    sn.channel = so.channel;
    sn.clock_rx = so.clock_rx;
    sn.transport_rx = so.transport_rx;
    sn.pitch_bend_depth = so.pitch_bend_depth;
    sn.program_change_rx = so.program_change_rx;
    sn.out_thru = so.out_thru;
    sn.latch = false;                               // new

    storageSave(0);

#else

    synth_storable_v1_0_0_t o;
    synth_storable_t n;
    machine_settings_v1_0_0_t so;
    machine_settings_t sn;

    for(int i = 0; i < NUMBER_OF_PRESETS; i++) {

        uint block = i;
        uint addr = storageGetAddressFromBlockNum(block);

        storageLoadToPointer((uint8_t*)&o, addr, sizeof(o));

        n.preset_name = o.preset_name;
        n.patch.osc[0] = o.patch.osc[0];
        n.patch.osc[1] = o.patch.osc[1];
        // n.patch.shared = o.patch.shared;

        n.patch.shared.portamento = o.patch.shared.portamento;
        n.patch.shared.phase_mod = o.patch.shared.phase_mod;
        n.patch.shared.sync = o.patch.shared.sync;
        // n.patch.shared.bitrate_reduce = o.patch.shared.bitrate_reduce;
        // n.patch.shared.samplerate_reduce = o.patch.shared.samplerate_reduce;
        n.patch.shared.bitrate_reduce = 0xFF;
        n.patch.shared.samplerate_reduce = 0xFF;
        n.patch.shared.legato = 0xFF;
        n.patch.shared.osc_duo = 0x00;

        n.patch.mixer = o.patch.mixer;
        n.patch.filter = o.patch.filter;
        n.patch.vca.level = o.patch.vca.level;
        n.patch.delay = o.patch.delay;
        for(int i = 0; i < NUMBER_OF_ENVS; i++) {
            n.patch.env[i].attack = o.patch.env[i].attack;
            n.patch.env[i].decay = o.patch.env[i].decay;
            n.patch.env[i].sustain = o.patch.env[i].sustain;
            n.patch.env[i].release = o.patch.env[i].release;
        }
        for(int i = 0; i < NUMBER_OF_LFOS; i++) {
            n.patch.lfo[i] = o.patch.lfo[i];
        }
        // for(int i = 0; i < NUM_MOD_SOURCES; i++) {
        //     n.patch.mod[i] = o.patch.mod[i];
        // }

        for(int i = 0; i < V0_1_2_NUM_MOD_SOURCES; i++) {
            n.patch.mod[i] = o.patch.mod[i];
        }
        n.patch.mod[MOD_SRC_NOTE].amount = 0xFF;
        n.patch.mod[MOD_SRC_NOTE].destination = 0x00;
        n.patch.mod[MOD_SRC_BREATH].amount = 0xFF;
        n.patch.mod[MOD_SRC_BREATH].destination = 0x00;

        n.seq_pattern_data = o.seq_pattern_data;
        for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

            n.seq_pattern_data.step[i].step_prob = 0x00;

        }
        n.step_data = o.step_data;
        n.step_data.step_prob = 0x00;
        n.pattern_gen = o.pattern_gen;
        n.pattern_gen.on = false;
        n.user_param = o.user_param;

        for(int i = 0; i < NUM_MOD_SOURCES; i++) {

            uint8_t d = n.patch.mod[i].destination;

            if(d > V1_0_0_MOD_DST_SYNC)             d -= 2;
            if(d >= V1_0_0_MOD_DST_ENV_1_ATTACK)    d += 3;
            if(d > V1_0_0_MOD_DST_ENV_1_AMOUNT)     d -= 1;
            if(d > V1_0_0_MOD_DST_ENV_2_AMOUNT)     d -= 1;
            if(d > V1_0_0_MOD_DST_ENV_3_AMOUNT)     d -= 1;

            n.patch.mod[i].destination = d;
        }

        for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

            uint8_t d = n.seq_pattern_data.step[i].p_lock_param;

            if(d > V1_0_0_MOD_DST_SYNC)             d -= 2;
            if(d >= V1_0_0_MOD_DST_ENV_1_ATTACK)    d += 3;
            if(d > V1_0_0_MOD_DST_ENV_1_AMOUNT)     d -= 1;
            if(d > V1_0_0_MOD_DST_ENV_2_AMOUNT)     d -= 1;
            if(d > V1_0_0_MOD_DST_ENV_3_AMOUNT)     d -= 1;

            n.seq_pattern_data.step[i].p_lock_param = d;
        }

        saveUpdatedPresetByIndex(n, i);

    }

    storageLoadToPointer((uint8_t*)&so, SETTINGS_ADDRESS, sizeof(so));

    sn.channel = so.channel;
    sn.clock_rx = so.clock_rx;
    sn.transport_rx = so.transport_rx;
    sn.pitch_bend_depth = so.pitch_bend_depth;
    sn.program_change_rx = so.program_change_rx;
    sn.out_thru = so.out_thru;

    storageSave(0);

#endif

} */