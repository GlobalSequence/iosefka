#include "cd4051.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "cd4051.h"

uint32_t mux_ctrl_pin_mask;
uint8_t mask_offset;
int mux_ctrl_pin[NUM_MUX_CTRL_PINS];
int analog_read_pin[NUM_MUXS];

void cd4051Setup(int mux_a, int mux_b, int mux_c, int mux_com_1, int mux_com_2) {
    // we are using 3 consecutive pins, in order of mux ctrl pin a, b, c from lowest to highest.
    // we are setting the pins all at once, using a mask to tell which pins to be put
    mux_ctrl_pin_mask = 0x7 << mux_a;
    mask_offset = mux_a;

    mux_ctrl_pin[MUX_CTRL_A] = mux_a;
    mux_ctrl_pin[MUX_CTRL_B] = mux_b;
    mux_ctrl_pin[MUX_CTRL_C] = mux_c;
    analog_read_pin[MUX_COM_1] =  mux_com_1;
    analog_read_pin[MUX_COM_2] = mux_com_2;

    // // initialize the control pins
    // gpio_init(mux_ctrl_pin[MUX_CTRL_A]);
    // gpio_init(mux_ctrl_pin[MUX_CTRL_B]);
    // gpio_init(mux_ctrl_pin[MUX_CTRL_C]);
    // gpio_set_dir(mux_ctrl_pin[MUX_CTRL_A], GPIO_OUT);
    // gpio_set_dir(mux_ctrl_pin[MUX_CTRL_B], GPIO_OUT);
    // gpio_set_dir(mux_ctrl_pin[MUX_CTRL_C], GPIO_OUT);
    // gpio_put(mux_ctrl_pin[MUX_CTRL_A], 0);
    // gpio_put(mux_ctrl_pin[MUX_CTRL_B], 0);
    // gpio_put(mux_ctrl_pin[MUX_CTRL_C], 0);

    // // setup the ADC
    // adc_init();
    // adc_gpio_init(analog_read_pin[MUX_COM_1]);
    // adc_gpio_init(analog_read_pin[MUX_COM_2]);
}

uint16_t cd4051SelectChannelAndRead(uint8_t adc_in_sel, uint8_t mux_in_sel) {
    gpio_put_masked(mux_ctrl_pin_mask, mux_in_sel << mask_offset);
    adc_select_input(adc_in_sel);
    // for using individual gpios, comment out the above, and uncomment this
    // gpio_put(mux_ctrl_pin[MUX_CTRL_A], (mux_in_sel >> MUX_CTRL_A) & 1);
    // gpio_put(mux_ctrl_pin[MUX_CTRL_B], (mux_in_sel >> MUX_CTRL_B) & 1);
    // gpio_put(mux_ctrl_pin[MUX_CTRL_C], (mux_in_sel >> MUX_CTRL_C) & 1);
    
    sleep_us(4);
    return adc_read();
}

void cd4051SelectAdcAndChannel(uint8_t adc_in_sel, uint8_t mux_in_sel) {
    gpio_put_masked(mux_ctrl_pin_mask, mux_in_sel << mask_offset);
    adc_select_input(adc_in_sel);
    // for using individual gpios, comment out the above, and uncomment this
    // gpio_put(mux_ctrl_pin[MUX_CTRL_A], (mux_in_sel >> MUX_CTRL_A) & 1);
    // gpio_put(mux_ctrl_pin[MUX_CTRL_B], (mux_in_sel >> MUX_CTRL_B) & 1);
    // gpio_put(mux_ctrl_pin[MUX_CTRL_C], (mux_in_sel >> MUX_CTRL_C) & 1);
}

uint16_t cd4051ReadCurrentAdc() {
    return adc_read();
}

uint16_t cd4051ReadSelectedAdc(uint8_t input_sel) {
    adc_select_input(input_sel);
    sleep_us(4);
    return adc_read();
}