#ifndef PWM_H
#define PWM_H

#include <stdio.h>
#include "pico/stdlib.h"

enum PwmCVSlices {
    PWM_CUTOFF_SLICE,
    PWM_RESONANCE_SLICE,

    NUMBER_OF_PWM_CV_SLICES
};

typedef enum cv_source_t {

    CV_SOURCE_CUTOFF,
    CV_SOURCE_RESONANCE,
    CV_SOURCE_VCA_1,
    CV_SOURCE_VCA_2,

    NUM_CV_SOURCES

} cv_source_t;

// void pwmSetup();
void pwmSetup(uint cutoff_slice, uint res_slice, uint vca_1_slice, uint vca_2_slice);
void pwmSetLevel(cv_source_t source, uint16_t value);
void pwmUpdateLevels();
uint16_t* pwmGetCutoffPointer();
uint16_t* pwmGetResonancePointer();
uint16_t* pwmGetVca1Pointer();
uint16_t* pwmGetVca2Pointer();


#endif