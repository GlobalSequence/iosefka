#ifndef MOD_MATRIX_H
#define MOD_MATRIX_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "voice2.h"
#include "cv.h"
#include "delay.h"
#include "patch.h"
#include "envelope.h"
#include "lfo.h"
#include "../common/globals.h"
#include "../common/moving_average.h"

class ModMatrix {

    public:
        ModMatrix(
            synth_parameters_t      *synth_params, 
            external_modulation_t   *ext_mod,
            voice_inputs_t          *voice_inputs,
            delay_inputs_t          *delay_inputs, 
            cv_outputs_t            *cv_outputs,
            uint16_t random_seed
        );

        void triggerNoteOn(uint8_t note, uint8_t velocity, bool trigger = 1);
        void triggerNoteOff(uint8_t note);
        void process();

    private:
        void loadSources();
        void processModulation();
        void updateDestinations();  

        synth_parameters_t      *patch_;
        external_modulation_t   *ext_mod_;
        voice_inputs_t          *v_i;
        delay_inputs_t          *d_i;
        cv_outputs_t            *c_o;

        envelope_t env_[NUMBER_OF_ENVS];
        lfo_t lfo_[NUMBER_OF_LFOS];

        MovingAverage32 PhaseModSmooth;

        uint16_t mod_source_[NUM_MOD_SOURCES];
        uint16_t mod_amount_[NUM_MOD_SOURCES];
        int16_t dst_[NUM_MOD_DESTINATIONS];
        int8_t mod_destination[NUM_MOD_DESTINATIONS];
        uint8_t velocity_;

        static const uint klfo_sync_div_headroom = 100;
        static const uint klfo_sync_divisor = AUDIO_RENDER_RATE_KHZ * klfo_sync_div_headroom;
        static const uint klfo_sync_tick_dividend = 1000000 * klfo_sync_div_headroom;
        static bool lfo_reset_sync[4][2];
        static uint16_t lfo_div_val[16];
        uint16_t lfo_tick_count[NUMBER_OF_LFOS];

        bool vca_drone_mode_;
        uint16_t seed_;

};

#endif