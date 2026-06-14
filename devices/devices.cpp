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

#include "devices.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "hardware/dma.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#include "peripheral_config.h"
#include "pin_assignments.h"
#include "cd4051.h"
#include "mcp230xx.h"
#include "encoder.h"
#include "ssd1306.h"
#include "eeprom.h"
#include "audio.h"
#include "pwm.h"
#include "gpio.h"

#include "../graphics/shapeRenderer/ShapeRenderer.h"
#include "../graphics/textRenderer/TextRenderer.h"

#include "pio_rotary_encoder.pio.h"
#include "spi.pio.h"

#include "../common/test.h"

#define UART_BAUD_RATE 31250
#define UART_DATA_BITS 8
#define UART_STOP_BITS 1
#define UART_PARITY UART_PARITY_NONE

typedef struct {
    dma_channel_config config;
    int channel;
} dma_settings_t;

dma_settings_t dma_display;
dma_settings_t dma_dac;
dma_settings_t dma_adc;

// ################################
// DISPLAY DEFINES
// ################################

static pico_ssd1306::SSD1306 display = pico_ssd1306::SSD1306(
    SPI_DISPLAY,
    PIN_DISPLAY_DC,
    true,
    &dma_display.channel,
    pico_ssd1306::Size::W128xH64);

static pico_ssd1306::SSD1306 *display_ptr = &display;

static mcp23017_t mcp23017[NUM_GPIO_EXP] = {
    {I2C_DEVICES, I2C_MCP_ADDR_1, 0b1111111111111111},
    {I2C_DEVICES, I2C_MCP_ADDR_2, 0b1111111100000000}};

// ################################
// PUBLIC FUNCTIONS
// ################################

