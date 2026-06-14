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

#include <math.h>

#include "ui.h"
#include "responsive_analog_read.h"
#include "../common/test.h"
#include "../common/ring_buffer.h"

#include "../devices/devices.h"

#include "../devices/peripheral_config.h"
#include "../devices/pin_assignments.h"
#include "../devices/cd4051.h"
#include "../devices/mcp230xx.h"
#include "../devices/encoder.h"
#include "../devices/ssd1306.h"

RingBuffer<event_t, EVENT_QUEUE_SIZE> event_queue;

// ################################
// UI TIMER DEFINES
// ################################

const unsigned long k_ui_timer_interval_us_pots = 2000;
const unsigned long k_ui_timer_interval_us_gpio_exp_port = 2000;
const unsigned long k_ui_timer_interval_us_encoder = 500;
unsigned long ui_timer_last_update_time_pots = 0;
unsigned long ui_timer_last_update_time_gpio_exp_port = 0;
unsigned long ui_timer_last_update_time_encoder = 0;

// ################################
// POT MUX DEFINES (pot mux == cd4051)
// ################################

static ResponsiveAnalogRead pot[k_num_mux_inputs_total];
bool lock_pot_read[k_num_mux_inputs_total];
uint16_t pot_val_upon_locking[k_num_mux_inputs_total];
const int k_pot_lock_timeout = 1000;
const uint8_t k_pot_unlock_threshold = 8;
bool pot_sleep_timer_active[k_num_mux_inputs_total];
unsigned long pot_began_sleeping_time[k_num_mux_inputs_total];

// temporary value to check potentiometer read
uint16_t param_val[k_num_mux_inputs_total];

// ################################
// ENCODER DEFINES
// ################################

static bool encoder_loop = true;
static uint16_t last_encoder_val = 0;

// ################################
// GPIO EXPANDER DEFINES (gpio expander == mcp23017)
// ################################

static mcp23017_t mcp23017[NUM_GPIO_EXP] = {
    {I2C_DEVICES, I2C_MCP_ADDR_1, 0b1111111111111111},
    {I2C_DEVICES, I2C_MCP_ADDR_2, 0b1111111100000000}
};

static bool gpio_exp_transmit_allow = false;
static bool gpio_exp_transmit_complete = false;

bool gpio_exp_transmit_leds;
uint8_t gpio_exp_led_bits;
static uint8_t gpio_exp_port;
static uint8_t gpio_exp_port_val[NUM_GPIO_EXP_PORTS];
static bool last_switch_val[MCP23017_NUM_GPIO * NUM_GPIO_EXP];
static uint8_t gpio_exp_led_array;

// ################################
// PRIVATE FUNCTION DEFINES
// ################################

void lockAllPotReads();
void potentiometerReadSetup();
uint16_t getRawPotVal();
uint16_t getRawPotValSelect(int adc, int mux_input);
uint16_t getPotValSelectShiftRight3(int adc, int mux_input);
uint8_t getSmoothPotVal(int index);

void testDisplayPtr();

// ################################
// PUBLIC FUNCTIONS
// ################################

void uiSetup() {

    potentiometerReadSetup();

    // initialize the switch event trigger bool to high to match the pulled-up gpios
    for(int i = 0; i < MCP23017_NUM_GPIO * NUM_GPIO_EXP; i++) {
        last_switch_val[i] = 1;
    }

}

void uiSetEncoderLoop(bool loop) {
    encoder_loop = loop;
}

// ################################
// PRIVATE FUNCTIONS
// ################################

void uiUpdatePotentiometers(uint8_t i) {

    // read the adc for the current mux input
    uint16_t val = getPotValSelectShiftRight3(i >> 3, i & 0x07);

    // check to see if the pot read is unlocked
    if(!lock_pot_read[i]) {

        // update the pot value smoothing
        pot[i].update(val);

        // get the smoothed pot value
        int16_t smooth_val = getSmoothPotVal(i);

        // check to see if the value has changed
        if(pot[i].hasChanged()) {

            // get the responsive value and create event struct
            event_t e = {EVENT_POT, i, smooth_val};

            // put the value into the queue
            event_queue.enqueue(e);
        }

        // check to see if the pot is 'sleeping' 
        if(pot[i].isSleeping()) {

            // get the current time in milliseconds
            unsigned long now = to_ms_since_boot(get_absolute_time());

            // if the sleep timer isn't active
            if(!pot_sleep_timer_active[i]) {

                // activate it
                pot_sleep_timer_active[i] = true;

                // and get a timestamp
                pot_began_sleeping_time[i] = now;

            } else {

                // if the timeout has elapsed since the pot started sleeping
                if(now - pot_began_sleeping_time[i] >= k_pot_lock_timeout) {

                    // reset sleep timer flag
                    pot_sleep_timer_active[i] = false;

                    // lock the pot read
                    lock_pot_read[i] = true;

                    // take the value of the pot when it was locked
                    pot_val_upon_locking[i] = smooth_val;
                }   
            }
        } else {
            
            // if we're not sleeping, the timer shouldn't be active! 
            pot_sleep_timer_active[i] = false;
        }
    
    // if the pot is currently locked
    } else {

        // check to see if the pot value has exceeded the unlock threshold
        // we truncate the 9-bit val >> 1 to compare to 8 bit stored value
        if(abs((val >> 1) - pot_val_upon_locking[i]) >= k_pot_unlock_threshold) {
            
            // unlock pot read
            lock_pot_read[i] = false;
        }

    }

}

