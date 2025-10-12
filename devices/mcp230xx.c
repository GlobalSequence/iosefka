#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "mcp230xx.h"
// #include "mcp230xx_registers.h"

// ################################
// MCP23X08
// ################################

// CREDIT:
//    FILE: MCP23x08_registers.h
//  AUTHOR: Rob Tillaart
// PURPOSE: MCP23x08 register file
//     URL: https://github.com/RobTillaart/MCP23008
//     URL: https://github.com/RobTillaart/MCP23S08

//  Registers                           //  description              datasheet P9
#define MCP23x08_DDR_A          0x00    //  Data Direction Register A   P 10
#define MCP23x08_POL_A          0x01    //  Input Polarity A            P 11
#define MCP23x08_GPINTEN_A      0x02    //  NOT USED interrupt enable   P 12
#define MCP23x08_DEFVAL_A       0x03    //  NOT USED interrupt def      P 13
#define MCP23x08_INTCON_A       0x04    //  NOT USED interrupt control  P 14
#define MCP23x08_IOCR           0x05    //  IO control register         P 15
#define MCP23x08_PUR_A          0x06    //  Pull Up Resistors A         P 16
#define MCP23x08_INTF_A         0x07    //  NOT USED interrupt flag     P 17
#define MCP23x08_INTCAP_A       0x08    //  NOT USED interrupt capture  P 18
#define MCP23x08_GPIO_A         0x09    //  General Purpose IO A        P 19
#define MCP23x08_OLAT_A         0x0A    //  NOT USED output latch       P 20


//  IOCR = IO CONTROL REGISTER bit masks   - details datasheet P15
#define MCP23x08_IOCR_SEQOP     0x20    //  Sequential Operation mode bit.
#define MCP23x08_IOCR_DISSLW    0x10    //  Slew Rate control bit for SDA output.
#define MCP23x08_IOCR_HAEN      0x08    //  Hardware Address Enable bit (MCP23S17 only).
#define MCP23x08_IOCR_ODR       0x04    //  Configures the INT pin as an open-drain output.
#define MCP23x08_IOCR_INTPOL    0x02    //  This bit sets the polarity of the INT output pin.
#define MCP23x08_IOCR_NI        0x01    //  Not implemented.

// ################################
// MCP23X17
// ################################

// CREDIT:
//    FILE: MCP23x17_registers.h
//  AUTHOR: Rob Tillaart
// PURPOSE: MCP23x17 register file
//     URL: https://github.com/RobTillaart/MCP23017_RT
//     URL: https://github.com/RobTillaart/MCP23S17


//  REGISTERS                           //  description                 datasheet
#define MCP23x17_DDR_A          0x00    //  Data Direction Register A     P18
#define MCP23x17_DDR_B          0x01    //  Data Direction Register B     P18
#define MCP23x17_POL_A          0x02    //  Input Polarity A              P18
#define MCP23x17_POL_B          0x03    //  Input Polarity B              P18
#define MCP23x17_GPINTEN_A      0x04    //  Interrupt enable              P19
#define MCP23x17_GPINTEN_B      0x05    //  Interrupt enable              P19
#define MCP23x17_DEFVAL_A       0x06    //  Interrupt default             P19
#define MCP23x17_DEFVAL_B       0x07    //  Interrupt default             P19
#define MCP23x17_INTCON_A       0x08    //  Interrupt control register    P20
#define MCP23x17_INTCON_B       0x09    //  Interrupt control register    P20
#define MCP23x17_IOCR           0x0A    //  IO control register           P20
#define MCP23x17_IOCR2          0x0B    //  NOT USED
#define MCP23x17_PUR_A          0x0C    //  Pull Up Resistors A           P22
#define MCP23x17_PUR_B          0x0D    //  Pull Up Resistors A           P22
#define MCP23x17_INTF_A         0x0E    //  Interrupt flag register       P22
#define MCP23x17_INTF_B         0x0F    //  Interrupt flag register       P22
#define MCP23x17_INTCAP_A       0x10    //  Interrupt capture register    P23
#define MCP23x17_INTCAP_B       0x11    //  Interrupt capture register    P23
#define MCP23x17_GPIO_A         0x12    //  General Purpose IO A          P23
#define MCP23x17_GPIO_B         0x13    //  General Purpose IO B          P23
#define MCP23x17_OLAT_A         0x14    //  NOT USED output latch         P24
#define MCP23x17_OLAT_B         0x15    //  NOT USED


