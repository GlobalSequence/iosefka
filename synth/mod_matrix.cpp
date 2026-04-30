#include "mod_matrix.h"

bool ModMatrix::lfo_reset_sync[4][2] {
    {false, false}, {true, false}, {false, true}, {true, true}
};

// uint16_t ModMatrix::clock_sync_div_val[] = {256, 192, 128, 96, 64, 48, 32, 24, 16, 12, 8, 6, 4, 3, 2, 1};
uint16_t ModMatrix::clock_sync_div_val[] = {576, 288, 192, 96, 72, 48, 36, 24, 18, 12, 9, 6, 4, 3, 2, 1};

ModMatrix::ModMatrix(
    synth_parameters_t      *synth_params,
    external_modulation_t   *ext_mod, 
    voice_inputs_t          *voice_inputs,
    delay_inputs_t          *delay_inputs, 
    cv_outputs_t            *cv_outputs,
    uint16_t random_seed
) {

    v_i = voice_inputs;
    d_i = delay_inputs;
    c_o = cv_outputs;

    patch_ = synth_params; 
    ext_mod_ = ext_mod;
    seed_ = random_seed;


    for(int i = 0; i < NUMBER_OF_ENVS; i++) {

        env_[i].Init();

    }

    for(int i = 0; i < NUMBER_OF_LFOS; i++) {

        LFOSetup(&lfo_[i], random_seed);

    }

}

void ModMatrix::process() {

    loadSources();
    processModulation();
    updateDestinations();

}

void ModMatrix::triggerNoteOn(uint8_t note, uint8_t velocity, bool trigger) {

    v_i->pitch_target = note << 8;
    uint8_t portamento = v_i->portamento;
    int32_t delta = v_i->pitch_target - v_i->pitch_value;
    int32_t increment = (res_env_portamento_increments[portamento >> 1]);
    v_i->pitch_increment = (delta * increment) >> 16;

    if (v_i->pitch_increment == 0) {

        if (delta < 0) {
            v_i->pitch_increment = -1;
        }
        else {
            v_i->pitch_increment = 1;
        }

    }

    if(trigger) {

        for (int i = 0; i < NUMBER_OF_ENVS; i++) {

            env_[i].Trigger(ENV_SEGMENT_ATTACK);

        }
    
        for (int i = 0; i < NUMBER_OF_LFOS; i++) {
    
            LFOReset(&lfo_[i]);
            if(lfo_[i].reset_) lfo_tick_count[i] = 0;
    
        }

    }

    velocity_ = velocity;

}

void ModMatrix::triggerNoteOff(uint8_t note) {

    for (int i = 0; i < NUMBER_OF_ENVS; i++) {

        env_[i].Trigger(ENV_SEGMENT_RELEASE);

    }

}

