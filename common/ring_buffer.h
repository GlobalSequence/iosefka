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

        T readElement(uint index) {
            // dangerous function, as you can exceed ring buffer size
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