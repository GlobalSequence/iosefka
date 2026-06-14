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

#ifndef PATTERN_GENERATOR_H
#define PATTERN_GENERATOR_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "sequencer.h"
#include "../common/globals.h"
#include "../common/utilities.h"

#define NUMBER_OF_WEIGHTED_CHANCE_VALS 8
#define NUMBER_OF_WEIGHTED_CHANCE_POINTS 8
#define NUMBER_OF_REPEAT_VALS 8
#define NUMBER_OF_NOTES_PER_SCALE 8

typedef struct pattern_gen_t {
    uint8_t chance;
    uint8_t base_note;
    uint8_t scale;
    uint8_t density;
    uint8_t repeat;
    uint8_t octave_range;
    uint8_t countdown;
    bool on;
} pattern_gen_t;

typedef enum scales_by_note_t {
    SCALE_MAJOR,
    SCALE_MINOR,

    NUM_SCALES
} scales_by_note_t;

#ifdef __cplusplus
extern "C"
{
#endif

void patternGenSetup(Sequencer *sequencer, pattern_gen_t *parameter, uint16_t rand_seed);
void patternGenExecute();
bool patternGenCheckLoopCount();
void patternGenSetCountdown();
bool patternGenOccurred();
bool patternGenIsActive();
uint16_t patternGenGetRand();

#ifdef __cplusplus
}
#endif

#endif