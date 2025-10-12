#include "display.h"
#include "../common/globals.h"
#include "../common/utilities.h"
#include "../devices/ssd1306.h"
#include "../graphics/graphics.h"
#include "../graphics/shapeRenderer/ShapeRenderer.h"
#include "../graphics/textRenderer/TextRenderer.h"
#include "../controller/storage.h"
#include "../controller/parameter.h"
#include "../controller/encoder_map.h"
#include "../controller/view.h"
#include "../controller/settings.h"
#include "../sequencer/p_lock.h"
#include "../devices/devices.h"
#include "../synth/patch.h"
#include "../common/test.h"

using namespace pico_ssd1306;

// ################################
// PARAMETER DEFINES
// ################################

static const uint8_t k_char_width = 8;
static const uint8_t k_char_height = 8;
typedef const char* (*paramFuncGetGraphic)(uint8_t);
typedef const char* (*settingsStringFunc)(settings_string_t);
const uint8_t k_num_disp_parameters = NUMBER_OF_POTENTIOMETERS;
const uint8_t param_coords_x[NUMBER_OF_PARAM_COLS] = {14, 35, 56, 77, 98};
const uint8_t param_coords_x_seq[NUMBER_OF_PARAM_COLS] = {27, 43, 56, 77, 98};
const uint8_t param_coords_y[NUMBER_OF_PARAM_ROWS] = {12, 22, 32};

typedef struct display_param_t {
    paramFuncGetGraphic func;
    uint8_t *val_p;
} display_param_t;

const uint8_t seq_p_lock_length = 80;
const uint8_t seq_p_lock_height = 9;

const uint8_t k_param_p_lock_length = 18;
const uint8_t k_param_p_lock_height = 9;

const uint8_t k_settings_text_x[] = {2, 65};
const uint8_t k_settings_text_y[] = {2, 12, 22, 32};
const uint8_t k_settings_param_x = 109;
const uint8_t k_settings_check_box_x[] = {55, 117};
const uint8_t k_check_size_x = 8;
const uint8_t k_check_size_y = 8;
const uint8_t k_settings_midi_mod_x[] = {2, 42, 84};
const uint8_t k_settings_midi_mod_y = 47;
const uint8_t k_thru_length = 32;

const uint8_t k_play_x = 0;
const uint8_t k_play_y = 0;
const uint8_t k_play_width = 8;
const uint8_t k_play_height = 8;

const uint8_t k_gen_text_x = 95;
const uint8_t k_gen_state_x = 119;
const uint8_t k_gen_y = 0;

const uint8_t k_storage_x = 7;
const uint8_t k_storage_text_x[] = {71, 31};
const uint8_t k_storage_y[] = {7, 15, 23, 31, 39, 47, 55};
// ################################
// STEP SEQ DEFINES
// ################################

const uint8_t k_step_spacing = 8;
const uint8_t k_step_width = 8;
const uint8_t k_step_height = 8;
const uint8_t k_step_row_y[] = {47, 55};

const uint8_t k_step_window_len = 64;
const uint8_t k_step_window_x[] = {0, 64};
const uint8_t k_step_window_y[] = {47, 63};

// ################################
// ENCODER MODE DEFINES
// ################################

typedef enum display_encoder_mode_event_t {

    DISP_MODE_EVENT_COUNTDOWN,
    DISP_MODE_EVENT_SHOW,
    DISP_MODE_EVENT_HIDE,

} display_encoder_mode_event_t;

const uint8_t k_enc_source_x = 40;
const uint8_t k_enc_source_y = 43;
const uint8_t k_enc_val_x = 0;
const uint8_t k_enc_val_y = 55;
const uint8_t k_enc_dec_x = 63;
const uint8_t k_enc_dec_indicator_x = 87;
const uint8_t k_enc_mod_layer_x = 15;

static const uint8_t k_num_mod_per_layer = 3;
static const view_mode_t enc_mode_env[k_num_mod_per_layer] = {
    MODE_MOD_DST_ENV_1,
    MODE_MOD_DST_ENV_2,
    MODE_MOD_DST_ENV_3
};

static const view_mode_t enc_mode_lfo[k_num_mod_per_layer] = {
    MODE_MOD_DST_LFO_1,
    MODE_MOD_DST_LFO_2,
    MODE_MOD_DST_LFO_3
};

typedef struct disp_encoder_mode_t {

    graphic_encoder_t graphic;
    int value;

} disp_encoder_mode_t;

// ################################
// DISPLAY DEFINES
// ################################

static bool display_trigger = false;

static unsigned long display_timer_started;
static const unsigned long k_display_timer_timeout = 1000000;
static bool display_timer_active = false;
static bool encoder_mode_show = false;
static bool seq_special_coords = false;

static bool show_storage_complete = false;

// ################################
// LED DECLARATIONS
// ################################

uint8_t *led_bits;
bool *led_transmit;

// ################################
// POINTER DEFINES
// ################################

// static SSD1306 *disp;
static synth_state_t *synth;
static synth_parameters_t *patch;

paramFuncGetGraphic param_func_s1[k_num_disp_parameters] = {

    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 

    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 

    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 

};

