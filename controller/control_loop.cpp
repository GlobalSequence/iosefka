#include "control_loop.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "ui.h"
#include "event_processor.h"
#include "display.h"
#include "storage.h"
#include "midi.h"
#include "parameter.h"
#include "../sequencer/seq_poll.h"
#include "../common/utilities.h"
#include "../common/globals.h"
#include "../common/test.h"

unsigned long last_update_pots;
static const unsigned long k_interval_us_pots = 2000;

unsigned long last_update_gpio_exp_port;
static const unsigned long k_interval_us_gpio_exp_port = 2000;
uint8_t gpio_exp_port_num_stepper;

unsigned long last_update_encoder;
static const unsigned long k_interval_us_encoder = 500;

unsigned long last_update_midi;
static const unsigned long k_interval_us_midi = 100;

unsigned long last_update_display;
static const unsigned long k_interval_us_display = 5000;

// ################################
// PRIVATE FUNCTIONS
// ################################

// ui polling loops
void pollTimerPotentiometers(unsigned long time);
void pollTimerGpioExpanders(unsigned long time);
void pollTimerEncoder(unsigned long time);

// MIDI polling loop
bool pollTimerMidi(unsigned long time);

// sequencer polling loop
bool pollTimerSequencer(unsigned long time);

// display polling loop
void pollTimerDisplay(unsigned long time);

// 

////////////////////////////////////////////////


uint8_t call_counter;

bool pollMidi();
void pollEncoder();
void pollPotentiometers(bool flag);
void pollGpioExpanders(uint8_t count, bool flag);
bool pollSequencer(bool flag);
void updateSynthParameters(bool flag);
void pollDisplay(uint8_t count, bool flag);

void controlLoopUpdateByFlag(bool flag) {

    uint32_t time = time_us_32();

    bool midi_event, ui_event, seq_event, storage_event, internal_event;

    midi_event = pollMidi();
    
    pollEncoder();

    pollPotentiometers(flag);

    pollGpioExpanders(call_counter, flag);

    ui_event = eventProcessorUpdate();

    updateSynthParameters(flag);

    seq_event = pollSequencer(flag);

    storage_event = storageStateChanged();
    storageCheckForFinished();

    internal_event = displayInternalEventTrigger();
    
    if(midi_event || ui_event || seq_event || storage_event || internal_event) {

        displayTrigger();

    }

    pollDisplay(call_counter, flag);

    storageExecuteTypeIfActive();

    if(flag) call_counter++;

}

bool pollMidi() {

    bool midi_event_happened = midiRxHandler();
    midiTxHandler();
    midiExtClockCountdown();

    return midi_event_happened;

}

void pollEncoder() {

    uiUpdateEncoder();

}

void pollPotentiometers(bool flag) {

    if(flag) {

        for(int i = 0; i < NUMBER_OF_POTENTIOMETERS; i++) {

            uiUpdatePotentiometers(i);

        }

    }

}

void pollGpioExpanders(uint8_t count, bool flag) {

    if(flag && count & 1) {

        uiUpdateGpioExpanders(gpio_exp_port_num_stepper);

        gpio_exp_port_num_stepper == NUM_GPIO_EXP_PORTS - 1? 

        gpio_exp_port_num_stepper = 0 : gpio_exp_port_num_stepper++;

    }

}

void updateSynthParameters(bool flag) {

    if(flag) parameterCopyValuesToSynth();

}

bool pollSequencer(bool flag) {

    if(flag) {

        unsigned long time = time_us_64();

        bool seq_event = seqPollUpdate(time);
        
        if(seqPollDidTickHappen()) midiEnqueueRealtime(MIDI_TICK);
    
        seqPollUpdateTickPeriod();
    
        return seq_event;

    }

    return false;

}

void pollDisplay(uint8_t count, bool flag) {

    if((flag && count & 1)) {

        displayDisplay();

    }

}