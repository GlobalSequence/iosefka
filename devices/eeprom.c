// BSD 3-Clause License
//
// Copyright (c) 2019, wvmarle
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

#include "eeprom.h"
#include <stdio.h>
#include "pico/stdlib.h"

i2c_inst_t *i2c;
uint8_t device_addr;

static unsigned long long wait_period_us = 6000;

uint8_t read_buffer[EEPROM_24LC256_PAGE_SIZE_BYTES];

// ################################
// PRIVATE FUNCTIONS
// ################################

void eepromWriteBytes(uint8_t *ptr, uint address, size_t size) {

    uint8_t write_out_buf[size + 2];

    write_out_buf[0] = address >> 8;
    write_out_buf[1] = address & 0xFF;
    
    for(int i = 0; i < size; i++) {

        write_out_buf[i + 2] = ptr[i];

    }

    i2c_write_blocking(i2c, device_addr, write_out_buf, size + 2, false);
    
}
    
void eepromReadBytes(uint8_t *ptr, uint address, size_t size) {

    uint8_t address_out[2];

    address_out[0] = address >> 8;
    address_out[1] = address & 0xFF;

    i2c_write_blocking(i2c, device_addr, address_out, 2, false);

    i2c_read_blocking(i2c, device_addr, ptr, size, false);
    
}

bool waitPeriodFinished() {

    unsigned long long start = time_us_64();
    unsigned long long now = start;

    while(now - start <= wait_period_us) {
        
        now = time_us_64();
        // grab a sandwich
    
    }

    return true;

}

bool compareBytes(uint8_t* a, uint8_t* b, uint8_t n) {
    for (uint8_t i = 0; i < n; i++) {
      if (*a != *b) {
        return false;
      }
      a++;
      b++;
    }
    return true;
}

// ################################
// PUBLIC FUNCTIONS
// ################################

void eepromSetup(i2c_inst_t *inst, uint8_t device_address) {

    i2c = inst;

    device_addr = device_address;

}

void eepromPut(uint8_t *ptr, uint address, size_t size) {

    uint8_t page_size = EEPROM_24LC256_PAGE_SIZE_BYTES;

    if(waitPeriodFinished()) {

        // the chip has a physical page boundary, so if we're beginning in a middle of the page, we write with fewer than page_size bytes
        uint8_t first_page_size = page_size * ((address / page_size) + 1) - address;

        if(first_page_size > size) first_page_size = size;

        eepromReadBytes(read_buffer, address, first_page_size);

        if(compareBytes(read_buffer, ptr, first_page_size) == false) {

            eepromWriteBytes(ptr, address, first_page_size);

            waitPeriodFinished();

        }

        ptr += first_page_size;

        if(size > first_page_size) {

            uint8_t next_page_size;

            for(uint i = first_page_size; i < size; i += page_size) {

                next_page_size = page_size;

                if(i + page_size > size) {

                    next_page_size = size - i;

                }

                eepromReadBytes(read_buffer, address + i, next_page_size);

                if(compareBytes(read_buffer, ptr, next_page_size) == false) {

                    eepromWriteBytes(ptr, address + i, next_page_size);

                    if(i + page_size < size) {

                        waitPeriodFinished();

                    }

                }

                ptr += next_page_size;

            }

        }

    }

}

uint8_t* eepromGet(uint8_t *ptr, uint address, size_t size) {

    if(waitPeriodFinished()) {

        eepromReadBytes(ptr, address, size);

    }

}