paramFuncGetGraphic param_func_s2[k_num_disp_parameters] = {

    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetFilterType, 
    graphicsGetHexString, 

    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 

    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 

};

paramFuncGetGraphic param_func_s_env[k_num_disp_parameters] = {

    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 

    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 

    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetVcaMode,

};

paramFuncGetGraphic param_func_env[k_num_disp_parameters] = {

    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 

    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 

    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetHexString, 

};

paramFuncGetGraphic param_func_lfo[k_num_disp_parameters] = {

    graphicsGetHexString, 
    graphicsGetLfoShape, 
    graphicsGetHexString, 
    graphicsGetResetSync, 
    graphicsGetHexString, 

    graphicsGetHexString, 
    graphicsGetLfoShape,
    graphicsGetHexString, 
    graphicsGetResetSync, 
    graphicsGetHexString, 

    graphicsGetHexString, 
    graphicsGetLfoShape, 
    graphicsGetHexString, 
    graphicsGetResetSync, 
    graphicsGetHexString, 

};

paramFuncGetGraphic param_func_seq[k_num_disp_parameters] = {

    graphicsGetMidiNote,
    graphicsPrintNothing, 
    graphicsGetHexString, 
    graphicsGetHexString, 
    graphicsGetSeqTranspose, 

    graphicsGetSeqDirection, 
    graphicsGetHexString, 
    graphicsGetSeqClkDiv, 
    graphicsGetHexString, 
    graphicsGetHexString, 

    graphicsGetHexString, 
    graphicsGetSeqScale, 
    graphicsGetHexString, 
    graphicsGetSeqRepeat, 
    graphicsGetHexString, 

};

display_param_t display_param_s1[k_num_disp_parameters];
display_param_t display_param_s2[k_num_disp_parameters];
display_param_t display_param_s_env[k_num_disp_parameters];
display_param_t display_param_env[k_num_disp_parameters];
display_param_t display_param_lfo[k_num_disp_parameters];
display_param_t display_param_seq[k_num_disp_parameters];

display_param_t *display_param[NUM_VIEW_LAYERS];

// ################################
// PRIVATE DECLARATIONS
// ################################

// the function that determines what to display
// void determineDisplay();
void displayLayerHeader(view_layer_t layer);
void displayParams(view_layer_t layer, view_page_t page, bool p_lock);
void displaySteps();
void displayEncoderMode(view_layer_t layer);
void displayEncoderModeNormal(view_mode_t mode);
void displayEncoderModeModLayer(view_mode_t mode, view_layer_t layer);
void showEncoderModeByEvent(display_encoder_mode_event_t event);
void disableAdditionalTimerFlags();
void displayTimerSet();
void displayTimerCheck();
// void draw8x8Rectangle(uint8_t x, uint8_t y);
void drawChars(const char* text, uint8_t x, uint8_t y, WriteMode write_mode);
void drawRectangle(uint8_t x, uint8_t y, uint8_t x_len, uint8_t y_len, bool fill, WriteMode mode);
uint8_t getSeqParamValue(uint8_t index, uint8_t step, bool p_lock);
disp_encoder_mode_t getEncoderParamValue(view_mode_t mode);
bool isModLayer(view_layer_t layer);

// ################################
// PRIVATE FUNCTION DEFINITIONS
// ################################

void displayTimerSet() {

    display_timer_started = time_us_64();
    display_timer_active = true;

}

void disableAdditionalTimerFlags() {

    show_storage_complete = false;

}

void displayTimerCheck() {

    if(display_timer_active) {

        if(time_us_64() - display_timer_started >= k_display_timer_timeout) {

            display_timer_active = false;
            display_trigger = true;
            disableAdditionalTimerFlags();

        }

    }

}

inline void drawChars(const char* text, uint8_t x, uint8_t y, WriteMode write_mode) {

    devicesDisplayDrawTextNormFont(text, x, y, write_mode);

}

inline void drawRectangle(uint8_t x, uint8_t y, uint8_t x_len, uint8_t y_len, bool fill, WriteMode mode) {

    devicesDisplayDrawRectangle(x, y, x_len, y_len, fill, mode);

}

inline void drawLine(uint8_t x, uint8_t y, uint8_t l, uint8_t h, WriteMode mode) {

    for(int len = 0; len < l; len++) {

        for(int hgt = 0; hgt < h; hgt++) {

            devicesDisplaySetPixel(x + len, y + hgt, mode);

        }

    }

}

inline uint8_t getSeqParamValue(uint8_t index, uint8_t step, bool p_lock) {

    uint8_t val;

    switch(index) {


        case 0:
            
            if(!p_lock) {
                val = synth->new_step.note;
            } else {
                val = synth->seq.getStepNote(step);
            }
        
            break;

        case 1:

            // if(!p_lock) {
            //     val = viewGetSeqParamStepOctave();
            // } else {
            //     // viewGetFirstPLockOctave();
            // }
            
            break;

        case 2:

            if(!p_lock) {
                val = synth->new_step.length;
            } else {
                val = synth->seq.getStepLength(step);
            }

            break;

        case 3:

            if(!p_lock) {
                val = synth->new_step.velocity;
            } else {
                val = synth->seq.getStepVelocity(step);
            }
            
            break;

        case 4:
            val = synth->seq.getTranspose();
            break;

        case 5:
            val = synth->seq.getDirection();
            break;

        case 6:
            val = synth->seq.getSwingAmt();
            break;

            case 7:
            val = synth->seq.getClockDiv();
            break;

            case 8:
            val = synth->seq.getFirstStep();
            break;

            case 9:
            val = synth->seq.getLastStep();
            break;

            case 10:
            val = synth->seq.getProbability();
            break;

            case 11:
            val = synth->pattern_gen.scale;
            break;

            case 12:
            val = synth->pattern_gen.density;
            break;

            case 13:
            val = synth->pattern_gen.repeat;
            break;

            case 14:
            val = synth->pattern_gen.octave_range;
            break;

            default:
            break;

    }

    return val;

}

