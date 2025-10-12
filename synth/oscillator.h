#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/globals.h"
#include "wavetable.h"

#define WAVE_INTERPOLATE_DELTA_X    (NUM_WAVETABLE_SAMPLES / NUM_X_WAVETABLES)
#define WAVE_INTERPOLATE_DELTA_Y    (NUM_WAVETABLE_SAMPLES / NUM_Y_WAVETABLES)
#define WAVE_INTERPOLATE_DELTA_BP   (NUM_WAVETABLE_SAMPLES / NUM_BL_WAVETABLES)

// #define WAVE_INTERPOLATE_DISTANCE 32
// #define NUM_X_WAVETABLES 8
// #define NUM_Y_WAVETABLES 8

class Oscillator
{
public:
    Oscillator(void) {}

    inline void render(uint32_t inc, uint8_t octave, uint8_t pitch_index, uint32_t *fm_buffer, uint8_t fm_amount, bool *sync_in, bool *sync_out, uint32_t *buffer)
    {
        inc_ = inc;
        // calculate this in voice.cpp
        // if(octave <= 3) octave_base_ = 0;
        // else octave_base_ = octave - 3;
        if(octave < 3) {
            octave_base_ = 0; 
            octave_next_ = 0;
        } else {
            octave_base_ = octave - 3;
            octave_next_ = octave_base_ + 1;
        }
        
        if(octave_next_ >= NUM_BL_WAVETABLES) {
            octave_next_ = NUM_BL_WAVETABLES - 1;
        }

        pitch_gradient_ = pitch_index;
        fm_buffer_ = fm_buffer;
        fm_amount_ = fm_amount;
        sync_in_ = sync_in;
        sync_out_ = sync_out;
        buffer_ = buffer;

        phaseAccumulator();
    }
    void setXMorph(uint8_t input);
    void setYMorph(uint8_t input);

private:
    const uint32_t kscale_osc_level_ = SEMITONES_PER_OCTAVE * WAVE_INTERPOLATE_DELTA_X * WAVE_INTERPOLATE_DELTA_Y;
    uint32_t inc_;
    uint8_t octave_base_;
    uint8_t octave_next_;
    uint8_t pitch_gradient_;
    uint8_t x_morph_;
    uint8_t x_index_base_;
    uint8_t x_index_next_;
    uint8_t y_morph_;
    uint8_t y_index_base_;
    uint8_t y_index_next_;
    uint32_t *fm_buffer_;
    uint8_t fm_amount_;
    uint8_t fm_index_;
    uint32_t phase_;
    uint32_t *buffer_;
    bool sync_on_;
    bool *sync_;
    bool *sync_in_;
    bool *sync_out_;

    void phaseAccumulator(void);

};


#endif