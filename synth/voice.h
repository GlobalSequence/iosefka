#ifndef VOICE_H
#define VOICE_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/globals.h"
#include "../common/moving_average.h"
#include "../synth/patch.h"
#include "../synth/envelope.h"
#include "../synth/lfo.h"
#include "../synth/oscillator.h"

class Voice {
    public:
        // Voice(){};
        // void setup(synth_parameters_t *synth_patch, uint16_t *buffer_ptr, uint16_t random_seed);
        Voice(synth_parameters_t *synth_patch, uint16_t *buffer_ptr, uint16_t random_seed); 
        void triggerNoteOn(uint8_t note, uint8_t velocity, bool trigger = 1);
        void triggerNoteOff(uint8_t note);
        void processAudioBlock();

        inline uint16_t getCutoff() {return cutoff_;}
        inline uint16_t getResonance() {return resonance_;}

        inline uint16_t getDelayAmount() {return d_amount_;}
        inline uint16_t getDelayRate() {return d_rate_;}
        inline uint16_t getDelayFeedback() {return d_feedback_;}
        inline uint16_t getDelaySaturation() {return d_saturation_;}
        inline uint16_t getDelayStereo() {return d_stereo_;}
        inline uint16_t getDelayPingPong() {return 0;}

    private:
        void loadSources();
        void processModulation();
        void updateDestinations();  
        void renderOscillators();

        synth_parameters_t *patch_;
        uint16_t *buf_ptr_;

        Oscillator test_osc_;
        Oscillator osc_[NUMBER_OF_OSCS];
        
        envelope_t env_[NUMBER_OF_ENVS];
        lfo_t lfo_[NUMBER_OF_LFOS];

        // Envelope env_[NUMBER_OF_ENVS];
        // LFO lfo_[NUMBER_OF_LFOS];

        const int8_t koctave_offset_[4] = {-12, 0, 12, 24};
        const uint8_t knote_base_offset_ = 12;

        bool dac_osc_buffer_half_to_fill_;
        uint16_t dac_osc_buffer_half_start_point_[2] = {0, DAC_SAMPLES_PER_AUDIO_BLOCK};

        uint16_t portamento_;
        int16_t pitch_value_;
        int16_t pitch_target_ = 64;
        int16_t pitch_increment_;
        uint16_t pitch_;
        int16_t osc_pitch_mod_[NUMBER_OF_OSCS];
        int16_t osc_scale_[NUMBER_OF_OSCS];
        int8_t osc_octave_[NUMBER_OF_OSCS];
        uint16_t pitch_bend_ = 8192;

        uint8_t phase_mod_;
        MovingAverage32 PhaseModSmooth;
        bool sync_;

        uint32_t osc_1_buffer_[AUDIO_BLOCK_SIZE];
        uint32_t osc_2_buffer_[AUDIO_BLOCK_SIZE];
        uint32_t dummy_phase_mod_buffer_[AUDIO_BLOCK_SIZE];
        bool no_sync_[AUDIO_BLOCK_SIZE];
        bool sync_buffer_[AUDIO_BLOCK_SIZE];
        bool dummy_sync_buffer_[AUDIO_BLOCK_SIZE];

        uint8_t osc_1_mix_;
        uint8_t osc_2_mix_;
        uint8_t noise_mix_;

        uint16_t seed_ = 0xFC25;

        uint16_t mod_source_[NUM_MOD_SOURCES];
        int16_t dst_[NUM_MOD_DESTINATIONS];
        int8_t mod_destination[NUM_MOD_DESTINATIONS];

        uint16_t vca_;
        uint16_t cutoff_;
        uint16_t resonance_;

        static const uint klfo_sync_div_headroom = 100;
        static const uint klfo_sync_divisor = AUDIO_RENDER_RATE_KHZ * klfo_sync_div_headroom;
        static const uint klfo_sync_tick_dividend = 1000000 * klfo_sync_div_headroom;
        static bool lfo_reset_sync[4][2];
        static uint16_t lfo_div_val[16];

        // delay params
        uint8_t d_amount_;
        uint8_t d_rate_;
        uint8_t d_feedback_;
        uint8_t d_saturation_;
        uint8_t d_stereo_;
    
};

#endif