disp_encoder_mode_t getEncoderParamValue(view_mode_t mode) {

    graphic_encoder_t graphic;
    int val;

    switch (mode) {

        case MODE_KBD_OCT:

            graphic = GRAPHIC_ENCODER_DECIMAL;
            val = viewGetKeyboardOctave();

            break;
    
        case MODE_MOD_DST_ENV_1:

            graphic = GRAPHIC_ENCODER_MOD_DST;
            val = patch->mod[MOD_SRC_ENV_1].destination;
            break;
    
        case MODE_MOD_DST_ENV_2:

            graphic = GRAPHIC_ENCODER_MOD_DST;
            val = patch->mod[MOD_SRC_ENV_2].destination;

            break;
    
        case MODE_MOD_DST_ENV_3:

            graphic = GRAPHIC_ENCODER_MOD_DST;
            val = patch->mod[MOD_SRC_ENV_3].destination;

            break;
    
        case MODE_MOD_DST_LFO_1:

            graphic = GRAPHIC_ENCODER_MOD_DST;
            val = patch->mod[MOD_SRC_LFO_1].destination;

            break;
    
        case MODE_MOD_DST_LFO_2:

            graphic = GRAPHIC_ENCODER_MOD_DST;
            val = patch->mod[MOD_SRC_LFO_2].destination;

            break;
    
        case MODE_MOD_DST_LFO_3:

            graphic = GRAPHIC_ENCODER_MOD_DST;
            val = patch->mod[MOD_SRC_LFO_3].destination;

            break;
    
        case MODE_SEQ_SET_BPM:

            graphic = GRAPHIC_ENCODER_DECIMAL;
            val = synth->seq.getBpm();

            break;
    
        case MODE_SEQ_STEP_NOTE:
            break;
    
        case MODE_SEQ_STEP_LENGTH:
            break;
    
        case MODE_SEQ_P_LOCK_DEST:
            break;
    
        case MODE_SAVE_PRESET_SEL:

            graphic = GRAPHIC_ENCODER_DECIMAL;
            val = viewGetPresetNumber();

            break;
    
        case MODE_MIDI_CHANNEL:

            graphic = GRAPHIC_ENCODER_DECIMAL;
            val = synth->midi.channel;

            break;
    
        case MODE_MIDI_VELOCITY_DST:

            graphic = GRAPHIC_ENCODER_MOD_DST;
            val = patch->mod[MOD_SRC_VELOCITY].destination;

            break;
    
        case MODE_MIDI_MOD_WHEEL_DST:

            graphic = GRAPHIC_ENCODER_MOD_DST;
            val = patch->mod[MOD_SRC_MOD_WHEEL].destination;

            break;
    
        case MODE_MIDI_AFTERTOUCH_DST:

            graphic = GRAPHIC_ENCODER_MOD_DST;
            val = patch->mod[MOD_SRC_AFTERTOUCH].destination;

            break;
    
        case MODE_MOD_LAYER_SEL:
            break;
    
        default:
            break;
    }

    disp_encoder_mode_t disp_encoder = {graphic, val};
    
    return disp_encoder;

}

bool isModLayer(view_layer_t layer) {

    if(layer == LAYER_ENV || layer == LAYER_LFO) {

        return true;

    }
    return false;

}

uint8_t getPLockStep() {

    uint8_t sw = pLockGetOrderFirst();
    uint8_t step = sw + viewGetSequencerWindowStepBase();
    return step;

}

bool isThisAPLockedSynthVal(uint8_t step, uint8_t *p) {

    mod_destinations_t dst = (mod_destinations_t)synth->seq.getStepPLockParam(step);

    uint8_t *p_lock_param_ptr = parameterGetPointer(dst);
    
    if(p_lock_param_ptr == p) return true;
    
    return false;
}

