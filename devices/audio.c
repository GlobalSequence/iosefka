#include "audio.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "../common/globals.h"
#include "../common/test.h"

volatile bool render_trigger = false;

// the coveted audio buffers
uint16_t dac_buf[DAC_OUTPUT_BUF_SIZE]__attribute__((aligned(DAC_OUTPUT_BUF_SIZE * 2)));
uint16_t adc_buf[ADC_INPUT_BUF_SIZE]__attribute__((aligned(ADC_INPUT_BUF_SIZE * 2)));

uint16_t *adc_double_buf_ptr;

uint16_t audio_buf_1[AUDIO_BLOCK_SIZE]__attribute__((aligned(AUDIO_BLOCK_SIZE * 2)));
uint16_t audio_buf_2[AUDIO_BLOCK_SIZE]__attribute__((aligned(AUDIO_BLOCK_SIZE * 2)));

static uint8_t dac_chan;
static uint8_t adc_chan;
static uint8_t int_chan;
static uint8_t irq_num;

static uint16_t counter;
// static uint16_t counter_adc_buf;
static uint8_t counter_adc_buf;
static uint16_t input_dummy;

bool transmitting_buffer_two;

void audioSetup(uint8_t dac_dma_channel, uint8_t adc_dma_channel, uint8_t int_channel, uint8_t irq_number) {

    dac_chan = dac_dma_channel;
    adc_chan = adc_dma_channel;
    // the adc and dac dma are chained together. the int channel determines which one triggers the chain
    int_chan = int_channel;
    irq_num = irq_number;

    // the pointer will always be on the other half of the buffer than is being filled
    adc_double_buf_ptr = &adc_buf[AUDIO_BLOCK_SIZE];

}

void audioIrqHandler() {

    bool execute = false;

    if(!irq_num) {

        execute = dma_channel_get_irq0_status(int_chan);

    } else {

        execute = dma_channel_get_irq1_status(int_chan);

    }

    if(execute) {

        if(!irq_num) {

            dma_hw->ints0 = 1u << int_chan;

        } else {

            dma_hw->ints1 = 1u << int_chan;

        }

        uint32_t dma_read_index = dma_channel_hw_addr(dac_chan)->read_addr;

        if(dma_read_index == (uint32_t)&dac_buf[0]) {

            transmitting_buffer_two = true;
            render_trigger = true;

        } else if (dma_read_index == (uint32_t)&dac_buf[DAC_SAMPLES_PER_AUDIO_BLOCK]) {
            
            transmitting_buffer_two = false;
            render_trigger = true;



        }

        uint16_t *adc_buf_ptr = &adc_buf[counter_adc_buf];

        if(counter & 0x1) {

            // adc_buf_ptr = &input_dummy;

        } else { 

            // adc_buf_ptr = &adc_buf[counter_adc_buf];

            counter_adc_buf++;

            if(!counter_adc_buf) {

                adc_double_buf_ptr = &adc_buf[AUDIO_BLOCK_SIZE];

            } else if(counter_adc_buf == AUDIO_BLOCK_SIZE) {
             
                adc_double_buf_ptr = &adc_buf[0];

            }

        }

        dma_channel_set_write_addr(adc_chan, adc_buf_ptr, false);

        counter++;

    }

}

void audioFillOutputBuffer() {

    uint16_t buf_ctr;
    uint16_t table_start;
    if(transmitting_buffer_two) {
        buf_ctr = 256;
        table_start = 128;
    }
    else {
        buf_ctr = 0;
        table_start = 0;
    }

    for(int i = 0; i < AUDIO_BLOCK_SIZE; i++) {
        // dac_buf[buf_ctr++] = audio_buf_1[i] | DAC_CONFIG_CHAN_A;
        // dac_buf[buf_ctr++] = audio_buf_2[i] | DAC_CONFIG_CHAN_B;
        dac_buf[buf_ctr++] = DAC_CONFIG_CHAN_B | (audio_buf_1[i] & 0xFFF);
        dac_buf[buf_ctr++] = DAC_CONFIG_CHAN_A | (audio_buf_2[i] & 0xFFF);
    }
    
}

// bool audioOutputTest() {

//     static bool last_buffer_transmitted;
//     if(transmitting_buffer_two != last_buffer_transmitted) {

//         // static unsigned long period;
//         // unsigned long now = time_us_64();

//         // TEST_PRINT_ONE_NL(now - period)

//         // period = now;

//         // printf("K\n");

//         last_buffer_transmitted = transmitting_buffer_two;  
//         uint16_t buf_ctr;
//         uint16_t table_start;
//         if(!transmitting_buffer_two) {
//             buf_ctr = 256;
//             table_start = 128;
//         }
//         else {
//             buf_ctr = 0;
//             table_start = 0;
//         }

//         for(int i = 0; i < AUDIO_BLOCK_SIZE; i++) {
//             dac_buf[buf_ctr++] = sin_wavetable[table_start + i] | DAC_CONFIG_CHAN_A;
//             dac_buf[buf_ctr++] = i << 5 | DAC_CONFIG_CHAN_B;
//         }

//         return true;
//     }

//     return false;

// }

uint16_t *audioGetBlockPointer1() {

    return audio_buf_1;

}

uint16_t *audioGetBlockPointer2() {

    return audio_buf_2;

}

uint16_t *audioGetAdcBufferPointer() {

    return adc_double_buf_ptr;

}

volatile bool *audioGetRenderTrigger() {

    return &render_trigger;

}