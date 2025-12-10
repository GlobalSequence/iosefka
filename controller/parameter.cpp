#include "parameter.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "view.h"
#include "settings.h"
#include "../synth/patch.h"
#include "../sequencer/sequencer.h"
#include "../sequencer/p_lock.h"
#include "../sequencer/pattern_generator.h"
#include "../common/globals.h"
#include "../common/utilities.h"
#include "../common/test.h"

// #define ALLOW_P_LOCK

static synth_state_t *synth;
static synth_parameters_t render_patch;

static uint8_t *param_ptr[NUM_MOD_DESTINATIONS];
static uint8_t *render_patch_ptr[NUM_MOD_DESTINATIONS];
static uint8_t *user_param[NUMBER_OF_POTENTIOMETERS];

static const uint8_t ktranspose_snap_low = 85; 
static const uint8_t ktranspose_snap_high = 170; 

static const uint8_t k_osc_scale_vals[NOTE_SCALE_RANGE] = {

    0x00, 0x0B, 0x16, 0x20, 0x2B, 0x36, 0x40, 0x4B,
    0x56, 0x60, 0x6B, 0x75, 0x80, 0x8B, 0x95, 0xA0,
    0xAB, 0xB5, 0xC0, 0xCB, 0xD5, 0xE0, 0xEB, 0xF5, 0xFF

};

static uint8_t current_p_lock_note[NUMBER_OF_BOTTOM_SWITCHES], current_p_lock_octave[NUMBER_OF_BOTTOM_SWITCHES];
static bool p_lock_param_override[NUMBER_OF_SEQUENCER_STEPS];
static mod_destinations_t p_lock_active_dst[NUMBER_OF_SEQUENCER_STEPS];
static uint8_t p_lock_active_value[NUMBER_OF_SEQUENCER_STEPS];

static const uint8_t k_latch_threshold = 8;

// ################################
// PRIVATE FUNCTIONS
// ################################

// void assignActivePLockValue(bool )

void setParamValSeqPLock(seq_params_t seq_param, uint8_t index, uint8_t value) {

    static uint8_t step_p_lock_note[NUMBER_OF_BOTTOM_SWITCHES];
    static uint8_t step_p_lock_octave[NUMBER_OF_BOTTOM_SWITCHES];

    uint8_t step = index + viewGetSequencerWindowStepBase();
    int v_1;
    int v_2;
    int v_3;
    int v_4;

    switch(seq_param) {

        case SEQ_PARAM_STEP_NOTE:

            v_1 = param8Bits0To24(value) - P_LOCK_NOTE_ADJUST_OFFSET;

            if(v_1 != step_p_lock_note[index]) {

                step_p_lock_note[index] = v_1;
                
                // v_2 = viewGetSeqNoteSnapshot(index);
                v_2 = pLockGetNote(index);
                
                v_3 = v_1 + v_2; 
                
                if(v_3 <= 0) v_3 = 0;

                synth->seq.setStepNote(step, v_3);

                // current_p_lock_note[index] = v_3;
                // viewSetPLockNote(index, v_3);
                // TEST_PRINT_TEXT("P NOTE: ")
                // TEST_PRINT_ONE_TAB(index)
                // TEST_PRINT_ONE_NL(v_3);

            }
            break;

        case SEQ_PARAM_STEP_OCTAVE:

            v_1 = param8Bits0To9(value);

            // purely to save us some compute from doing a ton of unnecessary calculations
            if(v_1 != step_p_lock_octave[index]) {
                
                step_p_lock_octave[index] = v_1;
                
                v_2 = synth->seq.getStepNote(step);
                
                v_3 = v_2;
                
                while(v_3 >= 12) { v_3 -= 12; } v_3 += (v_1 * 12);
                
                // viewSetSeqNoteSnapshot(index, v_3);
                pLockSetNote(index, v_3);
                
                synth->seq.setStepNote(step, v_3);

                // current_p_lock_octave[index] = v_3;
                // viewSetPLockOctave(index, v_3);
                
                // TEST_PRINT_TEXT("P OCT: ")
                // TEST_PRINT_ONE_TAB(index)
                // TEST_PRINT_ONE_NL(v_3);
            }

            break;

        case SEQ_PARAM_VELOCITY:

            synth->seq.setStepVelocity(step, value);

            // TEST_PRINT_TEXT("P VEL: ")
            // TEST_PRINT_ONE_TAB(index)
            // TEST_PRINT_ONE_NL(synth->seq.getStepVelocity(step));
            break;
        
        case SEQ_PARAM_STEP_LENGTH:

            synth->seq.setStepLength(step, param8bits0To31(value));

            // TEST_PRINT_TEXT("P LEN: ")
            // TEST_PRINT_ONE_TAB(index)
            // TEST_PRINT_ONE_NL(synth->seq.getStepVelocity(step));
            break;

        case SEQ_PARAM_PROBABILITY:

            synth->seq.setStepProbability(step, value);
        
        default:
            break;

    }

}

