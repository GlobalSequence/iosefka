#ifndef VOICE2_H
#define VOICE2_H

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
    bool osc_reset;

} voice_inputs_t;

class Voice2 {

    public:
        Voice2(uint16_t *buffer_ptr, uint16_t random_seed);
        void renderOscillators();

        voice_inputs_t* getParamPointer() { return &p; }

    private:

        uint16_t *buf_ptr_;

        Oscillator osc_[NUMBER_OF_OSCS];

        voice_inputs_t p;

        const int8_t koctave_offset_[4] = {-12, 0, 12, 24};
        const uint8_t knote_base_offset_ = 12;

        uint32_t osc_1_buffer_[AUDIO_BLOCK_SIZE];
        uint32_t osc_2_buffer_[AUDIO_BLOCK_SIZE];
        uint32_t dummy_phase_mod_buffer_[AUDIO_BLOCK_SIZE];
        bool no_sync_[AUDIO_BLOCK_SIZE];
        bool sync_buffer_[AUDIO_BLOCK_SIZE];
        bool dummy_sync_buffer_[AUDIO_BLOCK_SIZE];

        uint16_t seed_ = 0xFC25;

};


#endif