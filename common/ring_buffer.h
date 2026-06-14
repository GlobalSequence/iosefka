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

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdio.h>
#include "pico/stdlib.h"

template<typename T, uint size>
class RingBuffer {

    public:
        RingBuffer() : buffer_{}, head_(0), tail_(0), count_(0) {};
        
        bool isEmpty() {
            return count_ == 0;
        }

        bool isFull() {
            return count_ == size;
        }

        bool enqueue(T& event) {
            buffer_[tail_] = event;
            tail_ = (tail_ + 1) % size;
            count_++;
            return true;
        }

        T dequeue() {
            T out = buffer_[head_];
            head_ = (head_ + 1) % size;
            count_--;
            return out;
        }

        // dangerous function, as you can exceed ring buffer size
        T readElement(uint index) {
            return buffer_[index];
        }

        void flush() {
            tail_ = head_;
        }

        uint getCount() {
            return count_;
        }

        uint getHead() {
            return head_;
        }

        uint getTail() {
            return tail_;
        }

    private:
        T buffer_[size];
        uint head_;
        uint tail_;
        uint count_;
};

#endif