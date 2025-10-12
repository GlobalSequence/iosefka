#include "synth.h"
#include <stdio.h>
#include <new>
#include "pico/stdlib.h"
#include "voice.h"

#include "mod_matrix.h"
#include "voice2.h"
#include "delay.h"
#include "cv.h"

#include "../devices/audio.h"
#include "../devices/pwm.h"
#include "../devices/gpio.h"
#include "../controller/note_queue.h"
#include "../common/globals.h"
#include "../common/test.h"

// ################################
// STATIC DECLARATIONS
// ################################

volatile bool *render_trig;

// Voice voice;
alignas(Voice) static uint8_t voice_buffer[sizeof(Voice)];
Voice *voice = nullptr;

////

alignas(ModMatrix) static uint8_t modmatrix_object_buffer[sizeof(ModMatrix)];
ModMatrix *modmatrix = nullptr;

alignas(Voice2) static uint8_t voice2_object_buffer[sizeof(Voice2)];
Voice2 *voice2 = nullptr;

alignas(Delay<DELAY_BUF_SIZE>) static uint8_t delay_object_buffer[sizeof(Delay<DELAY_BUF_SIZE>)];
Delay<DELAY_BUF_SIZE> *delay = nullptr;

alignas(Cv) static uint8_t cv_object_buffer[sizeof(Cv)];
Cv *cv = nullptr;

// ################################
// PRIVATE FUNCTION DECLARATIONS
// ################################

void retrieveSynthNotes();

// ################################
// PUBLIC FUNCTION DEFINITIONS
// ################################

void synthSetup(synth_parameters_t *patch, external_modulation_t *ext_mod, uint16_t random_seed) {

    uint16_t *buf_ptr_1 = audioGetBlockPointer1(); 
    uint16_t *buf_ptr_2 = audioGetBlockPointer2(); 
    uint16_t *adc_buf_ptr = audioGetAdcBufferPointer();
    volatile bool *render_trigger = audioGetRenderTrigger();

    uint16_t *cutoff_ptr = pwmGetCutoffPointer();
    uint16_t *resonance_ptr = pwmGetResonancePointer();
    uint16_t *vca_1_ptr = pwmGetVca1Pointer();
    uint16_t *vca_2_ptr = pwmGetVca2Pointer();
    uint8_t *filter_type_ptr = gpioGetStateValPointer();
    
    
    // voice = new (voice_buffer) Voice(patch, buf_ptr_1, random_seed);

    voice2 = new (voice2_object_buffer) Voice2(buf_ptr_1, random_seed);

    delay = new (delay_object_buffer) Delay<DELAY_BUF_SIZE>(buf_ptr_2, adc_buf_ptr, AUDIO_BLOCK_SIZE);

    cv = new (cv_object_buffer) Cv(cutoff_ptr, resonance_ptr, vca_1_ptr, vca_2_ptr, filter_type_ptr);

    modmatrix = new (modmatrix_object_buffer) ModMatrix(
        patch,
        ext_mod,
        voice2->getParamPointer(),
        delay->getParamPointer(),
        cv->getParamPointer(),
        random_seed

    );

    render_trig = render_trigger;

}

uint32_t last_time;

bool synthUpdate() {

    if(*render_trig) {

        *render_trig = false;

        retrieveSynthNotes();

        modmatrix->process();

        voice2->renderOscillators();

        delay->setInputBufferPointer(audioGetAdcBufferPointer());

        delay->render();

        audioFillOutputBuffer();

        cv->update();

        pwmUpdateLevels();

        gpioSetTwo();

        return true;

    }

    return false;

}


// ################################
// PRIVATE FUNCTION DEFINITIONS
// ################################

void retrieveSynthNotes() {

    while(noteQueueAvailableInternal()) {

        note_data_t note = noteQueuePullInternal();
        // TEST_PRINT_ONE_TAB(note.note)
        // TEST_PRINT_TWO_NL(note.on, note.velocity)

        if(note.on) {

            modmatrix->triggerNoteOn(note.note, note.velocity, note.trigger);

        } else {

            modmatrix->triggerNoteOff(note.note);

        }

    }

}

