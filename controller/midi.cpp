#include "midi.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "settings.h"
#include "note_queue.h"
#include "../common/globals.h"
#include "../common/ring_buffer.h"
#include "../devices/peripheral_config.h"
#include "../controller/parameter.h"
#include "../common/test.h"

// #define MIDI_RX_BUFFER_SIZE 64
#define MIDI_RX_BUFFER_MASK (MIDI_RX_BUFFER_SIZE - 1)

enum CCParamNames
{
    CC_MOD_WHEEL = 1,
    CC_BREATH_CONTROL = 2,
    CC_OSC_1_PITCH = 10,
    CC_OSC_1_OCTAVE = 11,
    CC_OSC_1_X_MORPH = 12,
    CC_OSC_1_Y_MORPH = 13,
    CC_OSC_2_PITCH = 14,
    CC_OSC_2_OCTAVE = 15,
    CC_OSC_2_X_MORPH = 16,
    CC_OSC_2_Y_MORPH = 17,
    CC_PORTAMENTO = 5,
    CC_PHASE_MOD = 18,
    CC_SYNC = 19,
    CC_MIX_OSC_1_MIX = 20,
    CC_MIX_OSC_2_MIX = 21,
    CC_MIX_NOISE_MIX = 22,
    CC_FILTER_CUTOFF = 74,
    CC_FILTER_RESONANCE = 71,
    CC_FILTER_TYPE = 72,
    CC_FILTER_FM = 73,
    CC_FILTER_TRACKING = 76,
    CC_FILTER_ENV_AMT = 75,
    CC_VCA_DRONE_MODE = 77,
    CC_ENV_1_ATTACK = 25,
    CC_ENV_1_DECAY = 26,
    CC_ENV_1_SUSTAIN = 27,
    CC_ENV_1_RELEASE = 28,
    CC_ENV_1_AMOUNT = 29,
    CC_ENV_2_ATTACK = 30,
    CC_ENV_2_DECAY = 31,
    CC_ENV_2_SUSTAIN = 32,
    CC_ENV_2_RELEASE = 33,
    CC_ENV_2_AMOUNT = 34,
    CC_ENV_3_ATTACK = 35,
    CC_ENV_3_DECAY = 36,
    CC_ENV_3_SUSTAIN = 37,
    CC_ENV_3_RELEASE = 38,
    CC_ENV_3_AMOUNT = 39,
    CC_LFO_1_RATE = 40,
    CC_LFO_1_SHAPE = 41,
    CC_LFO_1_PHASE = 42,
    CC_LFO_1_SYNC = 43,
    CC_LFO_1_AMOUNT = 44,
    CC_LFO_2_RATE = 45,
    CC_LFO_2_SHAPE = 46,
    CC_LFO_2_PHASE = 47,
    CC_LFO_2_SYNC = 48,
    CC_LFO_2_AMOUNT = 49,
    CC_LFO_3_RATE = 50,
    CC_LFO_3_SHAPE = 51,
    CC_LFO_3_PHASE = 52,
    CC_LFO_3_SYNC = 53,
    CC_LFO_3_AMOUNT = 54,
    CC_ENV_1_DST = 55,
    CC_ENV_2_DST = 56,
    CC_ENV_3_DST = 57,
    CC_LFO_1_DST = 58,
    CC_LFO_2_DST = 59,
    CC_LFO_3_DST = 60,
    CC_DELAY_RATE = 61,
    CC_DELAY_FEEDBACK = 62,
    CC_DELAY_AMOUNT = 63,
    CC_DELAY_TYPE = 64,
    CC_DELAY_INPUT = 65,
    // CC_OSCS_PITCH_COARSE = 80,
    // CC_OSCS_PITCH_FINE = 81,
    // CC_OSCS_OCTAVE = 82,
    // CC_OSCS_X_WAVE = 83,
    // CC_OSCS_Y_WAVE = 84,
    CC_BRR = 85,
    CC_SRR = 86,
    CC_VCA = 87,
    CC_ENV_1_LOOP = 88,
    CC_ENV_2_LOOP = 89,
    CC_ENV_3_LOOP = 90,
    NUMBER_OF_MIDI_CCS
};

