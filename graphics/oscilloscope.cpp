#include "oscilloscope.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/test.h"
#include "../common/globals.h"

// static uint16_t* buf_pointer;
// static bool* reset_pointer;

static uint8_t scope_buffer[SCOPE_BUFFER_SIZE];
static uint8_t read_buffer[SCOPE_READ_BUFFER_SIZE];
static uint8_t* scope_active_buffer;

// number of waves that appear on screen at once
static uint8_t time_div_ctrl;
static uint8_t time_div[] = {0, 1, 3, 7};
static uint8_t modulo_ctrl;
static uint16_t modulo[] = {16, 32, 64, 128, 256};
static uint8_t change_buffer_counter;

static uint trigger_index;
// static bool trigger_index_collected;

static uint8_t scope_buffer_index;
// static int16_t last_read_index;
// static bool buffer_half_changed;

static uint8_t window;

uint8_t count;

bool pause_buffer_fill; 

// private function declarations

void fillReadBuffer(uint16_t* buf_ptr);
void getTriggerIndex(bool* reset_ptr);
void fillScopeBuffer();
// void iterateWindow();

// public functions

void oscilloscopeProcess(uint16_t* buf_ptr, bool* reset_ptr) {

    getTriggerIndex(reset_ptr);
    fillReadBuffer(buf_ptr);
    fillScopeBuffer();
    // iterateWindow();

}

void oscilloscopeSetTimeDiv(uint8_t div) { 

    if(div <= 3) time_div_ctrl = div;
    scope_buffer_index = scope_buffer_index > SCOPE_HALF_BUFFER_SIZE ? 0 : SCOPE_HALF_BUFFER_SIZE;

}

void oscilloscopeSetModulo(uint8_t mod) {

    if(mod <= 4) modulo_ctrl = mod;

}

uint8_t* oscilloscopeGetActiveBuffer() {

    scope_active_buffer = scope_buffer_index >= SCOPE_HALF_BUFFER_SIZE ? &scope_buffer[0] : &scope_buffer[SCOPE_HALF_BUFFER_SIZE];

    // TEST_PRINT_ONE_NL(scope_buffer_index >= SCOPE_HALF_BUFFER_SIZE ? 1 : 0)
    // scope_active_buffer = &scope_buffer[0];

    return scope_active_buffer;

}

// private functions

void fillReadBuffer(uint16_t* buf_ptr) {

    for(int i = 0; i < SCOPE_HALF_BUFFER_SIZE; i++) {

        // get the index of the upper half of the buffer
        uint8_t upper_half_index = i + SCOPE_HALF_BUFFER_SIZE;

        // write the second half of the buffer to the first half
        read_buffer[i] = read_buffer[upper_half_index];

        // fill the second half of the buffer with the input
        read_buffer[upper_half_index] = buf_ptr[i] >> 7;

    }

}

void getTriggerIndex(bool* reset_ptr) {

    uint8_t window_limit = time_div[time_div_ctrl];

    // try on every loop until a trigger index is collected
    if(window >= window_limit || pause_buffer_fill) {

        window = 0;

        bool trigger_index_collected = false;

        for(int i = 0; i < AUDIO_BLOCK_SIZE; i++) {

            if(reset_ptr[i]) {

                trigger_index = i;
                trigger_index_collected = true;
                pause_buffer_fill = false;
                return;

            } 

        }

        if(!trigger_index_collected) {

            // try to get the trigger_index on the next pass
            pause_buffer_fill = true;

        }

    } else {

        window++;

    }

    // // find the next trigger index
    // for(int i = 0; i < AUDIO_BLOCK_SIZE; i++) {

    //     if(reset_ptr[i] && !trigger_index_collected /* && !buffer_count */) {
            
    //         // if this is the first buffer in the division series, only collect the first trigger of the buffer
    //         trigger_index_collected = true;

    //         // get the trigger index
    //         trigger_index = i;


    //     }
    // }

}

void fillScopeBuffer() {

    if((scope_buffer_index % /* modulo[modulo_ctrl] */128 == 0) && pause_buffer_fill) return;

    // get the value for the time division
    uint8_t time_division = time_div[time_div_ctrl] + 1;
    // uint8_t time_division = 7;

    for(int i = 0; i < SCOPE_HALF_BUFFER_SIZE; i++) {

        // get the start index for the input buffer
        uint8_t input_buf_index = trigger_index + i;

        // see if the counter has overflowed the time division
        uint8_t check = count % time_division;

        // increment the counter 
        count++;

        // if we've surpassed the increment, add a sample to the buffer
        if(!check) {

            // fill the next buffer index from the read buffer index
            scope_buffer[scope_buffer_index] = read_buffer[input_buf_index];

            // increment the scope buffer index
            // no additional work needs to be done here since our scope buffer is size 256
            // if the buffer size changes, we'll have to reset the buffer index every time it's full
            scope_buffer_index++;

        }
        
    }

    // TEST_PRINT_TWO_NL(scope_buffer_index, window)

}

// void iterateWindow() {

//     uint8_t time_division = time_div[time_div_ctrl];

//     uint8_t iterations_until_reset;

//     if(time_division == 0 || time_division == 1) iterations_until_reset = 0;
//     else iterations_until_reset = (time_division + 1) >> 1;

//     if(window >= time_division) {

//         window = 0;
//         trigger_index_collected = false;
//         // scope_buffer_index = 0;

//     } else {

//         window++;
    
//     }

// }

/* 
NOTES
- RESET 'TRIGGER INDEX COLLECTED' ONCE BUFFER IS FULL
- 

*/

// uint8_t current_window;
// bool flip_buffer_half;

// void getTriggerIndex(bool* reset_ptr) {

//     uint8_t window_amount = time_div[time_div_ctrl];

//     if(current_window >= window_amount) {

//         current_window = 0; 
//         flip_buffer_half = !flip_buffer_half;

//             // find the next trigger index
//         for(int i = 0; i < AUDIO_BLOCK_SIZE; i++) {

//             if(reset_ptr[i]) {

//                 // get the trigger index
//                 trigger_index = i;
//                 return;

//             }
        
//         }

//     } else {

//         current_window++;


//     }

// }

// uint8_t input_buffer[AUDIO_BLOCK_SIZE * 8];

// void fillReadBuffer(uint16_t* buf_ptr) {

//     uint input_start_index = (AUDIO_BLOCK_SIZE * current_window);

//     for(int i = 0; i < SCOPE_HALF_BUFFER_SIZE; i++) {

//         uint j = input_start_index + i;

//         input_buffer[j] = buf_ptr[i] >> 7;

//     }

// }



// void fillScopeBuffer() {

//     uint8_t window_amount = time_div[time_div_ctrl];

//     if(current_window == window_amount) {

//         uint8_t write_start_index = flip_buffer_half ? SCOPE_HALF_BUFFER_SIZE : 0;
//         uint8_t count_delta = window_amount + 1;
//         uint limit = AUDIO_BLOCK_SIZE * (window_amount + 1);

//         for(int i = 0; i < AUDIO_BLOCK_SIZE; i++) {

//             uint r_i = trigger_index + (i * count_delta);
//             // uint r_i = i * count_delta;
//             uint8_t w_i = write_start_index + i;

//             if(r_i >= limit) r_i -= limit;
            
//             scope_buffer[w_i] = input_buffer[r_i];

//         }

//     }

// }

// uint8_t* oscilloscopeGetActiveBuffer() {

//     scope_active_buffer = flip_buffer_half ? &scope_buffer[SCOPE_HALF_BUFFER_SIZE] : &scope_buffer[0];

//     return scope_active_buffer;

// }