#ifndef SYNTH_H
#define SYNTH_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "patch.h"

#ifdef __cplusplus
extern "C"
{
#endif

void synthSetup(
    synth_parameters_t *patch,
    external_modulation_t *ext_mod, 
    uint16_t random_seed
);

bool synthUpdate();

#ifdef __cplusplus
}
#endif

#endif
