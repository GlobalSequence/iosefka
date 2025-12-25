#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/globals.h"

#define DAC_CONFIG_CHAN_A 0b0011000000000000
#define DAC_CONFIG_CHAN_B 0b1011000000000000
#define DAC_ADC_SPI_BIT_WIDTH 16
#define DAC_OUTPUT_BUF_HALF_SIZE_MASK DAC_SAMPLES_PER_AUDIO_BLOCK - 1
#define ADC_INPUT_BUF_SIZE_MASK ADC_INPUT_BUF_SIZE - 1
#define BIT_WIDTH_ADD_8_TO_16 1
#define BIT_WIDTH_MULT_8_TO_16 2

// the coveted audio buffers
extern uint16_t dac_buf[DAC_OUTPUT_BUF_SIZE]__attribute__((aligned(DAC_OUTPUT_BUF_SIZE * BIT_WIDTH_MULT_8_TO_16)));
extern uint16_t adc_buf[ADC_INPUT_BUF_SIZE]__attribute__((aligned(ADC_INPUT_BUF_SIZE * BIT_WIDTH_MULT_8_TO_16)));
extern bool transmitting_buffer_two;

#ifdef __cplusplus
extern "C"
{
#endif

void audioSetup(uint8_t dac_dma_channel, uint8_t adc_dma_channel, uint8_t int_channel, uint8_t irq_number);
void audioIrqHandler();
void audioFillOutputBuffer();
bool audioOutputTest();

uint16_t *audioGetBlockPointer1();
uint16_t *audioGetBlockPointer2();
uint16_t *audioGetAdcBufferPointer();
volatile bool *audioGetRenderTrigger();

#ifdef __cplusplus
}
#endif

#endif
