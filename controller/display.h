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

// bool displayScopeActive();
bool displayInternalEventTrigger();

#ifdef __cplusplus
}
#endif

#endif