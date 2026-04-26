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