void displayUpdateLeds(view_layer_t layer, view_page_t page) {

    if(viewGetViewChangeFlagLed()) {

        uint8_t page_led;
        uint8_t layer_led;
        uint8_t layer_led_2;  
        
        switch(page) {

            case PAGE_KEYS:
                page_led = LED_PAGE_KBD;
                break;

            case PAGE_SEQ:
                page_led = LED_PAGE_SEQ;
                break;

            case PAGE_SETTINGS:
                page_led = LED_PAGE_SET;
                break;

        }

        switch(layer) {
        
            case LAYER_SYNTH_1:
                layer_led    = LED_LAYER_SYNTH_1;
                layer_led_2  = LED_LAYER_SYNTH_1;
                break;
    
            case LAYER_SYNTH_2:
                layer_led    = LED_LAYER_SYNTH_2;
                layer_led_2  = LED_LAYER_SYNTH_2;
                break;
    
            case LAYER_SYNTH_ENV_SPLIT:
                layer_led    = LED_LAYER_SYNTH_1;
                layer_led_2  = LED_LAYER_ENV;
                break;
    
            case LAYER_ENV:
                layer_led    = LED_LAYER_ENV;
                layer_led_2  = LED_LAYER_ENV;
                break;
    
            case LAYER_LFO:
                layer_led    = LED_LAYER_LFO;
                layer_led_2  = LED_LAYER_LFO;
                break;  
                
            case LAYER_SEQ:
                layer_led    = LED_LAYER_SEQ;
                layer_led_2  = LED_LAYER_SEQ;
                break;  
        }

        // *led_bits = (0x80 >> page_led) | (0x80 >> layer_led) | (0x80 >> layer_led_2);
        *led_bits = (1 << page_led) | (1 << layer_led) | (1 << layer_led_2);
    
        // allow the GPIO expander to transmit
        *led_transmit = true;

    }

}

void checkBox(uint8_t x, uint8_t y, bool val) {

    drawRectangle(x, y, k_check_size_x, k_check_size_y, val, WriteMode::ADD);

}

void displaySeqGenHeader() {

    bool on = synth->pattern_gen.on;
    uint8_t countdown = synth->pattern_gen.countdown;

    if(on) drawChars(graphicsGetHexString(countdown), param_coords_x[0], PARAM_HEADER_Y, WriteMode::INVERT);

    drawChars("gen", k_gen_text_x, k_gen_y, WriteMode::INVERT);
    checkBox(k_gen_state_x, k_gen_y, on);
}

void displayLayerHeader(view_layer_t layer) {

    for(int i = 0; i < NUMBER_OF_PARAM_COLS; i++) {

        uint8_t x = param_coords_x[i];

        const char *p_h = graphicsGetParamHeader(layer, i);

        drawChars(p_h, x, PARAM_HEADER_Y, WriteMode::INVERT);

        if(layer == LAYER_SYNTH_ENV_SPLIT) {

            drawChars("e", 0, param_coords_y[2], WriteMode::INVERT);

        }

    }

}

