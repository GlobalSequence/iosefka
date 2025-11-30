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

void versionUpdate() {

    version_t version = storageLoadVersion();

    // version 1.0.0 update
    if(
        version.major == 0x00 && 
        version.minor == 0x01 && 
        version.patch == 0x02) {

        updateStoredVersionNumber(1, 0, 0);

        versionMigratePresetv0_1_2_to_1_0_0();

        version = storageLoadVersion();
    } // copy this and adapt for subsequent versions

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

    storageSaveSettings(sn);

}

void versionStorageTest() {

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
        n.patch.shared = o.patch.shared;
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
        for(int i = 0; i < NUM_MOD_SOURCES; i++) {
            n.patch.mod[i] = o.patch.mod[i];
        }
        n.seq_pattern_data = o.seq_pattern_data;
        n.step_data = o.step_data;
        n.pattern_gen = o.pattern_gen;
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

}