void setParamValSeq(seq_params_t seq_param, uint8_t value, bool shift) {

    // // these go into the p lock function
    // static uint8_t step_p_lock_note[NUMBER_OF_BOTTOM_SWITCHES];
    // static uint8_t step_p_lock_octave[NUMBER_OF_BOTTOM_SWITCHES];

    uint8_t note;
    uint8_t octave;
    uint8_t v_1;
    uint8_t v_2;
    uint8_t v_3;
    uint8_t v_4;

    switch(seq_param) {
        case SEQ_PARAM_STEP_NOTE:

            note = param8Bits0To11(value);
            viewSetSeqParamStepNote(note);
            octave = viewGetSeqParamStepOctave();
            value = note + (octave * NUMBER_OF_NOTES_PER_OCTAVE);

            break;

        case SEQ_PARAM_STEP_OCTAVE:

            octave = param8Bits0To9(value);
            viewSetSeqParamStepOctave(octave);
            note = viewGetSeqParamStepNote();
            value = note + (octave * NUMBER_OF_NOTES_PER_OCTAVE);

            break;

        case SEQ_PARAM_STEP_LENGTH:
        case SEQ_PARAM_FIRST_STEP:
        case SEQ_PARAM_LAST_STEP:
            value = param8bits0To31(value);
            break;

        case SEQ_PARAM_TRANSPOSE:
            if(!shift) {

                value = param8Bits0To24(value);
            
            } else {

                if(value < ktranspose_snap_low) value = 0;
                else if(value >= ktranspose_snap_low && value < ktranspose_snap_high) value = 12;
                else if (value >= ktranspose_snap_high) value = 24;

            }

            break;

        case SEQ_PARAM_DIRECTION:
            value = param8Bits0To3(value);
            break;

        case SEQ_PARAM_SWING:
            value = param8Bits0To3(value);
            break;

        case SEQ_PARAM_CLOCK_DIV:
            value = param8Bits0To5(value);
            break;

        case SEQ_PARAM_SCALE:
            // NOTE: IF ADDING MORE SCALES, CHANGE THIS TO REFLECT NUM OF SCALES
            value = param8Bits0To1(value);
            break;

        case SEQ_PARAM_DENSITY:
        case SEQ_PARAM_REPEAT:
            value = param8Bits0To7(value);
            break;

        case SEQ_PARAM_OCTAVE_RANGE:
            if(synth->pattern_gen.on) value = param8Bits0To7(value);
            else value = param8Bits0To11(value);
            break;

        default:
            break;
    }

    switch (seq_param) {
        case SEQ_PARAM_STEP_NOTE:
        case SEQ_PARAM_STEP_OCTAVE:
            synth->new_step.note = value;
            synth->pattern_gen.base_note = value;
            // calculateSeqOctaveLimitNote();
            break;
    
        case SEQ_PARAM_STEP_LENGTH:
            synth->new_step.length = value; 
            break;
    
        case SEQ_PARAM_VELOCITY:
            synth->new_step.velocity = value;
            break;
    
        case SEQ_PARAM_TRANSPOSE:
            synth->seq.setTranspose(value);
            break;
    
        case SEQ_PARAM_DIRECTION:
            synth->seq.setDirection(value);
            break;
    
        case SEQ_PARAM_SWING:
            synth->seq.setSwingAmt(value);
            break;
    
        case SEQ_PARAM_CLOCK_DIV:
            synth->seq.setClockDiv(value);
            break;
    
        case SEQ_PARAM_FIRST_STEP:
            v_1 = value;
            v_2 = synth->seq.getLastStep();
            if (v_1 >= v_2) v_1 = v_2;
            synth->seq.setFirstStep(v_1);
            break;
    
        case SEQ_PARAM_LAST_STEP:    
            v_1 = value;
            v_2 = synth->seq.getFirstStep();
            if (v_1 <= v_2) v_1 = v_2;
            synth->seq.setLastStep(v_1);
            break;
    
        case SEQ_PARAM_PROBABILITY:
            // if(synth->pattern_gen.on) synth->pattern_gen.chance = value;   
            // else synth->seq.setProbability(value);
            synth->seq.setProbability(value);
            break;
    
        case SEQ_PARAM_SCALE:          
            synth->pattern_gen.scale = value;
            break;
    
        case SEQ_PARAM_DENSITY:       
            synth->pattern_gen.density = value;
            break;
    
        case SEQ_PARAM_REPEAT:        
            synth->pattern_gen.repeat = value;
            patternGenSetCountdown();
            break;
    
        case SEQ_PARAM_OCTAVE_RANGE:   
            // if(synth->pattern_gen.on) synth->pattern_gen.octave_range = value;
            // else {
            //     synth->special.seq_octave_limit = value;
            // }
            synth->pattern_gen.octave_range = value;

            break;
    
        case NUM_SEQ_PARAMS:           
        default:
            break;
    }

    // TEST_PRINT_ONE_NL(viewGetSeqParamStepOctave())

}

uint8_t modifyParamValueModDst(mod_destinations_t *dst, uint8_t value, bool shift) {

    // mod_destinations_t new_dst;
    uint8_t mod_index;
    bool change_dst = false;

    switch(*dst) {
        case MOD_DST_OSC_1_PITCH:
        case MOD_DST_OSC_2_PITCH:
            
            if(!shift) value = k_osc_scale_vals[param8Bits0To24(value)];
            break;

        case MOD_DST_ENV_1_AMOUNT:
        case MOD_DST_ENV_2_AMOUNT:
        case MOD_DST_ENV_3_AMOUNT:
        case MOD_DST_LFO_1_AMOUNT:
        case MOD_DST_LFO_2_AMOUNT:
        case MOD_DST_LFO_3_AMOUNT:
        case MOD_DST_FILTER_ENV_AMT:

            if(shift) value = 0x80;
            break;

        case MOD_DST_ENV_1_ATTACK: mod_index = 0; change_dst = true; break;
        case MOD_DST_ENV_2_ATTACK: mod_index = 1; change_dst = true; break;
        case MOD_DST_ENV_3_ATTACK: mod_index = 2; change_dst = true; break;
        case MOD_DST_LFO_1_RATE: mod_index = 3; change_dst = true; break;
        case MOD_DST_LFO_2_RATE: mod_index = 4; change_dst = true; break;
        case MOD_DST_LFO_3_RATE: mod_index = 5; change_dst = true; break;

        case MOD_DST_ENV_1_DECAY: if(shift) *dst = MOD_DST_ENV_1_LOOP; break;
        case MOD_DST_ENV_2_DECAY: if(shift) *dst = MOD_DST_ENV_2_LOOP; break;
        case MOD_DST_ENV_3_DECAY: if(shift) *dst = MOD_DST_ENV_3_LOOP; break;

        case MOD_DST_OSC_1_X_MORPH:
        case MOD_DST_OSC_1_Y_MORPH:
        case MOD_DST_OSC_2_X_MORPH:
        case MOD_DST_OSC_2_Y_MORPH:

            if(shift) {

                uint8_t wave_index = value >> 4;
                value = wave_index << 4;

            }
            break;

        case MOD_DST_LFO_1_SYNC: if(value > 0x80);

        default:
            break;
    }

    if(change_dst && shift) {
        *dst = (mod_destinations_t) synth->patch.mod[mod_index].destination;
        // TEST_PRINT_TWO_NL(*dst, mod_index)
    }

    return value;

}

