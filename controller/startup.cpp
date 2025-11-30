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


const static bool write_all_memory = false;

// TODO: ADD LEGATO IN SETTINGS
bool temp_legato = true;

// private
void startupSetAllPresetsToDefault();
void checkForRestoreMemoryRequest();

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

    if(/* storageNeverWritten() */versionWriteVersionToCleanStorage() || write_all_memory) {

        startupSetAllPresetsToDefault();

    }

    // version_t v = storageLoadVersion();
    // TEST_PRINT_ONE_TAB(v.major)
    // TEST_PRINT_TWO_NL(v.minor, v.patch)

    versionUpdate();

    // versionStorageTest();
    // versionTest();
    
    storageLoad(STORAGE_LOAD_ADDRESS);

    switchActionsSetup(synth_state);
    
    parameterSetup(synth_state);
    
    encoderMapSetup(synth_state);

    midiSetup(synth_state);

    uint16_t seed = uiGetAdcValueForRandSeed();
    
    seqPollSetup(
        &synth_state->seq, 
        &synth_state->special.ext_clock_active, 
        noteQueueSubmit, 
        // noteQueueKillActiveNotes,
        parameterSetPLockByStep,
        settingsSetClockTickPeriod,
        settingsSetClockTickEvent,
        settingsSetStartEvent
    );
    
    patternGenSetup(&synth_state->seq, &synth_state->pattern_gen, seed);

    noteQueueSetup(&temp_legato);

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

void startupSetAllPresetsToDefault() {

    synth_parameters_t p;

    pattern_data_t pd;

    step_data_t sd;

    pattern_gen_t pg;

    synth_storable_t st;

    machine_settings_t ms;

    p.osc[0].scale          = 0x80;
    p.osc[0].octave         = 0x80;
    p.osc[0].x_morph        = 0x00;
    p.osc[0].y_morph        = 0x00;

    p.osc[1].scale          = 0x80;
    p.osc[1].octave         = 0x80;
    p.osc[1].x_morph        = 0x00;
    p.osc[1].y_morph        = 0x00;

    p.shared.phase_mod      = 0x00;
    p.shared.portamento     = 0x00;
    p.shared.sync           = 0x00;

    p.mixer.osc_1_mix       = 0xFF;
    p.mixer.osc_2_mix       = 0x00;
    p.mixer.noise_mix       = 0x00;

    p.filter.cutoff         = 0xFF;
    p.filter.resonance      = 0x00;
    p.filter.env_amt        = 0x80;
    p.filter.type           = 0x40;
    p.filter.fm             = 0x00;
    p.filter.tracking       = 0xFF;

    p.vca.level             = 0x00;
    p.vca.velocity_on       = 0x01;
    
    p.delay.amount          = 0x00;
    p.delay.rate            = 0x80;
    p.delay.diffusion       = 0x00;
    p.delay.feedback        = 0x00;
    p.delay.filter          = 0x00;

    p.env[0].attack         = 0x00;
    p.env[0].decay          = 0x80;
    p.env[0].sustain        = 0x80;
    p.env[0].release        = 0x80;
    p.env[0].loop           = 0x00;

    p.env[1].attack         = 0x00;
    p.env[1].decay          = 0x80;
    p.env[1].sustain        = 0x80;
    p.env[1].release        = 0x80;
    p.env[1].loop           = 0x00;

    p.env[2].attack         = 0x00;
    p.env[2].decay          = 0x80;
    p.env[2].sustain        = 0x80;
    p.env[2].release        = 0x80;
    p.env[2].loop           = 0x00;

    p.lfo[0].rate           = 0x00;
    p.lfo[0].shape          = 0x00;
    p.lfo[0].sync           = 0x00;
    p.lfo[0].phase          = 0x00;

    p.lfo[1].rate           = 0x00;
    p.lfo[1].shape          = 0x00;
    p.lfo[1].sync           = 0x00;
    p.lfo[1].phase          = 0x00;

    p.lfo[2].rate           = 0x00;
    p.lfo[2].shape          = 0x00;
    p.lfo[2].sync           = 0x00;
    p.lfo[2].phase          = 0x00;

    p.mod[0].destination    = 0x00;
    p.mod[0].amount         = 0x80;

    p.mod[1].destination    = 0x00;
    p.mod[1].amount         = 0x80;

    p.mod[2].destination    = 0x00;
    p.mod[2].amount         = 0x80;

    p.mod[3].destination    = 0x00;
    p.mod[3].amount         = 0x80;

    p.mod[4].destination    = 0x00;
    p.mod[4].amount         = 0x80;

    p.mod[5].destination    = 0x00;
    p.mod[5].amount         = 0x80;

    p.mod[6].destination    = 0x00;
    p.mod[6].amount         = 0x80;

    p.mod[7].destination    = 0x00;
    p.mod[7].amount         = 0x80;

    p.mod[8].destination    = 0x00;
    p.mod[8].amount         = 0x80;

    p.mod[9].destination    = 0x00;
    p.mod[9].amount         = 0x80;

    // step data;

    sd.note                 = 0x24;
    sd.length               = 0x00;
    sd.velocity             = 0xFF;
    sd.p_lock_param         = 0x00;
    sd.p_lock_value         = 0x00;

    // pattern data

    pd.bpm                  = 0x78; // 120 bpm
    pd.clock_div            = 0x04; // NUMBER_OF_CLOCK_DIVS - 1
    pd.direction            = 0x00; // forward
    pd.first_step           = 0x00; 
    pd.last_step            = 0x07;
    pd.probability          = 0xFF;
    pd.swing_amt            = 0x00;
    pd.transpose            = 0x0C; // 0 transposition

    // pattern generator

    pg.chance               = pd.probability;
    pg.base_note            = sd.note;
    pg.scale                = 0x00;
    pg.density              = 0x00;
    pg.repeat               = 0x01;
    pg.octave_range         = 0x03;
    pg.countdown            = 0x00;
    pg.on                   = true;

    // machine settings

    ms.channel              = 0x0B;
    ms.pitch_bend_depth     = 0x01;
    ms.clock_rx             = true;
    ms.transport_rx         = true;
    ms.program_change_rx    = true;
    ms.out_thru             = false;
    ms.latch                = false;

    // prepare storable data

    st.patch                = p;
    st.seq_pattern_data     = pd;
    st.step_data            = sd;
    st.pattern_gen          = pg;

    // settingsSetStoredData(st);
    // settingsSetMachineSettingsAll(ms);

    for(int i = 0; i < NUMBER_OF_PRESETS; i++) {

        settingsSetStoredData(st);

        settingsSetMachineSettingsAll(ms);

        displayShowWritingNewPresets(i);

        sleep_ms(10);

        storageSave(i);

    }

}

void checkForRestoreMemoryRequest() {

    for(int i = 0; i < NUM_GPIO_EXP_PORTS; i++) {

        uiUpdateGpioExpanders(i);

    }

    bool confirm = eventProcessorCheckForActionsOnStartup();

    TEST_PRINT_TEXT("CONFIRM: ")
    TEST_PRINT_ONE_NL(confirm)
}