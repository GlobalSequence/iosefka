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

#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/utilities.h"

#define NUMBER_OF_SEQUENCER_STEPS 32
#define NUMBER_OF_SEQUENCER_STEP_PAGES (NUMBER_OF_SEQUENCER_STEPS / 4)
static const uint8_t kmax_steps_ = 32;
static const uint8_t kppq_ = 24;
static const uint8_t ksixteenth_pulse_range_ = kppq_ / 4; 
static const int8_t ktranspose_offset = -12;

typedef enum sequencer_direction_t {
    FORWARD,
    BACKWARD,
    PING_PONG,
    PING_PONG_2,

    NUMBER_OF_SEQUENCER_DIRECTIONS
} sequencer_direction_t;

typedef enum sequencer_clock_div_t {
    CLK_DIV_ONE_EIGHTH,
    CLK_DIV_ONE_FOURTH,
    CLK_DIV_HALF,
    CLK_DIV_THREE_FOURTHS,
    CLK_DIV_ONE,
    CLK_DIV_TWO_TIMES,

    NUMBER_OF_CLOCK_DIVS
}sequencer_clock_div_t;

typedef enum swing_amount_t {
    SWING_50,
    SWING_58,
    SWING_67,
    SWING_75,

    NUMBER_OF_SWING_AMOUNT
} swing_amount_t;

typedef struct step_data_t {
    uint8_t note = 60;
    uint8_t velocity = 100;
    uint8_t length = 0; // steps will always have a length of 1, so 0 == 1
    uint8_t p_lock_param = 0;
    uint8_t p_lock_value = 0;
    uint8_t step_prob = 0;
} step_data_t;

// TODO: Initialize this (all 0) and create a default struct that loads on startup
typedef struct pattern_data_t {
    uint16_t bpm = 120;
    uint8_t first_step = 0;
    uint8_t last_step = 7;
    uint8_t direction = FORWARD;
    uint8_t swing_amt = SWING_50;
    uint8_t clock_div = CLK_DIV_ONE;
    uint8_t transpose = 12;
    uint8_t probability = 255;
    uint8_t rotation = 0;
    step_data_t step[NUMBER_OF_SEQUENCER_STEPS];
    bool step_written[NUMBER_OF_SEQUENCER_STEPS] = { false };
    bool step_active[NUMBER_OF_SEQUENCER_STEPS] = { false };

} pattern_data_t;

class Sequencer {
    public:
        Sequencer();
        bool update();

        inline bool noteOnEvent(uint8_t index) {
            if(note_on_event_[index]) {note_on_event_[index] = false; return true;}
            return false;
        }

        inline bool noteOffEvent(uint8_t index) {
            if(note_off_event_[index]) {note_off_event_[index] = false; return true;}
            return false;
        }

        void tickCounter();

        void run();
        void stop();

        void setBpm(uint16_t new_bpm);
        inline uint32_t getTickPeriod() {
            return tick_period_;
        }

        inline int16_t getCurrentStep() {return current_step_;}
        inline int16_t getPreviousStep() {return previous_step_;}

        void setPatternData(pattern_data_t pattern_data);
        void setSwingAmt(uint8_t new_swing);
        void setClockDiv(uint8_t new_div);
        void setDirection(uint8_t new_dir);
        void setTranspose(uint8_t new_trans);
        void setFirstStep(uint8_t new_first_step);
        void setLastStep(uint8_t new_last_step);
        void setProbability(uint8_t new_probability);
        void setRotation(uint8_t new_rotation);
        void shiftByPatternLength(bool advance);

        void setStepData(uint8_t index, step_data_t step_data);
        void setWrittenStep(uint8_t index);
        void resetWrittenStep(uint8_t index);
        void setActiveStep(uint8_t index);
        void resetActiveStep(uint8_t index);
        void clearAllSteps();

        void setStepNote(uint8_t index, uint8_t value);
        void setStepLength(uint8_t index, uint8_t value);
        void setStepVelocity(uint8_t index, uint8_t value);
        void setStepPLockParam(uint8_t index, uint8_t value);
        void setStepPLockValue(uint8_t index, uint8_t value); 
        void setStepProbability(uint8_t index, uint8_t value);
        void resetLoopCount();
        void resetStepNote(uint8_t index);
        void resetAllStepNotes();