seq_params_t layerLookupSeqParam(pot_index_t pot) {

    switch (pot) {
        case POT_0: return SEQ_PARAM_STEP_NOTE;
        case POT_1: return SEQ_PARAM_STEP_OCTAVE;
        case POT_2: return SEQ_PARAM_STEP_LENGTH;
        case POT_3: return SEQ_PARAM_VELOCITY;
        case POT_4: return SEQ_PARAM_TRANSPOSE;
        case POT_5: return SEQ_PARAM_DIRECTION;
        case POT_6: return SEQ_PARAM_SWING;
        case POT_7: return SEQ_PARAM_CLOCK_DIV;
        case POT_8: return SEQ_PARAM_FIRST_STEP;
        case POT_9: return SEQ_PARAM_LAST_STEP;
        case POT_10: return SEQ_PARAM_PROBABILITY;
        case POT_11: return SEQ_PARAM_SCALE;
        case POT_12: return SEQ_PARAM_DENSITY;
        case POT_13: return SEQ_PARAM_REPEAT;
        case POT_14: return SEQ_PARAM_OCTAVE_RANGE;

        default:
            return NUM_SEQ_PARAMS; // Invalid or unmapped pot
    }
    return NUM_SEQ_PARAMS;
}

mod_destinations_t getUserParameterModDestination(pot_index_t pot) {

    mod_destinations_t mod;

    switch (pot) {
        case POT_0: mod = (mod_destinations_t) synth->user_param.destination[0]; break; 
        case POT_1: mod = (mod_destinations_t) synth->user_param.destination[1]; break;
        case POT_2: mod = (mod_destinations_t) synth->user_param.destination[2]; break;
        case POT_3: mod = (mod_destinations_t) synth->user_param.destination[3]; break;
        case POT_4: mod = (mod_destinations_t) synth->user_param.destination[4]; break;

        case POT_5: mod = (mod_destinations_t) synth->user_param.destination[5]; break;
        case POT_6: mod = (mod_destinations_t) synth->user_param.destination[6]; break;
        case POT_7: mod = (mod_destinations_t) synth->user_param.destination[7]; break;
        case POT_8: mod = (mod_destinations_t) synth->user_param.destination[8]; break;
        case POT_9: mod = (mod_destinations_t) synth->user_param.destination[9]; break;

        case POT_10: mod = (mod_destinations_t) synth->user_param.destination[10]; break;
        case POT_11: mod = (mod_destinations_t) synth->user_param.destination[11]; break;
        case POT_12: mod = (mod_destinations_t) synth->user_param.destination[12]; break;
        case POT_13: mod = (mod_destinations_t) synth->user_param.destination[13]; break;
        case POT_14: mod = (mod_destinations_t) synth->user_param.destination[14]; break;

        default:     break;
    }
    
/*     for(int i = 0; i < NUMBER_OF_POTENTIOMETERS; i++) {
        TEST_PRINT_ONE_TAB(synth->user_param.destination[i])
    }
    TEST_PRINT_NL */
    
    switch(mod) {

        case MOD_DST_ENV_1_DST:
        case MOD_DST_ENV_2_DST:
        case MOD_DST_ENV_3_DST:
        case MOD_DST_LFO_1_DST:
        case MOD_DST_LFO_2_DST:
        case MOD_DST_LFO_3_DST:
            mod = MOD_DST_NONE;
            break;
            
        default: break;

    }
    

    return mod;

}