void devicesSetup()
{
    // ################
    // SPI setup + SPI DMA + devices
    // ################

    spi_init(SPI_DISPLAY, 10000000);
    gpio_set_function(PIN_DISPLAY_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_DISPLAY_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_DISPLAY_CS, GPIO_FUNC_SPI);
    gpio_init(PIN_DISPLAY_DC);
    gpio_set_dir(PIN_DISPLAY_DC, GPIO_OUT);

    dma_display.channel = dma_claim_unused_channel(true);
    dma_display.config = dma_channel_get_default_config(dma_display.channel);
    channel_config_set_transfer_data_size(&dma_display.config, DMA_SIZE_8);
    channel_config_set_read_increment(&dma_display.config, true);
    channel_config_set_write_increment(&dma_display.config, false);
    channel_config_set_dreq(&dma_display.config, DREQ_SPI1_TX);    

    dma_channel_configure(
        dma_display.channel,
        &dma_display.config,
        &spi_get_hw(SPI_DISPLAY)->dr,
        NULL,
        0,
        false
    );    

    display.setup();
    display.setOrientation(0);
    display.prepareBufferDMA();

    // ################
    // UART setup
    // ################

    uart_init(UART_MIDI, UART_BAUD_RATE);
    uart_set_hw_flow(UART_MIDI, false, false);
    uart_set_format(UART_MIDI, UART_DATA_BITS, UART_STOP_BITS, UART_PARITY);
    gpio_set_function(PIN_MIDI_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_MIDI_RX, GPIO_FUNC_UART);

    // ################
    // I2C setup + devices
    // ################

    i2c_init(I2C_DEVICES, I2C_DEVICES_RATE);
    gpio_set_function(PIN_DEVICE_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_DEVICE_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_DEVICE_SDA);
    gpio_pull_up(PIN_DEVICE_SCL);

    mcp23017Setup(&mcp23017[GPIO_EXP_KEYS], I2C_DEVICES, I2C_MCP_ADDR_1, mcp23017[GPIO_EXP_KEYS].direction);

    mcp23017Setup(&mcp23017[GPIO_EXP_LEDS], I2C_DEVICES, I2C_MCP_ADDR_2, mcp23017[GPIO_EXP_LEDS].direction);

    eepromSetup(I2C_DEVICES, I2C_EEPROM_ADDR);

    // ################
    // ADC setup + devices
    // ################

    cd4051Setup(PIN_MUX_CTRL_A, PIN_MUX_CTRL_B, PIN_MUX_CTRL_C, PIN_MUX_ANALOG_1, PIN_MUX_ANALOG_2);

    // initialize the control pins
    gpio_init(PIN_MUX_CTRL_A);
    gpio_init(PIN_MUX_CTRL_B);
    gpio_init(PIN_MUX_CTRL_C);
    gpio_set_dir(PIN_MUX_CTRL_A, GPIO_OUT);
    gpio_set_dir(PIN_MUX_CTRL_B, GPIO_OUT);
    gpio_set_dir(PIN_MUX_CTRL_C, GPIO_OUT);
    gpio_put(PIN_MUX_CTRL_A, 0);
    gpio_put(PIN_MUX_CTRL_B, 0);
    gpio_put(PIN_MUX_CTRL_C, 0);

    // setup the ADC
    adc_init();
    adc_gpio_init(PIN_MUX_ANALOG_1);
    adc_gpio_init(PIN_MUX_ANALOG_2);

    // ################
    // encoder setup + PIO
    // ################

/*     encoderSetup(PIO_ENCODER, PIO_ENCODER_STATE_MACHINE, PIN_ENCODER_A);

    // Configure pins as input with pullup
    pio_gpio_init(PIO_ENCODER, PIN_ENCODER_A);
    gpio_set_pulls(PIN_ENCODER_A, true, false);
    gpio_set_pulls(PIN_ENCODER_B, true, false);
    // Load encoder program into pio memory
    uint offset = pio_add_program(PIO_ENCODER, &pio_rotary_encoder_program);
    // Make sm config
    pio_sm_config c_e = pio_rotary_encoder_program_get_default_config(offset);
    // Set 'in' pins
    sm_config_set_in_pins(&c_e, PIN_ENCODER_A);
    // Set shift to left: bits shifted by 'in' to enter at LSB w/o autopush
    sm_config_set_in_shift(&c_e, false, false, 0);
    // Set IRQ handler
    // irq_set_exclusive_handler(PIO0_IRQ_0, encoderIrqHandler);
    irq_add_shared_handler(PIO0_IRQ_0, encoderIrqHandler, 254);
    // Enable the IRQ
    irq_set_enabled(PIO0_IRQ_0, true);
    pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
    // Init the sm.
    // Note: the program starts after the jump table -> initial_pc = 16
    pio_sm_init(PIO_ENCODER, PIO_ENCODER_STATE_MACHINE, 16, &c_e);
    // Enable th sm
    pio_sm_set_enabled(PIO_ENCODER, PIO_ENCODER_STATE_MACHINE, true);  */

    gpio_init(PIN_ENCODER_A);
    gpio_set_dir(PIN_ENCODER_A,GPIO_IN);
    gpio_pull_up(PIN_ENCODER_A);

    gpio_init(PIN_ENCODER_B);
    gpio_set_dir(PIN_ENCODER_B,GPIO_IN);
    gpio_pull_up(PIN_ENCODER_B);

    encoderGetPins(PIN_ENCODER_A);

    gpio_set_irq_enabled_with_callback(PIN_ENCODER_A, GPIO_IRQ_EDGE_FALL, true, &encoderCallback);
	gpio_set_irq_enabled(PIN_ENCODER_B, GPIO_IRQ_EDGE_FALL, true);

    // ################
    // pwm setup
    // ################

    gpio_set_function(PIN_PWM_CUTOFF, GPIO_FUNC_PWM);
    gpio_set_function(PIN_PWM_RESONANCE, GPIO_FUNC_PWM);
    gpio_set_function(PIN_PWM_VCA_1, GPIO_FUNC_PWM);
    gpio_set_function(PIN_PWM_VCA_2, GPIO_FUNC_PWM);

    uint cutoff_slice_num = pwm_gpio_to_slice_num(PIN_PWM_CUTOFF);
    uint resonance_slice_num = pwm_gpio_to_slice_num(PIN_PWM_RESONANCE);
    uint vca_1_slice_num = pwm_gpio_to_slice_num(PIN_PWM_VCA_1);
    uint vca_2_slice_num = pwm_gpio_to_slice_num(PIN_PWM_VCA_2);

    pwm_set_wrap(cutoff_slice_num, 2048);
    pwm_set_chan_level(cutoff_slice_num, PWM_CHAN_A, 1024);
    pwm_set_wrap(resonance_slice_num, 2048);
    pwm_set_chan_level(resonance_slice_num, PWM_CHAN_B, 1024);
    pwm_set_wrap(vca_1_slice_num, 2048);
    pwm_set_chan_level(vca_1_slice_num, PWM_CHAN_A, 1024);
    pwm_set_wrap(vca_2_slice_num, 2048);
    pwm_set_chan_level(vca_2_slice_num, PWM_CHAN_B, 1024);

    pwm_set_enabled(cutoff_slice_num, true);
    pwm_set_enabled(resonance_slice_num, true);
    pwm_set_enabled(vca_1_slice_num, true);
    pwm_set_enabled(vca_2_slice_num, true);

    pwmSetup(cutoff_slice_num, resonance_slice_num, vca_1_slice_num, vca_2_slice_num);

    // ################
    // gpio setup
    // ################

    gpio_init(PIN_FIL_SEL_1);
    gpio_init(PIN_FIL_SEL_2);
    gpio_set_dir(PIN_FIL_SEL_1, GPIO_OUT);
    gpio_set_dir(PIN_FIL_SEL_2, GPIO_OUT);  

    gpioSetup(PIN_FIL_SEL_1, PIN_FIL_SEL_2);

}

