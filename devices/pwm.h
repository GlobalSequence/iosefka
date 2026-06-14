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