#ifndef STEP_ENTRY_H
#define STEP_ENTRY_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "sequencer.h"

#ifdef __cplusplus
extern "C"
{
#endif

void stepEntrySetup(Sequencer* sequencer);
void stepEntryRecordStart(); // remember to set direction to forward
void stepEntryRecordStop();

step_data_t stepEntryCopy(uint8_t index);
void stepEntryArmRotate(bool arm);
void stepEntryRotate(bool dir);
void stepEntryClearAllStepsPLockAndProbability();

void stepEntryMIDIInput(uint8_t note, uint8_t velocity, bool on);
bool stepEntrySetLiveRecordStep(uint8_t note, uint8_t velocity, bool on);

bool stepEntryGetStepRecord();
bool stepEntryIsRotateArmed();
bool stepEntryDidCopyOccur();

#ifdef __cplusplus
}
#endif

#endif