typedef struct midi_event_t {
    uint8_t type;
    uint8_t val1;
    uint8_t val2;
} midi_event_t;

const uint8_t midiMsgLengths[] = {
    3, // 0x8x - Note off
    3, // 0x9x - Note on
    3, // 0xAx - Polyphonic key pressure
    3, // 0xBx - Control change
    2, // 0xCx - Program change
    2, // 0xDx - Channel pressure
    3, // 0xEx - Pitch bend
    1, // 0xFx - Sysex / realtime
};

static synth_state_t *synth;
midiPgmChgFunc pgmChgFunc;

RingBuffer<uint8_t, MIDI_TX_BUFFER_SIZE> midiTxBuffer;
uint8_t last_status_byte;

// convert incoming 7 bit values to 8 bits, adding 1 to make shifted values of 127 into 255
uint8_t shift7BitValueTo8Bits(uint8_t val);
void midiProcessEvent(midi_event_t m);
void ccParser(uint8_t cc_number_, uint8_t value);

static const unsigned long midi_ext_clk_timeout = 3000000;
static uint midi_tick_period;
static unsigned long midi_last_tick_time;

static uint8_t midiBytes[4];
static uint16_t midiLength;
static uint16_t midiCount;

// ################################
// PRIVATE FUNCTIONS
// ################################

uint8_t shift7BitValueTo8Bits(uint8_t val) {
    val <<= 1;
    if(val == 254) val = 255;
    return val;
}

