#ifndef SWITCH_HANDLER_H
#define SWITCH_HANDLER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "settings.h"
#include "../common/globals.h"

#ifdef __cplusplus
extern "C"
{
#endif

void switchActionsSetup(synth_state_t *synth_state);

void switchActionsTakeInput(
    switch_id_t sw, 
    bool pressed, 
    bool shift, 
    bool timeout, 
    bool encoder_switch,
    uint8_t num_all_switches_pressed, 
    uint8_t num_all_keys_pressed, 
    uint8_t num_bottom_keys_pressed
);

bool switchActionsViewChanged();

#ifdef __cplusplus
}
#endif

#endif
