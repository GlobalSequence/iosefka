#include "note_stack.h"

#include <string.h>

static const uint8_t kFreeSlot = 0xFF;

void NoteStack::noteOn(uint8_t note, uint8_t velocity) {

    // remove the note from the list if already there
    noteOff(note);

    // in case of saturation, remove the least recently played note from the stack
    if (size_ == kNoteStackSize) {

        uint8_t least_recent_note;
        for (uint8_t i = 1; i <= kNoteStackSize; i++) {

            if(pool_[i].next_ptr == 0) {

                least_recent_note = pool_[i].note;

            }

        }

        noteOff(least_recent_note);

    }

    // time to insert the new note. find a free slot to insert it. 
    uint8_t free_slot;
    for(uint8_t i = 1; i <= kNoteStackSize; i++) {

        if(pool_[i].note == kFreeSlot) {

            free_slot = i;

        }

    }

    pool_[free_slot].next_ptr = root_ptr_;
    pool_[free_slot].note = note;
    pool_[free_slot].velocity = velocity;
    root_ptr_ = free_slot;

    // last step is to insert the note in the sorted list
    for (uint8_t i = 0; i < size_; i++) {

        if(pool_[sorted_ptr_[i]].note > note) {


            for (uint8_t j = size_; j > i; j--) {

                sorted_ptr_[j] = sorted_ptr_[j - 1];

            }

            sorted_ptr_[i] = free_slot;
            free_slot = 0;
            break;

        }

        if (free_slot) {

            sorted_ptr_[size_] = free_slot;

        }

    }

    size_++;
    
}

void NoteStack::noteOff(uint8_t note) {

    uint8_t current = root_ptr_;
    uint8_t previous = 0;
    while (current) {

        if(pool_[current].note == note) {
            
            break;

        }

        previous  = current;
        current = pool_[current].next_ptr;
    }

    if(current) {

        if(previous) {

            pool_[previous].next_ptr = pool_[current].next_ptr;

        } else {

            root_ptr_ = pool_[current].next_ptr;

        }

        for (uint8_t i = 0; i < size_; i++) {

            if (sorted_ptr_[i] == current) {

                for(uint8_t j = i; j < size_ - 1; j++) {

                    sorted_ptr_[j] = sorted_ptr_[j + 1];

                }

                break;

            }
        }

        pool_[current].next_ptr = 0;
        pool_[current].note = kFreeSlot;
        pool_[current].velocity = 0;
        size_--;

    }
}   

void NoteStack::clear() {

    size_ = 0;
    memset(pool_ + 1, 0, sizeof(note_entry_t) * kNoteStackSize);
    memset(sorted_ptr_ + 1, 0, kNoteStackSize);
    root_ptr_ = 0;

    for (uint8_t i = 0; i <= kNoteStackSize; i++) {

        pool_[i].note = kFreeSlot;

    }

}