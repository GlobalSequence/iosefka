#ifndef NOTE_QUEUE_H
#define NOTE_QUEUE_H

#include <stdio.h>
#include "pico/stdlib.h"

#define NOTE_BUFFER_SIZE 256

typedef struct note_data_t {
    uint8_t note;
    uint8_t velocity;
    bool on;
    bool trigger;
} note_data_t;

#ifdef __cplusplus
extern "C"
{
#endif

void noteQueueSetup(uint8_t* legato_ptr);
void noteQueueSubmit(uint8_t note, uint8_t velocity, bool on);
// void noteQueueKillActiveNotes();

bool noteQueueAvailableInternal();
note_data_t noteQueuePullInternal();

bool noteQueueAvailableMidiOut();
note_data_t noteQueuePullMidiOut();
uint8_t noteQueueGetCount();

#ifdef __cplusplus
}
#endif

#endif
