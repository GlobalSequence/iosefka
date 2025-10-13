#ifndef EEPROM_H
#define EEPROM_H

/* 
*
* Based heavily on wvmarle's 24LC256 Arduino Library, adapted for RPI Pico SDK
* Found here: https://github.com/wvmarle/24LC256/tree/master
*
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define EEPROM_24LC256_PAGE_SIZE_BYTES 64

#ifdef __cplusplus
extern "C"
{
#endif

void eepromSetup(i2c_inst_t *inst, uint8_t device_address);

void eepromPut(uint8_t *ptr, uint address, size_t size);

uint8_t* eepromGet(uint8_t *ptr, uint address, size_t size);

#ifdef __cplusplus
}
#endif

#endif
