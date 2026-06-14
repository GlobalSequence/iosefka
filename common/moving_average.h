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

#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include <stdio.h>
#include "pico/stdlib.h"
#include <cstdlib>


#define MA32_SIZE 32

// ready made moving average class for 8-bit samples and buffer size of 32
class MovingAverage32
{
    public:
        MovingAverage32() {}
        inline uint8_t getAverage(uint8_t sample)
        {
            total_ -= buffer_[ctr_];
            buffer_[ctr_] = sample;
            total_ += buffer_[ctr_];

            ctr_ >= MA32_SIZE - 1 ? ctr_ = 0 : ctr_++;

            return total_ / MA32_SIZE;
        };

    private:
        uint16_t total_;
        uint8_t buffer_[MA32_SIZE];
        uint8_t ctr_;
};

// generalized moving average class
template<typename T>
class MovingAverage {
    public: 
        MovingAverage(uint size) {
            size_ = size;
            buffer_ = (T *) malloc(size_ * sizeof(T));
            for(int i = 0; i < size_; i++) buffer_[i] = 0;
        }

        ~MovingAverage() {
            free(buffer_);
            buffer_ = NULL;
        }

        inline T getAverage(T sample) {
            total_ -= buffer_[ctr_];
            buffer_[ctr_] = sample;
            total_ += buffer_[ctr_];

            ctr_ >= size_ - 1 ? ctr_ = 0 : ctr_++;

            return total_ / size_;
        }

    private:
        uint size_;
        int64_t total_;
        uint ctr_;
        T *buffer_;
};

#endif