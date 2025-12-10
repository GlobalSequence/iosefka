#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include "pico/stdlib.h"

#define BIT_8_TO_5_SHIFT 3
#define BIT_8_TO_2_SHIFT 6

#define BIT_WIDTH_ADD_8_TO_16 1
#define BIT_WIDTH_MULT_8_TO_16 2

#define SHIFT_16_TO_1  15
#define SHIFT_16_TO_2  14
#define SHIFT_16_TO_3  13
#define SHIFT_16_TO_4  12
#define SHIFT_16_TO_5  11
#define SHIFT_16_TO_6  10
#define SHIFT_16_TO_7   9
#define SHIFT_16_TO_8   8
#define SHIFT_16_TO_9   7
#define SHIFT_16_TO_10  6
#define SHIFT_16_TO_11  5
#define SHIFT_16_TO_12  4
#define SHIFT_16_TO_13  3
#define SHIFT_16_TO_14  2
#define SHIFT_16_TO_15  1

inline int32_t linearScale(int32_t amount, int32_t in_max, int32_t out_max) {

  return amount * out_max / in_max;

}

inline uint8_t scaleParam0to5(uint8_t value) {
  return value / 51;
}

inline uint8_t scaleParam0to25(uint8_t value) {
  return value / 10;
}

inline uint scale8BitValueToParam(uint8_t input, uint max) {
  return (uint)(float)input / 255.0 * max;
}

inline uint shiftBy(uint value, uint8_t shift) {
    return value >> shift;
}

inline int cycleIntAddOrSub(int value, int min, int max, int advance_by) {
  int out = value + advance_by;
  if(out > max) out = min;
  else if(out < min) out = max;
  return out;
}

inline uint cycleParameterValue(uint current_value, uint max, int increment) {
    current_value = (current_value + increment) % max;
    current_value = current_value >= max? max - 1 : current_value; 
    return current_value; 
}

inline uint selectParameterValueMinMax(uint current_val, uint min, uint max, int8_t increment) {
    uint16_t output_val_ = current_val + increment;
    if(output_val_ > current_val && increment < 0) { 
        output_val_ = min;
    } else if (output_val_ >= max) {
        output_val_ = max;
    } else if (output_val_ <= min) {
        output_val_ = min;
    }

    return output_val_;
}

inline int limitUpperValue(int value, int limit) {
    if(value >= limit) {
        return limit;
    }
    return value;
}

inline int limitLowerValue(int value, int limit) {
    if(value <= limit) {
        return limit;
    }
    return value;
}

static inline int8_t bipolarParameter(uint8_t input) { return input + 128; }

static inline void addWithCarry(volatile uint32_t &phase, uint32_t inc, volatile bool *carry) {
  __asm__ __volatile__(
      //"adds %[result], %[phase_in], %[inc]\n" // Pico ARM toolchain does not accept adds, but treats add the same I think
      "add %[result], %[phase_in], %[inc]\n"
      "mov %[carry], #0\n"
      "adc %[carry], %[carry], %[carry]\n"
      "mov %[phase_out], %[result]\n"
      : [phase_out] "=r"(phase), [result] "=r"(phase), [carry] "=r"(*carry)
      : [phase_in] "r"(phase), [inc] "r"(inc));
}

static inline uint32_t U16MixU32(uint16_t a, uint16_t b, uint16_t limit, uint16_t balance) {
  return a * (limit - balance) + b * balance;
}

static inline uint32_t U32MixU32(uint32_t a, uint32_t b, uint16_t limit, uint16_t balance) {
  return a * (limit - balance) + b * balance;
}

static inline uint8_t U8Mix(uint8_t a, uint8_t b, uint8_t balance) {
  return a * (255 - balance) + b * balance >> 8;
}

static inline uint8_t U8MixConst(const uint8_t a, const uint8_t b, uint8_t balance) {
  return a * (255 - balance) + b * balance >> 8;
}

static inline uint16_t U8MixU16(uint8_t a, uint8_t b, uint8_t balance) {
  return a * (255 - balance) + b * balance;
}

inline int16_t Mix(int16_t a, int16_t b, uint16_t balance) {
  return (a * (65535 - balance) + b * balance) >> 16;
}

inline uint16_t Mix(uint16_t a, uint16_t b, uint16_t balance) {
  return (a * (65535 - balance) + b * balance) >> 16;
}

