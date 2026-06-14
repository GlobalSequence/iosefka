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

#ifndef SSD1306_SHAPERENDERER_H
#define SSD1306_SHAPERENDERER_H

#include <math.h>
#include "../../devices/ssd1306.h"

namespace pico_ssd1306{

    /// \brief Draws a line from x0, y0 to x1, y1.
    /// It supports all drawing angles
    /// \param ssd1306 - is the pointer to a SSD1306 object aka an initialised display
    /// \param x0, y0, x1, y1 are the start and end coordinates between which the line will be drawn
    /// \param mode - mode describes setting behavior. See WriteMode doc for more information
    void drawLine (pico_ssd1306::SSD1306 *ssd1306, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, pico_ssd1306::WriteMode mode = pico_ssd1306::WriteMode::ADD);

    /// \brief Draws a 1px wide rectangle between x0, y0 and x1, y1
    /// \param x_start, x_end, y_start, y_end - corner points for the rectangle
    /// \param mode - mode describes setting behavior. See WriteMode doc for more information
    void drawRect (pico_ssd1306::SSD1306 *ssd1306 , uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, pico_ssd1306::WriteMode mode = pico_ssd1306::WriteMode::ADD);

    /// \brief Fills a rectangle from x0, y0 to x1, y1
    /// \param x_start, x_end, y_start, y_end - corner points for the rectangle
    /// \param mode - mode describes setting behavior. See WriteMode doc for more information
    void fillRect (pico_ssd1306::SSD1306 *ssd1306 , uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, pico_ssd1306::WriteMode mode = pico_ssd1306::WriteMode::ADD);
}

#endif //SSD1306_SHAPERENDERER_H
