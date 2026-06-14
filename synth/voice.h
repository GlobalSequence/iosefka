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
// ---
// Much of the code in this file is borrowed from Emilie Gillet's Shruthi-1. Thanks Emilie!

#ifndef Voice_H
#define Voice_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/globals.h"
#include "../synth/patch.h"
#include "../synth/oscillator.h"

typedef struct voice_inputs_t {

    uint16_t portamento;
    int16_t pitch_value;
    int16_t pitch_target = 64;
    int16_t pitch_increment;
    int16_t osc_pitch_mod[NUMBER_OF_OSCS];
    int16_t osc_scale[NUMBER_OF_OSCS];
    int8_t osc_octave[NUMBER_OF_OSCS];
    uint16_t pitch_bend = 8192;
    uint8_t x_morph[NUMBER_OF_OSCS];
    uint8_t y_morph[NUMBER_OF_OSCS];
    uint8_t phase_mod;
    bool sync;
    uint8_t osc_1_mix;
    uint8_t osc_2_mix;
    uint8_t noise_mix;
    uint8_t srr;
    uint8_t brr;
    bool osc_reset;
    bool duophony;

} voice_inputs_t;

class Voice {

    public:
        Voice(uint16_t *buffer_ptr, uint16_t random_seed);
        void renderOscillators();

        voice_inputs_t* getParamPointer() { return &p; }
        void getBassNote(uint8_t note) { bass_note_ = note << 8; }

        bool* getSyncBuffer() { return sync_buffer_; }

    private:

        uint16_t *buf_ptr_;

        Oscillator osc_[NUMBER_OF_OSCS];

        voice_inputs_t p;

        const int8_t koctave_offset_[4] = {-12, 0, 12, 24};
        const uint8_t knote_base_offset_ = 15;

        uint32_t osc_1_buffer_[AUDIO_BLOCK_SIZE];
        uint32_t osc_2_buffer_[AUDIO_BLOCK_SIZE];
        uint32_t dummy_phase_mod_buffer_[AUDIO_BLOCK_SIZE];
        bool no_sync_[AUDIO_BLOCK_SIZE];
        bool sync_buffer_[AUDIO_BLOCK_SIZE];
        bool dummy_sync_buffer_[AUDIO_BLOCK_SIZE];

        int16_t last_coarse[NUMBER_OF_OSCS];

        uint16_t srr_sample_;
        uint srr_count_ = 0;

        uint16_t bass_note_;

        uint16_t seed_ = 0xFC25;

};


#endif