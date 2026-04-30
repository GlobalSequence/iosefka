#include "note_queue.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/test.h"
#include "../common/ring_buffer.h"
#include "../controller/note_stack.h"

RingBuffer<note_data_t, NOTE_BUFFER_SIZE>  note_queue_internal;
RingBuffer<note_data_t, NOTE_BUFFER_SIZE>  note_queue_midi_out;

NoteStack noteStack;

uint8_t* legato_ptr;
uint8_t* duo_ptr;
static uint8_t note_count = 0;
static uint8_t last_on_velocity;
static uint8_t bass_note;

// private function

static void addToQueue(note_data_t note_data);
static void addToQueueMidi(uint8_t note, uint8_t velocity, bool on);

// public function

void noteQueueSetup(uint8_t* legato, uint8_t* osc_duo) {

    legato_ptr = legato;
    duo_ptr = osc_duo;
    noteStack.init();

}

void noteQueueSubmit(uint8_t note, uint8_t velocity, bool on) {

    note_data_t note_data;
    bool legato_on = *legato_ptr;

    // MIDI notes get the raw note on/off messages without any legato tricks
    addToQueueMidi(note, velocity, on);

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
                legato_on? !legato : true
            };

            addToQueue(note_data);

        }

        last_on_velocity = after.velocity;
        
        uint8_t least_recent = noteStack.leastRecentNote().note;
        bool bass_note_cancel = least_recent == after.note;
        bass_note = bass_note_cancel? 0 : least_recent;


    } else {

        const note_entry_t before = noteStack.mostRecentNote();

        const note_entry_t least = noteStack.leastRecentNote();
        
        noteStack.noteOff(note);
        
        const note_entry_t after = noteStack.mostRecentNote(); 

        bool add = true;

        if(noteStack.size() == 0) {

            note_data = {note, 0, 0, 0};

        } else if(*duo_ptr && least.note == note){

            note_data = {after.note, last_on_velocity, true, false};
            bass_note = after.note;
            
        } else if (legato_on) {
            
            note_data = {after.note, last_on_velocity, true, false};

        } else {

            add = false;

        }

        if(add) addToQueue(note_data);

    }

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

}

static void addToQueueMidi(uint8_t note, uint8_t velocity, bool on) {
    
    note_data_t note_data_midi;
    uint8_t midi_velocity = on ? velocity : 0;
    note_data_midi = {note, midi_velocity, on, false};
    note_queue_midi_out.enqueue(note_data_midi);

}

void noteQueueKillActiveNotes() {

    noteStack.clear();

    uint8_t count = note_queue_internal.getCount();
    uint8_t head = note_queue_internal.getHead();

    for(int i = 0; i < count; i++) {

        uint8_t index = (head + i) % NOTE_BUFFER_SIZE;

        note_data_t note = note_queue_internal.readElement(index);

        addToQueue(note);

    }

}

uint8_t noteQueueGetBassNote() { return bass_note; } 