void ccParser(uint8_t cc_number, uint8_t value) {

    uint8_t value_8b = shift7BitValueTo8Bits(value);

    mod_destinations_t dst;

    switch(cc_number){
        case CC_MOD_WHEEL: 
            dst = (mod_destinations_t)synth->patch.mod[MOD_SRC_MOD_WHEEL].destination; 
            // Mod wheel only does positive modulation for mod destinations
            switch(dst) {
                case MOD_DST_ENV_1_AMOUNT:
                case MOD_DST_ENV_2_AMOUNT:
                case MOD_DST_ENV_3_AMOUNT:
                case MOD_DST_LFO_1_AMOUNT:
                case MOD_DST_LFO_2_AMOUNT:
                case MOD_DST_LFO_3_AMOUNT:
                case MOD_DST_FILTER_ENV_AMT:
                    value_8b = value + 0x80;
                    break;
                default: break;
            }

            break;
        case CC_BREATH_CONTROL: dst = (mod_destinations_t)synth->patch.mod[MOD_SRC_BREATH].destination; break;
        case CC_OSC_1_PITCH: dst = MOD_DST_OSC_1_PITCH; break;
        case CC_OSC_1_OCTAVE: dst = MOD_DST_OSC_1_OCTAVE; break;
        case CC_OSC_1_X_MORPH: dst = MOD_DST_OSC_1_X_MORPH; break;
        case CC_OSC_1_Y_MORPH: dst = MOD_DST_OSC_1_Y_MORPH; break;
        case CC_OSC_2_PITCH: dst = MOD_DST_OSC_2_PITCH; break;
        case CC_OSC_2_OCTAVE: dst = MOD_DST_OSC_2_OCTAVE; break;
        case CC_OSC_2_X_MORPH: dst = MOD_DST_OSC_2_X_MORPH; break;
        case CC_OSC_2_Y_MORPH: dst = MOD_DST_OSC_2_Y_MORPH; break;
        case CC_PORTAMENTO: dst = MOD_DST_PORTAMENTO; break;
        case CC_PHASE_MOD: dst = MOD_DST_PHASE_MOD; break;
        case CC_SYNC: dst = MOD_DST_SYNC; break;
        case CC_MIX_OSC_1_MIX: dst = MOD_DST_MIX_OSC_1_MIX; break;
        case CC_MIX_OSC_2_MIX: dst = MOD_DST_MIX_OSC_2_MIX; break;
        case CC_MIX_NOISE_MIX: dst = MOD_DST_MIX_NOISE_MIX; break;
        case CC_FILTER_CUTOFF: dst = MOD_DST_FILTER_CUTOFF; break;
        case CC_FILTER_RESONANCE: dst = MOD_DST_FILTER_RESONANCE; break;
        case CC_FILTER_ENV_AMT: dst = MOD_DST_FILTER_ENV_AMT; break;
        case CC_FILTER_TYPE: dst = MOD_DST_FILTER_TYPE; break;
        case CC_FILTER_FM: dst = MOD_DST_FILTER_FM; break;
        case CC_FILTER_TRACKING: dst = MOD_DST_FILTER_TRACKING; break;
        case CC_VCA_DRONE_MODE: dst = MOD_DST_VCA; break;
        case CC_ENV_1_ATTACK: dst = MOD_DST_ENV_1_ATTACK; break;
        case CC_ENV_1_DECAY: dst = MOD_DST_ENV_1_DECAY; break;
        case CC_ENV_1_SUSTAIN: dst = MOD_DST_ENV_1_SUSTAIN; break;
        case CC_ENV_1_RELEASE: dst = MOD_DST_ENV_1_RELEASE; break;
        case CC_ENV_2_ATTACK: dst = MOD_DST_ENV_2_ATTACK; break;
        case CC_ENV_2_DECAY: dst = MOD_DST_ENV_2_DECAY; break;
        case CC_ENV_2_SUSTAIN: dst = MOD_DST_ENV_2_SUSTAIN; break;
        case CC_ENV_2_RELEASE: dst = MOD_DST_ENV_2_RELEASE; break;
        case CC_ENV_3_ATTACK: dst = MOD_DST_ENV_3_ATTACK; break;
        case CC_ENV_3_DECAY: dst = MOD_DST_ENV_3_DECAY; break;
        case CC_ENV_3_SUSTAIN: dst = MOD_DST_ENV_3_SUSTAIN; break;
        case CC_ENV_3_RELEASE: dst = MOD_DST_ENV_3_RELEASE; break;
        case CC_LFO_1_RATE: dst = MOD_DST_LFO_1_RATE; break;
        case CC_LFO_1_SHAPE: dst = MOD_DST_LFO_1_SHAPE; break;
        case CC_LFO_1_PHASE: dst = MOD_DST_LFO_1_PHASE; break;
        case CC_LFO_1_SYNC: dst = MOD_DST_LFO_1_SYNC; break;
        case CC_LFO_2_RATE: dst = MOD_DST_LFO_2_RATE; break;
        case CC_LFO_2_SHAPE: dst = MOD_DST_LFO_2_SHAPE; break;
        case CC_LFO_2_PHASE: dst = MOD_DST_LFO_2_PHASE; break;
        case CC_LFO_2_SYNC: dst = MOD_DST_LFO_2_SYNC; break;
        case CC_LFO_3_RATE: dst = MOD_DST_LFO_3_RATE; break;
        case CC_LFO_3_SHAPE: dst = MOD_DST_LFO_3_SHAPE; break;
        case CC_LFO_3_PHASE: dst = MOD_DST_LFO_3_PHASE; break;
        case CC_LFO_3_SYNC: dst = MOD_DST_LFO_3_SYNC; break;
        case CC_ENV_1_AMOUNT: dst = MOD_DST_ENV_1_AMOUNT; break;
        case CC_ENV_2_AMOUNT: dst = MOD_DST_ENV_2_AMOUNT; break;
        case CC_ENV_3_AMOUNT: dst = MOD_DST_ENV_3_AMOUNT; break;
        case CC_LFO_1_AMOUNT: dst = MOD_DST_LFO_1_AMOUNT; break;
        case CC_LFO_2_AMOUNT: dst = MOD_DST_LFO_2_AMOUNT; break;
        case CC_LFO_3_AMOUNT: dst = MOD_DST_LFO_3_AMOUNT; break;
        case CC_ENV_1_DST: dst = MOD_DST_ENV_1_DST; break;
        case CC_ENV_2_DST: dst = MOD_DST_ENV_2_DST; break;
        case CC_ENV_3_DST: dst = MOD_DST_ENV_3_DST; break;
        case CC_LFO_1_DST: dst = MOD_DST_LFO_1_DST; break;
        case CC_LFO_2_DST: dst = MOD_DST_LFO_2_DST; break;
        case CC_LFO_3_DST: dst = MOD_DST_LFO_3_DST; break;
        case CC_DELAY_RATE: dst = MOD_DST_DELAY_RATE; break;
        case CC_DELAY_FEEDBACK: dst = MOD_DST_DELAY_FEEDBACK; break;
        case CC_DELAY_AMOUNT: dst = MOD_DST_DELAY_AMOUNT; break;
        case CC_DELAY_TYPE: dst = MOD_DST_DELAY_TYPE; break;
        case CC_DELAY_INPUT: dst = MOD_DST_DELAY_INPUT; break;
        // case CC_OSCS_PITCH_COARSE: dst = MOD_DST_OSC_BOTH_PITCH; break; // These are only handled internally and will not respond to ext. messages
        // case CC_OSCS_PITCH_FINE: dst = MOD_DST_OSC_BOTH_PITCH_FINE; break;
        // case CC_OSCS_OCTAVE: dst = MOD_DST_OSC_BOTH_OCTAVE; break;
        // case CC_OSCS_X_WAVE: dst = MOD_DST_OSC_BOTH_X_MORPH; break;
        // case CC_OSCS_Y_WAVE: dst = MOD_DST_OSC_BOTH_Y_MORPH; break;
        case CC_BRR: dst = MOD_DST_BITRATE_REDUCE; break;
        case CC_SRR: dst = MOD_DST_SAMPLERATE_REDUCE; break;
        case CC_VCA: dst = MOD_DST_VCA; break;
        case CC_ENV_1_LOOP: dst = MOD_DST_ENV_1_LOOP; break;
        case CC_ENV_2_LOOP: dst = MOD_DST_ENV_2_LOOP; break;
        case CC_ENV_3_LOOP: dst = MOD_DST_ENV_3_LOOP; break;
        default: break;
    }

    parameterSetValueByModDst(dst, value_8b);

}

