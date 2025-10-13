#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

//#include "Arduino.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include <cstdlib>


#define MA32_SIZE 32

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