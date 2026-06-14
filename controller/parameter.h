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

#ifndef PARAMETER_H
#define PARAMETER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "settings.h"
#include "../common/globals.h"
#include "../synth/patch.h"

#ifdef __cplusplus
extern "C"
{
#endif

void parameterSetup(synth_state_t *synth_state);
void parameterSetValueWithPot(
    pot_index_t pot,
    uint8_t value,
    uint8_t num_bottom_keys_pressed,
    bool shift, 
    bool *bottom_key);
void parameterSetValueByModDst(mod_destinations_t dst, uint8_t value);
uint8_t parameterSetValueByModDstIndex(uint8_t index, uint8_t value);
void parameterSetPLockByStep(uint8_t step, bool active);
void parameterCopyValuesToSynth();

synth_parameters_t* parameterGetRenderedPatchPointer();
uint8_t* parameterGetPointer(mod_destinations_t dst);
uint8_t parameterGetParamFromCurrentLayer(uint8_t index);

void parameterSetUserParameter(uint8_t index, mod_destinations_t dst);
void parameterSetUserParameterAllPointers();
uint8_t parameterGetUserParameter(uint8_t index);

// uint8_t* parameterGetLayerPtr();

#ifdef __cplusplus
}
#endif

#endif
