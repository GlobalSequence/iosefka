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

#include "storage.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "settings.h"
#include "../devices/devices.h"
#include "../common/globals.h"
#include "../common/test.h"

storageViewCallbacks view;

static const uint k_storage_block_size = 512;
static const uint k_settings_block_size = 64;
static const uint k_settings_address = (k_storage_block_size * NUMBER_OF_PRESETS) - k_settings_block_size;
static const uint k_first_written_address = (k_settings_address + k_settings_block_size - 1);

static const uint8_t k_version_size = 3;
static const uint k_version_address = (k_storage_block_size * NUMBER_OF_PRESETS) - k_version_size;

static storage_type_t current_type = STORAGE_TYPE_NONE;
static storage_type_t last_type = STORAGE_TYPE_NONE;
static storage_state_t current_state = STORAGE_STATE_OFF;
static uint storage_address;
static bool storage_state_changed = false;
static bool storage_finished = false;

// ################################
// PRIVATE FUNCTIONS
// ################################

uint calculateAddress(uint8_t block_num);

// ################################
// PUBLIC FUNCTIONS
// ################################

void storageSetup(storageViewCallbacks viewCallbacks) {

    view = viewCallbacks;

}

void storageSetType(storage_type_t type) { 
    
    current_type = type; 

    if(type != STORAGE_TYPE_NONE) last_type = type;

}

void storageSetState(storage_state_t state) { 

    current_state = state; 

    storage_state_changed = true;

}

void storageSetBlockIndex(uint8_t index) {

    storage_address = index;

}

void storageDisableTypeAndState() {

    storageSetType(STORAGE_TYPE_NONE);
    storageSetState(STORAGE_STATE_OFF);  

}

void storageExecuteTypeIfActive() {

    if(current_state == STORAGE_STATE_ACTIVE) {


        if(current_type == STORAGE_TYPE_SAVE) {

            storageSave(storage_address);

        } else if (current_type == STORAGE_TYPE_LOAD) {

            storageLoad(storage_address);

        } 

        storageSetState(STORAGE_STATE_FINISHED);

    }

}

void storageCheckForFinished() {

    if(storage_finished) {

        storage_finished = false;

        storageSetState(STORAGE_STATE_FINISHED);

    }

}

storage_type_t storageGetType() { return current_type; }

storage_type_t storageGetLastType() { return last_type; }

storage_state_t storageGetState() { 

    // the only reason for finished is to notify once, so we change the state  as soon as we observe it
    if(current_state == STORAGE_STATE_FINISHED) {

        storageDisableTypeAndState();

        return STORAGE_STATE_FINISHED;

    }

    return current_state; 

}

bool storageStateChanged() {

    if(storage_state_changed) {

        storage_state_changed = false;

        return true;
    }

    return false;

}

void storageWriteAllMemory(write_callback func) {

    settingsSetInitPatch();

    for(int i = 0; i < NUMBER_OF_PRESETS; i++) {

        settingsSetBlankLoadedPresetNameByIndex(i);

        storageSave(i);

        sleep_ms(10);

        func(i);

    }

    machine_settings_t m = settingsGetInitMachineSettings();

}

void storageSave(uint block_num) {

    synth_storable_t s = settingsGetStorableData();

    storageSavePreset(s, block_num);

    machine_settings_t m = settingsGetMachineSettingsAll();

    uint8_t* m_ptr = (uint8_t*)&m;

    devicesStoragePut(m_ptr, SETTINGS_ADDRESS, sizeof(m));
}

void storageSavePreset(synth_storable_t s, uint block_num) {

    s.preset_name = settingsGetLoadedPresetNameByIndex(block_num);

    uint address = calculateAddress(block_num);
            
    uint8_t* s_ptr = (uint8_t*)&s;
    
    devicesStoragePut(s_ptr, address, sizeof(s));

    view.presetNumber(block_num);

}

void storageSaveSettings(machine_settings_t s) {

    devicesStoragePut((uint8_t*)&s, SETTINGS_ADDRESS, sizeof(s));

}

void storageSaveVersion(version_t v) {

    devicesStoragePut(&v.major, k_version_address, sizeof(version_t));

} 

void storageLoad(uint block_num) {

    storageLoadPresetInternal(block_num);

    machine_settings_t m;

    devicesStorageGet((uint8_t*)&m, SETTINGS_ADDRESS, sizeof(m));

    settingsSetMachineSettingsAll(m);
    
}

synth_storable_t storageLoadPreset(uint block_num) {

    uint address = calculateAddress(block_num);

    synth_storable_t s;

    size_t size = sizeof(s);

    devicesStorageGet((uint8_t*)&s, address, size);

    settingsSetLoadedPresetNameByIndex(s.preset_name, block_num);

    view.presetNumber(block_num);

    return s;

}

void storageLoadPresetInternal(uint block_num) {

    synth_storable_t s = storageLoadPreset(block_num);

    settingsSetStoredData(s);

}

machine_settings_t storageLoadSettings() {

    machine_settings_t s;

    devicesStorageGet((uint8_t*)&s, SETTINGS_ADDRESS, sizeof(s));

    return s;

}

version_t storageLoadVersion() {

    version_t v;
    devicesStorageGet(&v.major, k_version_address, sizeof(version_t));

    return v;

}

uint calculateAddress(uint8_t block_num) {

    return block_num * STORAGE_BLOCK_SIZE;

}

uint storageGetAddressFromBlockNum(uint block_num) {

    return calculateAddress(block_num);

}

void storageLoadToPointer(uint8_t* p, uint address, uint size) {

    devicesStorageGet(p, address, size);

}

void storageLoadPresetNames() {

    for(int i = 0; i < NUMBER_OF_PRESETS; i++) {

        synth_storable_t s = storageLoadPreset(i);

        sleep_ms(1);

    }

}