void displayPageSettings() {

    storage_state_t state = storageGetState();

    if(state || show_storage_complete) {

        storage_type_t type = storageGetType();
        storage_string_t type_string = type == STORAGE_TYPE_SAVE ? STRING_SAVE : STRING_LOAD;
        const char *storage_char;

        switch(state) {

            case STORAGE_STATE_READY:

                storage_char = graphicsGetStorageTypeString(type_string);
        
                drawChars("press", k_storage_x, k_storage_y[0], WriteMode::INVERT);
                drawChars("shift + ", k_storage_x, k_storage_y[1], WriteMode::INVERT);
                drawChars(storage_char, k_storage_text_x[0], k_storage_y[1], WriteMode::INVERT);
                drawChars("to ", k_storage_x, k_storage_y[2], WriteMode::INVERT);
                drawChars(storage_char, k_storage_text_x[1], k_storage_y[2], WriteMode::INVERT);
        
                drawChars("press any other", k_storage_x, k_storage_y[4], WriteMode::INVERT);
                drawChars("switch to exit", k_storage_x, k_storage_y[5], WriteMode::INVERT);

                break;

            case STORAGE_STATE_ACTIVE:

                storage_char = graphicsGetStorageActiveString(type_string);
                drawChars(storage_char, k_storage_x, k_storage_y[0], WriteMode::INVERT);

                TEST_PRINT_TEXT("STORAGE HAPPENING\n")

                break;

            case STORAGE_STATE_FINISHED:

                show_storage_complete = true;
                displayTimerSet();


                TEST_PRINT_TEXT("STORAGE COMPLETE\n")

                break;

            case STORAGE_STATE_OFF: 

                type = storageGetLastType();
                type_string = type == STORAGE_TYPE_SAVE ? STRING_SAVE : STRING_LOAD;

                storage_char = graphicsGetStorageTypeString(type_string);
                
                drawChars(storage_char, k_storage_x, k_storage_y[0], WriteMode::INVERT);
                drawChars("complete!", k_storage_x, k_storage_y[1], WriteMode::INVERT);

                TEST_PRINT_TEXT("STORAGE SCREEN HANGING\n")

                break;

        }


    } else {

        uint8_t preset = viewGetPresetNumber();
        uint8_t channel = synth->midi.channel;
        uint8_t velocity = patch->mod[MOD_SRC_VELOCITY].destination;
        uint8_t mod_wheel = patch->mod[MOD_SRC_MOD_WHEEL].destination;
        uint8_t aftertouch = patch->mod[MOD_SRC_AFTERTOUCH].destination;
        bool clock = synth->midi.clock_rx;
        bool transport = synth->midi.transport_rx;
        uint8_t pb_depth = synth->midi.pitch_bend_depth;
        // bool program_change = synth->midi.program_change_rx;
        // bool out_thru = synth->midi.out_thru;
    
        // settings_string_t d_out_through = out_thru ? STRING_THRU : STRING_OUT;
    
        paramFuncGetGraphic g = graphicsGetHexString;
        settingsStringFunc s = graphicsGetSettingsString;
        WriteMode m_a = WriteMode::ADD;
        WriteMode m_i = WriteMode::INVERT;
    
        uint8_t r_x_s, r_y_s, char_len;
        bool display_midi_mod_dst = false;
        const char *enc_val = NULL;
    
        switch(viewGetMode()) {
    
            case MODE_SAVE_PRESET_SEL:
    
            r_x_s = k_settings_text_x[0];
            r_y_s = k_settings_text_y[0];
            char_len = 6;
    
            break;
    
            case MODE_MIDI_CHANNEL:
    
            r_x_s = k_settings_text_x[0];
            r_y_s = k_settings_text_y[1];
            char_len = 7;
    
            break;
    
            case MODE_MIDI_VELOCITY_DST:
    
            r_x_s = k_settings_midi_mod_x[0];
            r_y_s = k_settings_midi_mod_y;
            char_len = 3;
            enc_val = graphicsGetModDst((mod_destinations_t)velocity);
            display_midi_mod_dst = true;
    
            break;
    
            case MODE_MIDI_MOD_WHEEL_DST:
    
            r_x_s = k_settings_midi_mod_x[1];
            r_y_s = k_settings_midi_mod_y;
            char_len = 4;
            enc_val = graphicsGetModDst((mod_destinations_t)mod_wheel);
            display_midi_mod_dst = true;
    
            break;
    
            case MODE_MIDI_AFTERTOUCH_DST:
    
            r_x_s = k_settings_midi_mod_x[2];
            r_y_s = k_settings_midi_mod_y;
            char_len = 4;
            enc_val = graphicsGetModDst((mod_destinations_t)aftertouch);
            display_midi_mod_dst = true;
    
            break;

            case MODE_MIDI_PITCHBEND_DEPTH:

            r_x_s = k_settings_text_x[0];
            r_y_s = k_settings_text_y[3];
            char_len = 5;
    
            default: break;
    
        }
    
        drawRectangle(r_x_s, r_y_s, char_len * k_char_width, k_char_height, true, m_a);
        drawChars(s(STRING_PRESET), k_settings_text_x[0], k_settings_text_y[0], WriteMode::INVERT);
        drawChars(graphicsGetDecString(preset), k_settings_param_x, k_settings_text_y[0], WriteMode::INVERT);
        drawChars(s(STRING_CHANNEL), k_settings_text_x[0], k_settings_text_y[1], WriteMode::INVERT);
        drawChars(graphicsGetDecString(channel + 1), k_settings_param_x, k_settings_text_y[1], WriteMode::INVERT);
        drawChars(s(STRING_CLOCK), k_settings_text_x[0], k_settings_text_y[2], WriteMode::INVERT);
        drawChars(s(STRING_TRANSPORT), k_settings_text_x[1], k_settings_text_y[2], WriteMode::INVERT);
        // drawChars(s(STRING_PROGRAM_CHANGE), k_settings_text_x[0], k_settings_text_y[3], WriteMode::INVERT);
        drawChars(s(STRING_PITCH_BEND_DEPTH), k_settings_text_x[0], k_settings_text_y[3], WriteMode::INVERT);
    
        // if(out_thru) {
        //     drawRectangle(k_settings_text_x[1], k_settings_text_y[3], k_thru_length, k_char_height, true, m_a);
        // }
        // drawChars(s(d_out_through), k_settings_text_x[1], k_settings_text_y[3], WriteMode::INVERT);
    
        checkBox(k_settings_check_box_x[0], k_settings_text_y[2], clock);
        checkBox(k_settings_check_box_x[1], k_settings_text_y[2], transport);
        // checkBox(k_settings_check_box_x[0], k_settings_text_y[3], program_change);

        drawChars(graphicsGetDecString(pb_depth + 1), k_settings_text_x[1] - (k_char_width * 2), k_settings_text_y[3], WriteMode::ADD);
    
        drawChars(s(STRING_VELOCITY), k_settings_midi_mod_x[0], k_settings_midi_mod_y, WriteMode::INVERT);
        drawChars(s(STRING_MOD_WHEEL), k_settings_midi_mod_x[1], k_settings_midi_mod_y, WriteMode::INVERT);
        drawChars(s(STRING_AFTERTOUCH), k_settings_midi_mod_x[2], k_settings_midi_mod_y, WriteMode::INVERT);
    
        // if(display_midi_mod_dst) drawText(disp, font_8x8, enc_val, 0, k_enc_val_y);
        drawChars(enc_val, 0, k_enc_val_y, WriteMode::INVERT);

    }

}

