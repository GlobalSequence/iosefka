#include "seq_poll.h"
#include "pattern_generator.h"
// #include "../controller/note_queue.h"
// #include "../controller/settings.h"
#include "../common/test.h"
#include "sequencer.h"

// static synth_state_t *synth;
static Sequencer *seq;
static bool *ext_clk;
static seqPollNoteFunc noteFunc;
// static seqPollKillNoteFunc killNoteFunc;
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

    // killNoteFunc = killNoteFunction;

    pLockFunc = pLockFunction;

    clockTickPeriodFunc = clockTickPeriodFunction;

    clockTickEventFunc = clockTickEventFunction;

    startEventFunc = startEventFunction;

    // TEST_STEP_DATA_PATTERN_GEN_ON

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

        // static uint32_t count;
        // TEST_PRINT_ONE_NL(count++)

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

        // TEST_PRINT_TEXT(".\n")

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

// ################################
// PRIVATE FUNCTION DEFINITIONS
// ################################


void noteOnEvent() {

    uint8_t step = seq->getCurrentStep();

    if(seq->noteOnEvent(step)) {

        uint16_t rand_var = patternGenGetRand();

        bool allow_step = patternGenIsActive()? true : seq->stepProbabilityCheck(rand_var, step);

        if(allow_step) {

            // TEST_PRINT_TEXT("N\t")

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