void ModMatrix::loadSources() {

    mod_source_[MOD_SRC_ENV_1] = env_[0].Render() >> 4;
    mod_source_[MOD_SRC_ENV_2] = env_[1].Render() >> 4;
    mod_source_[MOD_SRC_ENV_3] = env_[2].Render() >> 4;
    mod_source_[MOD_SRC_LFO_1] = LFORender(&lfo_[0]);
    mod_source_[MOD_SRC_LFO_2] = LFORender(&lfo_[1]);
    mod_source_[MOD_SRC_LFO_3] = LFORender(&lfo_[2]);

    // velocity is 8 bits internally, so no shifting needs to be done
    mod_source_[MOD_SRC_VELOCITY]   = velocity_ << 4;
    mod_source_[MOD_SRC_MOD_WHEEL]  = ext_mod_->mod_wheel_amt << 5;
    mod_source_[MOD_SRC_AFTERTOUCH] = ext_mod_->aftertouch_amt << 5;
    mod_source_[MOD_SRC_BREATH] = ext_mod_->breath_amt << 5;
    mod_source_[MOD_SRC_NOTE] = v_i->pitch_value >> 3;

    vca_drone_mode_ = patch_->vca.level;

    dst_[MOD_DST_VCA] = vca_drone_mode_ == false? 0 : patch_->vca.level << 4;

    dst_[MOD_DST_FILTER_CUTOFF] = patch_->filter.cutoff << 4;
    dst_[MOD_DST_FILTER_RESONANCE] = patch_->filter.resonance << 4;
    dst_[MOD_DST_FILTER_ENV_AMT] = patch_->filter.env_amt << 4;
    dst_[MOD_DST_FILTER_TYPE] = patch_->filter.type << 4;
    dst_[MOD_DST_FILTER_FM] =  patch_->filter.fm << 4;
    dst_[MOD_DST_FILTER_TRACKING] = patch_->filter.tracking << 4;

    dst_[MOD_DST_OSC_1_PITCH] = 2048;
    dst_[MOD_DST_OSC_2_PITCH] = 2048;
    dst_[MOD_DST_OSC_BOTH_PITCH] = 2048;
    dst_[MOD_DST_OSC_1_PITCH_FINE] = 0;
    dst_[MOD_DST_OSC_2_PITCH_FINE] = 0;
    dst_[MOD_DST_OSC_BOTH_PITCH_FINE] = 0;

    dst_[MOD_DST_OSC_1_OCTAVE] = patch_->osc[0].octave << 4;
    dst_[MOD_DST_OSC_2_OCTAVE] = patch_->osc[1].octave << 4;
    dst_[MOD_DST_OSC_BOTH_OCTAVE] = 2048;

    dst_[MOD_DST_OSC_1_X_MORPH] = patch_->osc[0].x_morph << 4;
    dst_[MOD_DST_OSC_1_Y_MORPH] = patch_->osc[0].y_morph << 4;
    dst_[MOD_DST_OSC_BOTH_X_MORPH] = 2048;

    dst_[MOD_DST_OSC_2_X_MORPH] = patch_->osc[1].x_morph << 4;
    dst_[MOD_DST_OSC_2_Y_MORPH] = patch_->osc[1].y_morph << 4;
    dst_[MOD_DST_OSC_BOTH_Y_MORPH] = 2048;    

    dst_[MOD_DST_PORTAMENTO] = patch_->shared.portamento << 4;
    dst_[MOD_DST_PHASE_MOD] = PhaseModSmooth.getAverage(patch_->shared.phase_mod) << 4;
    dst_[MOD_DST_SYNC] = patch_->shared.sync << 4;

    dst_[MOD_DST_MIX_OSC_1_MIX] = patch_->mixer.osc_1_mix << 4;
    dst_[MOD_DST_MIX_OSC_2_MIX] = patch_->mixer.osc_2_mix << 4;
    dst_[MOD_DST_MIX_NOISE_MIX] = patch_->mixer.noise_mix << 4;

    dst_[MOD_DST_ENV_1_ATTACK] =    patch_->env[0].attack << 4;
    dst_[MOD_DST_ENV_1_DECAY] =     patch_->env[0].decay << 4;
    dst_[MOD_DST_ENV_1_SUSTAIN] =   patch_->env[0].sustain << 4;
    dst_[MOD_DST_ENV_1_RELEASE] =   patch_->env[0].release << 4;
    dst_[MOD_DST_ENV_1_LOOP] =      patch_->env[0].loop << 4;

    dst_[MOD_DST_ENV_2_ATTACK] =    patch_->env[1].attack << 4;
    dst_[MOD_DST_ENV_2_DECAY] =     patch_->env[1].decay << 4;
    dst_[MOD_DST_ENV_2_SUSTAIN] =   patch_->env[1].sustain << 4;
    dst_[MOD_DST_ENV_2_RELEASE] =   patch_->env[1].release << 4;
    dst_[MOD_DST_ENV_2_LOOP] =      patch_->env[1].loop << 4;

    dst_[MOD_DST_ENV_3_ATTACK] =    patch_->env[2].attack << 4;
    dst_[MOD_DST_ENV_3_DECAY] =     patch_->env[2].decay << 4;
    dst_[MOD_DST_ENV_3_SUSTAIN] =   patch_->env[2].sustain << 4;
    dst_[MOD_DST_ENV_3_RELEASE] =   patch_->env[2].release << 4;
    dst_[MOD_DST_ENV_3_LOOP] =      patch_->env[2].loop << 4;

    // int16_t *lfo_parameters = &dst_[MOD_DST_LFO_1_RATE];
    // for (uint8_t i = 0; i < NUMBER_OF_LFOS; ++i)
    // {
    //     *lfo_parameters++ = patch_->lfo[i].rate << 4;
    //     *lfo_parameters++ = patch_->lfo[i].shape << 4;
    //     *lfo_parameters++ = patch_->lfo[i].phase << 4;
    //     *lfo_parameters++ = patch_->lfo[i].sync << 4;
    //     lfo_parameters += 1;
    // }

    dst_[MOD_DST_LFO_1_RATE] = patch_->lfo[0].rate << 4;
    dst_[MOD_DST_LFO_1_SHAPE] = patch_->lfo[0].shape << 4;
    dst_[MOD_DST_LFO_1_PHASE] = patch_->lfo[0].phase << 4;
    dst_[MOD_DST_LFO_1_SYNC] = patch_->lfo[0].sync << 4;

    dst_[MOD_DST_LFO_2_RATE] = patch_->lfo[1].rate << 4;
    dst_[MOD_DST_LFO_2_SHAPE] = patch_->lfo[1].shape << 4;
    dst_[MOD_DST_LFO_2_PHASE] = patch_->lfo[1].phase << 4;
    dst_[MOD_DST_LFO_2_SYNC] = patch_->lfo[1].sync << 4;

    dst_[MOD_DST_LFO_3_RATE] = patch_->lfo[2].rate << 4;
    dst_[MOD_DST_LFO_3_SHAPE] = patch_->lfo[2].shape << 4;
    dst_[MOD_DST_LFO_3_PHASE] = patch_->lfo[2].phase << 4;
    dst_[MOD_DST_LFO_3_SYNC] = patch_->lfo[2].sync << 4;

    dst_[MOD_DST_ENV_1_AMOUNT] = patch_->mod[MOD_SRC_ENV_1].amount << 4;
    dst_[MOD_DST_ENV_2_AMOUNT] = patch_->mod[MOD_SRC_ENV_2].amount << 4;
    dst_[MOD_DST_ENV_3_AMOUNT] = patch_->mod[MOD_SRC_ENV_3].amount << 4;
    dst_[MOD_DST_LFO_1_AMOUNT] = patch_->mod[MOD_SRC_LFO_1].amount << 4;
    dst_[MOD_DST_LFO_2_AMOUNT] = patch_->mod[MOD_SRC_LFO_2].amount << 4;
    dst_[MOD_DST_LFO_3_AMOUNT] = patch_->mod[MOD_SRC_LFO_3].amount << 4;

    dst_[MOD_DST_ENV_1_DST] = patch_->mod[MOD_SRC_ENV_1].destination << 5;
    dst_[MOD_DST_ENV_2_DST] = patch_->mod[MOD_SRC_ENV_2].destination << 5;
    dst_[MOD_DST_ENV_3_DST] = patch_->mod[MOD_SRC_ENV_3].destination << 5;
    dst_[MOD_DST_LFO_1_DST] = patch_->mod[MOD_SRC_LFO_1].destination << 5;
    dst_[MOD_DST_LFO_2_DST] = patch_->mod[MOD_SRC_LFO_2].destination << 5;
    dst_[MOD_DST_LFO_3_DST] = patch_->mod[MOD_SRC_LFO_3].destination << 5;

    dst_[MOD_DST_DELAY_AMOUNT] = patch_->delay.amount << 4;
    dst_[MOD_DST_DELAY_RATE] = patch_->delay.rate << 4;
    dst_[MOD_DST_DELAY_FEEDBACK] = patch_->delay.feedback << 4;
    dst_[MOD_DST_DELAY_TYPE] = patch_->delay.type << 4;
    dst_[MOD_DST_DELAY_INPUT] = patch_->delay.input << 4;

    dst_[MOD_DST_BITRATE_REDUCE] = patch_->shared.bitrate_reduce << 4;
    dst_[MOD_DST_SAMPLERATE_REDUCE] = patch_->shared.samplerate_reduce << 4;

    // lastly, swap out any destination with the Parameter Lock value
    mod_destinations_t p_lock_dst = (mod_destinations_t) ext_mod_->p_lock_dst;
    uint8_t p_lock_amt = ext_mod_->p_lock_amt;

    dst_[p_lock_dst] = p_lock_amt << 4;

}

