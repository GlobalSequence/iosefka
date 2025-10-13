#include "p_lock.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/globals.h"
#include "../common/test.h"

static bool enabled;
static bool engaged;
static int8_t note_order[NUMBER_OF_BOTTOM_SWITCHES] = {-1, -1, -1, -1, -1, -1, -1, -1};
static uint8_t note[NUMBER_OF_BOTTOM_SWITCHES];
static int8_t note_distance;

// ################################
// SET FUNCTIONS
// ################################

void pLockEnable(bool enable) { enabled = enable; }

void pLockEngage(bool engage) { engaged = engage; }

void pLockSetOrder(uint8_t index) {

    for(int i = 0; i < NUMBER_OF_BOTTOM_SWITCHES; i++) {

        if(note_order[i] == -1) {

            note_order[i] = index; 
            
            i = NUMBER_OF_BOTTOM_SWITCHES;
        
        }

    }

}

void pLockResetOrder(uint8_t index) {

    bool move_elements_forward = false;

    for(int i = 0; i < NUMBER_OF_BOTTOM_SWITCHES; i++) {

        if(note_order[i] == index) {

            move_elements_forward = true;
            
            note_order[i] = -1;

        }

        if(move_elements_forward) {

            if(i != NUMBER_OF_BOTTOM_SWITCHES -1) {

                note_order[i] = note_order[i + 1];

            } else {

                note_order[i] = -1;

            }

        }

    }

}

void pLockResetOrderAll() {

    for(int i = 0; i < NUMBER_OF_BOTTOM_SWITCHES; i++) {
     
        note_order[i] = -1;

    }

}

void pLockSetNote(uint8_t index, uint8_t value) { note[index] = value; }

void pLockSetNoteDistance(int8_t value) { note_distance = value; }

// ################################
// GET FUNCTIONS
// ################################

bool pLockIsEnabled() { return enabled; }

bool pLockIsEngaged() { return engaged; }

uint8_t pLockGetNote(uint8_t index) { return note[index]; }

uint8_t pLockGetNoteDistanceUnsigned(uint8_t index) { return note_distance + P_LOCK_NOTE_ADJUST_OFFSET; }

int8_t pLockGetOrder(uint8_t index) { return note_order[index]; }

int8_t pLockGetOrderFirst() { return note_order[0]; }