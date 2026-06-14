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

#ifndef PERIPHERAL_CONFIG_H
#define PERIPHERAL_CONFIG_H

#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/uart.h"

// SPI
#define SPI_DISPLAY spi1
#define SPI_DISPLAY_RATE 10000000

// I2C
#define I2C_DEVICES i2c0
#define I2C_GPIO_EXPANDER I2C_DEVICES
#define I2C_EEPROM I2C_DEVICES
#define I2C_DEVICES_RATE 400000 
// TODO: these are device specific, not peripheral specific.
//       move them when there's a better spot
#define I2C_MCP_ADDR_1 0x20
#define I2C_MCP_ADDR_2 0x21
#define I2C_NUM_READ_CHANNELS 3
#define I2C_EEPROM_ADDR 0x50

// PIO
#define PIO_ENCODER pio0
#define PIO_ENCODER_STATE_MACHINE 0

#define PIO_AUDIO pio1
#define PIO_AUDIO_STATE_MACHINE 0

// UART
#define UART_MIDI uart0

// DMA
#define DMA_AUDIO_INTERRUPT_NUM 0

#endif