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

#include "startup.h"
#include <stdio.h>
#include "pico/stdlib.h"

#include "ui.h"
#include "event_processor.h"
#include "encoder_map.h"
#include "parameter.h"
#include "switch_actions.h"
#include "display.h"
#include "storage.h"
#include "view.h"
#include "devices/devices.h"
#include "midi.h"
#include "settings.h"
#include "note_queue.h"
#include "version/version.h"
#include "../sequencer/seq_poll.h"
#include "../sequencer/pattern_generator.h"
#include "../sequencer/step_entry.h"
#include "../devices/audio.h"
#include "../synth/synth.h"
#include "../common/test.h"


// const static bool write_all_memory = false;

// TODO: ADD LEGATO IN SETTINGS
bool temp_legato = true;

// private
bool checkForRestoreMemoryRequest();

// public
void startupRun() {

    devicesSetup();

    uiSetup();
    
    viewSetup();

    synth_state_t *synth_state = settingsGetSynthState();

    displaySetup(
        synth_state, 
        parameterGetRenderedPatchPointer(), 
        &gpio_exp_led_bits, 
        &gpio_exp_transmit_leds
    );

    storageViewCallbacks storage_view = {viewSetPresetNumberSelectedAndLoaded};

    storageSetup(storage_view);

    if(checkForRestoreMemoryRequest()) {

        storageWriteAllMemory(displayShowWritingNewPresets);

    }

    storageLoadPresetNames();

    storageLoad(STORAGE_LOAD_ADDRESS);

    switchActionsSetup(synth_state, displayShowAltScreen);
    
    parameterSetup(synth_state);
    
    encoderMapSetup(synth_state);

    midiSetup(synth_state, storageLoadPresetInternal);

    uint16_t seed = uiGetAdcValueForRandSeed();
    
    seqPollSetup(
        &synth_state->seq, 
        &synth_state->special.ext_clock_active, 
        noteQueueSubmit, 
        parameterSetPLockByStep,
        settingsSetClockTickPeriod,
        settingsSetClockTickEvent,
        settingsSetStartEvent
    );
    
    patternGenSetup(&synth_state->seq, &synth_state->pattern_gen, seed);

    bool* record_click = settingsGetRecordClick();

    stepEntrySetup(&synth_state->seq, record_click);

    noteQueueSetup(&synth_state->patch.shared.legato, &synth_state->patch.shared.osc_duo);

    metro_data_t metro = {
        // stepEntryGetStepRecord,
        stepEntryRunClick,
        seqPollGetStepNumber
    };

    synthSetup(
        parameterGetRenderedPatchPointer(),
        settingsGetExternalModulation(),
        seed,
        metro
    );

    checkForRestoreMemoryRequest();

}

void startupRunCore1() {

    devicesSetupCore1();

}

bool checkForRestoreMemoryRequest() {

    for(int i = 0; i < NUM_GPIO_EXP_PORTS; i++) {

        uiUpdateGpioExpanders(i);

    }

    return eventProcessorCheckForActionsOnStartup();

}