void midiEnqueueTransfer(uint8_t type, uint8_t byte_1, uint8_t byte_2) {

    uint8_t out_length = midiMsgLengths[(type >> 4) & 0x7];

    uint8_t status_byte = type | synth->settings.channel;

    if(type & 0xF8 == 0xF8) {

        midiTxBuffer.enqueue(type);

        return;

    } 

    if(status_byte != last_status_byte) {

        last_status_byte = status_byte;

        midiTxBuffer.enqueue(status_byte);

    }

    if(out_length > 1) {

        midiTxBuffer.enqueue(byte_1);

    }

    if(out_length > 2) {

        midiTxBuffer.enqueue(byte_2);

    }

}

void midiEnqueueRealtime(uint8_t event_type) {

    midiTxBuffer.enqueue(event_type);

}

void midiTransmitNow() {

    while(!midiTxBuffer.isEmpty()) {

        // if the UART TX FIFO is writeable/is not full
        if(uart_is_writable(UART_MIDI)) {

            // dequeue the MIDI buffer
            uint8_t msg = midiTxBuffer.dequeue();

            // send the message
            uart_putc(UART_MIDI, msg);

        // if the UART TX FIFO is full
        } else {

            // leave and come back later to empty the MIDI buffer
            return;

        }

    }

}

// ################################
// PUBLIC FUNCTIONS
// ################################

void midiSetup(synth_state_t *synth_state, midiPgmChgFunc pgmChgFunction) {

    synth = synth_state;
    pgmChgFunc = pgmChgFunction;

}

