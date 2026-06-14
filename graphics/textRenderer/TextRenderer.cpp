// BSD 3-Clause License
//
// Copyright (c) 2021, Harbys
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

#include "TextRenderer.h"

namespace pico_ssd1306 {

    void drawText(pico_ssd1306::SSD1306 *ssd1306, const unsigned char *font, const char *text, uint8_t anchor_x,
                  uint8_t anchor_y, WriteMode mode, Rotation rotation) {
        if(!ssd1306 || !font || !text) return;

        uint8_t font_width = font[0];

        uint16_t n = 0;
        while (text[n] != '\0') {
            switch (rotation) {
                case Rotation::deg0:
                    drawChar(ssd1306, font, text[n], anchor_x + (n * font_width), anchor_y, mode, rotation);
                    break;
                case Rotation::deg90:
                    drawChar(ssd1306, font, text[n], anchor_x, anchor_y + (n * font_width), mode, rotation);
                    break;
            }

            n++;
        }
    }

    void drawChar(pico_ssd1306::SSD1306 *ssd1306, const unsigned char *font, char c, uint8_t anchor_x, uint8_t anchor_y,
                  WriteMode mode, Rotation rotation) {
        if(!ssd1306 || !font || c < 32) return;

        uint8_t font_width = font[0];
        uint8_t font_height = font[1];

        uint16_t seek = (c - 32) * (font_width * font_height) / 8 + 2;

        uint8_t b_seek = 0;

        for (uint8_t x = 0; x < font_width; x++) {
            for (uint8_t y = 0; y < font_height; y++) {
                if (font[seek] >> b_seek & 0b00000001) {
                    switch (rotation) {
                        case Rotation::deg0:
                            ssd1306->setPixel(x + anchor_x, y + anchor_y, mode);
                            break;
                        case Rotation::deg90:
                            ssd1306->setPixel(-y + anchor_x + font_height, x + anchor_y, mode);
                            break;
                    }
                }
                b_seek++;
                if (b_seek == 8) {
                    b_seek = 0;
                    seek++;
                }
            }
        }
    }

    void drawText8x8NoRotation(pico_ssd1306::SSD1306 *ssd1306, const char *text, uint8_t anchor_x, uint8_t anchor_y, WriteMode mode) {

        uint8_t font_width = 8;
        uint8_t font_height = 8;

        uint16_t n = 0;

        while(text[n] != '\0') {

            // draw char start
            uint16_t seek = ((text[n] - 32) << 3);

            uint8_t b_seek = 0;

            for (uint8_t x = 0; x < font_width; x++) {

                for (uint8_t y = 0; y < font_height; y++) {

                    if (font_8x8_sans_w_h[seek] >> b_seek & 0b00000001) {

                        ssd1306->setPixel(x + anchor_x + (n * 8), y + anchor_y, mode);

                    }
                }

                b_seek++;

                if (b_seek == 8) {
                    b_seek = 0;
                    seek++;
                }
            }

        // draw char end
        n++;
        }

    }

}