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
        // encoder_pulse_count_ = encoder_pulse_count_ + 1;
        encoder_pulse_count_ = encoder_pulse_count_ - 1;
    }
    // Test if irq 1 was raised
    if (pio0_hw->irq & 2) {
        // encoder_pulse_count_ = encoder_pulse_count_ - 1;
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

void encoderSetup(PIO pio_n, uint sm_n, uint pin) {
    // // Instantiate pio
    pio = pio_n;
    // PIO pio = pio_n;
    // uint8_t sm = sm_n; 

    // pin_a = pin;
    // pin_b = pin + 1;

    // // Configure pins as input with pullup
    // pio_gpio_init(pio, pin_a);
    // gpio_set_pulls(pin_a, true, false);
    // gpio_set_pulls(pin_b, true, false);
    // // Load encoder program into pio memory
    // uint offset = pio_add_program(pio, &pio_rotary_encoder_program);
    // // Make sm config
    // pio_sm_config c = pio_rotary_encoder_program_get_default_config(offset);
    // // Set 'in' pins
    // sm_config_set_in_pins(&c, pin_a);
    // // Set shift to left: bits shifted by 'in' to enter at LSB w/o autopush
    // sm_config_set_in_shift(&c, false, false, 0);
    // // Set IRQ handler
    // irq_set_exclusive_handler(PIO0_IRQ_0, pio_irq_handler);
    // // Enable the IRQ
    // irq_set_enabled(PIO0_IRQ_0, true);
    // pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
    // // Init the sm.
    // // Note: the program starts after the jump table -> initial_pc = 16
    // pio_sm_init(pio, sm, 16, &c);
    // // Enable th sm
    // pio_sm_set_enabled(pio, sm, true); 

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
