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