mod_destinations_t layerLookupModDst(view_layer_t layer, pot_index_t pot) {

    switch (layer) {
        case LAYER_SYNTH_1:
            switch (pot) {
                case POT_0: return MOD_DST_OSC_1_PITCH; 
                case POT_1: return MOD_DST_OSC_2_PITCH; 
                case POT_2: return MOD_DST_PORTAMENTO; 
                case POT_3: return MOD_DST_MIX_OSC_1_MIX; 
                case POT_4: return MOD_DST_FILTER_CUTOFF; 

                case POT_5: return MOD_DST_OSC_1_X_MORPH; 
                case POT_6: return MOD_DST_OSC_2_X_MORPH; 
                case POT_7: return MOD_DST_PHASE_MOD; 
                case POT_8: return MOD_DST_MIX_OSC_2_MIX; 
                case POT_9: return MOD_DST_FILTER_RESONANCE; 

                case POT_10: return MOD_DST_OSC_1_Y_MORPH; 
                case POT_11: return MOD_DST_OSC_2_Y_MORPH; 
                case POT_12: return MOD_DST_SYNC; 
                case POT_13: return MOD_DST_MIX_NOISE_MIX; 
                case POT_14: return MOD_DST_FILTER_ENV_AMT; 
                
                default:     break;
            }
            break;

        case LAYER_SYNTH_2:
            switch (pot) {
                case POT_0: return MOD_DST_OSC_1_OCTAVE; 
                case POT_1: return MOD_DST_OSC_2_OCTAVE; 
                case POT_2: return MOD_DST_DELAY_AMOUNT; 
                case POT_3: return MOD_DST_FILTER_TYPE; 
                case POT_4: return MOD_DST_FILTER_CUTOFF; 

                case POT_5: return MOD_DST_OSC_1_X_MORPH; 
                case POT_6: return MOD_DST_OSC_2_X_MORPH; 
                case POT_7: return MOD_DST_DELAY_RATE; 
                case POT_8: return MOD_DST_FILTER_FM;
                case POT_9: return MOD_DST_FILTER_RESONANCE; 

                case POT_10: return MOD_DST_OSC_1_Y_MORPH; 
                case POT_11: return MOD_DST_OSC_2_Y_MORPH; 
                case POT_12: return MOD_DST_DELAY_FEEDBACK; 
                case POT_13: return MOD_DST_FILTER_TRACKING; 
                case POT_14: return MOD_DST_FILTER_ENV_AMT; 

                default:     break;
            }
            break;

        case LAYER_ENV:
            switch (pot) {
                case POT_0: return MOD_DST_ENV_1_ATTACK; 
                case POT_1: return MOD_DST_ENV_1_DECAY; 
                case POT_2: return MOD_DST_ENV_1_SUSTAIN; 
                case POT_3: return MOD_DST_ENV_1_RELEASE; 
                case POT_4: return MOD_DST_ENV_1_AMOUNT; 

                case POT_5: return MOD_DST_ENV_2_ATTACK; 
                case POT_6: return MOD_DST_ENV_2_DECAY; 
                case POT_7: return MOD_DST_ENV_2_SUSTAIN; 
                case POT_8: return MOD_DST_ENV_2_RELEASE; 
                case POT_9: return MOD_DST_ENV_2_AMOUNT; 

                case POT_10: return MOD_DST_ENV_3_ATTACK; 
                case POT_11: return MOD_DST_ENV_3_DECAY; 
                case POT_12: return MOD_DST_ENV_3_SUSTAIN; 
                case POT_13: return MOD_DST_ENV_3_RELEASE; 
                case POT_14: return MOD_DST_ENV_3_AMOUNT; 

                default:     break;
            }
            break;

        case LAYER_LFO:
            switch (pot) {
                case POT_0: return MOD_DST_LFO_1_RATE; 
                case POT_1: return MOD_DST_LFO_1_SHAPE; 
                case POT_2: return MOD_DST_LFO_1_PHASE; 
                case POT_3: return MOD_DST_LFO_1_SYNC; 
                case POT_4: return MOD_DST_LFO_1_AMOUNT; 

                case POT_5: return MOD_DST_LFO_2_RATE; 
                case POT_6: return MOD_DST_LFO_2_SHAPE; 
                case POT_7: return MOD_DST_LFO_2_PHASE; 
                case POT_8: return MOD_DST_LFO_2_SYNC; 
                case POT_9: return MOD_DST_LFO_2_AMOUNT; 

                case POT_10: return MOD_DST_LFO_3_RATE; 
                case POT_11: return MOD_DST_LFO_3_SHAPE; 
                case POT_12: return MOD_DST_LFO_3_PHASE; 
                case POT_13: return MOD_DST_LFO_3_SYNC; 
                case POT_14: return MOD_DST_LFO_3_AMOUNT; 

                default:     break;
            }
            break;

        case LAYER_SYNTH_ENV_SPLIT:
            switch (pot) {
                case POT_0: return MOD_DST_OSC_1_PITCH; 
                case POT_1: return MOD_DST_OSC_2_PITCH; 
                case POT_2: return MOD_DST_PORTAMENTO; 
                case POT_3: return MOD_DST_MIX_OSC_1_MIX; 
                case POT_4: return MOD_DST_FILTER_CUTOFF; 

                case POT_5: return MOD_DST_OSC_1_X_MORPH; 
                case POT_6: return MOD_DST_OSC_2_X_MORPH; 
                case POT_7: return MOD_DST_PHASE_MOD; 
                case POT_8: return MOD_DST_MIX_OSC_2_MIX; 
                case POT_9: return MOD_DST_FILTER_RESONANCE; 

                case POT_10: return MOD_DST_ENV_3_ATTACK; 
                case POT_11: return MOD_DST_ENV_3_DECAY; 
                case POT_12: return MOD_DST_ENV_3_SUSTAIN; 
                case POT_13: return MOD_DST_ENV_3_RELEASE; 
                case POT_14: return MOD_DST_VCA; // for drone mode off/on

                default:     break;
            }
            break;

        case LAYER_USER_PARAM:

/*             switch (pot) {
                case POT_0: return (mod_destinations_t) synth->user_param.destination[0]; 
                case POT_1: return (mod_destinations_t) synth->user_param.destination[1]; 
                case POT_2: return (mod_destinations_t) synth->user_param.destination[2]; 
                case POT_3: return (mod_destinations_t) synth->user_param.destination[3]; 
                case POT_4: return (mod_destinations_t) synth->user_param.destination[4]; 

                case POT_5: return (mod_destinations_t) synth->user_param.destination[5]; 
                case POT_6: return (mod_destinations_t) synth->user_param.destination[6]; 
                case POT_7: return (mod_destinations_t) synth->user_param.destination[7]; 
                case POT_8: return (mod_destinations_t) synth->user_param.destination[8]; 
                case POT_9: return (mod_destinations_t) synth->user_param.destination[9]; 

                case POT_10: return (mod_destinations_t) synth->user_param.destination[10]; 
                case POT_11: return (mod_destinations_t) synth->user_param.destination[11]; 
                case POT_12: return (mod_destinations_t) synth->user_param.destination[12]; 
                case POT_13: return (mod_destinations_t) synth->user_param.destination[13]; 
                case POT_14: return (mod_destinations_t) synth->user_param.destination[14]; // for drone mode off/on

                default:     break;
            } */
            return getUserParameterModDestination(pot);

            break;

        default:
            // Handle unknown layer
            break;
    }

    return NUM_MOD_DESTINATIONS;

}