//  IOCR = IO CONTROL REGISTER bit masks   - details datasheet P20
#define MCP23x17_IOCR_BANK      0x80    //  Controls how the registers are addressed.
#define MCP23x17_IOCR_MIRROR    0x40    //  INT Pins Mirror bit.
#define MCP23x17_IOCR_SEQOP     0x20    //  Sequential Operation mode bit.
#define MCP23x17_IOCR_DISSLW    0x10    //  Slew Rate control bit for SDA output.
#define MCP23x17_IOCR_HAEN      0x08    //  Hardware Address Enable bit (MCP23S17 only).
#define MCP23x17_IOCR_ODR       0x04    //  Configures the INT pin as an open-drain output.
#define MCP23x17_IOCR_INTPOL    0x02    //  This bit sets the polarity of the INT output pin.
#define MCP23x17_IOCR_NI        0x01    //  Not implemented.



void mcp23008Setup(mcp23008_t* mcp, i2c_inst_t* i2c, uint8_t addr, uint8_t dir) {
    mcp->i2c_instance = i2c;
    mcp->address = addr;
    mcp->direction = dir;
    mcp230xx_write_message_t msg;
    msg.reg_addr = MCP23x08_DDR_A;
    msg.value = mcp->direction;
    i2c_write_blocking(mcp->i2c_instance, mcp->address, (uint8_t *)&msg, sizeof(msg), false);
    msg.reg_addr = MCP23x08_PUR_A; // we're using pull ups for all our inputs so msg.value is same
    i2c_write_blocking(mcp->i2c_instance, mcp->address, (uint8_t *)&msg, sizeof(msg), false);
}

void mcp23008Write(mcp23008_t* mcp, uint8_t val) {
    mcp230xx_write_message_t msg;
    msg.reg_addr = MCP23x08_GPIO_A;
    msg.value = val;
    i2c_write_blocking(mcp->i2c_instance, mcp->address, (uint8_t *)&msg, sizeof(msg), false);
}

uint8_t mcp23008Read(mcp23008_t* mcp) {
    mcp230xx_write_message_t msg;
    msg.reg_addr = MCP23x08_GPIO_A;
    i2c_write_blocking(mcp->i2c_instance, mcp->address, &msg.reg_addr, sizeof(msg.reg_addr), false);
    i2c_read_blocking(mcp->i2c_instance, mcp->address, &msg.value, sizeof(msg.value), false);
    return msg.value;
}

void mcp23017Setup(mcp23017_t* mcp, i2c_inst_t* i2c, uint8_t addr, uint16_t dir) {
    mcp->i2c_instance = i2c;
    mcp->address = addr;
    mcp->direction = dir;
    mcp230xx_write_message_t msg;
    msg.reg_addr = MCP23x17_DDR_A;
    uint8_t port_dir = (mcp->direction >> 8) & 0xFF;
    msg.value = port_dir;
    i2c_write_blocking(mcp->i2c_instance, mcp->address, (uint8_t *)&msg, sizeof(msg), false);
    msg.reg_addr = MCP23x17_PUR_A; // we're using pull ups for all our inputs so msg.value is same
    i2c_write_blocking(mcp->i2c_instance, mcp->address, (uint8_t *)&msg, sizeof(msg), false);

    msg.reg_addr = MCP23x17_DDR_B;
    port_dir = mcp->direction & 0xFF;
    msg.value = port_dir;
    i2c_write_blocking(mcp->i2c_instance, mcp->address, (uint8_t *)&msg, sizeof(msg), false);
    msg.reg_addr = MCP23x17_PUR_B; // we're using pull ups for all our inputs so msg.value is same
    i2c_write_blocking(mcp->i2c_instance, mcp->address, (uint8_t *)&msg, sizeof(msg), false);
}

void mcp23017Write(mcp23017_t* mcp, bool reg_b, uint8_t val) {
    mcp230xx_write_message_t msg;
    msg.reg_addr = reg_b == false? MCP23x17_GPIO_A : MCP23x17_GPIO_B;
    msg.value = val;
    i2c_write_blocking(mcp->i2c_instance, mcp->address, (uint8_t *)&msg, sizeof(msg), false);
}

uint8_t mcp23017Read(mcp23017_t* mcp, bool reg_b) {
    mcp230xx_write_message_t msg;
    msg.reg_addr = reg_b == false? MCP23x17_GPIO_A : MCP23x17_GPIO_B;
    i2c_write_blocking(mcp->i2c_instance, mcp->address, &msg.reg_addr, sizeof(msg.reg_addr), false);
    i2c_read_blocking(mcp->i2c_instance, mcp->address, &msg.value, sizeof(msg.value), false);
    return msg.value;
}