void ModMatrix::processModulation() {

    for (int i = 0; i < NUM_MOD_SOURCES; i++)
    {

        int16_t amount = bipolarParameter(mod_amount_[i]) << 1;
        int16_t source = mod_source_[i];
        // uint8_t destination = U8CLIPMAX(NUM_MOD_DESTINATIONS, patch_->mod[i].destination);
        uint8_t destination = mod_destination_[i];
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

void ModMatrix::updateDestinations()
{

    // if oscillator reset is done, pass it along and flip the bool
    v_i->osc_reset = ext_mod_->osc_reset;
    ext_mod_->osc_reset = false;

    // VCA 1 CV output
    int16_t vca_output;

    if(!vca_drone_mode_) {

        int16_t vca_out = mod_source_[MOD_SRC_ENV_3];

        if(patch_->vca.velocity_on) {

            vca_out = U8S16MULSHIFT8(mod_source_[MOD_SRC_ENV_3], velocity_);

        }

        dst_[MOD_DST_VCA] += vca_out;
    }

    c_o->vca1 = U12CLIPMAXMIN(dst_[MOD_DST_VCA]) >> 1;

    // VCA 2 (filter FM) CV output
    c_o->vca2 = U12CLIPMAXMIN(dst_[MOD_DST_FILTER_FM]) >> 1;

    // filter cutoff CV output
    int16_t tracking_amount = dst_[MOD_DST_FILTER_TRACKING] >> 4;
    int16_t pitch_scaled = v_i->pitch_value >> 3;
    int16_t tracking = U8S16MULSHIFT8(pitch_scaled , tracking_amount);

    int16_t cutoffModulation = dst_[MOD_DST_FILTER_CUTOFF] + tracking;
    int16_t cutoff_env= mod_source_[MOD_SRC_ENV_2];
    int16_t cutoff_env_amount = bipolarParameter(dst_[MOD_DST_FILTER_ENV_AMT] >> 4) << 1;
    
    cutoffModulation += U8S16MULSHIFT8(cutoff_env, cutoff_env_amount);
    dst_[MOD_DST_FILTER_CUTOFF] = U12CLIPMAXMIN(cutoffModulation);
    c_o->cutoff = dst_[MOD_DST_FILTER_CUTOFF] >> 1;

    // resonance CV output
    c_o->resonance = U12CLIPMAXMIN(dst_[MOD_DST_FILTER_RESONANCE]) >> 1;

    // filter type CV output (CV being binary output of two GPIO pins, that is)
    c_o->filter_type = dst_[MOD_DST_FILTER_TYPE] >> 10;

    int16_t pb = ext_mod_->pitch_bend_amt - 8192;
    uint16_t pb_range = (ext_mod_->pitch_bend_depth_send + 1) << 8;
    int16_t pitch_bend_scaled = linearScale(pb, 8192, pb_range);


    v_i->osc_pitch_mod[0] = 
        ((dst_[MOD_DST_OSC_1_PITCH] - 2048) + (dst_[MOD_DST_OSC_BOTH_PITCH] - 2048)) << 3;
    v_i->osc_scale[0] = 
        ((bipolarParameter(patch_->osc[0].scale) * 2) * 12) + 
        bipolarParameter((dst_[MOD_DST_OSC_1_PITCH_FINE]) >> 4) * 2 + 
        bipolarParameter((dst_[MOD_DST_OSC_BOTH_PITCH_FINE]) >> 4) * 4 + 
        pitch_bend_scaled;
    v_i->osc_octave[0] = 
        U12CLIPMAXMIN(dst_[MOD_DST_OSC_1_OCTAVE] + (dst_[MOD_DST_OSC_BOTH_OCTAVE]) - 2048) >> 10; 

    v_i->osc_pitch_mod[1] = 
        ((dst_[MOD_DST_OSC_2_PITCH] - 2048) + (dst_[MOD_DST_OSC_BOTH_PITCH] - 2048)) << 3;
    v_i->osc_scale[1] = 
        ((bipolarParameter(patch_->osc[1].scale) * 2) * 12) + 
        bipolarParameter((dst_[MOD_DST_OSC_2_PITCH_FINE]) >> 4) * 2 +
        bipolarParameter((dst_[MOD_DST_OSC_BOTH_PITCH_FINE]) >> 4) * 4 +  
        pitch_bend_scaled;
    v_i->osc_octave[1] = 
        U12CLIPMAXMIN(dst_[MOD_DST_OSC_2_OCTAVE] + (dst_[MOD_DST_OSC_BOTH_OCTAVE] - 2048)) >> 10; 

    v_i->x_morph[0] = U12CLIPMAXMIN(dst_[MOD_DST_OSC_1_X_MORPH] + (dst_[MOD_DST_OSC_BOTH_X_MORPH] - 2048)) >> 4;
    v_i->y_morph[0] = U12CLIPMAXMIN(dst_[MOD_DST_OSC_1_Y_MORPH] + (dst_[MOD_DST_OSC_BOTH_Y_MORPH] - 2048)) >> 4;
    v_i->x_morph[1] = U12CLIPMAXMIN(dst_[MOD_DST_OSC_2_X_MORPH] + (dst_[MOD_DST_OSC_BOTH_X_MORPH] - 2048)) >> 4;
    v_i->y_morph[1] = U12CLIPMAXMIN(dst_[MOD_DST_OSC_2_Y_MORPH] + (dst_[MOD_DST_OSC_BOTH_Y_MORPH] - 2048)) >> 4;

    v_i->portamento = dst_[MOD_DST_PORTAMENTO] >> 5;
    v_i->phase_mod = dst_[MOD_DST_PHASE_MOD] >> 4;
    v_i->sync = dst_[MOD_DST_SYNC] >> 11;

    v_i->osc_1_mix = dst_[MOD_DST_MIX_OSC_1_MIX] >> 4;
    v_i->osc_2_mix = dst_[MOD_DST_MIX_OSC_2_MIX] >> 4;
    v_i->noise_mix = dst_[MOD_DST_MIX_NOISE_MIX] >> 4;

    // bitrate reduction, value is reversed so that turning down increases the effect
    uint8_t notbrr = ~(dst_[MOD_DST_BITRATE_REDUCE] >> 4);
    v_i->brr = param8Bits0To10(notbrr);

    // samplerate reduction, ""
    uint8_t notsrr = ~(dst_[MOD_DST_SAMPLERATE_REDUCE] >> 4);
    v_i->srr = notsrr;

    // oscillator duophony
    v_i->duophony = patch_->shared.osc_duo;

    env_[0].Update(
        dst_[MOD_DST_ENV_1_ATTACK]      >> 5,
        dst_[MOD_DST_ENV_1_DECAY]       >> 5,
        dst_[MOD_DST_ENV_1_SUSTAIN]     >> 4,
        dst_[MOD_DST_ENV_1_RELEASE]     >> 5,
        dst_[MOD_DST_ENV_1_LOOP]        >> 11
    );

        env_[1].Update(
        dst_[MOD_DST_ENV_2_ATTACK]      >> 5,
        dst_[MOD_DST_ENV_2_DECAY]       >> 5,
        dst_[MOD_DST_ENV_2_SUSTAIN]     >> 4,
        dst_[MOD_DST_ENV_2_RELEASE]     >> 5,
        dst_[MOD_DST_ENV_2_LOOP]        >> 11
    );

    env_[2].Update(
        dst_[MOD_DST_ENV_3_ATTACK]      >> 5,
        dst_[MOD_DST_ENV_3_DECAY]       >> 5,
        dst_[MOD_DST_ENV_3_SUSTAIN]     >> 4,
        dst_[MOD_DST_ENV_3_RELEASE]     >> 5,
        dst_[MOD_DST_ENV_3_LOOP]        >> 11
    );

    bool tick_occurred = ext_mod_->clock_tick_occurred;
    ext_mod_->clock_tick_occurred = false;

    bool start_occurred = ext_mod_->start_occurred;
    ext_mod_->start_occurred = false;

    int16_t *lfo_parameters = &dst_[MOD_DST_LFO_1_RATE];

    uint32_t clock_period = ext_mod_->clock_tick_period;

    for (int i = 0; i < NUMBER_OF_LFOS; ++i)
    {
        uint8_t lfo_sync_reset_state;

        switch(i) {
            case 0: lfo_sync_reset_state = dst_[MOD_DST_LFO_1_SYNC] >> 10; break;
            case 1: lfo_sync_reset_state = dst_[MOD_DST_LFO_2_SYNC] >> 10; break;
            case 2: lfo_sync_reset_state = dst_[MOD_DST_LFO_3_SYNC] >> 10; break;
            default: break;
        }

        bool lfo_reset = lfo_reset_sync[lfo_sync_reset_state][0];
        bool lfo_sync = lfo_reset_sync[lfo_sync_reset_state][1];
        LFOSetSync(&lfo_[i], lfo_sync);
        LFOSetReset(&lfo_[i], lfo_reset);

        uint16_t sync_div_val = clock_sync_div_val[lfo_parameters[0] >> 8];

        uint32_t lfo_sync_inc = getClockDivValue12Bits(clock_period, sync_div_val);

        LFOUpdate(
            &lfo_[i], 
            LFOGetSync(&lfo_[i]) == false? lfo_parameters[0] + 1 : lfo_sync_inc,
            param8Bits0To4(lfo_parameters[1] >> 4),
            lfo_parameters[2] >> 4,
            0,
            0
        );
        lfo_parameters += 6;

        bool tick_reset = false;

        if(tick_occurred) {

            lfo_tick_count[i]++;
            
            if(lfo_tick_count[i] >= sync_div_val) {
                
                tick_reset = true;

            }
        }

        if((tick_reset || start_occurred) && lfo_[i].sync_) {
            
            LFOResetNow(&lfo_[i]);
            lfo_tick_count[i] = 0;   

        }
    }

    mod_amount_[MOD_SRC_ENV_1] = dst_[MOD_DST_ENV_1_AMOUNT] >> 4;
    mod_amount_[MOD_SRC_ENV_2] = dst_[MOD_DST_ENV_2_AMOUNT] >> 4;
    mod_amount_[MOD_SRC_ENV_3] = dst_[MOD_DST_ENV_3_AMOUNT] >> 4;
    mod_amount_[MOD_SRC_LFO_1] = dst_[MOD_DST_LFO_1_AMOUNT] >> 4;
    mod_amount_[MOD_SRC_LFO_2] = dst_[MOD_DST_LFO_2_AMOUNT] >> 4;
    mod_amount_[MOD_SRC_LFO_3] = dst_[MOD_DST_LFO_3_AMOUNT] >> 4;
    mod_amount_[MOD_SRC_VELOCITY] = MAX_PARAM_VALUE;
    mod_amount_[MOD_SRC_MOD_WHEEL] = MAX_PARAM_VALUE;
    mod_amount_[MOD_SRC_AFTERTOUCH] = MAX_PARAM_VALUE;
    mod_amount_[MOD_SRC_BREATH] = MAX_PARAM_VALUE;

    uint8_t mod_dst_max = NUM_MOD_DESTINATIONS - 1;

    mod_destination_[MOD_SRC_ENV_1] = S8ClipZeroMax(dst_[MOD_DST_ENV_1_DST] >> 5, mod_dst_max);
    mod_destination_[MOD_SRC_ENV_2] = S8ClipZeroMax(dst_[MOD_DST_ENV_2_DST] >> 5, mod_dst_max);
    mod_destination_[MOD_SRC_ENV_3] = S8ClipZeroMax(dst_[MOD_DST_ENV_3_DST] >> 5, mod_dst_max);
    mod_destination_[MOD_SRC_LFO_1] = S8ClipZeroMax(dst_[MOD_DST_LFO_1_DST] >> 5, mod_dst_max);
    mod_destination_[MOD_SRC_LFO_2] = S8ClipZeroMax(dst_[MOD_DST_LFO_2_DST] >> 5, mod_dst_max);
    mod_destination_[MOD_SRC_LFO_3] = S8ClipZeroMax(dst_[MOD_DST_LFO_3_DST] >> 5, mod_dst_max);
    mod_destination_[MOD_SRC_VELOCITY] = patch_->mod[MOD_SRC_VELOCITY].destination;
    mod_destination_[MOD_SRC_MOD_WHEEL] = patch_->mod[MOD_SRC_MOD_WHEEL].destination;
    mod_destination_[MOD_SRC_AFTERTOUCH] = patch_->mod[MOD_SRC_AFTERTOUCH].destination;
    mod_destination_[MOD_SRC_NOTE] = patch_->mod[MOD_SRC_NOTE].destination;
    mod_destination_[MOD_SRC_BREATH] = patch_->mod[MOD_SRC_BREATH].destination;


    d_i->amount = dst_[MOD_DST_DELAY_AMOUNT] >> 4;
    d_i->feedback = dst_[MOD_DST_DELAY_FEEDBACK] >> 4;
    d_i->input = dst_[MOD_DST_DELAY_INPUT] >> 4;

    // The d_i->type variable actually never touches the delay class. 
    // the type is determined here;
    d_i->type = dst_[MOD_DST_DELAY_TYPE] >> 10;

    uint16_t delay_rate_default = dst_[MOD_DST_DELAY_RATE] << 4;
    uint16_t delay_rate_sync = (48000ULL * ((uint64_t)clock_period * (uint64_t)clock_sync_div_val[15 - (dst_[MOD_DST_DELAY_RATE] >> 8)])) / 1000000ULL;
    uint16_t delay_rate_short = osc_increments[dst_[MOD_DST_DELAY_RATE] >> 5] >> 16;

    switch(d_i->type) {
        
        case DELAY_TYPE_SYNC:   d_i->length = delay_rate_sync;      break;
        case DELAY_TYPE_SHORT:  d_i->length = delay_rate_short;     break;
        default:                d_i->length = delay_rate_default;   break;

    }

}

uint16_t ModMatrix::getClockDivValue12Bits(uint32_t clock_period, uint16_t div_val) {

    return ((k_clock_sync_tick_dividend/(clock_period * div_val)) << 16)/k_clock_sync_divisor;

    // (60 seconds in us / (tick period * division of period) << 16 bit) / update frequency in HZ
    // It can be thought of like this:
    // say the value being shifted by 16 bits is 1 (1 << 16)...
    // at the update rate defined by the sync divisor, a total cycle will take 1 second
    // anything greater than 1 << 16 is less than a second, where anything less is more than a second
    // ... k_clock_syn_tick_dividend should be called something more like kone_second_in_us_times_100 or sth
}