inline void setParamSeq(
    pot_index_t pot,
    uint8_t value, 
    uint8_t num_bottom_keys_pressed,
    bool shift, 
    bool p_lock_set,
    bool *bottom_key
){

    seq_params_t seq_param = layerLookupSeqParam(pot);

    if(!p_lock_set) {

        setParamValSeq(seq_param, value, shift);

    } else {

        // TEST_PRINT_TEXT("SEQ P LOCK: ")

        for(int i = 0; i < NUMBER_OF_BOTTOM_SWITCHES; i++) {

            if(bottom_key[i]) {

                setParamValSeqPLock(seq_param, i, value);

                // TEST_PRINT_ONE_TAB(i)

            }

            // TEST_PRINT_NL

        }

    }

}

/* void testSynthParameters() {

    for(int i = 0; i < NUM_VIEW_LAYERS; i++) {

        if(i != LAYER_SEQ) {

            // TEST_PRINT_TEXT("LAYER: ")
            // TEST_PRINT_ONE_TAB(i)

            for(int j = 0; j < NUMBER_OF_PARAMETERS_PER_LAYER; j++) {

                view_layer_t layer = (view_layer_t)i;
                pot_index_t pot = (pot_index_t)j;

                mod_destinations_t dst = layerLookupModDst(layer, pot);
            
                // TEST_PRINT_ONE_TAB(*param_ptr[dst])
                // TEST_PRINT_ONE_TAB(*render_patch_ptr[dst])

            }

            // TEST_PRINT_NL

        }

    }

} */

inline void setParamModDst(
    view_layer_t layer,
    pot_index_t pot,
    uint8_t value, 
    uint8_t num_bottom_keys_pressed,
    bool shift, 
    bool p_lock_set,
    bool *bottom_key
) {

    mod_destinations_t dst = layerLookupModDst(layer, pot);
    uint8_t value_modified = modifyParamValueModDst(&dst, value, shift);

    // TEST_PRINT_TWO_NL(layer, pot)

    if(!p_lock_set) {

#ifdef ALLOW_P_LOCK



#else

        uint8_t* p = param_ptr[dst];

        if(synth->settings.latch && viewGetLatchSingle(pot)) {

            if(value_modified > *p - k_latch_threshold && value_modified <= *p + k_latch_threshold) {

                viewResetLatchSingle(pot);

            } else {

                return;

            }

        }

        *p = value_modified;
        // *param_ptr[dst] = value_modified;

#endif

    } else {

        uint8_t step_base = viewGetSequencerWindowStepBase();

        for(int i = 0; i < NUMBER_OF_BOTTOM_SWITCHES; i++) {

            if(bottom_key[i]) {

                uint8_t step = step_base + i;

                synth->seq.setStepPLockParam(step, dst);
                synth->seq.setStepPLockValue(step, value_modified);

#ifdef ALLOW_P_LOCK



#endif

            }

        }

    }

    

}

bool isPLockHappening() {

    if(pLockIsEnabled()) {

        pLockEngage(true);

    }

    return pLockIsEngaged();

}

void lfoSyncCheck(mod_destinations_t dst) {



}

// ################################
// PUBLIC FUNCTIONS
// ################################