inline int16_t Interpolate824(const int16_t* table, uint32_t phase) {
  int32_t a = table[phase >> 24];
  int32_t b = table[(phase >> 24) + 1];
  return a + ((b - a) * static_cast<int32_t>((phase >> 8) & 0xffff) >> 16);
}

inline uint16_t Interpolate824(const uint16_t* table, uint32_t phase) {
  uint32_t a = table[phase >> 24];
  uint32_t b = table[(phase >> 24) + 1];
  return a + ((b - a) * static_cast<uint32_t>((phase >> 8) & 0xffff) >> 16);
}

inline int16_t Interpolate824(const uint8_t* table, uint32_t phase) {
  int32_t a = table[phase >> 24];
  int32_t b = table[(phase >> 24) + 1];
  return (a << 8) + \
      ((b - a) * static_cast<int32_t>(phase & 0xffffff) >> 16) - 32768;
}

static inline uint8_t InterpolateSample(
    const uint8_t *table,
    uint16_t phase) {
  const uint8_t *tbl_ptr = table + (phase >> 8);
  const uint8_t *tbl_ptr_2 = 1 + table + (phase >> 8);

  return U8MixConst(*tbl_ptr, *tbl_ptr_2, phase & 0xff);
}

static inline uint16_t InterpolateSample16(
  const uint8_t *table,
  uint16_t phase
) {
  const uint8_t *tbl_ptr = table + (phase >> 8);
  const uint8_t *tbl_ptr_2 = 1 + table + (phase >> 8);
  uint16_t tbl_16 = *tbl_ptr << 8;
  uint16_t tbl_16_2 = *tbl_ptr_2 << 8;

  return U16MixU32(tbl_16, tbl_16_2, 255, phase & 0xFF) >> 8;
}

static inline uint16_t U8U8Mul(uint8_t a, uint8_t b) {
  return a * b;
}

static inline int16_t S8U8Mul(int8_t a, uint8_t b) {
  return a * b;
}

static inline int16_t S8S8Mul(int8_t a, int8_t b) {
  return a * b;
}

static inline int16_t U8S8MULSHIFT8(uint16_t signal, int8_t multiplier) {
  return (signal * multiplier) >> 8;
}

static inline int16_t U8S16MULSHIFT8(uint16_t signal, int16_t multiplier) {
  return (signal * multiplier) >> 8;
}

static inline uint16_t U16U8MULSHIFT8(uint16_t signal, uint8_t multiplier) {
  return (signal * multiplier) >> 8;
}

static inline uint16_t U16U8MULSHIFT4(uint16_t signal, uint8_t multiplier) {
  return (signal * multiplier) >> 4;
}

static inline int16_t S8S16MULSHIFT8(int16_t signal, int16_t multiplier) {
  return (signal * multiplier) >> 8;
}

static inline uint16_t U16U8MULSHIFT6(uint16_t signal, uint8_t multiplier) {
  return (signal * multiplier) >> 6;
}

static inline int16_t S16U8MULSHIFT6(int16_t signal, uint8_t multiplier) {
  return (signal * multiplier) >> 6;
}

static inline uint8_t U15ShiftRight7(uint16_t value) {
  return value >> 7;
}

static inline uint8_t S16ClipU8(int16_t value) {
  return value < 0 ? 0 : (value > 255 ? 255 : value);
}

static inline int16_t S16Clip(int16_t value) {
  if (value < -16383) {
    return -16383;
  }
  else if (value > 16383) {
    return 16383;
  }
  else {
    return value;
  }
}

static inline int16_t S16ClipU14(int16_t value) {
  uint8_t msb = static_cast<uint16_t>(value) >> 8;
  if (msb & 0x80) {
    return 0;
  }
  if (msb & 0x40) {
    return 16383;
  }
  return value;
}

static inline uint8_t U8CLIPMAX(uint8_t max, uint8_t value) {
  uint8_t output;
  value >= max? output = max - 1 : output = value;
  return output;
}

static inline uint16_t U12CLIPMAXMIN(int16_t value) {
    if (value >= 4095) {
        return 4095;
    }
    else if (value <= 0) {
        return 0;
    }
  return value;
}

static inline int16_t S12CLIP(int16_t value) {

  if(value >= 2047) return 2047;
  else if (value <= -2048) return 2048;
  return value;

}

