#ifndef UI_H
#define UI_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/ring_buffer.h"
#include "../devices/ssd1306.h"

// ################################
// EVENT QUEUE DEFINES
// ################################

typedef enum {
    GPIO_EXP_PORTS_KEYS_1,
    GPIO_EXP_PORTS_KEYS_2,
    GPIO_EXP_PORTS_KEYS_3,
    GPIO_EXP_PORTS_LEDS,
    NUM_GPIO_EXP_PORTS
} gpio_exp_port_t;

#define EVENT_QUEUE_SIZE 32

typedef enum {
    EVENT_SWITCH,
    EVENT_POT,
    EVENT_ENCODER,

    NUM_EVENT_TYPES
} event_type_t;

typedef struct {
    event_type_t event_type;
    uint8_t index;
    int16_t value;
} event_t;

extern RingBuffer<event_t, EVENT_QUEUE_SIZE> event_queue;
extern uint8_t gpio_exp_led_bits;
extern bool gpio_exp_transmit_leds;

#ifdef __cplusplus
extern "C"
{
#endif

void uiSetup();

// void uiSetLeds(uint8_t page, uint8_t layer, uint8_t layer_2);
void uiUpdatePotentiometers(uint8_t i);
void uiUpdateGpioExpanders(uint8_t i);
void uiUpdateEncoder();

uint16_t uiGetAdcValueForRandSeed();
pico_ssd1306::SSD1306* uiGetDisplayPtr();

void testDisplayPtr();

// DEPRECATED FUNCTIONS
void uiUpdate();
// we update the i2C GPIO expanders in the downtime on core 1 due to blocking gpio writes
void uiUpdateCore1();
// TODO: add this to the end of the audio loop
void uiAllowGpioExpTransmit();

#ifdef __cplusplus
}
#endif

#endif