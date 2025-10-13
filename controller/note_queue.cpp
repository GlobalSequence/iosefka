#include "note_queue.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/test.h"
#include "../common/ring_buffer.h"
#include "../controller/note_stack.h"

RingBuffer<note_data_t, NOTE_BUFFER_SIZE>  note_queue_internal;
RingBuffer<note_data_t, NOTE_BUFFER_SIZE>  note_queue_midi_out;

NoteStack noteStack;

bool* legato_ptr;
static uint8_t note_count = 0;
static uint8_t last_on_velocity;

// private function

static void addToQueue(note_data_t note_data);

// public function

void noteQueueSetup(bool* legato) {

    legato_ptr = legato;
    noteStack.init();

}

void noteQueueSubmit(uint8_t note, uint8_t velocity, bool on) {

    // on? note_count++ : note_count--;

    // note_data_t note_data = {note, velocity, on};

    // note_queue_internal.enqueue(note_data);
    // note_queue_midi_out.enqueue(note_data);

    // TEST_PRINT_TWO_NL(note, on)

    note_data_t note_data;

    if(on) {

        const note_entry_t before = noteStack.mostRecentNote();
        
        noteStack.noteOn(note, velocity);
        
        const note_entry_t after = noteStack.mostRecentNote(); 

        if(before.note != after.note) {

            bool legato = noteStack.size() > 1;

            note_data = {
                after.note, 
                after.velocity, 
                true,    
                *legato_ptr? !legato : true
            };

            addToQueue(note_data);

        }

        last_on_velocity = after.velocity;

    } else {

        // the before and after here are for operator duo mode, if ever implemented
        // for now I'm half assing it

        const note_entry_t before = noteStack.mostRecentNote();
        
        noteStack.noteOff(note);
        
        const note_entry_t after = noteStack.mostRecentNote(); 

        bool add = false;

        if(noteStack.size() == 0) {

            note_data = {note, 0, 0, 0};

            
        } else if (*legato_ptr) {

            note_data = {after.note, last_on_velocity, true, false};

        }

        addToQueue(note_data);

    }

    // TEST_PRINT_ONE_NL(*legato_ptr)

}

bool noteQueueAvailableInternal() {

    if(!note_queue_internal.isEmpty()) return true;

    return false;

}

note_data_t noteQueuePullInternal() {

    return note_queue_internal.dequeue();

}

bool noteQueueAvailableMidiOut() {

    if(!note_queue_midi_out.isEmpty()) return true;

    return false;

}

note_data_t noteQueuePullMidiOut() {

    return note_queue_midi_out.dequeue();

}

uint8_t noteQueueGetCount() {

    return note_count;

}

void addToQueue(note_data_t note_data) {

    note_queue_internal.enqueue(note_data);
    note_queue_midi_out.enqueue(note_data);

}

void noteQueueKillActiveNotes() {

    noteStack.clear();

    uint8_t count = note_queue_internal.getCount();
    uint8_t head = note_queue_internal.getHead();

    for(int i = 0; i < count; i++) {

        uint8_t index = (head + i) % NOTE_BUFFER_SIZE;

        note_data_t note = note_queue_internal.readElement(index);

        // noteQueueSubmit(note.note, 0, false);
        addToQueue(note);

    }

}