#include "storage.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "settings.h"
#include "../devices/devices.h"
#include "../common/globals.h"
#include "../common/test.h"

static const uint k_storage_block_size = 512;
static const uint k_settings_address_size = 64;
static const uint k_settings_address = (k_storage_block_size * NUMBER_OF_PRESETS) - k_settings_address_size;
static const uint k_first_written_address = (k_settings_address + k_settings_address_size - 1);

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

void storageSetType(storage_type_t type) { 
    
    current_type = type; 

    if(type != STORAGE_TYPE_NONE) last_type = type;

}

void storageSetState(storage_state_t state) { 

    current_state = state; 

    storage_state_changed = true;

}

void storageSetBlockIndex(uint8_t index) {

    // storage_address = index * k_storage_block_size;
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

        // storage_finished = true;
        // TEST_PRINT_TEXT("SAVE_OR_LOAD_HAPPENED\n")
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

    // TEST_PRINT_TEXT("STATE: ")
    // TEST_PRINT_ONE_NL(current_state);

    if(storage_state_changed) {

        storage_state_changed = false;

        return true;
    }

    return false;

}

void storageSave(uint block_num) {

    uint address = calculateAddress(block_num);

    synth_storable_t s = settingsGetStorableData();
            
    uint8_t* s_ptr = (uint8_t*)&s;
    
    devicesStoragePut(s_ptr, address, sizeof(s));

    midi_settings_t m = settingsGetMidiSettingsAll();

    uint8_t* m_ptr = (uint8_t*)&m;

    devicesStoragePut(m_ptr, k_settings_address, sizeof(m));

}

void storageSavePreset(synth_storable_t s, uint block_num) {

    uint address = calculateAddress(block_num);
            
    uint8_t* s_ptr = (uint8_t*)&s;
    
    devicesStoragePut(s_ptr, address, sizeof(s));

}

void storageSaveVersion(version_t v) {

    devicesStoragePut(&v.major, k_version_address, sizeof(version_t));

} 

void storageLoad(uint block_num) {

    uint address = calculateAddress(block_num);

    synth_storable_t s;

    size_t size = sizeof(s);

    devicesStorageGet((uint8_t*)&s, address, size);

    settingsSetStoredData(s);

    midi_settings_t m;

    devicesStorageGet((uint8_t*)&m, k_settings_address, sizeof(m));

    settingsSetMidiSettingsAll(m);
    
}

synth_storable_t storageLoadPreset(uint block_num) {

    uint address = calculateAddress(block_num);

    synth_storable_t s;

    size_t size = sizeof(s);

    devicesStorageGet((uint8_t*)&s, address, size);

    return s;

}

version_t storageLoadVersion() {

    version_t v;
    devicesStorageGet(&v.major, k_version_address, sizeof(version_t));

    return v;

}

// bool storageNeverWritten() {

// /*     uint8_t c;
//     uint8_t d = 0;

//     devicesStorageGet(&c, k_first_written_address, sizeof(c));

//     if(c) devicesStoragePut(&d, k_first_written_address, sizeof(d));

//     return (bool)c; */


//     // the eeprom comes with all bits set high
//     // so if the version comes up as 255.255.255, we know to write all default presets
//     version_t v;

//     devicesStorageGet(&v.major, k_version_address, sizeof(version_t));

//     if(v.major == 0xFF && v.minor == 0xFF && v.patch == 0xFF) {
     
//         v.major = VERSION_MAJOR;
//         v.minor = VERSION_MINOR;
//         v.patch = VERSION_PATCH;

//         storageSaveVersion(v);
        
//         return true;

//     }

//     return false; 

// }

// private

uint calculateAddress(uint8_t block_num) {

    return block_num * k_storage_block_size;

}