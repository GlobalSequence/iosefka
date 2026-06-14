// Copyright (c) 2019 Mostly Megan
// Copyright (c) 2026 Cory Oleson
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
//
// This file is derived from MostlyMegan's code found here:
// https://www.reddit.com/r/raspberrypipico/comments/pacarb/sharing_some_c_code_to_read_a_rotary_encoder/
//
// Modifications by Cory Oleson:
// - Changed some text

#include "encoder.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"

static uint8_t enc_pin_a, enc_pin_b;
static bool event_occurred;
static int dir_val;

void encoderGetPins(uint8_t pin_a) {

    enc_pin_a = pin_a;
    enc_pin_b = enc_pin_a + 1;

}

// code provided by u/mostly_megan on Reddit
void encoderCallback(uint gpio, uint32_t events) {

	uint32_t gpio_state = 0;

	gpio_state = (gpio_get_all() >> enc_pin_a) & 0b011;  	
	
	static bool ccw_fall = 0;  //bool used when falling edge is triggered
	static bool cw_fall = 0;
	
	uint8_t enc_value = 0;
	enc_value = (gpio_state & 0x03);

	
	if (gpio == enc_pin_a) 
	{
		if ((!cw_fall) && (enc_value == 0b10)) // cw_fall is set to TRUE when phase A interrupt is triggered
			cw_fall = 1; 

		if ((ccw_fall) && (enc_value == 0b00)) // if ccw_fall is already set to true from a previous B phase trigger, the ccw event will be triggered 
		{
			cw_fall = 0;
			ccw_fall = 0;

            event_occurred = true;
            dir_val = 1;
		}

	}	


	if (gpio == enc_pin_b) 
	{
		if ((!ccw_fall) && (enc_value == 0b01)) //ccw leading edge is true
			ccw_fall = 1;

		if ((cw_fall) && (enc_value == 0b00)) //cw trigger
		{
			cw_fall = 0;
			ccw_fall = 0;

            event_occurred = true;
            dir_val = -1;
		}

	}
	

}

bool encoderDidEventOccur() {

    if(event_occurred) {
        
        event_occurred = false;
        return true;

    }

    return false;

}

int encoderGetDirectionValue() {

    return dir_val;

}

// DEPRECATED
/* 

#include "pio_rotary_encoder.pio.h"

//TODO: CHANGE THIS TO AN INT, THIS LIBRARY WILL NEED TO COUNT
//DUE TO THE INTERRUPTS, OTHERWISE POLLING IS WEIRD
static PIO pio;
static uint sm;
static int encoder_pulse_count_;
static int pin_a;
static int pin_b;

static bool encoder_advanced;
static int8_t encoder_out;

static int min;
static int max;

static void pio_irq_handler() {
    // Test if irq 0 was raised
    if (pio0_hw->irq & 1) {
        encoder_pulse_count_ = encoder_pulse_count_ - 1;
    }
    // Test if irq 1 was raised
    if (pio0_hw->irq & 2) {
        encoder_pulse_count_ = encoder_pulse_count_ + 1;
    }
    pio0_hw->irq = 3;
}

void encoderIrqHandler() {
    // Test if irq 0 was raised
    if (pio->irq & 1) {
        encoder_pulse_count_ = encoder_pulse_count_ - 1;
    }
    // Test if irq 1 was raised
    if (pio->irq & 2) {
        encoder_pulse_count_ = encoder_pulse_count_ + 1;
    }
    pio0_hw->irq = 3;
}

// TODO: Remove excess parameters
void encoderSetup(PIO pio_n, uint sm_n, uint pin) {
    // Instantiate pio
    pio = pio_n;
}

bool encoderAdvanced() { 

    int8_t pulse_pos_max = 4;
    int8_t pulse_neg_max = -4;

    if(encoder_out == -1 && encoder_pulse_count_ > 0) {
        pulse_pos_max = 2;
    } else if(encoder_out == 1 && encoder_pulse_count_ < 0) {
        pulse_neg_max = -2;
    }

    if(encoder_pulse_count_ >= pulse_pos_max) {
        encoder_out = 1;
        encoder_pulse_count_ = 0;
        return true;
    } else if (encoder_pulse_count_ <= pulse_neg_max) {
        encoder_out = -1;
        encoder_pulse_count_ = 0;
        return true;
    }

    return false;
}

int encoderGetValue() {
    return encoder_out;
}

int encoderGetCountRaw() {
    return encoder_pulse_count_;
} */