void parameterSetup(synth_state_t *synth_state) {

    synth = synth_state;

    // set the control patch to initial synth state to reflect startup patch load
    render_patch = synth->patch;

    render_patch_ptr[MOD_DST_NONE] = NULL;
    render_patch_ptr[MOD_DST_OSC_1_PITCH] = &render_patch.osc[0].scale; 
    render_patch_ptr[MOD_DST_OSC_2_PITCH] = &render_patch.osc[1].scale; 
    render_patch_ptr[MOD_DST_PORTAMENTO] = &render_patch.shared.portamento; 
    render_patch_ptr[MOD_DST_MIX_OSC_1_MIX] = &render_patch.mixer.osc_1_mix;
    render_patch_ptr[MOD_DST_FILTER_CUTOFF] = &render_patch.filter.cutoff;
    render_patch_ptr[MOD_DST_OSC_1_X_MORPH] = &render_patch.osc[0].x_morph; 
    render_patch_ptr[MOD_DST_OSC_2_X_MORPH] = &render_patch.osc[1].x_morph; 
    render_patch_ptr[MOD_DST_PHASE_MOD] = &render_patch.shared.phase_mod; 
    render_patch_ptr[MOD_DST_MIX_OSC_2_MIX] = &render_patch.mixer.osc_2_mix;
    render_patch_ptr[MOD_DST_FILTER_RESONANCE] = &render_patch.filter.resonance; 
    render_patch_ptr[MOD_DST_FILTER_TYPE] = &render_patch.filter.type; 
    render_patch_ptr[MOD_DST_FILTER_FM] = &render_patch.filter.fm; 
    render_patch_ptr[MOD_DST_FILTER_TRACKING] = &render_patch.filter.tracking;
    render_patch_ptr[MOD_DST_VCA] = &render_patch.vca.level;
    render_patch_ptr[MOD_DST_OSC_1_Y_MORPH] = &render_patch.osc[0].y_morph; 
    render_patch_ptr[MOD_DST_OSC_2_Y_MORPH] = &render_patch.osc[1].y_morph; 
    render_patch_ptr[MOD_DST_SYNC] = &render_patch.shared.sync; 
    render_patch_ptr[MOD_DST_MIX_NOISE_MIX] = &render_patch.mixer.noise_mix;
    render_patch_ptr[MOD_DST_FILTER_ENV_AMT] = &render_patch.filter.env_amt; 
    render_patch_ptr[MOD_DST_OSC_1_OCTAVE] = &render_patch.osc[0].octave; 
    render_patch_ptr[MOD_DST_OSC_2_OCTAVE] = &render_patch.osc[1].octave; 
    render_patch_ptr[MOD_DST_DELAY_AMOUNT] = &render_patch.delay.amount; 
    render_patch_ptr[MOD_DST_DELAY_RATE] = &render_patch.delay.rate;
    render_patch_ptr[MOD_DST_DELAY_FEEDBACK] = &render_patch.delay.feedback; 
    // render_patch_ptr[MOD_DST_DELAY_FILTER] = &render_patch.delay.filter;
    render_patch_ptr[MOD_DST_ENV_1_ATTACK] = &render_patch.env[0].attack; 
    render_patch_ptr[MOD_DST_ENV_1_DECAY] = &render_patch.env[0].decay; 
    render_patch_ptr[MOD_DST_ENV_1_SUSTAIN] = &render_patch.env[0].sustain; 
    render_patch_ptr[MOD_DST_ENV_1_RELEASE] = &render_patch.env[0].release; 
    render_patch_ptr[MOD_DST_ENV_1_LOOP] = &render_patch.env[0].loop;
    render_patch_ptr[MOD_DST_ENV_1_AMOUNT] = &render_patch.mod[0].amount; 
    render_patch_ptr[MOD_DST_ENV_2_ATTACK] = &render_patch.env[1].attack; 
    render_patch_ptr[MOD_DST_ENV_2_DECAY] = &render_patch.env[1].decay; 
    render_patch_ptr[MOD_DST_ENV_2_SUSTAIN] = &render_patch.env[1].sustain; 
    render_patch_ptr[MOD_DST_ENV_2_RELEASE] = &render_patch.env[1].release; 
    render_patch_ptr[MOD_DST_ENV_2_LOOP] = &render_patch.env[1].loop;
    render_patch_ptr[MOD_DST_ENV_2_AMOUNT] = &render_patch.mod[1].amount;  
    render_patch_ptr[MOD_DST_ENV_3_ATTACK] = &render_patch.env[2].attack; 
    render_patch_ptr[MOD_DST_ENV_3_DECAY] = &render_patch.env[2].decay; 
    render_patch_ptr[MOD_DST_ENV_3_SUSTAIN] = &render_patch.env[2].sustain; 
    render_patch_ptr[MOD_DST_ENV_3_RELEASE] = &render_patch.env[2].release; 
    render_patch_ptr[MOD_DST_ENV_3_LOOP] = &render_patch.env[2].loop;
    render_patch_ptr[MOD_DST_ENV_3_AMOUNT] = &render_patch.mod[2].amount;  
    render_patch_ptr[MOD_DST_LFO_1_RATE] = &render_patch.lfo[0].rate; 
    render_patch_ptr[MOD_DST_LFO_1_SHAPE] = &render_patch.lfo[0].shape; 
    render_patch_ptr[MOD_DST_LFO_1_PHASE] = &render_patch.lfo[0].phase; 
    render_patch_ptr[MOD_DST_LFO_1_SYNC] = &render_patch.lfo[0].sync;
    render_patch_ptr[MOD_DST_LFO_1_AMOUNT] = &render_patch.mod[3].amount; 
    render_patch_ptr[MOD_DST_LFO_2_RATE] = &render_patch.lfo[1].rate; 
    render_patch_ptr[MOD_DST_LFO_2_SHAPE] = &render_patch.lfo[1].shape; 
    render_patch_ptr[MOD_DST_LFO_2_PHASE] = &render_patch.lfo[1].phase; 
    render_patch_ptr[MOD_DST_LFO_2_SYNC] = &render_patch.lfo[1].sync;
    render_patch_ptr[MOD_DST_LFO_2_AMOUNT] = &render_patch.mod[4].amount;  
    render_patch_ptr[MOD_DST_LFO_3_RATE] = &render_patch.lfo[2].rate; 
    render_patch_ptr[MOD_DST_LFO_3_SHAPE] = &render_patch.lfo[2].shape; 
    render_patch_ptr[MOD_DST_LFO_3_PHASE] = &render_patch.lfo[2].phase; 
    render_patch_ptr[MOD_DST_LFO_3_SYNC] = &render_patch.lfo[2].sync;
    render_patch_ptr[MOD_DST_LFO_3_AMOUNT] = &render_patch.mod[5].amount; 
    render_patch_ptr[MOD_DST_ENV_1_DST] = &render_patch.mod[0].destination;
    render_patch_ptr[MOD_DST_ENV_2_DST] = &render_patch.mod[1].destination;
    render_patch_ptr[MOD_DST_ENV_3_DST] = &render_patch.mod[2].destination;
    render_patch_ptr[MOD_DST_LFO_1_DST] = &render_patch.mod[3].destination;
    render_patch_ptr[MOD_DST_LFO_2_DST] = &render_patch.mod[4].destination;
    render_patch_ptr[MOD_DST_LFO_3_DST] = &render_patch.mod[5].destination;
    render_patch_ptr[MOD_DST_BITRATE_REDUCE] = &render_patch.shared.bitrate_reduce;
    render_patch_ptr[MOD_DST_SAMPLERATE_REDUCE] = &render_patch.shared.samplerate_reduce;

    param_ptr[MOD_DST_NONE] = NULL;
    param_ptr[MOD_DST_OSC_1_PITCH] = &synth->patch.osc[0].scale; 
    param_ptr[MOD_DST_OSC_2_PITCH] = &synth->patch.osc[1].scale; 
    param_ptr[MOD_DST_PORTAMENTO] = &synth->patch.shared.portamento; 
    param_ptr[MOD_DST_MIX_OSC_1_MIX] = &synth->patch.mixer.osc_1_mix;
    param_ptr[MOD_DST_FILTER_CUTOFF] = &synth->patch.filter.cutoff;
    param_ptr[MOD_DST_OSC_1_X_MORPH] = &synth->patch.osc[0].x_morph; 
    param_ptr[MOD_DST_OSC_2_X_MORPH] = &synth->patch.osc[1].x_morph; 
    param_ptr[MOD_DST_PHASE_MOD] = &synth->patch.shared.phase_mod; 
    param_ptr[MOD_DST_MIX_OSC_2_MIX] = &synth->patch.mixer.osc_2_mix;
    param_ptr[MOD_DST_FILTER_RESONANCE] = &synth->patch.filter.resonance; 
    param_ptr[MOD_DST_FILTER_TYPE] = &synth->patch.filter.type; 
    param_ptr[MOD_DST_FILTER_FM] = &synth->patch.filter.fm; 
    param_ptr[MOD_DST_FILTER_TRACKING] = &synth->patch.filter.tracking;
    param_ptr[MOD_DST_VCA] = &synth->patch.vca.level;
    param_ptr[MOD_DST_OSC_1_Y_MORPH] = &synth->patch.osc[0].y_morph; 
    param_ptr[MOD_DST_OSC_2_Y_MORPH] = &synth->patch.osc[1].y_morph; 
    param_ptr[MOD_DST_SYNC] = &synth->patch.shared.sync; 
    param_ptr[MOD_DST_MIX_NOISE_MIX] = &synth->patch.mixer.noise_mix;
    param_ptr[MOD_DST_FILTER_ENV_AMT] = &synth->patch.filter.env_amt; 
    param_ptr[MOD_DST_OSC_1_OCTAVE] = &synth->patch.osc[0].octave; 
    param_ptr[MOD_DST_OSC_2_OCTAVE] = &synth->patch.osc[1].octave; 
    param_ptr[MOD_DST_DELAY_AMOUNT] = &synth->patch.delay.amount; 
    param_ptr[MOD_DST_DELAY_RATE] = &synth->patch.delay.rate;
    param_ptr[MOD_DST_DELAY_FEEDBACK] = &synth->patch.delay.feedback; 
    // param_ptr[MOD_DST_DELAY_FILTER] = &synth->patch.delay.filter;
    param_ptr[MOD_DST_ENV_1_ATTACK] = &synth->patch.env[0].attack; 
    param_ptr[MOD_DST_ENV_1_DECAY] = &synth->patch.env[0].decay; 
    param_ptr[MOD_DST_ENV_1_SUSTAIN] = &synth->patch.env[0].sustain; 
    param_ptr[MOD_DST_ENV_1_RELEASE] = &synth->patch.env[0].release; 
    param_ptr[MOD_DST_ENV_1_LOOP] = &synth->patch.env[0].loop;
    param_ptr[MOD_DST_ENV_1_AMOUNT] = &synth->patch.mod[0].amount; 
    param_ptr[MOD_DST_ENV_2_ATTACK] = &synth->patch.env[1].attack; 
    param_ptr[MOD_DST_ENV_2_DECAY] = &synth->patch.env[1].decay; 
    param_ptr[MOD_DST_ENV_2_SUSTAIN] = &synth->patch.env[1].sustain; 
    param_ptr[MOD_DST_ENV_2_RELEASE] = &synth->patch.env[1].release; 
    param_ptr[MOD_DST_ENV_2_LOOP] = &synth->patch.env[1].loop;
    param_ptr[MOD_DST_ENV_2_AMOUNT] = &synth->patch.mod[1].amount;  
    param_ptr[MOD_DST_ENV_3_ATTACK] = &synth->patch.env[2].attack; 
    param_ptr[MOD_DST_ENV_3_DECAY] = &synth->patch.env[2].decay; 
    param_ptr[MOD_DST_ENV_3_SUSTAIN] = &synth->patch.env[2].sustain; 
    param_ptr[MOD_DST_ENV_3_RELEASE] = &synth->patch.env[2].release; 
    param_ptr[MOD_DST_ENV_3_LOOP] = &synth->patch.env[2].loop;
    param_ptr[MOD_DST_ENV_3_AMOUNT] = &synth->patch.mod[2].amount;  
    param_ptr[MOD_DST_LFO_1_RATE] = &synth->patch.lfo[0].rate; 
    param_ptr[MOD_DST_LFO_1_SHAPE] = &synth->patch.lfo[0].shape; 
    param_ptr[MOD_DST_LFO_1_PHASE] = &synth->patch.lfo[0].phase; 
    param_ptr[MOD_DST_LFO_1_SYNC] = &synth->patch.lfo[0].sync;
    param_ptr[MOD_DST_LFO_1_AMOUNT] = &synth->patch.mod[3].amount; 
    param_ptr[MOD_DST_LFO_2_RATE] = &synth->patch.lfo[1].rate; 
    param_ptr[MOD_DST_LFO_2_SHAPE] = &synth->patch.lfo[1].shape; 
    param_ptr[MOD_DST_LFO_2_PHASE] = &synth->patch.lfo[1].phase; 
    param_ptr[MOD_DST_LFO_2_SYNC] = &synth->patch.lfo[1].sync;
    param_ptr[MOD_DST_LFO_2_AMOUNT] = &synth->patch.mod[4].amount;  
    param_ptr[MOD_DST_LFO_3_RATE] = &synth->patch.lfo[2].rate; 
    param_ptr[MOD_DST_LFO_3_SHAPE] = &synth->patch.lfo[2].shape; 
    param_ptr[MOD_DST_LFO_3_PHASE] = &synth->patch.lfo[2].phase; 
    param_ptr[MOD_DST_LFO_3_SYNC] = &synth->patch.lfo[2].sync;
    param_ptr[MOD_DST_LFO_3_AMOUNT] = &synth->patch.mod[5].amount; 
    param_ptr[MOD_DST_ENV_1_DST] = &synth->patch.mod[0].destination;
    param_ptr[MOD_DST_ENV_2_DST] = &synth->patch.mod[1].destination;
    param_ptr[MOD_DST_ENV_3_DST] = &synth->patch.mod[2].destination;
    param_ptr[MOD_DST_LFO_1_DST] = &synth->patch.mod[3].destination;
    param_ptr[MOD_DST_LFO_2_DST] = &synth->patch.mod[4].destination;
    param_ptr[MOD_DST_LFO_3_DST] = &synth->patch.mod[5].destination;
    param_ptr[MOD_DST_BITRATE_REDUCE] = &synth->patch.shared.bitrate_reduce;
    param_ptr[MOD_DST_SAMPLERATE_REDUCE] = &synth->patch.shared.samplerate_reduce;

}