void devicesSetupCore1() {

    // ################
    // DAC/ADC setup + PIO + DMA
    // ################

    // to set the dma ring function, we need the dma buffer size in powers of 2. our buffer size is 512 16-bit integers (512 bytes * 2) meaning we tell the dma to reset at 2^10 bytes. 
    uint dac_buf_size_decrement = DAC_OUTPUT_BUF_SIZE - 1;

    int dac_buf_size_bits = 0;

    while(dac_buf_size_decrement) {

        dac_buf_size_decrement >>= 1;

        dac_buf_size_bits++;

    }

    dac_buf_size_bits += BIT_WIDTH_ADD_8_TO_16;

    float clkdiv = 8.0f;
    uint cpha0_prog_offs = pio_add_program(PIO_AUDIO, &spi_cpha0_cs_program);
    uint prog_offs = cpha0_prog_offs;
    bool cpha = false;
    bool cpol = false;
    uint8_t n_bits = 16;
    PIO pio = pio1;
    uint8_t sm = 0;

    pio_sm_config pc = cpha ? spi_cpha1_cs_program_get_default_config(prog_offs) : spi_cpha0_cs_program_get_default_config(prog_offs);
    sm_config_set_out_pins(&pc, PIN_AUDIO_MOSI, 1);
    sm_config_set_in_pins(&pc, PIN_AUDIO_MISO);
    sm_config_set_sideset_pins(&pc, PIN_AUDIO_SCK);
    sm_config_set_out_shift(&pc, false, true, n_bits);
    sm_config_set_in_shift(&pc, false, true, n_bits);
    sm_config_set_clkdiv(&pc, clkdiv);
    pio_sm_set_pins_with_mask(PIO_AUDIO, sm, (2u << PIN_AUDIO_SCK), (3u << PIN_AUDIO_SCK) | (1u << PIN_AUDIO_MOSI));
    pio_sm_set_pindirs_with_mask(PIO_AUDIO, sm, (3u << PIN_AUDIO_SCK) | (1u << PIN_AUDIO_MOSI), (3u << PIN_AUDIO_SCK) | (1u << PIN_AUDIO_MOSI) | (1u << PIN_AUDIO_MISO));
    pio_gpio_init(PIO_AUDIO, PIN_AUDIO_MOSI);
    pio_gpio_init(PIO_AUDIO, PIN_AUDIO_MISO);
    pio_gpio_init(PIO_AUDIO, PIN_AUDIO_SCK);
    pio_gpio_init(PIO_AUDIO, PIN_AUDIO_SCK + 1);
    gpio_set_outover(PIN_AUDIO_SCK, cpol ? GPIO_OVERRIDE_INVERT : GPIO_OVERRIDE_NORMAL);
    hw_set_bits(&pio->input_sync_bypass, 1u << PIN_AUDIO_MISO);
    uint entry_point = prog_offs + (cpha ? spi_cpha1_cs_offset_entry_point : spi_cpha0_cs_offset_entry_point);
    pio_sm_init(PIO_AUDIO, sm, entry_point, &pc);
    pio_sm_exec(PIO_AUDIO, sm, pio_encode_set(pio_x, n_bits - 2));
    pio_sm_exec(PIO_AUDIO, sm, pio_encode_set(pio_y, n_bits - 2));
    pio_sm_set_enabled(PIO_AUDIO, sm, true);

    // SETUP DMA
    dma_dac.channel = dma_claim_unused_channel(true); 
    dma_adc.channel = dma_claim_unused_channel(true);
    dma_timer_set_fraction(1, 0x01, 0x7d0);

	dma_dac.config = dma_channel_get_default_config(dma_dac.channel);
	channel_config_set_transfer_data_size(&dma_dac.config, DMA_SIZE_16); 
	channel_config_set_read_increment(&dma_dac.config, true);
	channel_config_set_write_increment(&dma_dac.config, false); 
    channel_config_set_dreq(&dma_dac.config, 0x3c);
	channel_config_set_ring(&dma_dac.config, false, dac_buf_size_bits);
    channel_config_set_chain_to(&dma_dac.config, dma_adc.channel);


	dma_channel_configure(
		dma_dac.channel,
		&dma_dac.config,
        &PIO_AUDIO->txf[PIO_AUDIO_STATE_MACHINE],
        &dac_buf[0],
		1,
        false
	);

    dma_adc.config = dma_channel_get_default_config(dma_adc.channel); 
    channel_config_set_transfer_data_size(&dma_adc.config, DMA_SIZE_16);
    channel_config_set_read_increment(&dma_adc.config, false);
    channel_config_set_write_increment(&dma_adc.config, false);
    channel_config_set_dreq(&dma_dac.config, 0x3c);
    channel_config_set_chain_to(&dma_adc.config, dma_dac.channel);

    dma_channel_configure(
        dma_adc.channel,
        &dma_adc.config,
        &adc_buf[0],
        &PIO_AUDIO->rxf[PIO_AUDIO_STATE_MACHINE],
        1,
        false
    );

    audioSetup(dma_dac.channel, dma_adc.channel, dma_adc.channel, DMA_AUDIO_INTERRUPT_NUM);

    // SETUP DMA IRQ
	dma_channel_set_irq0_enabled(dma_adc.channel, true);
    irq_add_shared_handler(DMA_IRQ_0, audioIrqHandler, 255);
    irq_set_enabled(DMA_IRQ_0, true); 
    dma_start_channel_mask(1u << dma_dac.channel | 1u << dma_adc.channel);

}

