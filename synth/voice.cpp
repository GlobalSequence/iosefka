#include "voice.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "resources.h"
#include "../common/test.h"
#include "../common/utilities.h"

bool Voice::lfo_reset_sync[4][2] {
    {false, false}, {true, false}, {false, true}, {true, true}
};

uint16_t Voice::lfo_div_val[] = {256, 192, 128, 96, 64, 48, 32, 24, 16, 12, 8, 6, 4, 3, 2, 1};

// void Voice::setup(synth_parameters_t *synth_patch, uint16_t *buffer_ptr, uint16_t random_seed) {

//     patch_ = synth_patch; 
//     buf_ptr_ = buffer_ptr;
//     seed_ = random_seed;

//     for(int i = 0; i < NUMBER_OF_ENVS; i++) {

//         EnvInit(&env_[i]);

//     }

//     for(int i = 0; i < NUMBER_OF_LFOS; i++) {

//         LFOSetup(&lfo_[i], random_seed);

//     }

// }

Voice::Voice(synth_parameters_t *synth_patch, uint16_t *buffer_ptr, uint16_t random_seed) {

    patch_ = synth_patch; 
    buf_ptr_ = buffer_ptr;
    seed_ = random_seed;

    for(int i = 0; i < NUMBER_OF_ENVS; i++) {

        EnvInit(&env_[i]);

    }

    for(int i = 0; i < NUMBER_OF_LFOS; i++) {

        LFOSetup(&lfo_[i], random_seed);

    }

}

void Voice::processAudioBlock() {

    loadSources();
    processModulation();
    updateDestinations();
    renderOscillators();

}

void Voice::triggerNoteOn(uint8_t note, uint8_t velocity, bool trigger) {

    pitch_target_ = note;
    uint8_t portamento = portamento_;
    int16_t delta = pitch_target_ - pitch_value_;
    int32_t increment = (res_env_portamento_increments[portamento >> 1]);
    pitch_increment_ = (delta * increment) >> 16;

    if (pitch_increment_ == 0) {

        if (delta < 0) {
            pitch_increment_ = -1;
        }
        else {
            pitch_increment_ = 1;
        }

    }

    for (int i = 0; i < NUMBER_OF_ENVS; i++) {

        EnvTrigger(ATTACK, &env_[i]);

    }

    for (int i = 0; i < NUMBER_OF_LFOS; i++) {

        LFOReset(&lfo_[i]);

    }

}

void Voice::triggerNoteOff(uint8_t note) {

    for (int i = 0; i < NUMBER_OF_ENVS; i++) {
        
        EnvTrigger(RELEASE, &env_[i]);

    }

}

