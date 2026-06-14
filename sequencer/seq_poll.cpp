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

#include "seq_poll.h"
#include "pattern_generator.h"
#include "../common/test.h"
#include "sequencer.h"

static Sequencer *seq;
static bool *ext_clk;
static seqPollNoteFunc noteFunc;
static seqPollPLockFunc pLockFunc;
static seqPollClockTickPeriodFunc clockTickPeriodFunc;
static seqPollClockTickEventFunc clockTickEventFunc;
static seqPollStartEventFunc startEventFunc;

seqPollMidiCallbacks midi;

unsigned long last_update_sequencer;
static uint8_t seq_active_step;
static uint32_t tick_period;
static bool tick_happened_func = false;
static bool tick_happened_ext = false;

static uint8_t p_lock_index;
static uint8_t p_lock_value;
static uint8_t real_param_value;

static uint8_t most_recent_step_number;
static uint8_t seq_active_note;

// ################################
// PRIVATE FUNCTION DECLARATIONS
// ################################

void noteOffEvent();
void noteOnEvent();

// unused
void noteOffEventSingle(uint8_t note);
void seqStepTest();

// ################################
// PUBLIC FUNCTIONS 
// ################################

void seqPollSetup(
    Sequencer *sequencer, 
    bool *external_clk, 
    seqPollNoteFunc noteFunction, 
    seqPollPLockFunc pLockFunction,
    seqPollClockTickPeriodFunc clockTickPeriodFunction,
    seqPollClockTickEventFunc clockTickEventFunction,
    seqPollStartEventFunc startEventFunction
) {

    seq = sequencer;

    ext_clk = external_clk;

    noteFunc = noteFunction;

    pLockFunc = pLockFunction;

    clockTickPeriodFunc = clockTickPeriodFunction;

    clockTickEventFunc = clockTickEventFunction;

    startEventFunc = startEventFunction;

}

void seqPollTickCounterNow() {

    seq->tickCounter();

}

bool seqPollUpdate(unsigned long time) {

    bool ext_clk_active = *ext_clk;

    if(seq->justStopped()) {

        noteFunc(seq_active_note, 0, NOTE_OFF);
        pLockFunc(seq_active_step, false);

    }

    if(!ext_clk_active) {

        tick_period = seq->getTickPeriod();

        if(seq->justStarted()) {
    
            last_update_sequencer = time - tick_period;

            startEventFunc();
        }
    
        if(time - last_update_sequencer >= tick_period) {
            
            last_update_sequencer += tick_period;
    
            seq->tickCounter();

            tick_happened_func = true;
            
            clockTickEventFunc();
            
        }

    }

    // if a tick happened, we want to check to see if there is a new loop to generate a pattern.
    // this is outside the tick counter because the ticks are also triggered by MIDI
    patternGenCheckLoopCount();

    bool step_advanced = seq->update();

    if(step_advanced) {

        noteOffEvent();

        noteOnEvent();

    }

    return step_advanced;
}

void seqPollUpdateTickPeriod() {

    if(!*ext_clk) {

        clockTickPeriodFunc(tick_period);

    }

}

bool seqPollDidTickHappen() {

    if(tick_happened_func) {

        tick_happened_func = false;
        return true;

    }
    return false;

}

bool seqPollIsExtClockActive() {

    return *ext_clk;

}

int16_t seqPollGetStepNumber() {

    return seq->getCurrentStep();

}

// ################################
// PRIVATE FUNCTION DEFINITIONS
// ################################


void noteOnEvent() {

    uint8_t step = seq->getCurrentStep();

    if(seq->noteOnEvent(step)) {

        uint16_t rand_var = patternGenGetRand();

        bool allow_step = patternGenIsActive()? true : seq->stepProbabilityCheck(rand_var, step);

        if(allow_step) {

            seq_active_step = step;

            uint8_t new_note = seq->getStepNote(step);
    
            uint8_t velocity = seq->getStepVelocity(step);

            noteFunc(seq_active_note, 0, NOTE_OFF);
    
            noteFunc(new_note, velocity, NOTE_ON);
    
            pLockFunc(seq_active_step, true);

            seq_active_note = new_note;
        

        }

    }

}

void noteOffEvent() {

    for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) { 

        // if there are no more steps in the event
        if(seq->noteOffEvent(i)) {

            uint8_t note_to_turn_off = seq->getStepNote(i);

            noteFunc(note_to_turn_off, 0, NOTE_OFF);

            pLockFunc(i, false);

        }
    
    }

}



