#ifndef ENVELOPE_H_
#define ENVELOPE_H_

#include <stdio.h>
#include "pico/stdlib.h"
#include <stdio.h>
#include "resources.h"
#include "../common/utilities.h"
#include "../common/test.h"

#define ENV_DEFAULT_RESOLUTION_BITS 16

// Uncomment this to use it as an AR envelope (No hold/sustain stage);
// #define AR_ENVELOPE

enum EnvelopeStage
{
#ifndef AR_ENVELOPE
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE,
    DEAD,

    NUM_SEGMENTS
# else
    ATTACK,
    RELEASE,
    DEAD,

    DECAY,
    SUSTAIN,
    NUM_SEGMENTS
#endif
};

typedef struct envelope_t {

    uint16_t stage_phase_increment_[NUM_SEGMENTS];
    uint16_t stage_target_[NUM_SEGMENTS];
    uint16_t a_;
    uint16_t b_;
    uint16_t phase_increment_;
    uint16_t phase_;
    uint16_t value_;
    uint8_t stage_;
    bool linear_ = false;

} envelope_t;

inline void EnvInit(struct envelope_t* e)
{
    e->stage_target_[ATTACK] = 255;
    e->stage_target_[RELEASE] = 0;
    e->stage_target_[DEAD] = 0;
    e->stage_phase_increment_[SUSTAIN] = 0;
    e->stage_phase_increment_[DEAD] = 0;
}

inline void EnvTrigger(uint8_t stage, struct envelope_t* e)
{
    if (stage == DEAD)
    {
        e->value_ = 0;
    }
    e->a_ = e->value_ >> 8;
    e->b_ = e->stage_target_[stage];
    e->stage_ = stage;
    e->phase_ = 0;
}

inline void EnvUpdate(
    uint8_t attack,
    uint8_t decay,
    uint8_t sustain,
    uint8_t release,
    struct envelope_t* e)
{
    e->stage_phase_increment_[ATTACK] = res_env_portamento_increments[attack >> 1];
    e->stage_phase_increment_[DECAY] = res_env_portamento_increments[decay >> 1];
    e->stage_phase_increment_[RELEASE] = res_env_portamento_increments[release >> 1];
    e->stage_target_[DECAY] = sustain;
    e->stage_target_[SUSTAIN] = e->stage_target_[DECAY];

    // added this line so that you could dynamically update or modulate the parameters
    // it's also why you need to call this function whenever the envelope updates
    e->phase_increment_ = e->stage_phase_increment_[e->stage_];
}

inline uint16_t EnvRender(struct envelope_t* e)
{
    e->phase_ += e->phase_increment_;
    if (e->phase_ < e->phase_increment_)
    {
        e->value_ = U8MixU16(e->a_, e->b_, 255);
        EnvTrigger(++e->stage_, e);
    }
    if (e->phase_increment_)
    {
        uint8_t step;

        if(!e->linear_) {

            step = InterpolateSample(wav_res_env_expo, e->phase_);

        } else {

            uint8_t phase_8 = e->phase_ >> 8;
            uint16_t phase_8_2 = phase_8 + 1;
            if(phase_8_2 >= 0xFF) phase_8_2 = 0xFF;

            step = U8Mix(phase_8, phase_8_2, e->phase_ & 0xFF);

        }

        e->value_ = U8MixU16(e->a_, e->b_, step);
    }

    return e->stage_ == SUSTAIN ? e->stage_target_[DECAY] << 8 : e->value_;
}

inline void EnvInit2(struct envelope_t* e)
{
    e->stage_target_[ATTACK] = 65535;
    e->stage_target_[RELEASE] = 0;
    e->stage_target_[DEAD] = 0;
    e->stage_phase_increment_[SUSTAIN] = 0;
    e->stage_phase_increment_[DEAD] = 0;
}

inline void EnvTrigger2(uint8_t stage, struct envelope_t* e)
{
    if (stage == DEAD)
    {
        e->value_ = 0;
    }
    e->a_ = e->value_;
    e->b_ = e->stage_target_[stage];
    e->stage_ = stage;
    e->phase_ = 0;
}

inline void EnvUpdate2(
    uint8_t attack,
    uint8_t decay,
    uint8_t sustain,
    uint8_t release,
    struct envelope_t* e)
{
    e->stage_phase_increment_[ATTACK] = res_env_portamento_increments[attack >> 1];
    e->stage_phase_increment_[DECAY] = res_env_portamento_increments[decay >> 1];
    e->stage_phase_increment_[RELEASE] = res_env_portamento_increments[release >> 1];
    e->stage_target_[DECAY] = sustain << 8;
    e->stage_target_[SUSTAIN] = e->stage_target_[DECAY];

