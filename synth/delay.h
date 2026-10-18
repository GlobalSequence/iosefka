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

#ifndef DELAY_H
#define DELAY_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/utilities.h"
#include "../common/moving_average.h"
#include "../common/test.h"

#define DELAY_SIGNED

typedef struct delay_inputs_t {

	uint16_t amount;    // 8 bit value
    uint16_t length; 
	uint16_t feedback;  // 8 bit value
    uint16_t type;      
	uint16_t input;     // 8 bit value
    uint16_t div;       // 9 bit value
	bool reset;     // 1 bit value

} delay_inputs_t;

typedef enum delay_type_t {

    DELAY_TYPE_DEFAULT,
    DELAY_TYPE_SYNC,
    DELAY_TYPE_SHORT,
    DELAY_TYPE_FREEZE

} delay_type_t;

template<size_t size>
class Delay {

    public:

        Delay(uint16_t *audio_buf, uint16_t *input_buf, uint16_t audio_block_size) {

            buf_ptr_ = audio_buf;

            input_buf_ = input_buf;

            block_size_ = audio_block_size;

        }

        void render() {

/*             // we count the number of block fills since clock tick reset
            // so that we know where in the buffer to return to on clock tick
            bool freeze_reset_occurred = p.reset;

            if(freeze_reset_occurred) {

                buffers_since_reset_ = 0;

            } else {

                buffers_since_reset_++;

            } */

            if(p.type != DELAY_TYPE_FREEZE) {

                // FOR THIS TO BE MODULAR, YOU HAVE TO HARD CODE IT SO THAT DELAY SIZE IS ALWAYS A POWER OF 2. THAT IS OKAY, BUT EDIT THE DELAY TO DO THAT.
                int32_t time = writeAvg.getAverage(p.length << 11);
                step_ = (time - last_time_) >> 7;
                current_time_ = last_time_;
                last_time_ = time;
                read_position_ = (index_write_ - size - 1) << 8;

                uint8_t in_level = p.input;

                // this ensures that when freeze mode is entered, the freeze buffer begins filling right away
                freeze_loop_counter_ = 0;
                fill_freeze_buffer_ = true;

                for(int i = 0; i < block_size_; i++) {
                    current_time_ += step_;
                    uint32_t readp = read_position_ - (current_time_ >> 4);
                    uint32_t read_index = (readp >> 8) & (size - 1);

                    // shift right once for mcp3201 extra bit.
                    // TODO: try putting the shift in the DMA callback function, then remove it here
                    int sample = S8S16MULSHIFT8((input_buf_[i] >> 1) - k_sign_offset, in_level);

                    int32_t feedback_sample = S8S16MULSHIFT8(delay_buf_[read_index], p.feedback);
                    int32_t combined_sample = sample + feedback_sample;
                    delay_buf_[index_write_ + i] = combined_sample;

                    int32_t output_sample = S8S16MULSHIFT8(delay_buf_[read_index], p.amount) << 1;
                    buf_ptr_[i] = U12CLIPMAXMIN(output_sample + k_sign_offset);

                    read_position_ += (1 << 8);

                }


            } else {

                uint8_t output_level = p.amount;

                // delay buffer is 15 bits, so divide length parameter in half to get exact freeze buffer size
                int32_t freeze_size = p.length >> 1;

                // scale feedback parameter to 4 bit count of freeze repeats
                uint8_t freeze_limit = p.feedback >> 4;

                // enable freeze if input parameter is anywhere above 0
                bool freeze_enable = p.input;

                // trigger the freeze immediately when turned on
                bool freeze_enable_trigger = false; //

                // if freeze not enabled, we shut off the delay by silencing the delay output
                if(!freeze_enable) {

                    output_level = 0;
                
                // otherwise we trigger a new buffer fill
                } else {

                    if(last_freeze_enable_state_ != freeze_enable) {

                        freeze_enable_trigger = true;
                        // reset_freeze_tick_counter_ = true;

                    }

                }

                last_freeze_enable_state_ = freeze_enable;

                // start a brand new freeze on delay type change
                bool last_delay_type_was_freeze = last_delay_type_ == DELAY_TYPE_FREEZE;

                // bool trigger_tick_counter_reset = false;

                if(!last_delay_type_was_freeze || freeze_enable_trigger) {

                    freeze_recently_enabled_ = true;

                }
                
                // ... or if loop count exceeded loop limit
                bool loop_limit_exceeded = false;

                // indicate when to reset loop counter
                bool reset_loop_counter = false;

                // indicate when to reset sample counter
                bool reset_sample_counter = false;

                if(p.reset) {

                    uint16_t tick_counter_limit = p.div - 1;

                    // TEST_PRINT_TWO_NL(tick_counter_limit, freeze_tick_counter_)

                    if(freeze_tick_counter_ >= tick_counter_limit || freeze_recently_enabled_) {

                        freeze_tick_counter_ = 0;
                        freeze_read_index_ = 0;
                        freeze_write_index_ = 0;
                        // if the freeze is not infinite, count loops
                        if(freeze_limit != 0xF) {
                            
                            if(freeze_loop_counter_ >= freeze_limit) {

                                freeze_loop_counter_ = 0;
                                fill_freeze_buffer_ = true;

                            } else {

                                freeze_loop_counter_++;
                                fill_freeze_buffer_ = false;

                            }


                        } else {

                            freeze_loop_counter_ = 0;
                            if(freeze_recently_enabled_) {
                                
                                fill_freeze_buffer_ = true;

                            } else {

                                fill_freeze_buffer_ = false;

                            }

                        }

                        freeze_recently_enabled_ = false;

                    } else {

                        freeze_tick_counter_++;

                    }

                } 

                // if(!last_delay_type_was_freeze || loop_limit_exceeded || freeze_enable_trigger) {


                // }

                // if(reset_loop_counter) freeze_loop_counter_ = 0;

                // if(reset_sample_counter) freeze_sample_counter_ = 0;

                for(int i = 0; i < AUDIO_BLOCK_SIZE; i++) {

                    if(fill_freeze_buffer_) {

                        int sample = (input_buf_[i] >> 1) - k_sign_offset;
                        delay_buf_[freeze_write_index_] = sample;
                        
                        freeze_write_index_++;
                        if(freeze_write_index_ >= size) freeze_write_index_ -= size;
                    
                    }

                    int32_t output_sample = S8S16MULSHIFT8(delay_buf_[freeze_read_index_], output_level) << 1;
                    buf_ptr_[i] = U12CLIPMAXMIN(output_sample + k_sign_offset);

                    freeze_read_index_++; 
                    if(freeze_read_index_ >= size) freeze_read_index_ -= size;

                }

            }

            index_write_ += block_size_;
            if(index_write_ >= size) index_write_ -= size;

            last_delay_type_ = (delay_type_t)p.type;
    
        }

