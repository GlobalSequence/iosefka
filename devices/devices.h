#ifndef DEVICES_H
#define DEVICES_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../devices/ssd1306.h"

enum gpio_exp_enum {
    GPIO_EXP_KEYS,
    GPIO_EXP_LEDS,
    
    NUM_GPIO_EXP
};

typedef enum devices_disp_write_mode_t {

    DISP_ADD,
    DISP_SUB,
    DISP_INV,

} devices_disp_write_mode_t;

#ifdef __cplusplus
extern "C"
{
#endif

void devicesSetup();
void devicesSetupCore1();

uint16_t devicesMuxSelectAndRead(uint8_t adc_input, uint8_t mux_input);

bool devicesEncoderAdvanced();
int8_t devicesEncoderGetValue();

uint8_t devicesGpioExpRead(uint8_t index, bool port_b);
void devicesGpioExpWrite(uint8_t leds_to_write);

void devicesDisplayClear();
void devicesDisplayDrawTextNormFont(const char* text, uint8_t x, uint8_t y, pico_ssd1306::WriteMode write_mode);
void devicesDisplayDrawRectangle(uint8_t x, uint8_t y, uint8_t x_length, uint8_t y_height, bool fill, pico_ssd1306::WriteMode write_mode);
void devicesDisplayDrawBitmap(uint8_t x, uint8_t y, uint8_t x_length, uint8_t y_height, uint8_t *image, pico_ssd1306::WriteMode write_mode);
void devicesDisplayDrawLine(uint8_t x, uint8_t y, uint8_t l, uint8_t w, pico_ssd1306::WriteMode write_mode);
void devicesDisplaySetPixel(uint8_t x, uint8_t y, pico_ssd1306::WriteMode write_mode);
void devicesDisplayDisplay();

void devicesStoragePut(uint8_t *ptr, uint address, size_t size);
void devicesStorageGet(uint8_t *ptr, uint address, size_t size);

#ifdef __cplusplus
}
#endif

#endif
