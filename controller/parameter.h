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

// uint8_t* parameterGetLayerPtr();

#ifdef __cplusplus
}
#endif

#endif