static inline uint16_t U16CLIPMAXMINVAR(int16_t maxValue, int16_t value) {
    if (value >= maxValue) {
        return maxValue;
    }
    else if (value <= 0) {
        return 0;
    }
  return value;
}

static inline uint8_t S16CLIPU8(int16_t value) {
  return value < 0 ? 0 : (value > 255 ? 255 : value);
}

static inline int16_t S16CLIPMAXMIN(int16_t value) {
  if (value <= 0) {
    return 0;
  }
  else if (value >= 32767) {
    return 32767;
  }
  else {
    return value;
  }
}

static inline int16_t S32CLIPS16(int32_t value) {
  if (value < -32768) {
    return -32768;
  }
  else if (value > 32767) {
    return 32767;
  }
  else {
    return value;
  }
}

static inline int16_t S16CLIPU14(int16_t value) {
  uint8_t msb = static_cast<uint16_t>(value) >> 8;
  if (msb & 0x80) {
    return 0;
  }
  if (msb & 0x40) {
    return 16383;
  }
  return value;
}


static inline void XOR798(volatile uint16_t *seed) {
  uint16_t x = *seed;
  x ^= x << 7;
  x ^= x >> 9;
  x ^= x << 8;
  *seed = x;
}

// typedef struct RingBufferState
// {
//   uint8_t size;
//   uint8_t ctr;
//   uint16_t total;
//   uint8_t buffer[size];
// };

static inline uint8_t U8RingBuffer(uint8_t size, uint8_t& ctr, uint8_t& total, uint8_t* buffer, uint8_t sample) {
    total -= *(buffer + ctr);
    *(buffer + ctr) = sample;
    total += *(buffer + ctr);

    ctr >= size - 1 ? ctr= 0 : ctr++;

    return total / size;
}

static inline uint8_t param8Bits0To1(uint8_t value) {

  return value >> 7;
}

static inline uint8_t param8Bits0To2(uint8_t value) {

  if (value >= 0 && value < 86) {
    return 0;
  } else if (value >= 86 && value < 172) {
    return 1;
  } else { // value >= 172 && value <= 255
    return 2;
  }
  return 0;
}

static inline uint8_t param8Bits0To3(uint8_t value) {

  return value >> 6;
}

static inline uint8_t param8Bits0To4(uint8_t value) {

  if (value >= 0 && value < 51) {
    return 0;
  } else if (value >= 51 && value < 102) {
    return 1;
  } else if (value >= 102 && value < 153) {
    return 2;
  } else if (value >= 153 && value < 204) {
    return 3;
  } else { // value >= 204 && value <= 255
    return 4;
  }
  return 0;
}

static inline uint8_t param8Bits0To5(uint8_t value) {

  if (value >= 0 && value < 43) {
    return 0;
  } else if (value >= 43 && value < 85) {
    return 1;
  } else if (value >= 85 && value < 128) {
    return 2;
  } else if (value >= 128 && value < 170) {
    return 3;
  } else if (value >= 170 && value < 213) {
    return 4;
  } else { // value >= 213 && value <= 255
    return 5;
  }
  return 0;
}

static inline uint8_t param8Bits0To7(uint8_t value) {
  return value >> 5;
}

static inline uint8_t param8Bits0To9(uint8_t value) {
  if (value >= 0 && value < 26) {
    return 0;
  } else if (value >= 26 && value < 51) {
    return 1; 
  } else if (value >= 51 && value < 77) {
    return 2; 
  } else if (value >= 77 && value < 102) {
    return 3; 
  } else if (value >= 102 && value < 128) {
    return 4;  
  } else if (value >= 128 && value < 153) {
    return 5;  
  } else if (value >= 153 && value < 179) {
    return 6;  
  } else if (value >= 179 && value < 204) {
    return 7; 
  } else if (value >= 204 && value < 230) {
    return 8;  
  } else { // value >= 230 && value <= 255
    return 9;  
  }
  return 0;
}

static inline uint8_t param8Bits0To10(uint8_t value) {

    if (value < 24) {
        return 0;
    } else if (value < 47) {
        return 1;
    } else if (value < 70) {
        return 2;
    } else if (value < 93) {
        return 3;
    } else if (value < 116) {
        return 4;
    } else if (value < 139) {
        return 5;
    } else if (value < 162) {
        return 6;
    } else if (value < 185) {
        return 7;
    } else if (value < 208) {
        return 8;
    } else if (value < 231) {
        return 9;
    } else {
        return 10;
    }
}