// ################
// pots/mux/adc
// ################

uint16_t devicesMuxSelectAndRead(uint8_t adc_input, uint8_t mux_input)
{

    return cd4051SelectChannelAndRead(adc_input, mux_input);
}

// ################
// encoder
// ################

bool devicesEncoderAdvanced()
{
    // return encoderAdvanced();
    return encoderDidEventOccur();
}

int8_t devicesEncoderGetValue()
{
    // return encoderGetValue();
    return encoderGetDirectionValue();
}

// ################
// gpio expander
// ################

uint8_t devicesGpioExpRead(uint8_t index, bool port_b)
{

    return mcp23017Read(&mcp23017[index], port_b);
}

void devicesGpioExpWrite(uint8_t leds_to_write)
{

    mcp23017Write(&mcp23017[GPIO_EXP_LEDS], 1, leds_to_write);
}

// drawChars(, WriteMode::INVERT);

// ################
// display
// ################

void devicesDisplayClear()
{
    display.clear();
}

void devicesDisplayDrawTextNormFont(const char *text, uint8_t x, uint8_t y, pico_ssd1306::WriteMode write_mode)
{

    pico_ssd1306::drawText(&display, font_8x8, text, x, y, write_mode);
    
}

void devicesDisplayDrawRectangle(uint8_t x, uint8_t y, uint8_t x_length, uint8_t y_height, bool fill, pico_ssd1306::WriteMode write_mode)
{
    uint8_t x_end = x + x_length;
    uint8_t y_end = y + y_height;

    if (fill)
    {

        pico_ssd1306::fillRect(&display, x, y, x_end, y_end, write_mode);
    }
    else
    {

        pico_ssd1306::drawRect(&display, x, y, x_end, y_end, write_mode);
    }
}

void devicesDisplayDrawBitmap(uint8_t x, uint8_t y, uint8_t x_length, uint8_t y_height, uint8_t *image, pico_ssd1306::WriteMode write_mode)
{
    display.addBitmapImage(x, y, x_length, y_height, image, write_mode);
}

void devicesDisplayDrawLine(uint8_t x, uint8_t y, uint8_t l, uint8_t w, pico_ssd1306::WriteMode write_mode) 
{
    pico_ssd1306::drawLine(&display, x, y, l, w, write_mode);
}


void devicesDisplaySetPixel(uint8_t x, uint8_t y, pico_ssd1306::WriteMode write_mode)
{
    display.setPixel(x, y, write_mode);
}

void devicesDisplayDisplay()
{
    display.sendBufferDMA();
}

// ################
// storage
// ################

void devicesStoragePut(uint8_t *ptr, uint address, size_t size) {

    eepromPut(ptr, address, size);

}

void devicesStorageGet(uint8_t *ptr, uint address, size_t size) {

    eepromGet(ptr, address, size);

}