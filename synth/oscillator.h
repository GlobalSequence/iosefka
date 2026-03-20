#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/globals.h"
#include "../common/moving_average.h"
#include "wavetable.h"

#include "../common/test.h"

#define WAVE_INTERPOLATE_DELTA_X    (NUM_WAVETABLE_SAMPLES / NUM_X_WAVETABLES)
#define WAVE_INTERPOLATE_DELTA_Y    (NUM_WAVETABLE_SAMPLES / NUM_Y_WAVETABLES)
#define WAVE_INTERPOLATE_DELTA_BP   (NUM_WAVETABLE_SAMPLES / NUM_BL_WAVETABLES)
#define BANDLIMIT_OCTAVE_FLOOR 3

#define BANDLIMIT_NOTE_FLOOR 60
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
        if(octave < BANDLIMIT_OCTAVE_FLOOR) {
            octave_base_ = 0; 
            octave_next_ = 0;
        } else {
            octave_base_ = octave - BANDLIMIT_OCTAVE_FLOOR;
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

    inline void render2(uint32_t inc, uint8_t pitch, uint32_t *fm_buffer, uint8_t fm_amount, bool *sync_in, bool *sync_out, uint32_t *buffer)
    {
        inc_ = inc;

        int16_t prepitch = pitch - BANDLIMIT_NOTE_FLOOR;
        uint8_t bl_table = prepitch >> 3;
        uint8_t bl_table_next = bl_table + 1;
        uint8_t bl_table_gradient = (prepitch & 0b111) << 2;

        if(prepitch >= 0) {

            octave_base_ = bl_table;
            octave_next_ = bl_table_next;

            if(octave_next_ >= NUM_BL_WAVETABLES) {
                octave_base_ = k_bl_limit_;
                octave_next_ = k_bl_limit_;
            }

            pitch_gradient_ = bl_table_gradient;


        } else {
        
            octave_base_ = 0;
            octave_next_ = 0;
            pitch_gradient_ = 0;

        }

        fm_buffer_ = fm_buffer;
        fm_amount_ = fm_amount;
        sync_in_ = sync_in;
        sync_out_ = sync_out;
        buffer_ = buffer;

        phaseAccumulator2();
    }

    void setXMorph(uint8_t input);
    void setYMorph(uint8_t input);
    void reset();

private:
    const uint32_t kscale_osc_level_ = SEMITONES_PER_OCTAVE * WAVE_INTERPOLATE_DELTA_X * WAVE_INTERPOLATE_DELTA_Y;
    const uint32_t kscale_osc_level_2_ = 8 * WAVE_INTERPOLATE_DELTA_X * WAVE_INTERPOLATE_DELTA_Y;
    uint32_t inc_;
    uint8_t octave_base_;
    uint8_t octave_next_;
    const uint8_t k_bl_limit_ = NUM_BL_WAVETABLES - 1;
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
    void phaseAccumulator2(void);

    MovingAverage32 phaseModSmooth;
    MovingAverage32 blTableSmooth;

};


#endif