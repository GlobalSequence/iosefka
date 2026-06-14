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

#ifndef PIN_ASSIGNMENTS_H
#define PIN_ASSIGNMENTS_H

// MIDI PINS
#define PIN_MIDI_TX         0
#define PIN_MIDI_RX         1

// GPIO PINS
#define PIN_FIL_SEL_1       2
#define PIN_FIL_SEL_2       3

// I2C DEVICE PINS
#define PIN_DEVICE_SDA      4
#define PIN_DEVICE_SCL      5

// PWM CV PINS
#define PIN_PWM_CUTOFF      6
#define PIN_PWM_RESONANCE   7
#define PIN_PWM_VCA_1       8
#define PIN_PWM_VCA_2       9

// DISPLAY PINS
#define PIN_DISPLAY_SCK     10
#define PIN_DISPLAY_MOSI    11
#define PIN_DISPLAY_DC      12
#define PIN_DISPLAY_CS      13

// ENCODER PINS
#define PIN_ENCODER_A       14
#define PIN_ENCODER_B       15

// AUDIO SPI PINS
#define PIN_AUDIO_MOSI      16
#define PIN_AUDIO_MISO      17
#define PIN_AUDIO_SCK       18
#define PIN_AUDIO_CS        19

// MUX CTRL PINS
#define PIN_MUX_CTRL_A      20
#define PIN_MUX_CTRL_B      21
#define PIN_MUX_CTRL_C      22

// MUX ANALOG PINS
#define PIN_MUX_ANALOG_1    26
#define PIN_MUX_ANALOG_2    27

#endif