void parameterSetValueWithPot(
    pot_index_t pot,
    uint8_t value,
    uint8_t num_bottom_keys_pressed,
    bool shift,
    bool *bottom_key
) {

    view_layer_t layer = viewGetLayer();
    seq_params_t seq_param;
    bool p_lock_setting = isPLockHappening();

    switch(layer) {

        case LAYER_SEQ:

            setParamSeq(
                pot, 
                value, 
                num_bottom_keys_pressed, 
                shift, 
                p_lock_setting, 
                bottom_key);

            break;

        case LAYER_SYNTH_1:
        case LAYER_SYNTH_2:
        case LAYER_ENV:
        case LAYER_LFO:
        case LAYER_SYNTH_ENV_SPLIT:
        case LAYER_USER_PARAM:

            setParamModDst(
                layer, 
                pot, 
                value, 
                num_bottom_keys_pressed,
                shift, 
                p_lock_setting, 
                bottom_key);

            break;

        default: break;

    }

    // testSynthParameters();

}

void parameterSetValueByModDst(mod_destinations_t dst, uint8_t value) {

    uint8_t value_modified = modifyParamValueModDst(&dst, value, false);
    *param_ptr[dst] = value_modified;
    // *render_patch_ptr[dst] = value_modified;

}

// TODO: not urgent, but this function can be removed, and the synth ext_mod params should be fed directly into 
// seq_poll.h via a pointer. 
void parameterSetPLockByStep(uint8_t step, bool active) {

    if(active) {

        synth->ext_mod.p_lock_dst = synth->seq.getStepPLockParam(step);
        synth->ext_mod.p_lock_amt = synth->seq.getStepPLockValue(step);

    }

    // if(active) {

    //     p_lock_active_dst[step] = (mod_destinations_t) synth->seq.getStepPLockParam(step);
    //     p_lock_active_value[step] = synth->seq.getStepPLockValue(step);

    // } else {

    //     p_lock_active_dst[step] = MOD_DST_NONE;
    //     p_lock_active_value[step] = 0;

    // }

#ifdef ALLOW_P_LOCK


#endif
    
}

