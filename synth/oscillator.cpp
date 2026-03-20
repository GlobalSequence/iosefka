#include "oscillator.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/utilities.h"
#include "../common/globals.h"
#include "wavetable.h"

void Oscillator::phaseAccumulator(void)
{

    for (int i = 0; i < AUDIO_BLOCK_SIZE; i++)
    {

        addWithCarry(phase_, inc_, sync_out_);
        if (*sync_in_)
        {
            phase_ = 0;
        }
        uint8_t waveIndex = phase_ >> 24;
        waveIndex += *fm_buffer_ * phaseModSmooth.getAverage(fm_amount_) >> 14;

        uint8_t phase_gradient = (phase_ >> 16) & 0xFF;
        uint8_t not_phase_gradient = ~phase_gradient;
        uint8_t next_wave_index = (waveIndex + 1) & 0xFF;

        // We're interpolating in 4 dimensions:
        // - w[n]: the wavetable index to reduce quantization artifacts
        // - bl[n]: the bandlimited wavetables, scanned by pitch above 3rd octave
        // - x[n]: the x-axis scanned wavetables
        // - y[n]: the y-axis scanned wavetables

        uint16_t y1x1b1w1 = wavetable[y_index_base_][x_index_base_][octave_base_][waveIndex];
        uint16_t y1x1b1w2 = wavetable[y_index_base_][x_index_base_][octave_base_][next_wave_index];

        uint16_t y1x1b2w1 = wavetable[y_index_base_][x_index_base_][octave_next_][waveIndex];
        uint16_t y1x1b2w2 = wavetable[y_index_base_][x_index_base_][octave_next_][next_wave_index];

        uint16_t y1x2b1w1 = wavetable[y_index_base_][x_index_next_][octave_base_][waveIndex];
        uint16_t y1x2b1w2 = wavetable[y_index_base_][x_index_next_][octave_base_][next_wave_index];

        uint16_t y1x2b2w1 = wavetable[y_index_base_][x_index_next_][octave_next_][waveIndex];
        uint16_t y1x2b2w2 = wavetable[y_index_base_][x_index_next_][octave_next_][next_wave_index];

        uint16_t y2x1b1w1 = wavetable[y_index_next_][x_index_base_][octave_base_][waveIndex];
        uint16_t y2x1b1w2 = wavetable[y_index_next_][x_index_base_][octave_base_][next_wave_index];

        uint16_t y2x1b2w1 = wavetable[y_index_next_][x_index_base_][octave_next_][waveIndex];
        uint16_t y2x1b2w2 = wavetable[y_index_next_][x_index_base_][octave_next_][next_wave_index];

        uint16_t y2x2b1w1 = wavetable[y_index_next_][x_index_next_][octave_base_][waveIndex];
        uint16_t y2x2b1w2 = wavetable[y_index_next_][x_index_next_][octave_base_][next_wave_index];

        uint16_t y2x2b2w1 = wavetable[y_index_next_][x_index_next_][octave_next_][waveIndex];
        uint16_t y2x2b2w2 = wavetable[y_index_next_][x_index_next_][octave_next_][next_wave_index];

        uint16_t y1x1b1 = U32Interp8Bit(y1x1b1w1, y1x1b1w2, phase_gradient, not_phase_gradient);
        uint16_t y1x1b2 = U32Interp8Bit(y1x1b2w1, y1x1b2w2, phase_gradient, not_phase_gradient);
        uint16_t y1x2b1 = U32Interp8Bit(y1x2b1w1, y1x2b1w2, phase_gradient, not_phase_gradient);
        uint16_t y1x2b2 = U32Interp8Bit(y1x2b2w1, y1x2b2w2, phase_gradient, not_phase_gradient);
        uint16_t y2x1b1 = U32Interp8Bit(y2x1b1w1, y2x1b1w2, phase_gradient, not_phase_gradient);
        uint16_t y2x1b2 = U32Interp8Bit(y2x1b2w1, y2x1b2w2, phase_gradient, not_phase_gradient);
        uint16_t y2x2b1 = U32Interp8Bit(y2x2b1w1, y2x2b1w2, phase_gradient, not_phase_gradient);
        uint16_t y2x2b2 = U32Interp8Bit(y2x2b2w1, y2x2b2w2, phase_gradient, not_phase_gradient);

        uint32_t y1x1 = U32MixU32(y1x1b1, y1x1b2, SEMITONES_PER_OCTAVE, pitch_gradient_);
        uint32_t y1x2 = U32MixU32(y1x2b1, y1x2b2, SEMITONES_PER_OCTAVE, pitch_gradient_);
        uint32_t y2x1 = U32MixU32(y2x1b1, y2x1b2, SEMITONES_PER_OCTAVE, pitch_gradient_);
        uint32_t y2x2 = U32MixU32(y2x2b1, y2x2b2, SEMITONES_PER_OCTAVE, pitch_gradient_);

        uint32_t y1 = U32MixU32(y1x1, y1x2, WAVE_INTERPOLATE_DELTA_X, x_morph_);
        uint32_t y2 = U32MixU32(y2x1, y2x2, WAVE_INTERPOLATE_DELTA_X, x_morph_);

        *buffer_++ = (U32MixU32(y1, y2, WAVE_INTERPOLATE_DELTA_Y, y_morph_) / (kscale_osc_level_));

/* 
        // NOTE: THIS IS THE MORE EFFICIENT VERSION BUT WITH NO SAMPLE-LEVEL INTERPOLATION

        // We're interpolating in 3 dimensions:
        // - bl[n]: the bandlimited wavetables, scanned by pitch above 3rd octave
        // - x[n]: the x-axis scanned wavetables
        // - y[n]: the y-axis scanned wavetables

        uint16_t y1x1b1 = wavetable[y_index_base_][x_index_base_][octave_base_][waveIndex];
        uint16_t y1x1b2 = wavetable[y_index_base_][x_index_base_][octave_next_][waveIndex];
        uint16_t y1x2b1 = wavetable[y_index_base_][x_index_next_][octave_base_][waveIndex];
        uint16_t y1x2b2 = wavetable[y_index_base_][x_index_next_][octave_next_][waveIndex];
        uint16_t y2x1b1 = wavetable[y_index_next_][x_index_base_][octave_base_][waveIndex];
        uint16_t y2x1b2 = wavetable[y_index_next_][x_index_base_][octave_next_][waveIndex];
        uint16_t y2x2b1 = wavetable[y_index_next_][x_index_next_][octave_base_][waveIndex];
        uint16_t y2x2b2 = wavetable[y_index_next_][x_index_next_][octave_next_][waveIndex];

        uint32_t y1x1 = U16MixU32(y1x1b1, y1x1b2, SEMITONES_PER_OCTAVE, pitch_gradient_);
        uint32_t y1x2 = U16MixU32(y1x2b1, y1x2b2, SEMITONES_PER_OCTAVE, pitch_gradient_);
        uint32_t y2x1 = U16MixU32(y2x1b1, y2x1b2, SEMITONES_PER_OCTAVE, pitch_gradient_);
        uint32_t y2x2 = U16MixU32(y2x2b1, y2x2b2, SEMITONES_PER_OCTAVE, pitch_gradient_);

        uint32_t y1 = U32MixU32(y1x1, y1x2, WAVE_INTERPOLATE_DELTA_X, x_morph_);
        uint32_t y2 = U32MixU32(y2x1, y2x2, WAVE_INTERPOLATE_DELTA_X, x_morph_);

        *buffer_++ = (U32MixU32(y1, y2, WAVE_INTERPOLATE_DELTA_Y, y_morph_) / (kscale_osc_level_));
 */
        sync_in_++;
        sync_out_++;
        fm_buffer_++;
    }
}

