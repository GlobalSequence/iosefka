#include "sequencer.h"
#include "../common/test.h"

static const unsigned long ksecond_in_us_ = 60000000;
uint8_t Sequencer::swing_ticks_by_division[NUMBER_OF_CLOCK_DIVS][NUMBER_OF_SWING_AMOUNT][2] = {
    {{48, 48}, {56, 40}, {64, 32}, {72, 24}},
    {{24, 24}, {28, 20}, {32, 16}, {36, 12}},
    {{12, 12}, {14, 10}, {16, 8}, {18, 6}},
    {{9, 9}, {11, 7}, {13, 5}, {15, 3}},
    {{6, 6}, {7, 5}, {8, 4}, {9, 3}},
    {{3, 3}, {3, 3}, {4, 2}, {5, 1}}
};

Sequencer::Sequencer() {

    this->tickCalculator();

}

void Sequencer::tickCounter() {
    if(!run_) {
        return;
    }
    if(tick_counter_ < (ticks_per_sixteenth_note_[is_swing_step_] - 1)) {
        tick_counter_++;
    } else {
        tick_counter_ = 0;
        this->trigger();
    }
}

void Sequencer::trigger() {
    // trigger the step action
    // then increment the step
    // TODO: ADD ROTATION TO THIS FUNCTION

    if(pending_current_step_change_) {
        pending_current_step_change_ = false;
        current_step_ = pending_current_step_;
    }
    previous_step_ = current_step_;
    uint8_t first_ = pattern_data_.first_step;
    uint8_t last_ = pattern_data_.last_step;
    uint8_t pattern_direction = pattern_data_.direction;
    bool will_be_swing_step = (current_step_ & 1) == 0;
    switch(pattern_direction) {
        case BACKWARD:
            current_step_--;
            if(current_step_ < first_) {
                current_step_ = last_;
                loop_count_++;
            }
            break;
        case PING_PONG:
            if(dir_fwd_) {
                current_step_++;
                if(current_step_ > last_) {
                    dir_fwd_ = false;
                    current_step_ = last_ - 1;
                }
            } else {
                current_step_--;
                if(current_step_ < first_) {
                    dir_fwd_ = true;
                    current_step_ = first_ + 1;
                    loop_count_++;
                }
            }
            break;
        case PING_PONG_2:
            if(dir_fwd_) {
                current_step_++;
                if(current_step_ > last_) {
                    dir_fwd_ = false;
                    current_step_ = last_;
                }
            } else {
                current_step_--;
                will_be_swing_step = !will_be_swing_step;
                if(current_step_ < first_) {
                    dir_fwd_ = true;
                    current_step_ = first_;
                    loop_count_++;
                }
            }
            break;
        default: // FORWARD or anything else
            current_step_++;
            if(current_step_ > last_) {
                current_step_ = first_;
                loop_count_++;
            }
            break;
    }
    
    is_swing_step_ = will_be_swing_step;
    step_advanced_ = true;
}

bool Sequencer::update() {
    
    if(this->stepAdvanced()) {

        for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

            bool step_on = false;

            if(step_length_countdown_[i]) {

                step_on = true;
                step_length_countdown_[i]--;

            }

            if(!step_length_countdown_[i] && step_on) {

                note_off_event_[i] = true;

            }

        }

        if(this->isStepActive(current_step_)) {

            note_on_event_[current_step_] = true;

            most_recent_step_note_ = this->getStepNote(current_step_);

            step_length_countdown_[current_step_] = this->getStepLength(current_step_) + 1;

        }

        return true;

    }

    return false;
}


void Sequencer::run() {
    run_ = true;
    just_started_ = true;
    loop_count_ = 0;
}

void Sequencer::stop() {
    run_ = false;
    if(pattern_data_.direction != BACKWARD) {
        current_step_ = pattern_data_.first_step - 1;
        dir_fwd_ = true;
    } else {
        current_step_ = pattern_data_.last_step + 1;
        dir_fwd_ = false;
    }
    tick_counter_ = ticks_per_sixteenth_note_[0]; 
    is_swing_step_ = false; 
    just_stopped_ = true;

    this->resetAllStepNotes();

}

void Sequencer::setPatternData(pattern_data_t pattern_data) {
    pattern_data_ = pattern_data;
    this->setBpm(pattern_data_.bpm);
    tickCalculator();
    this->stop();
}

void Sequencer::setStepData(uint8_t index, step_data_t step_data) {
    pattern_data_.step[index] = step_data;
    resetStepNote(index);
}

void Sequencer::setBpm(uint16_t new_bpm) {
    pattern_data_.bpm = new_bpm;
    tick_period_ = ksecond_in_us_ / (kppq_ * new_bpm);
    bpm_has_changed_ = true;
}

void Sequencer::setSwingAmt(uint8_t new_swing) {
    pattern_data_.swing_amt = new_swing;
    tickCalculator();
}

void Sequencer::setClockDiv(uint8_t new_div) {
    pattern_data_.clock_div = new_div;
    tickCalculator();
}