        void setInputBufferPointer(uint16_t *input_buf) {

            input_buf_ = input_buf;

        }

        delay_inputs_t* getParamPointer() { return &p; }

    private:

        delay_inputs_t p;

        int16_t delay_buf_[size]__attribute__((aligned(size)));
        const int k_sign_offset = 2048;

        uint16_t *buf_ptr_;
        uint16_t *input_buf_;
        uint16_t block_size_;
        uint16_t index_write_ = 0;
        uint index_;
        uint current_frame_;

        MovingAverage<int32_t> writeAvg = MovingAverage<int32_t>(64);

        int32_t step_;
        int32_t last_time_;
        int32_t current_time_;
        int32_t read_position_;

        // initialize the last delay type as default so freeze starts from 0 at startup
        delay_type_t last_delay_type_ = DELAY_TYPE_DEFAULT;

        uint32_t write_start_index_; 
        uint16_t freeze_write_index_;
        uint16_t freeze_read_index_;

        bool last_freeze_enable_state_ = false;
        bool fill_freeze_buffer_ = true;
        uint16_t last_freeze_size_;
        uint8_t freeze_loop_counter_ = 0;
        uint32_t freeze_sample_counter_ = 0;

        uint16_t buffers_since_reset_;
        uint16_t buffers_to_wait_until_reset_;
        
        uint16_t buffer_fill_reset_wait_counter = 0;
        

        uint16_t freeze_tick_counter_ = 0;

        bool freeze_recently_enabled_;
};

#endif