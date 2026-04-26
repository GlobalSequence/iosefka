#include "step_entry.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "sequencer.h"
#include "../common/test.h"
// ###################
// private variables
// ###################
static Sequencer* seq;

// live step record variables
bool step_record = false;

uint8_t step_tick_boundary[NUMBER_OF_CLOCK_DIVS];
const uint8_t k_tick_boundary_divider = 4;

sequencer_direction_t last_direction; 

int16_t input_step;
uint16_t live_input_counter = 0;
bool last_live_input_state = false;

// step input variables

// rotate steps
bool rotate_armed;

// copy steps
bool copy_occurred;

// private function declarations
void setStepLength(uint16_t first, uint16_t last, int16_t step);
void copyStepByIndex(uint to, step_data_t data, bool active, bool written);

// public functions

void stepEntrySetup(Sequencer* sequencer) {

    seq = sequencer;

    for(int i = 0; i < NUMBER_OF_CLOCK_DIVS; i++) {

        // get the number of ticks per clock division (ignore swing ticks)
        uint8_t tick = seq->getSwingTicksByDivision(i, 0, 0);
        // get the number of 'pre-ticks' that count as registered for the next step
        uint8_t overflow = tick / k_tick_boundary_divider;
        // set the step tick boundary 
        step_tick_boundary[i] = tick - overflow;

    }

}

void stepEntryRecordStart() {

    // set the sequencer to forward. ngl I don't want to figure out all the stuff to account for the other directions LMAO.
    last_direction = (sequencer_direction_t) seq->getDirection();
    seq->setDirection(FORWARD);

    step_record = true;

} 

void stepEntryRecordStop() {

    step_record = false;

    int16_t step = seq->getCurrentStep();
    uint16_t first = seq->getFirstStep();
    uint16_t last = seq->getLastStep();

    setStepLength(first, last, step);

    last_live_input_state = false;

    seq->setDirection((uint8_t)last_direction);

}

step_data_t stepEntryCopy(uint8_t index) {

    copy_occurred = true;

    pattern_data_t* p = seq->getPatternDataPtr();

    return p->step[index];

}

void stepEntryArmRotate(bool arm) {

    rotate_armed = arm;

}

void stepEntryRotate(bool dir) {

    step_data_t steps[NUMBER_OF_SEQUENCER_STEPS];
    bool active[NUMBER_OF_SEQUENCER_STEPS];
    bool written[NUMBER_OF_SEQUENCER_STEPS];
    uint num = seq->getPatternLength();
    uint8_t first = seq->getFirstStep();
    uint8_t last = seq->getLastStep();

    uint to, from;

    for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

        steps[i] = seq->getStepByIndex(i);
        active[i] = seq->isStepActive(i);
        written[i] = seq->isStepWritten(i);

    }

    if(dir) {

        for(int i = 0; i < num; i++) {

            uint pos = first + i;

            if(pos == last) {

                to = first;
                from = last;

            } else {

                to = pos + 1;
                from = pos;

            }

            copyStepByIndex(to, steps[from], active[from], written[from]);

        }

    } else {

        for(int i = num - 1; i >= 0; i--) {

            uint pos = last - i;
 
            if(pos == first) {

                to = last;
                from = first;

            } else {

                to = pos - 1;
                from = pos;

            }

            copyStepByIndex(to, steps[from], active[from], written[from]);

        }

    }

}

void stepEntryClearAllStepsPLockAndProbability() {

    for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

        seq->setStepPLockParam(i, 0); // 0 = MOD_DST_NONE
        seq->setStepProbability(i, 0);

    }

}

void stepEntryMIDIInput(uint8_t note, uint8_t velocity, bool on) {

    if(!stepEntrySetLiveRecordStep(note, velocity, on)) {

    }

}

bool stepEntrySetLiveRecordStep(uint8_t note, uint8_t velocity, bool on) {

    if(!step_record) return false;

    int16_t step = seq->getCurrentStep();
    uint16_t tick = seq->getTickCounter();
    uint16_t div = seq->getClockDiv();
    uint16_t first = seq->getFirstStep();
    uint16_t last = seq->getLastStep();

    setStepLength(first, last, step);

    if(on) {

        last_live_input_state = true;

        input_step = step;

        if(tick > step_tick_boundary[div]) {

            input_step += 1;

            if(input_step > last) input_step = first;

        }

        seq->setStepNote(input_step, note);
        seq->setStepVelocity(input_step, velocity);
        seq->setActiveStep(input_step);
        seq->setWrittenStep(input_step);

    } else {

        last_live_input_state = false;

    }

    return true;

}

bool stepEntryGetStepRecord() { return step_record; }

bool stepEntryIsRotateArmed() { return rotate_armed; }

bool stepEntryDidCopyOccur() {

    if(copy_occurred) {

        copy_occurred = false;

        return true;

    }

    return false;
}

// private functions

void setStepLength(uint16_t first, uint16_t last, int16_t step) {

    // if a new step is input while another is live, or if this new note is off, 
    // we set the length of the previous step
    if(!last_live_input_state) return;

    int16_t pattern_length = seq->getPatternLength();

    int16_t step_length = step - input_step;
    if(step_length < 0) step_length += pattern_length;

    seq->setStepLength(input_step, step_length);

}

void copyStepByIndex(uint to, step_data_t data, bool active, bool written) {

    seq->setStepData(to, data);
    if(written) {
        seq->setWrittenStep(to);
        active ? seq->setActiveStep(to) : seq->resetActiveStep(to);
    } else {
        seq->resetWrittenStep(to);
        seq->resetActiveStep(to);
    }
}