static inline uint8_t param8Bits0To11(uint8_t value) {

  if (value >= 0 && value < 22) {
    return 0;
  } else if (value >= 22 && value < 43) {
    return 1;
  } else if (value >= 43 && value < 64) {
    return 2;
  } else if (value >= 64 && value < 85) {
    return 3;
  } else if (value >= 85 && value < 106) {
    return 4;
  } else if (value >= 106 && value < 127) {
    return 5;
  } else if (value >= 127 && value < 148) {
    return 6;
  } else if (value >= 148 && value < 169) {
    return 7;
  } else if (value >= 169 && value < 190) {
    return 8;
  } else if (value >= 190 && value < 211) {
    return 9;
  } else if (value >= 211 && value < 232) {
    return 10;
  } else { // value >= 232 && value <= 255
    return 11;
  }
  return 0;
}

static inline uint8_t param8Bits0To24(uint8_t value) {
  if (value >= 0 && value < 10) {
      return 0;
  } else if (value >= 10 && value < 20) {
      return 1;
  } else if (value >= 20 && value < 30) {
      return 2;
  } else if (value >= 30 && value < 40) {
      return 3;
  } else if (value >= 40 && value < 50) {
      return 4;
  } else if (value >= 50 && value < 60) {
      return 5;
  } else if (value >= 60 && value < 70) {
      return 6;
  } else if (value >= 70 && value < 80) {
      return 7;
  } else if (value >= 80 && value < 90) {
      return 8;
  } else if (value >= 90 && value < 100) {
      return 9;
  } else if (value >= 100 && value < 110) {
      return 10;
  } else if (value >= 110 && value < 120) {
      return 11;
  } else if (value >= 120 && value < 130) {
      return 12;
  } else if (value >= 130 && value < 140) {
      return 13;
  } else if (value >= 140 && value < 150) {
      return 14;
  } else if (value >= 150 && value < 160) {
      return 15;
  } else if (value >= 160 && value < 170) {
      return 16;
  } else if (value >= 170 && value < 180) {
      return 17;
  } else if (value >= 180 && value < 190) {
      return 18;
  } else if (value >= 190 && value < 200) {
      return 19;
  } else if (value >= 200 && value < 210) {
      return 20;
  } else if (value >= 210 && value < 220) {
      return 21;
  } else if (value >= 220 && value < 230) {
      return 22;
  } else if (value >= 230 && value < 240) {
      return 23;
  } else { // value >= 240 && value <= 255
      return 24;
  }
  return 0;
}

static inline uint8_t param8bits0To31(uint8_t value) {
  
  return value >> 3;
}

static inline uint8_t param8BitsOscScale(uint8_t value) {
  if (value == 0) {
    return 0;
  } else if (value > 0 && value < 12) {
    return 12;
  } else if (value >= 12 && value < 23) {
    return 22;
  } else if (value >= 23 && value < 34) {
    return 33;
  } else if (value >= 34 && value < 45) {
    return 43;
  } else if (value >= 45 && value < 56) {
    return 55;
  } else if (value >= 56 && value < 67) {
    return 65;
  } else if (value >= 67 && value < 78) {
    return 77;
  } else if (value >= 78 && value < 89) {
    return 86;
  } else if (value >= 89 && value < 100) {
    return 96;
  } else if (value >= 100 && value < 111) {
    return 107;
  } else if (value >= 111 && value < 122) {
    return 117;
  } else if (value >= 122 && value < 133) {
    return 128;
  } else if (value >= 133 && value < 144) {
    return 138;
  } else if (value >= 144 && value < 155) {
    return 149;
  } else if (value >= 155 && value < 166) {
    return 159;
  } else if (value >= 166 && value < 177) {
    return 170;
  } else if (value >= 177 && value < 188) {
    return 180;
  } else if (value >= 188 && value < 199) {
    return 191;
  } else if (value >= 199 && value < 210) {
    return 201;
  } else if (value >= 210 && value < 221) {
    return 212;
  } else if (value >= 221 && value < 232) {
    return 222;
  } else if (value >= 232 && value < 243) {
    return 233;
  } else if (value >= 243 && value < 254) {
    return 243;
  } else { // value == 255
    return 255;
  }
  return 0;
}



#endif