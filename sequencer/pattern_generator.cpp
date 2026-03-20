#include "pattern_generator.h"
#include "sequencer.h"
#include "../common/test.h"

static const uint8_t scale_major[NUMBER_OF_NOTES_PER_SCALE] = {0, 2, 4, 5, 7, 9, 11, 0};
static const uint8_t scale_minor[NUMBER_OF_NOTES_PER_SCALE] = {0, 2, 3, 5, 7, 8, 10, 0};
static const uint8_t k_num_weighted_len_values = 5;
static const uint8_t weighted_length[k_num_weighted_len_values] = {0, 1, 3, 7, 15};

static const uint8_t weighted_chance[NUMBER_OF_WEIGHTED_CHANCE_VALS][NUMBER_OF_WEIGHTED_CHANCE_POINTS] {

/*     {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 1, 1, 1, 2},
    {0, 0, 0, 1, 1, 1, 2, 2},
    {0, 0, 1, 1, 2, 2, 3, 3},
    {1, 2, 2, 2, 3, 3, 3, 3},
    {2, 2, 3, 3, 3, 3, 3, 3},
    {3, 3, 3, 3, 3, 3, 3, 3}, */

/*     {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1},
    {0, 0, 0, 1, 1, 1, 3, 3},
    {0, 0, 1, 1, 3, 3, 7, 7},
    {0, 1, 1, 3, 3, 7, 7, 15},
    {0, 1, 3, 3, 7, 7, 15, 15},
    {0, 1, 3, 7, 7, 15, 15, 15},
    {0, 1, 3, 7, 15, 15, 15, 15} */

    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1},
    {0, 0, 0, 1, 1, 1, 2, 2},
    {0, 0, 1, 1, 2, 2, 3, 3},
    {0, 1, 1, 2, 2, 3, 3, 4},
    {0, 1, 2, 2, 3, 3, 4, 4},
    {0, 1, 2, 3, 3, 4, 4, 4},
    {0, 1, 2, 3, 4, 4, 4, 4}

};

static const uint8_t loop_repeat_limit[NUMBER_OF_REPEAT_VALS] = {0, 2, 4, 8, 16, 32, 64, 128};

static const uint8_t *scale_ptr[NUM_SCALES];

Sequencer *seq;
pattern_gen_t *param;

typedef enum step_length_limiter_t {
    STEP_LIMIT_FOUR = 4,
    STEP_LIMIT_EIGHT = 8,
    STEP_LIMIT_SIXTEEN = 16,
    STEP_LIMIT_THIRTY_TWO = 32,

    NUM_STEP_LIMITERS
} step_length_limiter_t;

uint16_t seed;
int last_repeat_count;
bool pattern_generated = false;

// ################################
// PRIVATE FUNCTIONS
// ################################

uint8_t generateNote();
uint8_t generateVelocity();
uint8_t generateStepLength(uint8_t pattern_length);
bool generateActiveStep(uint8_t chance);
uint16_t getRand();
uint8_t getWeightedChanceValue(uint8_t parameter);

void patternGenSetup(Sequencer *sequencer, pattern_gen_t *parameter, uint16_t rand_seed) {

    seq = sequencer;
    param = parameter;
    seed = rand_seed;

    scale_ptr[SCALE_MAJOR] = scale_major;
    scale_ptr[SCALE_MINOR] = scale_minor;

    patternGenSetCountdown();

}

void patternGenExecute() {

    uint8_t len = seq->getPatternLength() + 1;
    uint8_t first = seq->getFirstStep();
    uint8_t chance = seq->getProbability();

    for(int i = 0; i < len; i++) {

        uint8_t step = first + i;

        step_data_t step_data;

        // generate note
        uint8_t note = generateNote();

        // generate velocity
        uint8_t velocity = generateVelocity();

        // generate density
        uint8_t step_length = generateStepLength(len);

        // determine if step is active
        bool active = generateActiveStep(chance);

        step_data.note = note;
        step_data.length = step_length;
        step_data.velocity = velocity;
        step_data.step_prob = 0;

        seq->setStepData(step, step_data);
        seq->setWrittenStep(step);
        if(active) seq->setActiveStep(step);
        else seq->resetActiveStep(step);

    }
    
    pattern_generated = true;

}

bool patternGenCheckLoopCount() {

    if(!param->on) return false;

    int repeat_count = seq->getLoopCount();

    if(repeat_count != last_repeat_count) {

        last_repeat_count = repeat_count;

        param->countdown--;

        uint8_t repeat_limit = loop_repeat_limit[param->repeat];

        if(repeat_count % repeat_limit == 0) {
            
            patternGenExecute();

            int16_t new_countdown = repeat_limit - 1;
            if(new_countdown < 0) param->countdown = 0;
            else param->countdown = new_countdown;

            return true;
        }

    }

    return false;

}

void patternGenSetCountdown() {

    int repeat_count = seq->getLoopCount();
    uint8_t repeat_limit = loop_repeat_limit[param->repeat];
    uint8_t countup = repeat_count % repeat_limit;
    int16_t set = (repeat_limit - countup) - 1;
    if(set < 0) set = 0;
    param->countdown = set;

}

bool patternGenOccurred() {
    if(pattern_generated) {
        pattern_generated = false;
        return true;
    }
    return false;
}

bool patternGenIsActive() {

    return param->on;

}

uint16_t patternGenGetRand() {

    return getRand();
    
}

uint8_t generateStepLength(uint8_t pattern_length) {

/*     
    // TODO: Evaluate this. in retrospect, no real need to limit the step length.

    uint8_t step_len_limiter;

    if(pattern_length < 4) {
        step_len_limiter = 2;
    } else if (pattern_length < 8) {
        step_len_limiter = 1;
    }

    uint8_t rand_val_raw = getWeightedChanceValue(param->density);

    int8_t rand_limited_val = rand_val_raw - step_len_limiter;
    
    if(rand_limited_val <= 0) rand_limited_val = 0; 
    
    return rand_limited_val;
    */

    uint8_t rand_val_raw = getWeightedChanceValue(param->density);

    return weighted_length[rand_val_raw];

}

uint8_t generateNote() {

    uint8_t rand_note = getRand() >> 13;
    uint8_t note = param->base_note + scale_ptr[param->scale][rand_note];
    uint8_t octave = getWeightedChanceValue(param->octave_range) * 12;
    uint8_t output = note + octave;
    if(output >= 127) output = 127;

    return output;

}

uint8_t generateVelocity() {

    // velocity will always be at least half max velocity
    uint8_t rand = getRand() >> 9;
    uint8_t velocity = rand +  0x7F;

    return velocity;

}

bool generateActiveStep(uint8_t chance) {

    return chance >= (getRand() >> 8);

}

uint16_t getRand() {
    XOR798(&seed);
    return seed;
}

uint8_t getWeightedChanceValue(uint8_t parameter) {

    uint8_t roll = param8Bits0To7(getRand() >> 8);
    return weighted_chance[parameter][roll];

}