void displayParams(view_layer_t layer, view_page_t page, bool p_lock) {

    uint8_t step = getPLockStep();

    uint8_t k = 0;

    for(int i = 0; i < NUMBER_OF_PARAM_ROWS; i++) {

        for(int j = 0; j < NUMBER_OF_PARAM_COLS; j++) {

            uint8_t x, y;

            x = param_coords_x[j];
            y = param_coords_y[i];

            display_param_t p = display_param[layer][k];
            uint8_t val;

            if(layer != LAYER_SEQ) {

                val = val = *p.val_p;

                if(p_lock && isThisAPLockedSynthVal(step, p.val_p)) {

                    val = synth->seq.getStepPLockValue(step);
                    drawRectangle(x - 1, y - 1, k_param_p_lock_length, k_param_p_lock_height, true, WriteMode::ADD);
            
                }

                if(p.val_p == &patch->shared.sync) val = param8Bits0To1(val);
                // else if (p.val_p == &patch->vca.level) val = param8Bits0To1(val);
                else if (p.val_p == &patch->osc[0].octave) val = param8Bits0To3(val);
                else if (p.val_p == &patch->osc[1].octave) val = param8Bits0To3(val);
                else if (p.val_p == &patch->lfo[0].shape) val = param8Bits0To4(val);
                else if (p.val_p == &patch->lfo[1].shape) val = param8Bits0To4(val);
                else if (p.val_p == &patch->lfo[2].shape) val = param8Bits0To4(val);
                else if (p.val_p == &patch->lfo[0].sync) val = param8Bits0To3(val);
                else if (p.val_p == &patch->lfo[1].sync) val = param8Bits0To3(val);
                else if (p.val_p == &patch->lfo[2].sync) val = param8Bits0To3(val);
                else if (p.val_p == &patch->filter.type) val = param8Bits0To3(val);

            } else {

                if(p_lock && k == 0) {
                    
                    drawRectangle(x - 1, y - 1, seq_p_lock_length, seq_p_lock_height, true, WriteMode::ADD);

                }

                val = getSeqParamValue(k, step, p_lock);

            }

            const char *param = p.func(val);  
            drawChars(param, x , y, WriteMode::INVERT);
            k++;

        }

    }

}

void displaySteps() {

    uint8_t j = 0;
    bool row = 0;
    uint8_t first = synth->seq.getFirstStep();
    uint8_t last = synth->seq.getLastStep(); 

    for(int i = 0; i < NUMBER_OF_SEQUENCER_STEPS; i++) {

        if(i == NUMBER_OF_SEQUENCER_STEPS / 2) {

            row = 1;
            j = 0;

        }

        graphic_step_icon_t icon_sel;
        bool active = synth->seq.isStepActive(i);
        bool written = synth->seq.isStepWritten(i);

        if(!written) {

            icon_sel = STEP_ICON_UNWRITTEN;

        } else if (!active) {

            icon_sel = STEP_ICON_INACTIVE;

        } else if (active) {

            icon_sel = STEP_ICON_ACTIVE;

        }

        unsigned char *icon_img = graphicsGetStepIcon(icon_sel);
        WriteMode write_mode = WriteMode::ADD;

        uint8_t x_start = j * k_step_spacing;
        uint8_t x_end = x_start + k_step_width;
        uint8_t y_start = k_step_row_y[row];
        uint8_t y_end = y_start + k_step_height;

        if(i == synth->seq.getCurrentStep()) {

            write_mode = WriteMode::INVERT;

            drawRectangle(x_start, y_start, k_step_width, k_step_height, true, WriteMode::ADD);
        }

        if(i == first) {

            drawLine(x_start, y_start, 1, k_step_height, WriteMode::INVERT);

        }

        if(i == last) {

            drawLine(x_end, y_start, 1, k_step_height, WriteMode::INVERT);

        }

        devicesDisplayDrawBitmap(
            x_start, 
            y_start, 
            k_step_width, 
            k_step_height, 
            icon_img,
            write_mode  
        );

        j++;

    }

    uint8_t window = viewGetSequencerWindow();
    uint8_t w_x, w_y;

    switch(window) {

        case 0:
            w_x = k_step_window_x[0];
            w_y = k_step_window_y[0];
            break;

        case 1:
            w_x = k_step_window_x[1];
            w_y = k_step_window_y[0];
            break;

        case 2:
            w_x = k_step_window_x[0];
            w_y = k_step_window_y[1];
            break;

        case 3:
            w_x = k_step_window_x[1];
            w_y = k_step_window_y[1];
            break;

    }
    
    drawLine(w_x, w_y, k_step_window_len, 1, WriteMode::INVERT);
    
    // for(int i = 0; i < k_step_window_len; i++) {

    //     devicesDisplaySetPixel(w_x + i, w_y, WriteMode::INVERT);

    // }
}

void displayEncoderMode(view_layer_t layer) {

    view_mode_t mode = viewGetMode();

    if(isModLayer(layer)) {

        displayEncoderModeModLayer(mode, layer);
        
    } else {

        displayEncoderModeNormal(mode);

    }

}

