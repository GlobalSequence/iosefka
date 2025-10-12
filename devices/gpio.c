#include "gpio.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pin_assignments.h"
#include "../common/test.h"

static int pin_a, pin_b;
static uint8_t state_val;

void gpioSetup(int a, int b) {

    pin_a = a;
    pin_b = b;

}

uint8_t* gpioGetStateValPointer() {

    return &state_val;

}

void gpioSetTwo() {

    bool a, b;

    a = state_val & 1;
    b = (state_val >> 1) & 1;

    gpio_put(pin_a, a);
    gpio_put(pin_b, b);

}