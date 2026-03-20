#include "pwm.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "pin_assignments.h"

static uint cutoff_slice_num;
static uint resonance_slice_num;

static uint vca_1_slice_num;
static uint vca_2_slice_num;

static uint16_t cutoff_level;
static uint16_t resonance_level;
static uint16_t vca_1_level;
static uint16_t vca_2_level;

void pwmSetup(uint cutoff_slice, uint res_slice, uint vca_1_slice, uint vca_2_slice) {

    cutoff_slice_num = cutoff_slice;
    resonance_slice_num = res_slice;
    vca_1_slice_num = vca_1_slice;
    vca_2_slice_num = vca_2_slice;

}

void pwmSetLevel(cv_source_t source, uint16_t value) {

    switch(source) {

        case CV_SOURCE_CUTOFF:
        pwm_set_chan_level(cutoff_slice_num, PWM_CHAN_A, value);
        break;

        case CV_SOURCE_RESONANCE:
        pwm_set_chan_level(resonance_slice_num, PWM_CHAN_B, value);
        break;

        case CV_SOURCE_VCA_1:
        pwm_set_chan_level(vca_1_slice_num, PWM_CHAN_A, value);
        break;

        case CV_SOURCE_VCA_2:
        pwm_set_chan_level(vca_2_slice_num, PWM_CHAN_B, value);
        break;

        default:
        break;
    }
}

void pwmUpdateLevels() {

    // cutoff
    pwm_set_chan_level(cutoff_slice_num, PWM_CHAN_A, cutoff_level);
    //resonance
    pwm_set_chan_level(resonance_slice_num, PWM_CHAN_B, resonance_level);
    // VCA 1
    pwm_set_chan_level(vca_1_slice_num, PWM_CHAN_A, vca_1_level);
    // VCA 2
    pwm_set_chan_level(vca_2_slice_num, PWM_CHAN_B, vca_2_level);


}

uint16_t* pwmGetCutoffPointer() {

    return &cutoff_level;

}

uint16_t* pwmGetResonancePointer() {

    return &resonance_level;

}

uint16_t* pwmGetVca1Pointer() {

    return &vca_1_level;

}

uint16_t* pwmGetVca2Pointer() {

    return &vca_2_level;

}