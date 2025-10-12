#ifndef STORAGE_H
#define STORAGE_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "settings.h"

#define STORAGE_LOAD_ADDRESS 0

// ################################
// STORAGE ENUMS
// ################################

typedef void (*saveFunc)(void);
typedef void (*loadFunc)(void);

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

void storageSavePreset(synth_storable_t s, uint block_num);
synth_storable_t storageLoadPreset(uint block_num);

void storageSaveVersion(version_t v);
version_t storageLoadVersion();

storage_type_t storageGetType();
storage_type_t storageGetLastType();
storage_state_t storageGetState();
bool storageStateChanged();

void storageSave(uint block_num);
void storageLoad(uint block_num);

// bool storageNeverWritten();

#ifdef __cplusplus
}
#endif

#endif
