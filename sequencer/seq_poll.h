#ifndef SEQ_POLL_H
#define SEQ_POLL_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "sequencer.h"
// #include "../common/globals.h"
// #include "../controller/settings.h"

typedef void (*seqPollNoteFunc)(uint8_t, uint8_t, bool);
// typedef uint8_t (*seqPollPLockFunc)(uint8_t, uint8_t);
// typedef void (*seqPollKillNoteFunc)(void);
typedef void (*seqPollPLockFunc)(uint8_t, bool);
typedef void (*seqPollClockTickPeriodFunc)(uint32_t);
typedef void (*seqPollClockTickEventFunc)(void);
typedef void (*seqPollStartEventFunc)(void);

#ifdef __cplusplus
extern "C"
{
#endif

// void seqPollSetup(synth_state_t *synth_state);

void seqPollSetup(
    Sequencer *sequencer, 
    bool *external_clock, 
    seqPollNoteFunc noteFunction, 
    // seqPollKillNoteFunc killNoteFunction,
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

#ifdef __cplusplus
}
#endif

#endif