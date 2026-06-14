// Copyright (C) 2026 Cory Oleson <globeseq@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef CV_H
#define CV_H

#include <stdio.h>
#include "pico/stdlib.h"

typedef struct cv_outputs_t {

    uint16_t cutoff;
    uint16_t resonance;
    uint16_t vca1;
    uint16_t vca2;
    uint8_t filter_type;

} cv_outputs_t;

class Cv {

    public:
        Cv(uint16_t *cutoff_ptr, uint16_t *resonance_ptr, uint16_t *vca_1_ptr, uint16_t *vca_2_ptr, uint8_t *filter_type_ptr) {

            cutoff_ptr_ = cutoff_ptr;
            resonance_ptr_ = resonance_ptr;
            vca_1_ptr_ = vca_1_ptr;
            vca_2_ptr_ = vca_2_ptr;
            filter_type_ptr_ = filter_type_ptr;

        }

        void update() {

            *cutoff_ptr_ = p.cutoff;
            *resonance_ptr_ = p.resonance;
            *vca_1_ptr_ = p.vca1;
            *vca_2_ptr_ = p.vca2;
            *filter_type_ptr_ = p.filter_type;

        }

        cv_outputs_t* getParamPointer() { return &p; }

    public:
        cv_outputs_t p;

        uint16_t *cutoff_ptr_;
        uint16_t *resonance_ptr_;
        uint16_t *vca_1_ptr_;
        uint16_t *vca_2_ptr_;
        uint8_t *filter_type_ptr_;


};

#endif
