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

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "settings.h"
#include "common/globals.h"
#include "../synth/patch.h"
#include "../devices/ssd1306.h"

using namespace pico_ssd1306;

#ifdef __cplusplus
extern "C"
{
#endif

void displaySetup(synth_state_t *synth_state, synth_parameters_t *synth_patch, uint8_t *led, bool *led_tx);
void displayTrigger();
void displayDisplay();
void displayShowEncoderMode();
void displayHideEncoderMode();
void displayShowEncoderModeCountdown();

void displayShowWritingNewPresets(uint8_t index);
void displayShowUpdatingPresets(uint8_t index);

void displayShowAltScreen(displayable_events_t evt, bool timer);

bool displayInternalEventTrigger();

#ifdef __cplusplus
}
#endif

#endif