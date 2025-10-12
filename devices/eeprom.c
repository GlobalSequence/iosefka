#include "eeprom.h"
#include <stdio.h>
#include "pico/stdlib.h"

i2c_inst_t *i2c;
uint8_t device_addr;

static unsigned long wait_period_us = 6000;

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

    unsigned long start = time_us_64();

    while(time_us_64() - start <= wait_period_us) {
        
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

        // printf("first page size: %d\n", first_page_size);

        if(first_page_size > size) first_page_size = size;

        eepromReadBytes(read_buffer, address, first_page_size);

        if(compareBytes(read_buffer, ptr, first_page_size) == false) {

            eepromWriteBytes(ptr, address, first_page_size);

            waitPeriodFinished();

            // printf("first page written?\n");
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

                    // printf("%d address written?\n", i + address);

                }

                ptr += next_page_size;

            }

        }

    }

}

uint8_t* eepromGet(uint8_t *ptr, uint address, size_t size) {

    if(waitPeriodFinished()) {

        // printf("%d address read?\n", address);

        eepromReadBytes(ptr, address, size);

    }

}

