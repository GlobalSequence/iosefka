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

	uint16_t amount; // should be 8 bit number
    uint16_t length; // should be 16 bit number
	uint16_t feedback;
	// uint16_t diffusion;
	// uint16_t saturation;
    uint16_t type;
	uint16_t input;
	uint16_t stereo;

} delay_inputs_t;

typedef enum delay_type_t {

    DELAY_TYPE_DEFAULT,
    DELAY_TYPE_SYNC,
    DELAY_TYPE_SHORT,

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

            // FOR THIS TO BE MODULAR, YOU HAVE TO HARD CODE IT SO THAT DELAY SIZE IS ALWAYS A POWER OF 2. THAT IS OKAY, BUT EDIT THE DELAY TO DO THAT.
            int32_t time = writeAvg.getAverage(p.length << 11);
            step_ = (time - last_time_) >> 7;
            current_time_ = last_time_;
            last_time_ = time;
            read_position_ = (index_write_ - size - 1) << 8;

            uint8_t in_level = p.input;

            for(int i = 0; i < block_size_; i++) {
                current_time_ += step_;
                uint32_t readp = read_position_ - (current_time_ >> 4);
                uint32_t read_index = (readp >> 8) & (size - 1);

#ifdef DELAY_SIGNED
                
                // shift right once for mcp3201 extra bit.
                // TODO: try putting the shift in the DMA callback function, then remove it here
                int sample = S8S16MULSHIFT8((input_buf_[i] >> 1) - k_sign_offset, in_level);
                // int sample = (input_buf_[i] >> 1) - k_sign_offset;

                int32_t feedback_sample = S8S16MULSHIFT8(delay_buf_[read_index], p.feedback);
                int32_t combined_sample = sample + feedback_sample;
                delay_buf_[index_write_ + i] = combined_sample;

                int32_t output_sample = S8S16MULSHIFT8(delay_buf_[read_index], p.amount) << 1;
                buf_ptr_[i] = U12CLIPMAXMIN(output_sample + k_sign_offset);

                read_position_ += (1 << 8);
                
/*                 // shift right once for mcp3201 extra bit.
                // TODO: try putting the shift in the DMA callback function, then remove it here

                delay_buf_[index_write_ + i] = (input_buf_[i] >> 1) - k_sign_offset;

                int32_t feedback_sample = S8S16MULSHIFT8(delay_buf_[read_index], p.feedback);
                int32_t combined_sample = delay_buf_[index_write_ + i] + feedback_sample;
                delay_buf_[index_write_ + i] = combined_sample;

                int32_t output_sample = S8S16MULSHIFT8(delay_buf_[read_index], p.amount) << 1;
                buf_ptr_[i] = U12CLIPMAXMIN(output_sample + k_sign_offset);

                read_position_ += (1 << 8); */

#else
                
                delay_buf_[index_write_ + i] = input_buf_[i] >> 1;

                uint32_t feedback_sample = U16U8MULSHIFT8(delay_buf_[read_index], p.feedback);
                uint32_t combined_sample = delay_buf_[index_write_ + i] + feedback_sample;
                delay_buf_[index_write_ + i] = U12CLIPMAXMIN(combined_sample);

                uint32_t output_sample = U16U8MULSHIFT6(delay_buf_[read_index], p.amount) >> 1;
                buf_ptr_[i] = U12CLIPMAXMIN(output_sample);

                read_position_ += (1 << 8);
            
#endif
            }

            index_write_ += block_size_;
            if(index_write_ >= size) index_write_ -= size;
    
        }

        void setInputBufferPointer(uint16_t *input_buf) {

            input_buf_ = input_buf;

        }

        delay_inputs_t* getParamPointer() { return &p; }

    private:

        delay_inputs_t p;
#ifdef DELAY_SIGNED

        int16_t delay_buf_[size]__attribute__((aligned(size)));
        const int k_sign_offset = 2048;

#else
        uint16_t delay_buf_[size];

#endif
        uint16_t *buf_ptr_;
        uint16_t *input_buf_;
        uint16_t block_size_;
        uint16_t index_write_ = 0;
        uint index_;
        uint current_frame_;

        uint16_t last_write_offset_;
        uint16_t write_offset_difference_;

        const uint8_t max_length_ = 192; // allow room in buffer for stereo
        
        MovingAverage<int32_t> writeAvg = MovingAverage<int32_t>(64);

        int32_t step_;
        int32_t last_time_;
        int32_t current_time_;
        int32_t read_position_;

};

#endif