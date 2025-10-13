// Copied directly from the Shruthi-1 Note Stack library
// thanks again Emilie <3
// TODO: Proper attribution
// https://github.com/pichenettes/shruthi-1/blob/master/shruthi/note_stack.h

#ifndef NOTE_STACK_H
#define NOTE_STACK_H

#include <stdio.h>
#include "pico/stdlib.h"

static const uint8_t kNoteStackSize = 16;

typedef struct note_entry_t {

    uint8_t note;
    uint8_t velocity;
    uint8_t next_ptr; // base 1

} note_entry_t;

class NoteStack {

    public:
        NoteStack() {}
        void init() { clear(); }

        void noteOn(uint8_t note, uint8_t velocity);
        void noteOff(uint8_t note);
        void clear();

        uint8_t size() { return size_; }

        uint8_t maxSize() { return kNoteStackSize; }

        const note_entry_t& mostRecentNote() { return pool_[root_ptr_]; }

        const note_entry_t& leastRecentNote() {
            
            uint8_t current = root_ptr_;
            while(current && pool_[current].next_ptr) {
                
                current = pool_[current].next_ptr;

            }
            return pool_[current];
        }

        const note_entry_t& note(uint8_t index) { return pool_[index]; }

        note_entry_t* mutableNote(uint8_t index) { return &pool_[index]; }

        const note_entry_t& dummy() { return pool_[0]; }

    private:
        uint8_t size_;
        note_entry_t pool_[kNoteStackSize + 1];
        uint8_t root_ptr_; // base 1
        uint8_t sorted_ptr_[kNoteStackSize + 1];

};

#endif