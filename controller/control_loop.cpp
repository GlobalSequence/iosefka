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

    bool midi_event, ui_event, seq_event, storage_event;

    midi_event = pollMidi();
    
    pollEncoder();

    pollPotentiometers(flag);

    pollGpioExpanders(call_counter, flag);

    ui_event = eventProcessorUpdate();

    updateSynthParameters(flag);

    seq_event = pollSequencer(flag);

    storage_event = storageStateChanged();
    storageCheckForFinished();
    
    if(midi_event || ui_event || seq_event || storage_event) {

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

    if(flag && count & 1) {

        displayDisplay();

    }

}

// DEPRECATED

// void controlLoopUpdate() {

    //     TEST_PRINT_ONE_NL(0)
    
    //     bool midi_event, ui_event, seq_event;
    
    //     // poll UART buffer for MIDI events
    //     uint64_t time_midi = time_us_64();
    //     midi_event = pollTimerMidi(time_midi);
    
    //     TEST_PRINT_ONE_NL(1)
    
    //     // poll UI elements
    //     uint64_t time_ui = time_us_64();
    //     pollTimerEncoder(time_ui);
    //     pollTimerPotentiometers(time_ui);
    //     pollTimerGpioExpanders(time_ui);
    
    //     TEST_PRINT_ONE_NL(2)
    
    //     ui_event = eventProcessorUpdate();
    
    //     TEST_PRINT_ONE_NL(3)
    
    //     // poll Sequencer for next step
    //     uint64_t time_seq = time_us_64();
    //     seq_event = pollTimerSequencer(time_seq);
    
    //     TEST_PRINT_ONE_NL(4)
        
    //     // check for storage events
    //     bool storage_event = storageStateChanged();
    //     storageCheckForFinished();
    
    //     TEST_PRINT_ONE_NL(5)
    
    //     // check to see if an update event occurred which would trigger the display
    //     bool display_update = midi_event || ui_event || seq_event || storage_event;
    //     if(display_update) {
    
    //         displayTrigger();
    
    //     }
    
    //     TEST_PRINT_ONE_NL(6)
        
    //     // poll Display for next update
    //     uint64_t time_display = time_us_64();
    //     pollTimerDisplay(time_display);
    
    //     storageExecuteTypeIfActive();
    
    //     TEST_PRINT_ONE_NL(7)
    
    // }
    
    // bool pollTimerMidi(unsigned long time) {
    
    //     bool midi_event_happened = false;
    
    //     if((time - last_update_midi) >= k_interval_us_midi) {
    
    //         midiRxHandler();
    //         midiTxHandler();
    //         midiExtClockCountdown();
    
    //     }
    
    //     return midi_event_happened;
    
    // }
    
    
    // void pollTimerPotentiometers(unsigned long time) {
    
    //     if((time - last_update_pots) >= k_interval_us_pots) {
    
    //         last_update_pots = time;
    
    //         for(int i = 0; i < NUMBER_OF_POTENTIOMETERS; i++) {
    
    //             uiUpdatePotentiometers(i);
    
    //         }
    
    //     }
    
    // }
    
    // void pollTimerGpioExpanders(unsigned long time) {
    
    //     if((time - last_update_gpio_exp_port) >= k_interval_us_gpio_exp_port) {
    
    //         last_update_gpio_exp_port = time;
    
    //         uiUpdateGpioExpanders(gpio_exp_port_num_stepper);
    
    //         gpio_exp_port_num_stepper == NUM_GPIO_EXP_PORTS - 1? 
    
    //         gpio_exp_port_num_stepper = 0 : gpio_exp_port_num_stepper++;
    
    //     }
    
    // }
    
    // void pollTimerEncoder(unsigned long time) {
    
    //     if((time - last_update_encoder) >= k_interval_us_encoder) {
    
    //         last_update_encoder = time;
    
    //         uiUpdateEncoder();
    
    //     }
    
    // }
    
    
    // bool pollTimerSequencer(unsigned long time) {
    
    //     bool seq_event = seqPollUpdate(time);
        
    //     if(seqPollDidTickHappen()) midiEnqueueRealtime(MIDI_TICK);
    
    //     return seq_event;
    //     // return seqPollUpdate(time);
    
    // }
    
    // void pollTimerDisplay(unsigned long time) {
    
    //     if(time - last_update_display >= k_interval_us_display) {
    
    //         last_update_display = time;
    
    //         displayDisplay();
    
    //     }
        
    
    // }