// MIT License
//
// Copyright (c) 2022-2026 Rob Tillaart
// Copyright (c) 2026 Cory Oleson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Modifications by Cory Oleson:
// - Combined a hardware specific files into one file with the 'xx' suffix

#ifndef MCP230XX_H
#define MCP230XX_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define MCP23017_NUM_GPIO 16
#define MCP23017_NUM_PORT 2
#define MCP23017_NUM_GPIO_PER_PORT MCP23017_NUM_GPIO / MCP23017_NUM_PORT
#define MCP23008_NUM_GPIO 8

typedef struct  {
    uint8_t reg_addr;
    uint8_t value;
} mcp230xx_write_message_t;

typedef struct {
    i2c_inst_t* i2c_instance;
    uint8_t address;
    uint8_t direction;
} mcp23008_t;

typedef struct {
    i2c_inst_t* i2c_instance;
    uint8_t address;
    uint16_t direction;
} mcp23017_t;

#ifdef __cplusplus
 extern "C" {
#endif

void mcp23008Setup(mcp23008_t* mcp, i2c_inst_t* i2c, uint8_t addr, uint8_t dir);
void mcp23008Write(mcp23008_t* mcp, uint8_t val);
uint8_t mcp23008Read(mcp23008_t* mcp);

void mcp23017Setup(mcp23017_t* mcp, i2c_inst_t* i2c, uint8_t addr, uint16_t dir);
void mcp23017Write(mcp23017_t* mcp, bool reg_b, uint8_t val);
uint8_t mcp23017Read(mcp23017_t* mcp, bool reg_b);

#ifdef __cplusplus
}
#endif

#endif