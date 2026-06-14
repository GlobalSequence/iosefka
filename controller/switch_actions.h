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

#ifndef SWITCH_HANDLER_H
#define SWITCH_HANDLER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "settings.h"
#include "../common/globals.h"

typedef void (*switchEventDisplayFunc)(displayable_events_t, bool);

#ifdef __cplusplus
extern "C"
{
#endif

void switchActionsSetup(synth_state_t *synth_state, switchEventDisplayFunc func);

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