        inline bool isRunning() {return run_;}
        inline bool isStepActive(uint8_t index) {return pattern_data_.step_active[index];}
        inline bool isStepWritten(uint8_t index) {return pattern_data_.step_written[index]; }
        uint8_t getStepMostRecentNote() { return most_recent_step_note_; }
        uint8_t getStepNote(uint8_t index);
        uint8_t getStepLength(uint8_t index);
        uint8_t getStepVelocity(uint8_t index);
        uint8_t getStepPLockParam(uint8_t index);
        uint8_t getStepPLockValue(uint8_t index); 
        step_data_t getStepByIndex(uint index) { return pattern_data_.step[index]; }
        
        inline uint16_t getBpm() {return pattern_data_.bpm;}
        inline uint16_t getClockDiv() {return pattern_data_.clock_div;}
        inline uint16_t getDirection() {return pattern_data_.direction;}
        inline uint16_t getTranspose() {return pattern_data_.transpose;}
        inline uint16_t getFirstStep() {return pattern_data_.first_step;}
        inline uint16_t getLastStep() {return pattern_data_.last_step;}
        inline uint16_t getPatternLength() {return pattern_data_.last_step - pattern_data_.first_step + 1;}
        inline uint16_t getSwingAmt() {return pattern_data_.swing_amt;}
        inline uint8_t getProbability() {return pattern_data_.probability;}
        inline uint8_t getStepProbability(uint8_t index) {return pattern_data_.step[index].step_prob;}
        inline uint8_t getRotation() {return pattern_data_.rotation;}
        inline int getLoopCount() {return loop_count_;}

        uint8_t getTickCounter() { return tick_counter_; }
        uint8_t getSwingTicksByDivision(uint8_t div, uint8_t swing_amt, uint8_t swing_step) { return swing_ticks_by_division[div][swing_amt][swing_step]; }


        inline bool probabilityCheck(uint16_t var) {
            return (var >> 8) <= pattern_data_.probability;
        }

        inline bool stepProbabilityCheck(uint16_t var, uint8_t index) {
            uint8_t step_prob = this->pattern_data_.step[index].step_prob;
            if(step_prob) return (var >> 8) <= step_prob;
            else return probabilityCheck(var);
            return false;
        }

        inline pattern_data_t* getPatternDataPtr() { return &pattern_data_; }

        inline bool justStarted() {
            if (just_started_) {
                just_started_ = false; 
                return true;
            }
            return false;
        }

        inline bool stepAdvanced() {
            if (step_advanced_) {
                step_advanced_ = false; 
                return true;
            }
            return false; 
        }

        inline bool justStopped() {
            if (just_stopped_) {
                just_stopped_ = false; 
                return true;}
            return false;
        }

        inline bool bpmChanged() {
            if(bpm_has_changed_) { 
                bpm_has_changed_ = false; 
                return true; 
            }
            return false;
        }
        // REMEMBER WHEN YOU MAKE A RUN FUNCTION THAT THE SWING BEAT WILL BE FALSE
    
    private: 
        void trigger();
        void tickCalculator();
        uint32_t tick_period_;
        uint64_t last_tick_time_;
        uint8_t tick_counter_ = 0;
        uint8_t steps_in_sequence_ = 8;
        uint32_t steps_ = 0;
        bool bpm_has_changed_;

        bool run_ = false;
        int16_t current_step_ = -1;
        int16_t previous_step_ = 0;
        bool dir_fwd_ = true;
        bool pp2_hang_ = false;
        bool just_started_;
        bool step_advanced_;
        bool just_stopped_;
        bool note_on_event_[NUMBER_OF_SEQUENCER_STEPS] = { false };
        bool note_off_event_[NUMBER_OF_SEQUENCER_STEPS] = { false };
        bool note_playing[NUMBER_OF_SEQUENCER_STEPS] = { false };
        uint8_t step_length_countdown_[NUMBER_OF_SEQUENCER_STEPS] = {0};

        // variables tied to changing first and last step
        int16_t pending_current_step_;
        bool pending_current_step_change_;
        void changeCurrentStep(int16_t delta);

        uint8_t most_recent_step_note_;

        uint8_t ticks_per_sixteenth_note_[2];
        uint8_t is_swing_step_ = 1;
        bool advance_step_ = false;
        int loop_count_ = 0;

        pattern_data_t pattern_data_;
        static uint8_t swing_ticks_by_division[NUMBER_OF_CLOCK_DIVS][NUMBER_OF_SWING_AMOUNT][2];
        

};

#endif