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

#ifndef STEP_ENTRY_H
#define STEP_ENTRY_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "sequencer.h"

#ifdef __cplusplus
extern "C"
{
#endif

void stepEntrySetup(Sequencer* sequencer, bool* record_click);
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
bool stepEntryRunClick();

#ifdef __cplusplus
}
#endif

#endif