// this function is in use and is entrenched in the way that parameters are fed to the display and the synth, 
// but should be changed and deprecated in the future, so that the synth patch is used directly. 
// consider this a TODO that is not happening soon...
void parameterCopyValuesToSynth() {

    render_patch = synth->patch;

    // for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

    //     mod_destinations_t dst = p_lock_active_dst[i];

    //     if(dst != MOD_DST_NONE) {

    //         *param_ptr[dst] = p_lock_active_value[i];

    //     }

    // }

}


uint8_t parameterSetValueByModDstIndex(uint8_t index, uint8_t value) {

    mod_destinations_t dst = (mod_destinations_t) index;
    uint8_t value_modified = modifyParamValueModDst(&dst, value, false);

    uint8_t output = *param_ptr[dst];

    *param_ptr[dst] = value_modified;

    return output;

}

synth_parameters_t* parameterGetRenderedPatchPointer() {

    return &render_patch;

}

uint8_t* parameterGetPointer(mod_destinations_t dst) {

    // return param_ptr[dst];
return render_patch_ptr[dst];

}


uint8_t parameterGetParamFromCurrentLayer(uint8_t index) {

    view_layer_t layer = viewGetLayer();
    bool p_lock_setting = viewIsSubLayerPLockReady();

    if(layer != LAYER_SEQ) {

        mod_destinations_t dst = layerLookupModDst(layer, (pot_index_t)index);
        
        return *param_ptr[dst];

    } else {

        switch((seq_params_t)index) {

            case SEQ_PARAM_STEP_NOTE: 
                
                if(!p_lock_setting) {
                    // return viewGetSeqParamStepNote(); 
                } else {
                    // return current_p_lock_note[index_to_show];
                    // return viewGetFirstPLockNote();
                }
                
                break;

            case SEQ_PARAM_STEP_OCTAVE: 

                if(!p_lock_setting) {
                    // return viewGetSeqParamStepOctave(); 
                } else {
                    // return current_p_lock_octave[index_to_show];
                    // return viewGetFirstPLockOctave();
                }
                
                break;

            case SEQ_PARAM_VELOCITY: 

                if(!p_lock_setting) {
                    return synth->new_step.velocity; 
                } else {
                    // return synth->seq.getStepVelocity(step_to_show);
                }
                
                break;

            case SEQ_PARAM_STEP_LENGTH: 

                if(!p_lock_setting) {
                    return synth->new_step.length; 
                } else {
                    // return synth->seq.getStepLength(step_to_show);
                }
    
                break;

            case SEQ_PARAM_DIRECTION: return synth->seq.getDirection(); break;
            case SEQ_PARAM_SWING: return synth->seq.getSwingAmt(); break;
            case SEQ_PARAM_CLOCK_DIV: return synth->seq.getClockDiv(); break;
            case SEQ_PARAM_FIRST_STEP: return synth->seq.getFirstStep(); break;
            case SEQ_PARAM_LAST_STEP: return synth->seq.getLastStep(); break;
            case SEQ_PARAM_PROBABILITY: return synth->seq.getProbability(); break;
            case SEQ_PARAM_SCALE: return synth->pattern_gen.scale; break;
            case SEQ_PARAM_REPEAT: return synth->pattern_gen.repeat; break;
            case SEQ_PARAM_OCTAVE_RANGE: return synth->pattern_gen.octave_range; break;

        }

    }

    return 0;

}

void parameterSetUserParameter(uint8_t index, mod_destinations_t dst) {

    synth->user_param.destination[index] = (uint8_t) dst;
    user_param[index] = param_ptr[dst];

}

void parameterSetUserParameterAllPointers() {

    for(int i = 0; i < NUMBER_OF_POTENTIOMETERS; i++) {

        mod_destinations_t dst = (mod_destinations_t) synth->user_param.destination[i];
        
        parameterSetUserParameter(i, dst);

    }

}

uint8_t parameterGetUserParameter(uint8_t index) {

    return *user_param[index];

}