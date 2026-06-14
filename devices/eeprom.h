// BSD 3-Clause License
//
// Copyright (c) 2019, wvmarle
// Copyright (c) 2026, Cory Oleson
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Modifications by Cory Oleson
// - Ported to RPI Pico IDE compatibility from Arduino code

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