void uiUpdateEncoder() {

    if(devicesEncoderAdvanced()) {

        int8_t encoder_val = devicesEncoderGetValue();

        // create event struct
        event_t e = {EVENT_ENCODER, 0, encoder_val};

        // put the value into queue
        event_queue.enqueue(e);

    }
}

void uiUpdateGpioExpanders(uint8_t i) {

    // "i" represents the 4 ports across 2 gpio expanders, 
    // every other port is port b on the respective expander
    bool port_b = i % 2;
    bool gpio_exp_active = i / 2;

    // if "i" is not the number of the dedicated LED port...
    if(i != GPIO_EXP_PORTS_LEDS) {

        // read the port
        gpio_exp_port_val[i] = devicesGpioExpRead(gpio_exp_active, port_b);

        // loop to extract each bit from the gpio expander port output
        for(int j = 0; j < MCP23017_NUM_GPIO_PER_PORT; j++) {

            // get the consecutive index of the switch
            uint8_t k = (i * MCP23017_NUM_GPIO_PER_PORT) + j;

            // said extraction
            bool switch_val = (gpio_exp_port_val[i] >> j) & 1;

            // check for changed value
            if(switch_val != last_switch_val[k]) {

                // record the switch value
                last_switch_val[k] = switch_val;

                // create event struct
                event_t e = {EVENT_SWITCH, k, switch_val};

                // put the value into the queue
                event_queue.enqueue(e);
            }

        }

    // if it IS the dedicated LED port heh heh
    } else {

        // if the LED transmit flag is high
        if(gpio_exp_transmit_leds) {

            // reset the LED transmit flag
            gpio_exp_transmit_leds = false;

            // write the LEDs to the port
            devicesGpioExpWrite(gpio_exp_led_bits);
        }
    }

}

void lockAllPotReads() {

    for(int i = 0; i < k_num_mux_inputs_total; i++) {
        
        // lock the pot read
        lock_pot_read[i] = true;

        // set the lock val to the current smooth value
        pot_val_upon_locking[i] = getSmoothPotVal(i); 
    }
}

void potentiometerReadSetup() {

    // for selecting the mux
    for(int i = 0; i < NUM_MUXS; i++) {

        // for selecting the mux input
        for(int j = 0; j < NUM_MUX_INPUTS; j++) { 
            
            // get the consecutive index of pots
            uint8_t k = (i * NUM_MUX_INPUTS) + j;
            
            // initialize our smoothing library
            // NOTE: We make the library 9 bits, and truncate to 8 bits
            //       because if we just use 8 bits, the higest value never reaches 255
            pot[k].begin(true, 0.01);
            pot[k].setAnalogResolution(512);
            pot[k].setActivityThreshold(8);

            // loop through the update twice so the smoothing library has a read of the current value
            for(int l = 0; l < 2; l++) {

                pot[k].update(getPotValSelectShiftRight3(i, j));

            }
        }
    }
    lockAllPotReads();
}

uint16_t getRawPotVal() {
    return cd4051ReadCurrentAdc() >> 3;
}

uint16_t getPotValSelectShiftRight3(int adc, int mux_input) {
    return getRawPotValSelect(adc, mux_input) >> 3;
}

uint16_t getRawPotValSelect(int adc, int mux_input) {
    return devicesMuxSelectAndRead(adc, mux_input);
}

uint8_t getSmoothPotVal(int index) {
    return pot[index].getValue() >> 1;
}

uint16_t uiGetAdcValueForRandSeed() {
    
    uint16_t v_1 = cd4051ReadSelectedAdc(0);
    
    uint16_t v_2 = cd4051ReadSelectedAdc(1);

    return (v_1 << 4) ^ v_2;
}

// DEPRECATED FUNCTIONS

void uiUpdate() {

    uint64_t now = time_us_64();

    if((now - ui_timer_last_update_time_pots) >= k_ui_timer_interval_us_pots) {
        for(int i = 0; i < k_num_mux_inputs_total; i++) {
            uiUpdatePotentiometers(i);
        }
    }

    if((now - ui_timer_last_update_time_gpio_exp_port) >= k_ui_timer_interval_us_gpio_exp_port) {
        uiUpdateGpioExpanders(gpio_exp_port);
        gpio_exp_port == NUM_GPIO_EXP_PORTS - 1? gpio_exp_port = 0 : gpio_exp_port ++;
    }

    if((now - ui_timer_last_update_time_encoder) >= k_ui_timer_interval_us_encoder) {
        uiUpdateEncoder();
    }

}