void Oscillator::phaseAccumulator2(void)
{

    for (int i = 0; i < AUDIO_BLOCK_SIZE; i++)
    {

        addWithCarry(phase_, inc_, sync_out_);
        if (*sync_in_)
        {
            phase_ = 0;
        }
        uint8_t waveIndex = phase_ >> 24;
        waveIndex += *fm_buffer_ * phaseModSmooth.getAverage(fm_amount_) >> 14;

        uint8_t phase_gradient = (phase_ >> 16) & 0xFF;
        uint8_t not_phase_gradient = ~phase_gradient;
        uint8_t next_wave_index = (waveIndex + 1) & 0xFF;

        uint8_t smooth_table_gradient_ = blTableSmooth.getAverage(pitch_gradient_);

        // We're interpolating in 4 dimensions:
        // - w[n]: the wavetable index to reduce quantization artifacts
        // - bl[n]: the bandlimited wavetables, scanned by pitch above 3rd octave
        // - x[n]: the x-axis scanned wavetables
        // - y[n]: the y-axis scanned wavetables

        uint16_t y1x1b1w1 = wavetable[y_index_base_][x_index_base_][octave_base_][waveIndex];
        uint16_t y1x1b1w2 = wavetable[y_index_base_][x_index_base_][octave_base_][next_wave_index];

        uint16_t y1x1b2w1 = wavetable[y_index_base_][x_index_base_][octave_next_][waveIndex];
        uint16_t y1x1b2w2 = wavetable[y_index_base_][x_index_base_][octave_next_][next_wave_index];

        uint16_t y1x2b1w1 = wavetable[y_index_base_][x_index_next_][octave_base_][waveIndex];
        uint16_t y1x2b1w2 = wavetable[y_index_base_][x_index_next_][octave_base_][next_wave_index];

        uint16_t y1x2b2w1 = wavetable[y_index_base_][x_index_next_][octave_next_][waveIndex];
        uint16_t y1x2b2w2 = wavetable[y_index_base_][x_index_next_][octave_next_][next_wave_index];

        uint16_t y2x1b1w1 = wavetable[y_index_next_][x_index_base_][octave_base_][waveIndex];
        uint16_t y2x1b1w2 = wavetable[y_index_next_][x_index_base_][octave_base_][next_wave_index];

        uint16_t y2x1b2w1 = wavetable[y_index_next_][x_index_base_][octave_next_][waveIndex];
        uint16_t y2x1b2w2 = wavetable[y_index_next_][x_index_base_][octave_next_][next_wave_index];

        uint16_t y2x2b1w1 = wavetable[y_index_next_][x_index_next_][octave_base_][waveIndex];
        uint16_t y2x2b1w2 = wavetable[y_index_next_][x_index_next_][octave_base_][next_wave_index];

        uint16_t y2x2b2w1 = wavetable[y_index_next_][x_index_next_][octave_next_][waveIndex];
        uint16_t y2x2b2w2 = wavetable[y_index_next_][x_index_next_][octave_next_][next_wave_index];

        uint16_t y1x1b1 = U32Interp8Bit(y1x1b1w1, y1x1b1w2, phase_gradient, not_phase_gradient);
        uint16_t y1x1b2 = U32Interp8Bit(y1x1b2w1, y1x1b2w2, phase_gradient, not_phase_gradient);
        uint16_t y1x2b1 = U32Interp8Bit(y1x2b1w1, y1x2b1w2, phase_gradient, not_phase_gradient);
        uint16_t y1x2b2 = U32Interp8Bit(y1x2b2w1, y1x2b2w2, phase_gradient, not_phase_gradient);
        uint16_t y2x1b1 = U32Interp8Bit(y2x1b1w1, y2x1b1w2, phase_gradient, not_phase_gradient);
        uint16_t y2x1b2 = U32Interp8Bit(y2x1b2w1, y2x1b2w2, phase_gradient, not_phase_gradient);
        uint16_t y2x2b1 = U32Interp8Bit(y2x2b1w1, y2x2b1w2, phase_gradient, not_phase_gradient);
        uint16_t y2x2b2 = U32Interp8Bit(y2x2b2w1, y2x2b2w2, phase_gradient, not_phase_gradient);

        // uint32_t y1x1 = U32MixU32(y1x1b1, y1x1b2, 8, pitch_gradient_);
        // uint32_t y1x2 = U32MixU32(y1x2b1, y1x2b2, 8, pitch_gradient_);
        // uint32_t y2x1 = U32MixU32(y2x1b1, y2x1b2, 8, pitch_gradient_);
        // uint32_t y2x2 = U32MixU32(y2x2b1, y2x2b2, 8, pitch_gradient_);
        // uint32_t y1x1 = U32MixU32(y1x1b1, y1x1b2, 32, table_gradient);
        // uint32_t y1x2 = U32MixU32(y1x2b1, y1x2b2, 32, table_gradient);
        // uint32_t y2x1 = U32MixU32(y2x1b1, y2x1b2, 32, table_gradient);
        // uint32_t y2x2 = U32MixU32(y2x2b1, y2x2b2, 32, table_gradient);
        uint32_t y1x1 = U32MixU32(y1x1b1, y1x1b2, 32, smooth_table_gradient_);
        uint32_t y1x2 = U32MixU32(y1x2b1, y1x2b2, 32, smooth_table_gradient_);
        uint32_t y2x1 = U32MixU32(y2x1b1, y2x1b2, 32, smooth_table_gradient_);
        uint32_t y2x2 = U32MixU32(y2x2b1, y2x2b2, 32, smooth_table_gradient_);

        uint32_t y1 = U32MixU32(y1x1, y1x2, WAVE_INTERPOLATE_DELTA_X, x_morph_);
        uint32_t y2 = U32MixU32(y2x1, y2x2, WAVE_INTERPOLATE_DELTA_X, x_morph_);

        // *buffer_++ = (U32MixU32(y1, y2, WAVE_INTERPOLATE_DELTA_Y, y_morph_) / (kscale_osc_level_2_));

        *buffer_++ = U32MixU32(y1, y2, WAVE_INTERPOLATE_DELTA_Y, y_morph_) >> 13;

/* 
        // NOTE: THIS IS THE MORE EFFICIENT VERSION BUT WITH NO SAMPLE-LEVEL INTERPOLATION

        // We're interpolating in 3 dimensions:
        // - bl[n]: the bandlimited wavetables, scanned by pitch above 3rd octave
        // - x[n]: the x-axis scanned wavetables
        // - y[n]: the y-axis scanned wavetables

        uint16_t y1x1b1 = wavetable[y_index_base_][x_index_base_][octave_base_][waveIndex];
        uint16_t y1x1b2 = wavetable[y_index_base_][x_index_base_][octave_next_][waveIndex];
        uint16_t y1x2b1 = wavetable[y_index_base_][x_index_next_][octave_base_][waveIndex];
        uint16_t y1x2b2 = wavetable[y_index_base_][x_index_next_][octave_next_][waveIndex];
        uint16_t y2x1b1 = wavetable[y_index_next_][x_index_base_][octave_base_][waveIndex];
        uint16_t y2x1b2 = wavetable[y_index_next_][x_index_base_][octave_next_][waveIndex];
        uint16_t y2x2b1 = wavetable[y_index_next_][x_index_next_][octave_base_][waveIndex];
        uint16_t y2x2b2 = wavetable[y_index_next_][x_index_next_][octave_next_][waveIndex];

        uint32_t y1x1 = U16MixU32(y1x1b1, y1x1b2, SEMITONES_PER_OCTAVE, pitch_gradient_);
        uint32_t y1x2 = U16MixU32(y1x2b1, y1x2b2, SEMITONES_PER_OCTAVE, pitch_gradient_);
        uint32_t y2x1 = U16MixU32(y2x1b1, y2x1b2, SEMITONES_PER_OCTAVE, pitch_gradient_);
        uint32_t y2x2 = U16MixU32(y2x2b1, y2x2b2, SEMITONES_PER_OCTAVE, pitch_gradient_);

        uint32_t y1 = U32MixU32(y1x1, y1x2, WAVE_INTERPOLATE_DELTA_X, x_morph_);
        uint32_t y2 = U32MixU32(y2x1, y2x2, WAVE_INTERPOLATE_DELTA_X, x_morph_);

        *buffer_++ = (U32MixU32(y1, y2, WAVE_INTERPOLATE_DELTA_Y, y_morph_) / (kscale_osc_level_));
 */
        sync_in_++;
        sync_out_++;
        fm_buffer_++;
    }
}

void Oscillator::setXMorph(uint8_t input)
{
    x_morph_ = input % WAVE_INTERPOLATE_DELTA_X;
    x_index_base_ = input / WAVE_INTERPOLATE_DELTA_X;
    x_index_next_ = x_index_base_ + 1;
    if (x_index_next_ >= NUM_X_WAVETABLES)
    {
        x_index_next_ = 0;
    }
}

void Oscillator::setYMorph(uint8_t input)
{
    y_morph_ = input % WAVE_INTERPOLATE_DELTA_Y;
    y_index_base_ = input / WAVE_INTERPOLATE_DELTA_Y;
    y_index_next_ = y_index_base_ + 1;
    if (y_index_next_ >= NUM_Y_WAVETABLES)
    {
        y_index_next_ = 0;
    }
}

void Oscillator::reset()
{

    phase_ = 0;
}