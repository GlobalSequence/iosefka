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

#ifndef SSD1306_TEXTRENDERER_H
#define SSD1306_TEXTRENDERER_H

#include "../../devices/ssd1306.h"

#include "5x8_font.h"
#include "8x8_font.h"
#include "12x16_font.h"
#include "16x32_font.h"

namespace pico_ssd1306{

    /// \enum pico_ssd1306::Rotation
    enum class Rotation{
        /// deg0 - means no rotation
        deg0,
        /// deg 90 - means 90 deg rotation
        deg90,
    };

    /// \brief Draws a single glyph on the screen
    /// \param ssd1306 - pointer to a SSD1306 object aka initialised display
    /// \param font - pointer to a font data array
    /// \param c - char to be drawn
    /// \param anchor_x, anchor_y - coordinates setting where to put the glyph
    /// \param mode - mode describes setting behavior. See WriteMode doc for more information
    /// \param rotation - either rotates the char by 90 deg or leaves it unrotated
    void drawChar(pico_ssd1306::SSD1306 *ssd1306, const unsigned char * font, char c, uint8_t anchor_x, uint8_t anchor_y, WriteMode mode = WriteMode::ADD, Rotation rotation = Rotation::deg0);

    /// \brief Draws text on screen
    /// \param ssd1306 - pointer to a SSD1306 object aka initialised display
    /// \param font - pointer to a font data array
    /// \param text - text to be drawn
    /// \param anchor_x, anchor_y - coordinates setting where to put the text
    /// \param mode - mode describes setting behavior. See WriteMode doc for more information
    /// \param rotation - either rotates the text by 90 deg or leaves it unrotated
    void drawText(pico_ssd1306::SSD1306 *ssd1306, const unsigned char * font, const char * text, uint8_t anchor_x, uint8_t anchor_y, WriteMode mode = WriteMode::ADD, Rotation rotation = Rotation::deg0);

    void drawText8x8NoRotation(pico_ssd1306::SSD1306 *ssd1306, const char *text, uint8_t anchor_x, uint8_t anchor_y, WriteMode mode);
}

#endif //SSD1306_TEXTRENDERER_H