    // added this line so that you could dynamically update or modulate the parameters
    // it's also why you need to call this function whenever the envelope updates
    e->phase_increment_ = e->stage_phase_increment_[e->stage_];
}

inline uint16_t EnvRender2(struct envelope_t* e)
{
    e->phase_ += e->phase_increment_;
    if (e->phase_ < e->phase_increment_)
    {
        e->value_ = U16MixU32(e->a_, e->b_, 255, 255) >> 8;
        EnvTrigger(++e->stage_, e);
    }
    if (e->phase_increment_)
    {
        uint8_t step;
        uint16_t step_up; 

        if(!e->linear_) {

            step_up = InterpolateSample16(wav_res_env_expo, e->phase_);


        } else {

            uint8_t phase_8 = e->phase_ >> 8;
            uint16_t phase_8_2 = phase_8 + 1;
            if(phase_8_2 >= 0xFF) phase_8_2 = 0xFF;

            step = U8Mix(phase_8, phase_8_2, e->phase_ & 0xFF);

        }

        e->value_ = U16MixU32(e->a_, e->b_, 65535, step_up) >> 16;

    }

    return e->stage_ == SUSTAIN ? e->stage_target_[DECAY] : e->value_;
}

enum EnvelopeSegment {
  ENV_SEGMENT_ATTACK,
  ENV_SEGMENT_DECAY,
  ENV_SEGMENT_SUSTAIN,
  ENV_SEGMENT_RELEASE,
  ENV_SEGMENT_DEAD,
  ENV_NUM_SEGMENTS,
};

class Envelope {
 public:
  Envelope() { }
  ~Envelope() { }

  void Init() {
    target_[ENV_SEGMENT_ATTACK] = 65535;
    target_[ENV_SEGMENT_RELEASE] = 0;
    target_[ENV_SEGMENT_DEAD] = 0;
    increment_[ENV_SEGMENT_SUSTAIN] = 0;
    increment_[ENV_SEGMENT_DEAD] = 0;
  }

  inline EnvelopeSegment segment() const {
    return static_cast<EnvelopeSegment>(segment_);
  }

  inline void Update(int32_t a, int32_t d, int32_t s, int32_t r, bool loop) {
    increment_[ENV_SEGMENT_ATTACK] = lut_env_portamento_increments[a];
    increment_[ENV_SEGMENT_DECAY] = lut_env_portamento_increments[d];
    increment_[ENV_SEGMENT_RELEASE] = lut_env_portamento_increments[r];
    target_[ENV_SEGMENT_DECAY] = s << 8;
    target_[ENV_SEGMENT_SUSTAIN] = target_[ENV_SEGMENT_DECAY];

    loop_ = loop;
  }
  
  inline void Trigger(EnvelopeSegment segment) {
    
    if(loop_) {

        switch(segment) {

            case SUSTAIN:
                segment = ENV_SEGMENT_RELEASE;
                break;
            case DEAD:
                segment = ENV_SEGMENT_ATTACK;
                break;
            default:
                break;

        }

    }
    
    if (segment == ENV_SEGMENT_DEAD) {
            value_ = 0;
    }
    a_ = value_;
    b_ = target_[segment];
    segment_ = segment;
    phase_ = 0;
  }

  inline uint16_t Render() {
    uint32_t increment = increment_[segment_];
    phase_ += increment;
    if (phase_ < increment) {
      value_ = Mix(a_, b_, 65535);
      Trigger(static_cast<EnvelopeSegment>(segment_ + 1));
    }
    if (increment_[segment_]) {
      value_ = Mix(a_, b_, Interpolate824(lut_env_expo, phase_));
    }
    return value_;
  }
  
  inline uint16_t value() const { return value_; }

 private:
  // Phase increments for each segment.
  uint32_t increment_[ENV_NUM_SEGMENTS];
  
  // Value that needs to be reached at the end of each segment.
  uint16_t target_[ENV_NUM_SEGMENTS];
  
  // Current segment.
  size_t segment_;
  
  // Start and end value of the current segment.
  uint16_t a_;
  uint16_t b_;
  uint16_t value_;
  uint32_t phase_;

  bool loop_;

//   DISALLOW_COPY_AND_ASSIGN(Envelope);
};


#endif