void Voice::loadSources() {

    mod_source_[MOD_SRC_ENV_1] = EnvRender(&env_[0]) >> 4;
    mod_source_[MOD_SRC_ENV_2] = EnvRender(&env_[1]) >> 4;
    mod_source_[MOD_SRC_ENV_3] = EnvRender(&env_[2]) >> 4;
    mod_source_[MOD_SRC_LFO_1] = LFORender(&lfo_[0]);
    mod_source_[MOD_SRC_LFO_2] = LFORender(&lfo_[1]);
    mod_source_[MOD_SRC_LFO_3] = LFORender(&lfo_[2]);

    dst_[MOD_DST_VCA] = 0;

    dst_[MOD_DST_FILTER_CUTOFF] = patch_->filter.cutoff << 4;
    dst_[MOD_DST_FILTER_RESONANCE] = patch_->filter.resonance << 4;
    dst_[MOD_DST_FILTER_ENV_AMT] = patch_->filter.env_amt << 4;

    dst_[MOD_DST_OSC_1_PITCH] = 2048;
    dst_[MOD_DST_OSC_2_PITCH] = 2048;

    dst_[MOD_DST_OSC_1_OCTAVE] = patch_->osc[0].octave << 4;
    dst_[MOD_DST_OSC_2_OCTAVE] = patch_->osc[1].octave << 4;

    dst_[MOD_DST_OSC_1_X_MORPH] = patch_->osc[0].x_morph << 4;
    dst_[MOD_DST_OSC_1_Y_MORPH] = patch_->osc[0].y_morph << 4;

    dst_[MOD_DST_OSC_2_X_MORPH] = patch_->osc[1].x_morph << 4;
    dst_[MOD_DST_OSC_2_Y_MORPH] = patch_->osc[1].y_morph << 4;

    dst_[MOD_DST_PORTAMENTO] = patch_->shared.portamento << 4;
    dst_[MOD_DST_PHASE_MOD] = PhaseModSmooth.getAverage(patch_->shared.phase_mod) << 4;
    dst_[MOD_DST_SYNC] = patch_->shared.sync << 4;

    dst_[MOD_DST_MIX_OSC_1_MIX] = patch_->mixer.osc_1_mix << 4;
    dst_[MOD_DST_MIX_OSC_2_MIX] = patch_->mixer.osc_2_mix << 4;
    dst_[MOD_DST_MIX_NOISE_MIX] = patch_->mixer.noise_mix << 4;

    int16_t *envelope_parameters = &dst_[MOD_DST_ENV_1_ATTACK];
    for (uint8_t i = 0; i < NUMBER_OF_ENVS; ++i)
    {
        *envelope_parameters++ = patch_->env[i].attack << 4;
        *envelope_parameters++ = patch_->env[i].decay << 4;
        *envelope_parameters++ = patch_->env[i].sustain << 4;
        *envelope_parameters++ = patch_->env[i].release << 4;
        envelope_parameters += 1;
    }


    int16_t *lfo_parameters = &dst_[MOD_DST_LFO_1_RATE];
    for (uint8_t i = 0; i < NUMBER_OF_ENVS; ++i)
    {
        *lfo_parameters++ = patch_->lfo[i].rate << 4;
        *lfo_parameters++ = patch_->lfo[i].shape << 4;
        *lfo_parameters++ = patch_->lfo[i].phase << 4;
        lfo_parameters += 2;
    }

    int16_t *mod_amount = &dst_[MOD_DST_ENV_1_AMOUNT];
    for (uint8_t i = 0; i < NUMBER_OF_ENVS + NUMBER_OF_LFOS; i++)
    {
        *mod_amount = patch_->mod[i].amount << 4;
        mod_amount += 5;
    }

    dst_[MOD_DST_DELAY_AMOUNT] = patch_->delay.amount << 4;
    dst_[MOD_DST_DELAY_RATE] = patch_->delay.rate << 4;
    dst_[MOD_DST_DELAY_FEEDBACK] = patch_->delay.feedback << 4;
    dst_[MOD_DST_DELAY_FILTER] = patch_->delay.filter << 4;
    // dst_[MOD_DST_DELAY_STEREO] = patch_->delay.stereo << 4;

}

void Voice::processModulation()
{
    for (int i = 0; i < NUM_MOD_SOURCES; i++)
    {
        // change velocity to non-bipolar parameter
        int16_t amount = bipolarParameter(patch_->mod[i].amount) << 1;

        int16_t source = mod_source_[i];
        uint8_t destination = U8CLIPMAX(NUM_MOD_DESTINATIONS, patch_->mod[i].destination);
        int16_t modulation = dst_[destination];

        if (i >= MOD_SRC_LFO_1 && i <= MOD_SRC_LFO_3)
        {
            modulation += S8S16MULSHIFT8(source - 2048, amount);
        }
        else
        {
            modulation += U8S16MULSHIFT8(source, amount);
        }

        dst_[destination] = U12CLIPMAXMIN(modulation);
    }
}

