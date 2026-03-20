#include "event_processor.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "ui.h"
#include "encoder_map.h"
#include "parameter.h"
#include "switch_actions.h"
#include "switch_reader.h"
#include "display.h"
#include "../common/test.h"

// ################################
// TEST FUNCTIONS
// ################################

void testSwitchReader();

// ################################
// PRIVATE FUNCTIONS
// ################################

void switchEvent(uint8_t index, uint8_t value) {

    switch_id_t sw = (switch_id_t)index;

    // !CRITICAL! Invert the switch value so that pressed = HIGH
    bool pressed = (bool)!value;

    switchReaderSubmit(sw, pressed); 

    switch_reader_t *s = switchReaderGetStructPtr();

    switchActionsTakeInput(
        sw, 
        pressed, 
        s->shift, 
        s->timeout[index], 
        s->encoder_switch, 
        s->num_all_pressed,
        s->num_keys_pressed,
        s->num_bottom_keys_pressed
    );

    if(sw == SW_ENCODER) {

        pressed? displayShowEncoderMode() : displayHideEncoderMode();

    }

    if(switchActionsViewChanged()) displayShowEncoderModeCountdown();

}

void potEvent(uint8_t index, uint8_t value) {

    pot_index_t pot = (pot_index_t)index;
    switch_reader_t *s = switchReaderGetStructPtr();

    parameterSetValueWithPot(pot, 
        value, 
        s->num_bottom_keys_pressed, 
        s->shift, 
        s->state_bottom);

}

void encoderEvent(int8_t value) {

    switch_reader_t *s = switchReaderGetStructPtr();

    encoderMapAssignValue(value, s->encoder_switch);

    displayShowEncoderModeCountdown();

}

bool processUiEvents() {

    bool event_happened = false;

    while(!event_queue.isEmpty()) {

        event_t e = event_queue.dequeue();

        switch(e.event_type) {
                
            case EVENT_SWITCH:

                switchEvent(e.index, e.value);

                break;

            case EVENT_POT:

                potEvent(e.index, e.value);

                break;

            case EVENT_ENCODER:

                encoderEvent(e.value);

                break;        

        }

        event_happened = true;

    }

    return event_happened;

}


bool eventProcessorCheckForActionsOnStartup() {

    bool seq_pressed = false; 
    bool env_pressed = false; 
    bool sy1_pressed = false;
    bool play_pressed = false;

    while(!event_queue.isEmpty()) {

        event_t e = event_queue.dequeue();

        switch(e.event_type) {
                
            case EVENT_SWITCH:
                if(e.value == 0) {
                    switch(e.index) {
                        case SW_SEQ_VIEW: seq_pressed = true; break;
                        case SW_MOD_ENV_VIEW: env_pressed = true; break;
                        case SW_SYNTH_1_VIEW: sy1_pressed = true; break;
                        case SW_PLAY: play_pressed = true; break;
                        default: break;
                    }
                }
                break;

            case EVENT_POT:
            case EVENT_ENCODER:
            default:
                break;        

        }

    }
    if(seq_pressed && env_pressed && sy1_pressed && play_pressed) return true;

    return false;

}

bool eventProcessorUpdate() {

    bool event_happened_ui = false;

    event_happened_ui = processUiEvents();

    return event_happened_ui /* || other event || etc... */;
}