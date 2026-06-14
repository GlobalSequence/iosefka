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

#ifndef SEQ_POLL_H
#define SEQ_POLL_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "sequencer.h"

typedef void (*seqPollNoteFunc)(uint8_t, uint8_t, bool);
typedef void (*seqPollPLockFunc)(uint8_t, bool);
typedef void (*seqPollClockTickPeriodFunc)(uint32_t);
typedef void (*seqPollClockTickEventFunc)(void);
typedef void (*seqPollStartEventFunc)(void);

// for future refactoring efforts... if that ever happens
struct seqPollMidiCallbacks {
    void (*tick)();
    void (*start)();
    void (*stop)();
};

#ifdef __cplusplus
extern "C"
{
#endif

void seqPollSetup(
    Sequencer *sequencer, 
    bool *external_clock, 
    seqPollNoteFunc noteFunction, 
    seqPollPLockFunc pLockFunction,
    seqPollClockTickPeriodFunc clockTickPeriodFunction,
    seqPollClockTickEventFunc clockTickEventFunction,
    seqPollStartEventFunc startEventFunction
);

void seqPollTickCounterNow();

bool seqPollUpdate(unsigned long time);

void seqPollUpdateTickPeriod();

bool seqPollDidTickHappen();

bool seqPollIsExtClockActive();

int16_t seqPollGetStepNumber();

#ifdef __cplusplus
}
#endif

#endif