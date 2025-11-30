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

    // testSwitchReader();

}

void potEvent(uint8_t index, uint8_t value) {

    pot_index_t pot = (pot_index_t)index;
    switch_reader_t *s = switchReaderGetStructPtr();

    parameterSetValueWithPot(pot, 
        value, 
        s->num_bottom_keys_pressed, 
        s->shift, 
        s->state_bottom);

    // for(int i = 0; i < NUMBER_OF_PARAMETERS_PER_LAYER; i++) {

    //     uint8_t val = parameterGetParamFromCurrentLayer(i);
    //     TEST_PRINT_ONE_TAB(val)

    // }
    // TEST_PRINT_NL

}

void encoderEvent(int8_t value) {

    switch_reader_t *s = switchReaderGetStructPtr();

    encoderMapAssignValue(value, s->encoder_switch);

    displayShowEncoderModeCountdown();

    // TEST_PRINT_TEXT("ENCODER: ")
    // TEST_PRINT_ONE_NL(encoderMapGetCount());

}

bool processUiEvents() {

    bool event_happened = false;

    while(!event_queue.isEmpty()) {

        event_t e = event_queue.dequeue();

        switch(e.event_type) {
                
            case EVENT_SWITCH:

                switchEvent(e.index, e.value);

                // TEST_PRINT_TEXT("SWITCH: ")
                // TEST_PRINT_ONE_NL(e.index)

                break;

            case EVENT_POT:

                potEvent(e.index, e.value);

                break;

            case EVENT_ENCODER:

                encoderEvent(e.value);

                break;        

        }

        // TEST_PRINT_TWO_NL(e.index, e.value)
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



void testSwitchReader() {

    // switch_reader_t *s = switchReaderGetStructPtr();

    // TEST_PRINT_TEXT("STATE: ")

    // for(int i = 0; i < NUMBER_OF_SWITCHES_TOTAL; i++) {

    //     TEST_PRINT_ONE_TAB(s->state[i])

    // }

    // TEST_PRINT_NL

    // TEST_PRINT_TEXT("TIMEOUT: ")

    // for(int i = 0; i < NUMBER_OF_SWITCHES_TOTAL; i++) {

    //     TEST_PRINT_ONE_TAB(s->timeout[i])

    // }

    // TEST_PRINT_NL

    // TEST_PRINT_TEXT("SHIFT: ")
    // TEST_PRINT_ONE_NL(s->shift)
    // TEST_PRINT_TEXT("ALL: ")
    // TEST_PRINT_ONE_NL(s->num_all_pressed)
    // TEST_PRINT_TEXT("KEYS: ")
    // TEST_PRINT_ONE_NL(s->num_keys_pressed)
    // TEST_PRINT_TEXT("BOTTOM KEYS COUNT: ")
    // TEST_PRINT_ONE_NL(s->num_bottom_keys_pressed)
    // TEST_PRINT_TEXT("BOTTOM KEYS PRESSED")

    // for(int i = 0; i < NUMBER_OF_BOTTOM_SWITCHES; i++) {

    //     TEST_PRINT_ONE_TAB(s->state_bottom[i])

    // }
    // TEST_PRINT_NL

}