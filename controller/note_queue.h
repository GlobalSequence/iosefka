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

#ifndef NOTE_QUEUE_H
#define NOTE_QUEUE_H

#include <stdio.h>
#include "pico/stdlib.h"

#define NOTE_BUFFER_SIZE 256

typedef struct note_data_t {
    uint8_t note;
    uint8_t velocity;
    bool on;
    bool trigger;
} note_data_t;

#ifdef __cplusplus
extern "C"
{
#endif

void noteQueueSetup(uint8_t* legato, uint8_t* osc_duo);
void noteQueueSubmit(uint8_t note, uint8_t velocity, bool on);

bool noteQueueAvailableInternal();
note_data_t noteQueuePullInternal();

bool noteQueueAvailableMidiOut();
note_data_t noteQueuePullMidiOut();
uint8_t noteQueueGetCount();

uint8_t noteQueueGetBassNote();

#ifdef __cplusplus
}
#endif

#endif