void Voice::updateDestinations()
{
    // int16_t VCAModulation = dst_[MOD_DST_VCA];
    // int16_t VCACVSource = mod_source_[MOD_SRC_ENV_1];
    // VCAModulation += U8S16MULSHIFT8(VCACVSource, volume_);
    // dst_[MOD_DST_VCA] = U16CLIPMAXMINVAR(volume_ << 4, VCAModulation);
    //dst_[MOD_DST_VCA] += mod_source_[MOD_SRC_ENV_1];
    dst_[MOD_DST_VCA] += mod_source_[MOD_SRC_ENV_1];
    vca_ = U12CLIPMAXMIN(dst_[MOD_DST_VCA]);

    int16_t cutoffModulation = dst_[MOD_DST_FILTER_CUTOFF];
    int16_t cutoffCVSource = mod_source_[MOD_SRC_ENV_2];
    int16_t cutoffEnvAmount = bipolarParameter(dst_[MOD_DST_FILTER_ENV_AMT] >> 4) << 1;
    // cutoffModulation += U8S16MULSHIFT8(cutoffCVSource, dst_[MOD_DST_FILTER_ENV_AMT] >> 4);
    cutoffModulation += U8S16MULSHIFT8(cutoffCVSource, cutoffEnvAmount);
    dst_[MOD_DST_FILTER_CUTOFF] = U12CLIPMAXMIN(cutoffModulation);
    cutoff_ = dst_[MOD_DST_FILTER_CUTOFF] >> 1;

    //dst_[MOD_DST_FILTER_RESONANCE] += mod_source_[MOD_SRC_ENV_1];
    resonance_ = U12CLIPMAXMIN(dst_[MOD_DST_FILTER_RESONANCE]) >> 1;

    osc_pitch_mod_[0] = (dst_[MOD_DST_OSC_1_PITCH] - 2048) << 3;
    //printf("%d\n", osc_pitch_mod_[0]);
    osc_pitch_mod_[1] = (dst_[MOD_DST_OSC_2_PITCH] - 2048) << 3;
    osc_scale_[0] = (bipolarParameter(patch_->osc[0].scale) * 2) * 12;
    osc_scale_[1] = (bipolarParameter(patch_->osc[1].scale) * 2) * 12;
    osc_octave_[0] = (dst_[MOD_DST_OSC_1_OCTAVE] >> 10);
    osc_octave_[1] = (dst_[MOD_DST_OSC_2_OCTAVE] >> 10);

    osc_[0].setXMorph(dst_[MOD_DST_OSC_1_X_MORPH] >> 4);
    osc_[0].setYMorph(dst_[MOD_DST_OSC_1_Y_MORPH] >> 4);
    osc_[1].setXMorph(dst_[MOD_DST_OSC_2_X_MORPH] >> 4);
    osc_[1].setYMorph(dst_[MOD_DST_OSC_2_Y_MORPH] >> 4);

    portamento_ = dst_[MOD_DST_PORTAMENTO] >> 4;
    phase_mod_ = dst_[MOD_DST_PHASE_MOD] >> 4;
    sync_ = dst_[MOD_DST_SYNC] >> 11;

    osc_1_mix_ = dst_[MOD_DST_MIX_OSC_1_MIX] >> 4;
    osc_2_mix_ = dst_[MOD_DST_MIX_OSC_2_MIX] >> 4;
    noise_mix_ = dst_[MOD_DST_MIX_NOISE_MIX] >> 4;

    int16_t *envelope_parameters = &dst_[MOD_DST_ENV_1_ATTACK];
    for (int i = 0; i < NUMBER_OF_ENVS; ++i)
    {
        EnvUpdate(
            envelope_parameters[0] >> 5,
            envelope_parameters[1] >> 5,
            envelope_parameters[2] >> 5,
            envelope_parameters[3] >> 5,
            &env_[i]            
        );
        envelope_parameters += 5;
    }

    int16_t *lfo_parameters = &dst_[MOD_DST_LFO_1_RATE];
    for (int i = 0; i < NUMBER_OF_LFOS; ++i)
    {
        uint8_t lfo_sync_reset_state = patch_->lfo[i].sync >> 6;
        bool lfo_reset = lfo_reset_sync[lfo_sync_reset_state][0];
        bool lfo_sync = lfo_reset_sync[lfo_sync_reset_state][1];
        // lfo_[i].setSync(lfo_sync);
        // lfo_[i].setReset(lfo_reset);
        LFOSetSync(&lfo_[i], lfo_sync);
        LFOSetReset(&lfo_[i], lfo_reset);

        // uint16_t sync_div_val = lfo_div_val[lfo_parameters[0] >> 8];
        // uint32_t lfo_sync_inc = 
        //     ((klfo_sync_tick_dividend/(state.tick_period * sync_div_val)) << 16)/klfo_sync_divisor;
        uint32_t lfo_sync_inc = 0;

            // (60 seconds in us / (tick period * division of period) << 16 bit) / update frequency in HZ
            // It can be thought of like this:
            // say the value being shifted by 16 bits is 1 (1 << 16)...
            // at the update rate defined by the sync divisor, a total cycle will take 1 second
            // anything greater than 1 << 16 is less than a second, where anything less is more than a second
            // ... klfo_syn_tick_dividend should be called something more like kone_second_in_us_times_100 or sth
        //if(i == 0) printf("%d, %d, %d\t", lfo_[0].getSync(), lfo_sync_inc, sync_div_val);

        // lfo_[i].Update(
        //     lfo_[i].getSync() == false? lfo_parameters[0] + 16 : lfo_sync_inc,
        //     scaleParam0to5(lfo_parameters[1] >> 4),
        //     lfo_parameters[2] >> 4);
        LFOUpdate(
            &lfo_[i], 
            LFOGetSync(&lfo_[i]) == false? lfo_parameters[0] + 16 : lfo_sync_inc,
            scaleParam0to5(lfo_parameters[1] >> 4),
            lfo_parameters[2] >> 4,
            0,
            0
        );
        lfo_parameters += 5;
    }

    d_amount_ = dst_[MOD_DST_DELAY_AMOUNT] >> 4;
    d_rate_ = dst_[MOD_DST_DELAY_RATE] >> 4;
    d_feedback_ = dst_[MOD_DST_DELAY_FEEDBACK] >> 4;
    d_saturation_ = dst_[MOD_DST_DELAY_FILTER] >> 4;
    d_stereo_ = dst_[MOD_DST_DELAY_STEREO] >> 4;

}

