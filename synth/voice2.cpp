#include "voice2.h"
#include "resources.h"
#include "../common/globals.h"
#include "../common/utilities.h"

#include "../common/test.h"

Voice2::Voice2(uint16_t *buffer_ptr, uint16_t random_seed) {

    buf_ptr_ = buffer_ptr;
    
    seed_ = random_seed;

}

void Voice2::renderOscillators()
{

    osc_[0].setXMorph(p.x_morph[0]);
    osc_[0].setYMorph(p.y_morph[0]);
    osc_[1].setXMorph(p.x_morph[1]);
    osc_[1].setYMorph(p.y_morph[1]);

    if(p.osc_reset) {
        osc_[0].reset();
        osc_[1].reset();
    }

    int16_t base_pitch = p.pitch_value + p.pitch_increment;
    if ((p.pitch_increment > 0) ^ (base_pitch < p.pitch_target))
    {
        base_pitch = p.pitch_target;
        p.pitch_increment = 0;
    }
    p.pitch_value = base_pitch;
    // uint16_t pitch = base_pitch << 8;
    uint16_t pitch = base_pitch;

    for (int i = 0; i < NUMBER_OF_OSCS; i++)
    {

        int16_t oscPitch = pitch + p.osc_scale[i] + p.osc_pitch_mod[i];
        int16_t coarse = (oscPitch >> 8) + koctave_offset_[p.osc_octave[i]] + knote_base_offset_;
        if(coarse >= OSC_INCREMENTS_MAX - 1) coarse = OSC_INCREMENTS_MAX - 1;
        if(coarse <= 0) coarse = 0;
        uint16_t fine = oscPitch & 0xFF;
        uint8_t octave = (coarse / SEMITONES_PER_OCTAVE);
        uint32_t i1 = osc_increments[coarse];
        uint32_t i2 = osc_increments[coarse + 1];
        uint32_t increment = (((i2 - i1) * fine) >> 8) + i1;
        uint8_t pitch_gradient = coarse % SEMITONES_PER_OCTAVE; 

        if (i == 0)
        {
            osc_[0].render(increment, octave, pitch_gradient, dummy_phase_mod_buffer_, 0, no_sync_, sync_buffer_, osc_1_buffer_);
        }
        else
        {
            bool* sync_select;
            sync_select = p.sync == true? sync_buffer_ : no_sync_;
            osc_[1].render(increment, octave, pitch_gradient, osc_1_buffer_, p.phase_mod, sync_select, dummy_sync_buffer_, osc_2_buffer_);
        }
    }
    

    for(int i = 0; i < AUDIO_BLOCK_SIZE; i++) {

        XOR798(&seed_);

        uint16_t noise_sample = seed_ >> 4;

        uint16_t osc_1_mixed = U16U8MULSHIFT8(osc_1_buffer_[i], p.osc_1_mix);
        uint16_t osc_2_mixed = U16U8MULSHIFT8(osc_2_buffer_[i], p.osc_2_mix);
        uint16_t noise_mixed = U16U8MULSHIFT8(noise_sample, p.noise_mix);

        seed_ -= osc_1_buffer_[i];

        // wavetable test
        uint32_t osc_mix = (osc_1_mixed + osc_2_mixed) >> 4;

        uint16_t generator_mix = (osc_mix + noise_mixed) / 3;

        // bitrate reduction effect
        uint16_t effect_1_mix = (generator_mix >> p.brr) << p.brr;

        // buf_ptr_[i] = effect_1_mix;

        // samplerate reduction effect
        srr_count++;
        if(srr_count >= p.srr) {
            srr_count = 0;
            srr_sample = effect_1_mix;
        }
        uint16_t effect_2_mix = srr_sample;

        buf_ptr_[i] = effect_2_mix;


/*         uint16_t dac_osc_audio_ = (osc_mix + noise_mixed) / 3;

        buf_ptr_[i] = dac_osc_audio_; */

    }

}