void displayEncoderModeNormal(view_mode_t mode) {

    disp_encoder_mode_t encoder = getEncoderParamValue(mode);
    const char *enc_source = graphicsGetEncoderMode(mode);
    drawChars("enc:", 0, k_enc_source_y, WriteMode::INVERT);
    drawChars(enc_source, k_enc_source_x, k_enc_source_y, WriteMode::INVERT);

    const char *enc_val;
    if(encoder.graphic == GRAPHIC_ENCODER_MOD_DST) {

        enc_val = graphicsGetModDst((mod_destinations_t)encoder.value);
        drawChars(enc_val, 0, k_enc_val_y, WriteMode::INVERT);

    } else if(encoder.graphic == GRAPHIC_ENCODER_DECIMAL) {

        if(synth->special.ext_clock_active && mode == MODE_SEQ_SET_BPM) {

            drawChars("EXT CLK", k_enc_dec_x, k_enc_val_y, WriteMode::INVERT);
        } else {

            enc_val = graphicsGetDecString(encoder.value);
            drawChars("(dec)", k_enc_dec_indicator_x, k_enc_val_y, WriteMode::INVERT);
            drawChars(enc_val, k_enc_dec_x, k_enc_val_y, WriteMode::INVERT);

        }


    }

}

void displayEncoderModeModLayer(view_mode_t mode, view_layer_t layer) {

    const view_mode_t *mod_mode;

    if(layer == LAYER_ENV)          mod_mode = enc_mode_env;
    else if(layer == LAYER_LFO)     mod_mode = enc_mode_lfo;

    for(int i = 0; i < k_num_mod_per_layer; i++) {

        WriteMode write_mode = WriteMode::ADD;

        uint8_t x = 0;
        uint8_t y = param_coords_y[i];

        if(mode == mod_mode[i]) {
            drawRectangle(x, y - 1, k_char_width, k_char_height, true, WriteMode::ADD);
            write_mode = WriteMode::INVERT;
        }

        const char *index = graphicsGetDecString(i);
        drawChars(index, 0, y, WriteMode::INVERT);

        x = k_enc_mod_layer_x;
        disp_encoder_mode_t encoder = getEncoderParamValue(mod_mode[i]);
        const char *dst_val = graphicsGetModDst((mod_destinations_t)encoder.value);
        drawChars(dst_val, x, y, WriteMode::INVERT);
    }

}



void showEncoderModeByEvent(display_encoder_mode_event_t event) {

    switch(event) {

        case DISP_MODE_EVENT_HIDE:

            encoder_mode_show = false;

        case DISP_MODE_EVENT_COUNTDOWN:
            
            displayTimerSet();
            
            break;

        case DISP_MODE_EVENT_SHOW:
            
            encoder_mode_show = true;
        
        break;


        default:
            break;

    }

    display_trigger = true;

}

void displayPlayAndExtClk() {

    if(synth->seq.isRunning()) {

        devicesDisplayDrawBitmap(
            k_play_x, 
            k_play_y, 
            k_play_width, 
            k_play_height, 
            graphicsGetPlayIcon(),
            WriteMode::INVERT);

    }

}

// ################################
// PUBLIC FUNCTION DEFINITIONS
// ################################

void displaySetup(synth_state_t *synth_state, synth_parameters_t *synth_patch, uint8_t *led, bool *led_tx) {

    // disp = display;
    synth = synth_state;
    patch = synth_patch;

    led_bits = led;
    led_transmit = led_tx;

    uint8_t *param_ptr_s1[k_num_disp_parameters] = {
        &patch->osc[0].scale, 
        &patch->osc[1].scale, 
        &patch->shared.portamento, 
        &patch->mixer.osc_1_mix,
        &patch->filter.cutoff,
        &patch->osc[0].x_morph, 
        &patch->osc[1].x_morph, 
        &patch->shared.phase_mod, 
        &patch->mixer.osc_2_mix,
        &patch->filter.resonance, 
        &patch->osc[0].y_morph, 
        &patch->osc[1].y_morph, 
        &patch->shared.sync, 
        &patch->mixer.noise_mix,
        &patch->filter.env_amt, 
    };

    uint8_t *param_ptr_s2[k_num_disp_parameters] = {
        &patch->osc[0].octave, 
        &patch->osc[1].octave, 
        &patch->delay.amount, 
        &patch->filter.type,
        &patch->filter.cutoff,
        &patch->osc[0].x_morph, 
        &patch->osc[1].x_morph, 
        &patch->delay.rate, 
        &patch->filter.fm,
        &patch->filter.resonance, 
        &patch->osc[0].y_morph, 
        &patch->osc[1].y_morph, 
        &patch->delay.feedback, 
        &patch->filter.tracking,
        &patch->filter.env_amt, 
    };

    uint8_t *param_ptr_s_env[k_num_disp_parameters] = {
        &patch->osc[0].scale, 
        &patch->osc[1].scale, 
        &patch->shared.portamento, 
        &patch->mixer.osc_1_mix,
        &patch->filter.cutoff,
        &patch->osc[0].x_morph, 
        &patch->osc[1].x_morph, 
        &patch->shared.phase_mod, 
        &patch->mixer.osc_2_mix,
        &patch->filter.resonance, 
        &patch->env[2].attack, 
        &patch->env[2].decay, 
        &patch->env[2].sustain, 
        &patch->env[2].release,
        &patch->vca.level, 
    };

    uint8_t *param_ptr_env[k_num_disp_parameters] = {
        &patch->env[0].attack, 
        &patch->env[0].decay, 
        &patch->env[0].sustain, 
        &patch->env[0].release, 
        &patch->mod[0].amount, 
        &patch->env[1].attack, 
        &patch->env[1].decay, 
        &patch->env[1].sustain, 
        &patch->env[1].release, 
        &patch->mod[1].amount,  
        &patch->env[2].attack, 
        &patch->env[2].decay, 
        &patch->env[2].sustain, 
        &patch->env[2].release, 
        &patch->mod[2].amount,  
    };

    uint8_t *param_ptr_lfo[k_num_disp_parameters] = {
        &patch->lfo[0].rate, 
        &patch->lfo[0].shape, 
        &patch->lfo[0].phase, 
        &patch->lfo[0].sync,
        &patch->mod[3].amount, 
        &patch->lfo[1].rate, 
        &patch->lfo[1].shape, 
        &patch->lfo[1].phase, 
        &patch->lfo[1].sync,
        &patch->mod[4].amount,  
        &patch->lfo[2].rate, 
        &patch->lfo[2].shape, 
        &patch->lfo[2].phase, 
        &patch->lfo[2].sync,
        &patch->mod[5].amount, 
    };

    uint8_t *param_ptr_seq[k_num_disp_parameters] = {
        NULL, NULL, NULL, NULL, NULL, 
        NULL, NULL, NULL, NULL, NULL, 
        NULL, NULL, NULL, NULL, NULL, 
    };

    for(int i = 0; i < k_num_disp_parameters; i++) {

        display_param_s1[i] = (display_param_t) { param_func_s1[i], param_ptr_s1[i] };
        display_param_s2[i] = (display_param_t) { param_func_s2[i], param_ptr_s2[i] };
        display_param_s_env[i] = (display_param_t) { param_func_s_env[i], param_ptr_s_env[i] };
        display_param_env[i] = (display_param_t) { param_func_env[i], param_ptr_env[i] };
        display_param_lfo[i] = (display_param_t) { param_func_lfo[i], param_ptr_lfo[i] };
        display_param_seq[i] = (display_param_t) { param_func_seq[i], param_ptr_seq[i] };

    }

    display_param[LAYER_SYNTH_1] = display_param_s1;
    display_param[LAYER_SYNTH_2] = display_param_s2;
    display_param[LAYER_SYNTH_ENV_SPLIT] = display_param_s_env;
    display_param[LAYER_ENV] = display_param_env;
    display_param[LAYER_LFO] = display_param_lfo;
    display_param[LAYER_SEQ] = display_param_seq;

    display_trigger = true;

}

