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

#ifndef MIDI_H
#define MIDI_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "settings.h"
#include "../common/globals.h"


/* MIDI - Status bytes */
#define MIDI_NOTE_ON 0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_AFTERTOUCH 0xA0
#define MIDI_CC 0xB0
#define MIDI_PROGRAM 0xC0
#define MIDI_PRESSURE 0xD0
#define MIDI_PITCHBEND 0xE0

/* MIDI - Sysex bytes */
#define MIDI_SYSEX_BEGIN 0xF0
#define MIDI_SYSEX_END 0xF7

/* MIDI - Realtime messages */
#define MIDI_TICK 0xF8
#define MIDI_START 0xFA
#define MIDI_CONTINUE 0xFB
#define MIDI_STOP 0xFC
#define MIDI_ACTIVESENSING 0xFE

/* MIDI - Standard CC numbers */
#define MIDI_CC_MODWHEEL 1
#define MIDI_CC_VOLUME 7
#define MIDI_CC_BALANCE 8
#define MIDI_CC_PAN 10
#define MIDI_CC_EXPRESSION 11

#define MIDI_CC_WAVE 70
#define MIDI_CC_PATTERN 71
#define MIDI_CC_CUTOFF 74

/* MIDI - Special CC numbers */
#define MIDI_CC_ALLSOUNDSOFF 120
#define MIDI_CC_RESETCTRLS 121
#define MIDI_CC_ALLNOTESOFF 123
#define MIDI_CC_OMNIOFF 124
#define MIDI_CC_OMNION 125
#define MIDI_CC_MONO_MODE 126
#define MIDI_CC_POLY_MODE 127

typedef void (*midiSetParameter)(mod_destinations_t, uint8_t);
typedef void (*midiPgmChgFunc)(uint);

void midiSetup(synth_state_t *synth_state, midiPgmChgFunc pgmChgFunction);

bool midiRxHandler();
void midiTxHandler();
void midiProcessEvents();
void midiExtClockCountdown();

void midiEnqueueTransfer(uint8_t type, uint8_t byte_1, uint8_t byte_2);
void midiEnqueueRealtime(uint8_t event_type);
void midiTransmitNow();

void midiEnqueueNote(uint8_t note, uint8_t velocity);
void midiEnqueueTick();
void midiEnqueueStart();
void midiEnqueueStop();

#endif