void Voice::renderOscillators()
{
    int16_t base_pitch = pitch_value_ + pitch_increment_;
    if ((pitch_increment_ > 0) ^ (base_pitch < pitch_target_))
    {
        base_pitch = pitch_target_;
        pitch_increment_ = 0;
    }
    pitch_value_ = base_pitch;
    pitch_ = base_pitch << 8;

    for (int i = 0; i < NUMBER_OF_OSCS; i++)
    {
        //int16_t oscPitch = pitch_ + osc_pitch_[i];
        int16_t oscPitch = pitch_ + osc_scale_[i] + osc_pitch_mod_[i];
        int16_t coarse = (oscPitch >> 8) + koctave_offset_[osc_octave_[i]] + knote_base_offset_;
        if(coarse >= OSC_INCREMENTS_MAX - 1) coarse = OSC_INCREMENTS_MAX - 1;
        if(coarse <= 0) coarse = 0;
        uint16_t fine = oscPitch & 0xFF;
        uint8_t octave = (coarse / SEMITONES_PER_OCTAVE);
        uint32_t i1 = osc_increments[coarse];
        uint32_t i2 = osc_increments[coarse + 1];
        uint32_t increment = (((i2 - i1) * fine) >> 8) + i1;
        uint8_t pitch_gradient_ = coarse % SEMITONES_PER_OCTAVE; 

        if (i == 0)
        {
            osc_[0].render(increment, octave, pitch_gradient_, dummy_phase_mod_buffer_, 0, no_sync_, sync_buffer_, osc_1_buffer_);
        }
        else
        {
            bool* sync_select_;
            sync_select_ = sync_ == true? sync_buffer_ : no_sync_;
            osc_[1].render(increment, octave, pitch_gradient_, osc_1_buffer_, phase_mod_, sync_select_ /* sync_buffer_ */ /* no_sync_ */, dummy_sync_buffer_, osc_2_buffer_);
        }
    }
    
    uint16_t dac_index_start_point_ = dac_osc_buffer_half_start_point_[dac_osc_buffer_half_to_fill_];
    // uint16_t* dac_osc_buffer_ptr_ = &dac_output_buf[dac_index_start_point_];
    for(int i = 0; i < AUDIO_BLOCK_SIZE; i++) {

        XOR798(&seed_);

        uint16_t noise_sample_ = seed_ >> 4;

        uint16_t osc_1_mixed_ = U16U8MULSHIFT8(osc_1_buffer_[i], osc_1_mix_);
        uint16_t osc_2_mixed_ = U16U8MULSHIFT8(osc_2_buffer_[i], osc_2_mix_);
        uint16_t noise_mixed_ = U16U8MULSHIFT8(noise_sample_, noise_mix_);

        uint16_t dac_osc_audio_ = (osc_1_mixed_ + osc_2_mixed_ + noise_mixed_) / 3;

        seed_ -= dac_osc_audio_;

        // buf_ptr_[i] = 0;
        buf_ptr_[i] = dac_osc_audio_;

        // TODO: CONSIDER ADDING THE PART WHERE THE BUFFER IS FILLED 
        // AS A FUNCTION TO BUFFERS.H TO KEEP SEPARATION OF CONCERNS        

        // *dac_osc_buffer_ptr_++ = DAC_config_chan_A | dac_osc_audio_ & 0xFFF;   

        // *dac_osc_buffer_ptr_++ = DAC_config_chan_B | mod_source_[MOD_SOURCE_ENV_1] & 0xFFF;
    }

}