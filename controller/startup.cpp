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
#include "../devices/audio.h"
#include "../synth/synth.h"
#include "../common/test.h"


// const static bool write_all_memory = false;

// TODO: ADD LEGATO IN SETTINGS
bool temp_legato = true;

// private
// void startupSetAllPresetsToDefault();
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

    if(versionWriteVersionToCleanStorage() || checkForRestoreMemoryRequest()) {

        storageWriteAllMemory(displayShowWritingNewPresets);

    }

    versionUpdate();

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

    noteQueueSetup(&synth_state->patch.shared.legato, &synth_state->patch.shared.osc_duo);

    synthSetup(
        parameterGetRenderedPatchPointer(),
        settingsGetExternalModulation(),
        seed
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