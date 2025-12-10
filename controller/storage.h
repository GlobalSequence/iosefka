#ifndef STORAGE_H
#define STORAGE_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "settings.h"
#include "common/globals.h"

#define STORAGE_LOAD_ADDRESS 0
#define STORAGE_BLOCK_SIZE 512
#define SETTINGS_BLOCK_SIZE 64
#define SETTINGS_ADDRESS (STORAGE_BLOCK_SIZE * NUMBER_OF_PRESETS) - SETTINGS_BLOCK_SIZE

// ################################
// STORAGE ENUMS
// ################################

typedef void (*saveFunc)(void);
typedef void (*loadFunc)(void);
typedef void (*write_callback)(uint8_t);

typedef enum storage_type_t {

    STORAGE_TYPE_SAVE,
    STORAGE_TYPE_LOAD,
    STORAGE_TYPE_NONE

} storage_type_t;

typedef enum storage_state_t {

    STORAGE_STATE_OFF,
    STORAGE_STATE_READY,
    STORAGE_STATE_ACTIVE,
    STORAGE_STATE_FINISHED

} storage_state_t;

#ifdef __cplusplus
extern "C"
{
#endif

void storageSetup(synth_state_t synth_state, saveFunc, loadFunc);
void storageSetType(storage_type_t type);
void storageSetState(storage_state_t state);
void storageSetBlockIndex(uint8_t index);
void storageDisableTypeAndState();
void storageExecuteTypeIfActive();
void storageCheckForFinished();

// big scary function
void storageWriteAllMemory(write_callback func);

void storageSave(uint block_num); // saves specified preset and machine settings
void storageLoad(uint block_num); // loads specified preset and machine settings

void storageSavePreset(synth_storable_t s, uint block_num);
synth_storable_t storageLoadPreset(uint block_num);

void storageSaveSettings(machine_settings_t s);
machine_settings_t storageLoadSettings();

void storageSaveVersion(version_t v);
version_t storageLoadVersion();

storage_type_t storageGetType();
storage_type_t storageGetLastType();
storage_state_t storageGetState();
bool storageStateChanged();

uint storageGetAddressFromBlockNum(uint block_num);
void storageLoadToPointer(uint8_t* p, uint address, uint size);

void storageLoadPresetNames();

// void storageLoadSettingsWithPointer(uint8_t* r);
// bool storageNeverWritten();

#ifdef __cplusplus
}
#endif

#endif
