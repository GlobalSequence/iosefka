#include "encoder.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"

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
        encoder_pulse_count_ = encoder_pulse_count_ + 1;
    }
    // Test if irq 1 was raised
    if (pio->irq & 2) {
        encoder_pulse_count_ = encoder_pulse_count_ - 1;
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
}
