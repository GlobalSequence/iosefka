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

#ifndef SYNTH_H
#define SYNTH_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "patch.h"

typedef struct metro_data_t {

    bool (*getStepRecord)();
    int16_t (*getStepNumber)();

} metro_data;

#ifdef __cplusplus
extern "C"
{
#endif

void synthSetup(
    synth_parameters_t *patch,
    external_modulation_t *ext_mod, 
    uint16_t random_seed,
    metro_data_t metro
);

bool synthUpdate();

#ifdef __cplusplus
}
#endif

#endif