void Sequencer::setDirection(uint8_t new_dir) {
    pattern_data_.direction = new_dir;
}

void Sequencer::setTranspose(uint8_t new_trans) {
    pattern_data_.transpose = new_trans;
}

void Sequencer::setFirstStep(uint8_t new_first_step) {

    int8_t delta = new_first_step - pattern_data_.first_step;

    pattern_data_.first_step = new_first_step;

    for(int i = 0; i < new_first_step; i++) {

        this->resetStepNote(i);

    }

    if(dir_fwd_) changeCurrentStep(delta);

}

void Sequencer::setLastStep(uint8_t new_last_step) {

    int8_t delta = new_last_step - pattern_data_.last_step;

    pattern_data_.last_step = new_last_step;

    uint8_t last_step_index = new_last_step + 1;

    uint8_t reset_count = NUMBER_OF_SEQUENCER_STEPS - last_step_index;

    for(int i = 0; i < reset_count; i++) {

        uint8_t reset_index = i + last_step_index;

        this->resetStepNote(reset_index);

    }

    if(!dir_fwd_) changeCurrentStep(delta);

}

void Sequencer::changeCurrentStep(int16_t delta) {
    pending_current_step_change_ = true;
    pending_current_step_ = current_step_ + delta;
}

void Sequencer::setActiveStep(uint8_t index) {
    pattern_data_.step_active[index] = true;
}

void Sequencer::resetActiveStep(uint8_t index) {
    pattern_data_.step_active[index] = false;
    // unsure why but this fixes a bug where in either ping pong mode the first and last step persist once after being set to inactive
    note_on_event_[index] = false;
}

void Sequencer::setProbability(uint8_t new_probability) {
    pattern_data_.probability = new_probability;
}

void Sequencer::setRotation(uint8_t new_rotation) {
    pattern_data_.rotation = new_rotation;
}

void Sequencer::setWrittenStep(uint8_t index) {
    pattern_data_.step_written[index] = true;
}

void Sequencer::resetWrittenStep(uint8_t index) {
    pattern_data_.step_written[index] = false;
}
 
void Sequencer::setStepNote(uint8_t index, uint8_t value) {
    pattern_data_.step[index].note = value;
}

void Sequencer::setStepLength(uint8_t index, uint8_t value) {
    pattern_data_.step[index].length = value;
}

void Sequencer::setStepVelocity(uint8_t index, uint8_t value) {
    pattern_data_.step[index].velocity = value;   
}

void Sequencer::setStepPLockParam(uint8_t index, uint8_t value) {
    pattern_data_.step[index].p_lock_param = value;
}

void Sequencer::setStepPLockValue(uint8_t index, uint8_t value) {
    pattern_data_.step[index].p_lock_value = value;   
}

void Sequencer::setStepProbability(uint8_t index, uint8_t value) {
    pattern_data_.step[index].step_prob = value;
}

void Sequencer::shiftByPatternLength(bool advance) {
    uint8_t len = this->getPatternLength();
    if(advance) {
        int new_last = pattern_data_.last_step + len + 1;
        if(!(new_last >= NUMBER_OF_SEQUENCER_STEPS)) {
            this->setFirstStep(new_last - len);
            this->setLastStep(new_last);
        }
    } else {
        int new_first = pattern_data_.first_step - len - 1;
        if(!(new_first < 0)) {
            this->setFirstStep(new_first);
            this->setLastStep(new_first + len);
        }
    }

}

void Sequencer::resetLoopCount() {
    loop_count_ = 0;
}

uint8_t Sequencer::getStepNote(uint8_t index) {
    uint8_t step_note = pattern_data_.step[index].note + pattern_data_.transpose + ktranspose_offset;
    if(step_note <= 0) return 0;
    else if(step_note > 127) return 127; 
    return step_note;
}

uint8_t Sequencer::getStepLength(uint8_t index) {
    return pattern_data_.step[index].length;
}

uint8_t Sequencer::getStepVelocity(uint8_t index) {
    return pattern_data_.step[index].velocity;
}

uint8_t Sequencer::getStepPLockParam(uint8_t index) {
    return pattern_data_.step[index].p_lock_param;
}

uint8_t Sequencer::getStepPLockValue(uint8_t index) {
    return pattern_data_.step[index].p_lock_value;
} 

void Sequencer::tickCalculator() {
    uint8_t div = pattern_data_.clock_div;
    uint8_t swing = pattern_data_.swing_amt;
    ticks_per_sixteenth_note_[0] = swing_ticks_by_division[div][swing][0];
    ticks_per_sixteenth_note_[1] = swing_ticks_by_division[div][swing][1];
}



void Sequencer::resetStepNote(uint8_t index) {

    if(step_length_countdown_[index]) {

        step_length_countdown_[index] = 0;
        note_off_event_[index] = true;

    }

}

void Sequencer::resetAllStepNotes() {

    for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

        resetStepNote(i);

    }

}