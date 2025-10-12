#ifndef SWITCH_READER_H
#define SWITCH_READER_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "common/globals.h"

typedef struct switch_reader_t {

    bool state[NUMBER_OF_SWITCHES_TOTAL]            = { 0 };
    bool timeout[NUMBER_OF_SWITCHES_TOTAL]          = { 0 };
    bool state_bottom[NUMBER_OF_BOTTOM_SWITCHES]    = { 0 };
    bool shift                                      = false;
    bool encoder_switch                             = false;
    uint8_t num_all_pressed                         = 0;
    uint8_t num_keys_pressed                        = 0;
    uint8_t num_bottom_keys_pressed                 = 0;

} switch_reader_t;

#ifdef __cplusplus
extern "C"
{
#endif

void switchReaderSubmit(switch_id_t sw, bool pressed);

switch_reader_t* switchReaderGetStructPtr();

#ifdef __cplusplus
}
#endif

#endif
