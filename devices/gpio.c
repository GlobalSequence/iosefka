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