bool midiRxHandler() {

    bool midi_event_happened = false;

    while(uart_is_readable(UART_MIDI)) {

        uint8_t b = uart_getc(UART_MIDI);

        // Realtime messages
        if ((b & 0xF8) == 0xF8)
        {
            bool allow_thru = false;

            switch (b)
            {
            case MIDI_TICK:

                if(synth->settings.clock_rx) {

                    synth->special.ext_clock_active = true;
                    uint64_t tick_time = time_us_64();
                    synth->ext_mod.clock_tick_period = tick_time - midi_last_tick_time;
                    synth->ext_mod.clock_tick_occurred = true;
                    midi_last_tick_time = tick_time;
                    if(synth->seq.isRunning()) synth->seq.tickCounter();
                    allow_thru = true;

                } 

                break;

            case MIDI_START:

                if(synth->settings.transport_rx) {
                    
                    synth->seq.run();
                    allow_thru = true;

                }

                break;

            case MIDI_CONTINUE:
                break;

            case MIDI_STOP:

                if(synth->settings.transport_rx) {

                    synth->seq.stop();
                    allow_thru = true;

                }

                break;

            default:
                break;
            }

            if(allow_thru) uart_putc(UART_MIDI, b);

            continue;
        }
        
        

        // New MIDI status
        if (b & 0x80)
        {
            midiBytes[0] = b;
            midiLength = midiMsgLengths[(b >> 4) & 0x7];
            midiCount = 1;
        }
        else
        {
            midiBytes[midiCount++] = b;
            if (midiCount >= 3)
                midiCount = 3;
        }

        /* Parse a complete message */
        if (!midiBytes[0])
            continue;
        if (midiBytes[0] == MIDI_SYSEX_BEGIN)
            continue;
        if (midiBytes[0] == MIDI_SYSEX_END)
            continue;

        if (midiCount == midiLength)
        {
            midiCount = 1;
            int status = midiBytes[0] & 0xF0;
            int channel = midiBytes[0] & 0x0F;  

            if (channel != synth->settings.channel)
                continue;

            midi_event_t m;
            m.type = status;
            m.val1 = midiBytes[1];
            m.val2 = midiBytes[2];

            midiProcessEvent(m);
        }

        midi_event_happened = true;

    }

    return midi_event_happened;

}

void midiProcessEvent(midi_event_t m) {

    switch (m.type) {
        case MIDI_NOTE_ON:
            if (m.val2){
                m.val2 = shift7BitValueTo8Bits(m.val2); 
                noteQueueSubmit(m.val1, m.val2, NOTE_ON);

            } else {

                noteQueueSubmit(m.val1, 0, NOTE_OFF);

            }
            break;

        case MIDI_NOTE_OFF:

            noteQueueSubmit(m.val1, 0, NOTE_OFF);

            break;

        case MIDI_CC:

            ccParser(m.val1, m.val2);

            break;

        case MIDI_PITCHBEND:

            synth->ext_mod.pitch_bend_amt = (m.val2 << 7) | m.val1;
            // TODO: THIS IS BAD
            // this variable is being handed to the ext_mod struct from the midi struct
            // there is no pointer to the midi struct in the voice library
            // so this is a band-aid because I didn't want to think about it :)
            // figure out later
            synth->ext_mod.pitch_bend_depth_send = synth->settings.pitch_bend_depth;

            break;

        // "aFtErToUcH"
        case MIDI_PRESSURE:

            synth->ext_mod.aftertouch_amt = m.val1;

            break;

        case MIDI_PROGRAM:

            if(synth->settings.program_change_rx && m.val1 < NUMBER_OF_PRESETS) {

                pgmChgFunc(m.val1);

            }

            break;

        default:
            break;
        }



}

void midiTxHandler() {

    while(noteQueueAvailableMidiOut()) {

        note_data_t note_data = noteQueuePullMidiOut();

        midiEnqueueTransfer(MIDI_NOTE_ON, note_data.note, note_data.velocity >> 1);

    }

    midiTransmitNow();

}

void midiExtClockCountdown() {

    if(synth->special.ext_clock_active) {

        unsigned long now = time_us_64();

        if(now - midi_last_tick_time >= midi_ext_clk_timeout) {

            synth->special.ext_clock_active = false;
    
        }

    }

}

void midiEnqueueNote(uint8_t note, uint8_t velocity) {

    midiEnqueueTransfer(MIDI_NOTE_ON, note, velocity);

}

void midiEnqueueTick() {

    midiEnqueueTransfer(MIDI_TICK, 0, 0);

}

void midiEnqueueStart() {

    midiEnqueueTransfer(MIDI_START, 0, 0);

}

void midiEnqueueStop() {

    midiEnqueueTransfer(MIDI_STOP, 0, 0);

}