void displayTrigger() {

    display_trigger = true;

}

void displayDisplay() {

    displayTimerCheck();

    if(!display_trigger) {
        return;
    } else {
        display_trigger = false;
    }

    devicesDisplayClear();

    view_layer_t layer = viewGetLayer();
    view_page_t page = viewGetPage();
    bool p_lock = pLockIsEnabled();
    bool showing_encoder_mode = encoder_mode_show || display_timer_active;

    if(page == PAGE_SETTINGS) {

        displayPageSettings();

    } else {

        if(page == PAGE_SEQ) {

            displaySeqGenHeader();

        } else {

            displayLayerHeader(layer);

        }

        displayPlayAndExtClk(); 

        switch (layer) {
    
            case LAYER_SYNTH_1:
            case LAYER_SYNTH_2:
            case LAYER_SYNTH_ENV_SPLIT:
            case LAYER_SEQ:
    
                displayParams(layer, page, p_lock);
                if(!showing_encoder_mode) displaySteps();
                else displayEncoderMode(layer);
    
                break;
    
            case LAYER_ENV:
            case LAYER_LFO:
    
                if(!showing_encoder_mode) displayParams(layer, page, p_lock);
                else displayEncoderMode(layer);
                displaySteps();
    
                break;
    
            default:
    
                break;
        }

    }

    displayUpdateLeds(layer, page);


    devicesDisplayDisplay();
}

void displayShowEncoderMode() {

    showEncoderModeByEvent(DISP_MODE_EVENT_SHOW);

}

void displayHideEncoderMode() {

    showEncoderModeByEvent(DISP_MODE_EVENT_HIDE);

}

void displayShowEncoderModeCountdown() {

    showEncoderModeByEvent(DISP_MODE_EVENT_COUNTDOWN);

}

void displayShowPresetCounterText(uint8_t index) {

    drawChars("presets. ", 0, k_storage_y[0], WriteMode::ADD);
    drawChars("please wait.", 0, k_storage_y[1], WriteMode::ADD);

    drawChars(graphicsGetHexString(index), 0, k_storage_y[3], WriteMode::ADD);

    devicesDisplayDisplay();

    // display_trigger = true;
    displayTrigger();

}

void displayShowWritingNewPresets(uint8_t index) {

    devicesDisplayClear();

    drawChars("writing new", 0, 0, WriteMode::ADD);
    // drawChars("presets. ", 0, k_storage_y[0], WriteMode::ADD);
    // drawChars("please wait.", 0, k_storage_y[1], WriteMode::ADD);

    // drawChars(graphicsGetHexString(index), 0, k_storage_y[3], WriteMode::ADD);

    // devicesDisplayDisplay();

    // // display_trigger = true;
    // displayTrigger();

    displayShowPresetCounterText(index);
}

void displayShowUpdatingPresets(uint8_t index) {

    devicesDisplayClear();

    drawChars("updating", 0, 0, WriteMode::ADD);

    displayShowPresetCounterText(index);

}