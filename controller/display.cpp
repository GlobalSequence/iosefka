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
// SETTINGS DEFINES
// ################################

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// #define NUMBER_OF_VISIBLE_SETTINGS_ROWS 8
#define NUMBER_OF_VISIBLE_SETTINGS_ROWS 6

// const uint8_t k_settings_row_y[NUMBER_OF_VISIBLE_SETTINGS_ROWS] = {0, 8, 16, 24, 32, 40, 48, 54};
const uint8_t k_settings_row_x = 8;
const uint8_t k_settings_row_y[NUMBER_OF_VISIBLE_SETTINGS_ROWS] = {2, 12, 22, 32, 42, 52};
const uint8_t k_settings_value_x[] = {110, 102, 94};
const uint8_t k_settings_mod_src_x = 56;
const uint8_t k_settings_checkbox_x = 110;
const uint8_t k_settings_preset_name_x = 54;
const uint8_t k_settings_mode_focus_w = 121;

const uint8_t k_scroll_bar_frame_x = 121;
const uint8_t k_scroll_bar_frame_w = 5;
const uint8_t k_scroll_bar_x = 123;
const uint8_t k_scroll_bar_w = 1;

static uint8_t set_row_top;
static uint8_t set_row_bottom;
static uint8_t settings_scroll_pos;
static uint8_t settings_scroll_bar_height;
static uint8_t settings_scroll_bar_increment;

// ################################
// PARAMETER DEFINES
// ################################

static const uint8_t k_char_width = 8;
static const uint8_t k_char_height = 8;
static const uint8_t k_param_width = 16;
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
const uint8_t k_seq_prob_index = 10;

const uint8_t k_param_p_lock_length = 18;
const uint8_t k_param_p_lock_height = 9;

const uint8_t k_settings_text_x[] = {2, 65};
const uint8_t k_settings_text_y[] = {2, 12, 22, 32};
const uint8_t k_settings_param_x = 109;
const uint8_t k_settings_check_box_x[] = {55, 117};
const uint8_t k_check_size_x = 8;
const uint8_t k_check_size_y = 8;
const uint8_t k_settings_midi_mod_x[] = {2, 46, 90};
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

// indices of constrained value parameters on their respective layers
const uint8_t k_param_sync_i = 12;
const uint8_t k_param_oct_1_i = 0;
const uint8_t k_param_oct_2_i = 1;
const uint8_t k_param_fil_t_i = 3;
const uint8_t k_param_lfo_1_sh_i = 1;
const uint8_t k_param_lfo_2_sh_i = 6;
const uint8_t k_param_lfo_3_sh_i = 11;
const uint8_t k_param_lfo_1_rs_i = 3;
const uint8_t k_param_lfo_2_rs_i = 8;
const uint8_t k_param_lfo_3_rs_i = 13;

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

static unsigned long display_hang_timer_started;
static const unsigned long k_display_hang_timer_timeout = 1000000;

static unsigned long display_off_timer_started;
static const unsigned long k_display_off_timer_timeout = 300000000;

static bool display_hang_timer_active = false;
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

paramFuncGetGraphic param_func_user_param[k_num_disp_parameters] = {

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

display_param_t display_param_s1[k_num_disp_parameters];
display_param_t display_param_s2[k_num_disp_parameters];
display_param_t display_param_s_env[k_num_disp_parameters];
display_param_t display_param_env[k_num_disp_parameters];
display_param_t display_param_lfo[k_num_disp_parameters];
display_param_t display_param_seq[k_num_disp_parameters];
display_param_t display_param_usr[k_num_disp_parameters];

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
void displayHangTimerSet();
void displayHangTimerCheck();
void displayOffTimer(bool reset);
// void draw8x8Rectangle(uint8_t x, uint8_t y);
void drawChars(const char* text, uint8_t x, uint8_t y, WriteMode write_mode);
void drawRectangle(uint8_t x, uint8_t y, uint8_t x_len, uint8_t y_len, bool fill, WriteMode mode);
uint8_t getSeqParamValue(uint8_t index, uint8_t step, bool p_lock);
disp_encoder_mode_t getEncoderParamValue(view_mode_t mode);
bool isModLayer(view_layer_t layer);
void drawModLayer(view_layer_t layer, uint8_t index);
uint8_t setValueXCoordIndexByTens(int value);

// ################################
// PRIVATE FUNCTION DEFINITIONS
// ################################

void displayHangTimerSet() {

    display_hang_timer_started = time_us_64();
    display_hang_timer_active = true;

}

void disableAdditionalTimerFlags() {

    show_storage_complete = false;

}

void displayHangTimerCheck() {

    if(display_hang_timer_active) {

        if(time_us_64() - display_hang_timer_started >= k_display_hang_timer_timeout) {

            display_hang_timer_active = false;
            display_trigger = true;
            disableAdditionalTimerFlags();

        }

    }

}

void displayOffTimer(bool reset) {

    unsigned long time = time_us_64();

    if(reset) {
        display_off_timer_started = time;
        return;
    }

    if(time - display_off_timer_started >= k_display_off_timer_timeout) {

        devicesDisplayClear();
        devicesDisplayDisplay();

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
            val = synth->settings.channel;

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

        case MODE_USER_PARAM_DST:

            graphic = GRAPHIC_ENCODER_MOD_DST;
            val = synth->user_param.destination[viewGetCurrentUserParameter()];

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
// this is basically the same as the encoder mode mod layer except with the loop and minus the mod dsts
// TODO: merge them
void drawModLayer(view_layer_t layer, uint8_t index) {

        uint8_t y = param_coords_y[index];
        const char* i = "0";

        switch(index) {
            case 0: i = "1"; break;
            case 1: i = "2"; break;
            case 2: i = "3"; break;
            default: break;
        }

        uint8_t sel;
        view_mode_t mode = viewGetMode();

        if(layer == LAYER_ENV) {

            switch(mode) {
                case MODE_MOD_DST_ENV_1: sel = 0; break;
                case MODE_MOD_DST_ENV_2: sel = 1; break;
                case MODE_MOD_DST_ENV_3: sel = 2; break;
                default: break;
            }

            const char* loop = " ";
            if(patch->env[index].loop) loop = "L";

            drawChars(loop, 120, y, WriteMode::ADD);

        } else if(layer == LAYER_LFO) {

            switch(mode) {
                case MODE_MOD_DST_LFO_1: sel = 0; break;
                case MODE_MOD_DST_LFO_2: sel = 1; break;
                case MODE_MOD_DST_LFO_3: sel = 2; break;
                default: break;
            }

        }
        
        if(index == sel) drawRectangle(0, y - 1, k_char_width, k_char_height, true, WriteMode::ADD);
        drawChars(i, 0, y, WriteMode::INVERT);
}

uint8_t getPLockStep() {

    uint8_t sw = pLockGetOrderFirst();
    uint8_t step = sw + viewGetSequencerWindowStepBase();
    return step;

}

bool isThisAPLockedSynthVal(uint8_t step, uint8_t *p) {

    mod_destinations_t dst = (mod_destinations_t)synth->seq.getStepPLockParam(step);

    if(dst == MOD_DST_NONE) return false;

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

            case LAYER_USER_PARAM:
                layer_led    = LED_LAYER_SYNTH_2;
                layer_led_2  = LED_LAYER_SYNTH_1;
                break;

            default:
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

        } else if(layer == LAYER_USER_PARAM) {

            return;

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

                break;

            case STORAGE_STATE_FINISHED:

                show_storage_complete = true;
                displayHangTimerSet();

                break;

            case STORAGE_STATE_OFF: 

                type = storageGetLastType();
                type_string = type == STORAGE_TYPE_SAVE ? STRING_SAVE : STRING_LOAD;

                storage_char = graphicsGetStorageTypeString(type_string);
                
                drawChars(storage_char, k_storage_x, k_storage_y[0], WriteMode::INVERT);
                drawChars("complete!", k_storage_x, k_storage_y[1], WriteMode::INVERT);

                break;

        }

    } else {

        view_mode_t mode = viewGetMode();

        uint8_t row_ref = viewGetSettingsRow();
        bool outline = true;

        if(row_ref > set_row_bottom) {

            set_row_bottom = row_ref;
            set_row_top = set_row_bottom - (NUMBER_OF_VISIBLE_SETTINGS_ROWS - 1);

        }

        if(row_ref < set_row_top) {

            set_row_top = row_ref;
            set_row_bottom = set_row_top + (NUMBER_OF_VISIBLE_SETTINGS_ROWS - 1);

        }

        settings_row_t row_type[NUMBER_OF_VISIBLE_SETTINGS_ROWS];
        bool checkbox[NUMBER_OF_VISIBLE_SETTINGS_ROWS];
        const char* str[NUMBER_OF_VISIBLE_SETTINGS_ROWS];
        const char* val[NUMBER_OF_VISIBLE_SETTINGS_ROWS];

        for(int i = 0; i < NUMBER_OF_VISIBLE_SETTINGS_ROWS; i++) {

            checkbox[i] = false;
            val[i] = "";

            uint8_t row_vis = set_row_top + i;
            row_type[i] = (settings_row_t) row_vis;
            paramFuncGetGraphic h = graphicsGetHexString;
            settingsStringFunc s = graphicsGetSettingsString;
            mod_sources_t src = (mod_sources_t) viewGetModSourceFromSettings();

            switch (row_vis)
            {
                case SET_ROW_PRESET_NUM:
                    str[i] = s(STRING_PRESET);
                    val[i] = graphicsGetDecString(viewGetPresetNumber() + 1);
                    break;

                case SET_ROW_PRESET_NAME:
                    str[i] = /* s(STRING_NAME) */"";
                    val[i] = /* graphicsGetDecString(viewGetPresetNumber()) */"xxx";
                    break;

                case SET_ROW_MIDI_CHAN:
                    str[i] = s(STRING_CHANNEL);
                    val[i] = graphicsGetDecString(synth->settings.channel + 1);
                    break;

                case SET_ROW_MIDI_CLOCK:
                    str[i] = s(STRING_CLOCK);
                    checkbox[i] = synth->settings.clock_rx;
                    break;

                case SET_ROW_MIDI_TRANS:
                    str[i] = s(STRING_TRANSPORT);
                    checkbox[i] = synth->settings.transport_rx;
                    break;

                case SET_ROW_MIDI_PGM:
                    str[i] = s(STRING_PROGRAM_CHANGE);
                    checkbox[i] = synth->settings.program_change_rx;
                    break;

                case SET_ROW_LEGATO:
                    str[i] = s(STRING_LEGATO);
                    // checkbox[i] = false;
                    break;

                case SET_ROW_OSC_DUO:
                    str[i] = s(STRING_OSC_DUO);
                    // checkbox[i] = false;
                    break;

                case SET_ROW_VCA_VELOCITY:
                    str[i] = s(STRING_VCA_VELOCITY);
                    checkbox[i] = synth->patch.vca.velocity_on;
                    break;

                case SET_ROW_POT_LATCH:
                    str[i] = s(STRING_LATCH);
                    checkbox[i] = synth->settings.latch;
                    break;

                case SET_ROW_PB_DEPTH:
                    str[i] = s(STRING_PITCH_BEND_DEPTH);
                    val[i] = graphicsGetDecString(synth->settings.pitch_bend_depth + 1);
                    break;

                case SET_ROW_MOD_SRC:
                    str[i] = s(STRING_MOD_SOURCE);
                    val[i] = graphicsGetModSource(src);
                    break;

                case SET_ROW_MOD_DST:
                    str[i] = "";
                    val[i] = graphicsGetModDst((mod_destinations_t)synth->patch.mod[src].destination);
                    break;

                case SET_ROW_INIT_PATCH:
                    str[i] = s(STRING_INIT_PATCH);
                    break;

                case NUM_SET_ROWS:
                default:
                    str[i] = "";
                    val[i] = "";
                    // handle invalid row_vis
                    break;
            }

            // drawChars(string, )

        }

        for(int i = 0; i < NUMBER_OF_VISIBLE_SETTINGS_ROWS; i++) {

            WriteMode m_a = WriteMode::ADD;
            WriteMode m_i = WriteMode::INVERT;

            uint8_t y = k_settings_row_y[i];

            drawChars(str[i], k_settings_row_x, y, m_a);

            switch(row_type[i]) {

                case SET_ROW_MIDI_CLOCK:
                case SET_ROW_MIDI_TRANS:
                case SET_ROW_MIDI_PGM:
                case SET_ROW_LEGATO:
                case SET_ROW_OSC_DUO:
                case SET_ROW_VCA_VELOCITY:
                case SET_ROW_POT_LATCH:
                    if(row_ref == row_type[i]) outline = false;
                    checkBox(k_settings_checkbox_x, y, checkbox[i]);
                    break;

                case SET_ROW_PRESET_NUM:
                case SET_ROW_MIDI_CHAN:
                case SET_ROW_PB_DEPTH:
                    drawChars(val[i], k_settings_value_x[strlen(val[i]) - 1], y, m_a);
                    break;
                
                case SET_ROW_MOD_SRC:
                    drawChars(val[i], k_settings_mod_src_x, y, m_a);
                    break;
                
                case SET_ROW_MOD_DST:
                    drawChars(val[i], k_settings_row_x, y, m_a);
                    break;

                case SET_ROW_PRESET_NAME:
                    drawChars("xxx", k_settings_row_x, y, m_a);

                default: break;

            }

        }

        uint8_t scroll_pos = row_ref - set_row_top;
        uint8_t y_pos = k_settings_row_y[scroll_pos];

        // drawChars(graphicsGetDecString(row_ref), 0, k_settings_row_y[scroll_pos], WriteMode::ADD);
        devicesDisplayDrawBitmap(0, /* k_settings_row_y[scroll_pos] */y_pos, k_char_height, k_char_width, graphicsGetStepIcon(STEP_ICON_INACTIVE), WriteMode::ADD);

        uint8_t scroll_bar_y_top = (settings_scroll_bar_increment * set_row_top) + 2; // the "+ 2" is a manual adjustment for visual purposes

        drawRectangle(k_scroll_bar_frame_x, 0, k_scroll_bar_frame_w, SCREEN_HEIGHT - 1, false, WriteMode::ADD);
        drawRectangle(k_scroll_bar_x, scroll_bar_y_top, k_scroll_bar_w, settings_scroll_bar_height, true, WriteMode::ADD);

        if(mode != MODE_SET_SCROLL && outline) {

            drawLine(0, y_pos - 2, k_settings_mode_focus_w, 1, WriteMode::ADD);
            drawLine(0, y_pos + 10, k_settings_mode_focus_w, 1, WriteMode::ADD);

        }
/*         machine_setting_t m = synth->settings;

        uint8_t channel = m.channel + 1;
        uint8_t clock = m.clock_rx;
        uint8_t transport = m.transport_rx;
        uint8_t pb_depth = m.pitch_bend_depth + 1;
        bool latch = m.latch;


        uint8_t preset = viewGetPresetNumber() + 1;
        // uint8_t channel = synth->settings.channel + 1;
        bool vca_vel = patch->vca.velocity_on;
        uint8_t velocity = patch->mod[MOD_SRC_VELOCITY].destination;
        uint8_t mod_wheel = patch->mod[MOD_SRC_MOD_WHEEL].destination;
        uint8_t aftertouch = patch->mod[MOD_SRC_AFTERTOUCH].destination;
        // bool clock = synth->settings.clock_rx;
        // bool transport = synth->settings.transport_rx;
        // uint8_t pb_depth = synth->settings.pitch_bend_depth;
        // bool latch = synth->settings.latch;
    
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

            r_x_s = k_settings_text_x[1];
            r_y_s = k_settings_text_y[3];
            char_len = 5;
    
            default: break;
    
        }
    
        drawRectangle(r_x_s, r_y_s, char_len * k_char_width, k_char_height, true, m_a);
        drawChars(s(STRING_PRESET), k_settings_text_x[0], k_settings_text_y[0], WriteMode::INVERT);
        drawChars(graphicsGetDecString(preset), k_settings_param_x, k_settings_text_y[0], WriteMode::INVERT);
        drawChars(s(STRING_CHANNEL), k_settings_text_x[0], k_settings_text_y[1], WriteMode::INVERT);
        drawChars(graphicsGetDecString(channel), k_settings_param_x, k_settings_text_y[1], WriteMode::INVERT);
        drawChars(s(STRING_CLOCK), k_settings_text_x[0], k_settings_text_y[2], WriteMode::INVERT);
        drawChars(s(STRING_TRANSPORT), k_settings_text_x[1], k_settings_text_y[2], WriteMode::INVERT);
        drawChars(s(STRING_LATCH), k_settings_text_x[0], k_settings_text_y[3], WriteMode::INVERT);
        drawChars(s(STRING_PITCH_BEND_DEPTH), k_settings_text_x[1], k_settings_text_y[3], WriteMode::INVERT);


    
        // if(out_thru) {
        //     drawRectangle(k_settings_text_x[1], k_settings_text_y[3], k_thru_length, k_char_height, true, m_a);
        // }
        // drawChars(s(d_out_through), k_settings_text_x[1], k_settings_text_y[3], WriteMode::INVERT);
    
        checkBox(k_settings_check_box_x[0], k_settings_text_y[2], clock);
        checkBox(k_settings_check_box_x[1], k_settings_text_y[2], transport);
        checkBox(k_settings_check_box_x[0], k_settings_text_y[3], latch);

        drawChars(graphicsGetDecString(pb_depth), k_settings_text_x[1] + (k_char_width * 6), k_settings_text_y[3], WriteMode::ADD);
    
        drawChars(s(STRING_VELOCITY), k_settings_midi_mod_x[0], k_settings_midi_mod_y, WriteMode::INVERT);
        drawChars(s(STRING_MOD_WHEEL), k_settings_midi_mod_x[1], k_settings_midi_mod_y, WriteMode::INVERT);
        drawChars(s(STRING_AFTERTOUCH), k_settings_midi_mod_x[2], k_settings_midi_mod_y, WriteMode::INVERT);
    
        // if(display_midi_mod_dst) drawText(disp, font_8x8, enc_val, 0, k_enc_val_y);
        drawChars(enc_val, 0, k_enc_val_y, WriteMode::INVERT);

        const char* vca_indicator = " ";
        if(vca_vel) vca_indicator = "*";
        drawChars(vca_indicator, k_settings_midi_mod_x[0] + k_char_width * 3, k_settings_midi_mod_y, WriteMode::INVERT);
 */
    }

}

void displayParams(view_layer_t layer, view_page_t page, bool p_lock) {

    uint8_t step = getPLockStep();
    uint8_t k = 0;
    bool mod_layer = isModLayer(layer);

    for(int i = 0; i < NUMBER_OF_PARAM_ROWS; i++) {

        for(int j = 0; j < NUMBER_OF_PARAM_COLS; j++) {

            display_param_t p = display_param[layer][k];
            paramFuncGetGraphic func = p.func;
            uint8_t* ptr;
            uint8_t val;
            uint8_t x = param_coords_x[j];
            uint8_t y = param_coords_y[i];

            if(layer == LAYER_SEQ) {

                if(p_lock) {
                    if(k == 0) drawRectangle(x - 1, y - 1, seq_p_lock_length, seq_p_lock_height, true, WriteMode::ADD);
                    if(k == k_seq_prob_index) drawRectangle(x - 1, y - 1, k_param_width + 1, k_char_height + 1, true, WriteMode::ADD);
                }

                val = getSeqParamValue(k, step, p_lock);

            } else {

                mod_destinations_t dst;

                if(layer == LAYER_USER_PARAM) {

                    dst = (mod_destinations_t)*p.val_p;
                    ptr = parameterGetPointer(dst);

                    if(dst == MOD_DST_NONE) func = graphicsGetDoubleDash;

                    if(k == viewGetCurrentUserParameter()) {
                        drawRectangle(x - 1, y - 1, k_param_p_lock_length, k_param_p_lock_height, false, WriteMode::ADD);
                    }

                } else {

                    ptr = p.val_p;

                }

                if(p_lock && isThisAPLockedSynthVal(step, ptr)) {

                    val = synth->seq.getStepPLockValue(step);
                    drawRectangle(x - 1, y - 1, k_param_p_lock_length, k_param_p_lock_height, true, WriteMode::ADD);
            
                } else {

                    val = *ptr;

                }

                switch(layer) {

                    case LAYER_SYNTH_1:
                        if(k == k_param_sync_i) val = param8Bits0To1(val);
                        break;
                    case LAYER_SYNTH_2:
                        if (k == k_param_oct_1_i) val = param8Bits0To3(val);
                        else if (k == k_param_oct_2_i) val = param8Bits0To3(val);
                        else if (k == k_param_fil_t_i) val = param8Bits0To3(val);
                        break;
                    case LAYER_LFO:
                        if (k == k_param_lfo_1_sh_i) val = param8Bits0To4(val);
                        else if (k == k_param_lfo_2_sh_i) val = param8Bits0To4(val);
                        else if (k == k_param_lfo_3_sh_i) val = param8Bits0To4(val);
                        else if (k == k_param_lfo_1_rs_i) val = param8Bits0To3(val);
                        else if (k == k_param_lfo_2_rs_i) val = param8Bits0To3(val);
                        else if (k == k_param_lfo_3_rs_i) val = param8Bits0To3(val);
                        break;    
                    case LAYER_USER_PARAM:
                        
                        switch(dst) {
                            case MOD_DST_SYNC: 
                            case MOD_DST_ENV_1_LOOP:
                            case MOD_DST_ENV_2_LOOP:
                            case MOD_DST_ENV_3_LOOP:
                                val = param8Bits0To1(val); 
                                break;

                            case MOD_DST_OSC_1_OCTAVE:
                            case MOD_DST_OSC_2_OCTAVE:
                            case MOD_DST_FILTER_TYPE:
                            case MOD_DST_LFO_1_SYNC:
                            case MOD_DST_LFO_2_SYNC:
                            case MOD_DST_LFO_3_SYNC:
                                val = param8Bits0To3(val);
                                break;

                            case MOD_DST_LFO_1_SHAPE:
                            case MOD_DST_LFO_2_SHAPE:
                            case MOD_DST_LFO_3_SHAPE:
                                val = param8Bits0To4(val);
                                break;
                            
                            default: break;
                        }

                        break;

                    default: 
                        break;
                }

            }
            const char* param = func(val);
            drawChars(param, x, y, WriteMode::INVERT);
            k++;

        }

        if(mod_layer) drawModLayer(layer, i);

    }

/*     uint8_t step = getPLockStep();

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

        // WHEN YOU MOVE THIS, REPLACE THIS WITH THE 'ISMODLAYER' FUNCTION IN THIS FILE
        if(layer == LAYER_ENV || layer == LAYER_LFO) {

            uint8_t y = param_coords_y[i];
            const char* index = "0";

            switch(i) {
                case 0:
                    index = "1";
                    break;
                case 1:
                    index = "2";
                    break;
                case 2:
                    index = "3";
                    break;
                default:
                    break;
            }

            drawChars(index, 0, y, WriteMode::INVERT);

            if(layer == LAYER_ENV) {

                const char* loop = " ";
                if(patch->env[i].loop) loop = "L";

                drawChars(loop, 120, y, WriteMode::ADD);

            }

        }

    } */

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

            // pull back the vertical last step indicator by 1 px if on step 7 or 15
            uint8_t x_place;
            if(last == 0x0F || last == 0x1F) x_place = x_end -= 1;
            else x_place = x_end;
            drawLine(x_place, y_start, 1, k_step_height, WriteMode::INVERT);

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
    // hack to make it so that you're always seeing the user param dst
    if(mode == MODE_USER_PARAM_SEL) mode = MODE_USER_PARAM_DST; 

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

        // put a rectangle around the selectable modulator
        if(mode == mod_mode[i]) {
            drawRectangle(x, y - 1, k_char_width, k_char_height, true, WriteMode::ADD);
            write_mode = WriteMode::INVERT;
        }

        // list the modulator numbers, 1, 2, 3
        uint8_t mod_number = i + 1;
        const char *index = graphicsGetDecString(mod_number);
        drawChars(index, 0, y, WriteMode::INVERT);

        // display the destinations
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
            
            displayHangTimerSet();
            
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

uint8_t setValueXCoordIndexByTens(int value) {

    if(value < 10) return 0;
    else if (value >= 10 && value < 100) return 1;
    else if (value >= 100 && value < 1000) return 2;
    return 0;
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

    uint8_t *param_ptr_user_param[k_num_disp_parameters] = {
        &synth->user_param.destination[0],
        &synth->user_param.destination[1],
        &synth->user_param.destination[2],
        &synth->user_param.destination[3],
        &synth->user_param.destination[4],
        &synth->user_param.destination[5],
        &synth->user_param.destination[6],
        &synth->user_param.destination[7],
        &synth->user_param.destination[8],
        &synth->user_param.destination[9],
        &synth->user_param.destination[10],
        &synth->user_param.destination[11],
        &synth->user_param.destination[12],
        &synth->user_param.destination[13],
        &synth->user_param.destination[14],
    };

    for(int i = 0; i < k_num_disp_parameters; i++) {

        display_param_s1[i] = (display_param_t) { param_func_s1[i], param_ptr_s1[i] };
        display_param_s2[i] = (display_param_t) { param_func_s2[i], param_ptr_s2[i] };
        display_param_s_env[i] = (display_param_t) { param_func_s_env[i], param_ptr_s_env[i] };
        display_param_env[i] = (display_param_t) { param_func_env[i], param_ptr_env[i] };
        display_param_lfo[i] = (display_param_t) { param_func_lfo[i], param_ptr_lfo[i] };
        display_param_seq[i] = (display_param_t) { param_func_seq[i], param_ptr_seq[i] };
        display_param_usr[i] = (display_param_t) { param_func_user_param[i], param_ptr_user_param[i]};
    }

    display_param[LAYER_SYNTH_1] = display_param_s1;
    display_param[LAYER_SYNTH_2] = display_param_s2;
    display_param[LAYER_SYNTH_ENV_SPLIT] = display_param_s_env;
    display_param[LAYER_ENV] = display_param_env;
    display_param[LAYER_LFO] = display_param_lfo;
    display_param[LAYER_SEQ] = display_param_seq;
    display_param[LAYER_USER_PARAM] = display_param_usr;

    display_trigger = true;

    set_row_top = 0;
    set_row_bottom = NUMBER_OF_VISIBLE_SETTINGS_ROWS - 1;

    // float scroll_bar_proportion = (float)(NUMBER_OF_VISIBLE_SETTINGS_ROWS) / (float)(NUM_SET_ROWS);
    // // float scroll_bar_height_float = scroll_bar_proportion * (float)(SCREEN_HEIGHT);
    // float scroll_bar_increment_float = scroll_bar_proportion * ((float)(SCREEN_HEIGHT) / (float)(NUMBER_OF_VISIBLE_SETTINGS_ROWS));
    // float scroll_bar_height_float = scroll_bar_increment_float * ((float)(NUMBER_OF_VISIBLE_SETTINGS_ROWS));
    // settings_scroll_bar_height = (uint8_t)(scroll_bar_height_float);
    // settings_scroll_bar_increment = (uint8_t)(scroll_bar_increment_float);

    float p = (float)NUMBER_OF_VISIBLE_SETTINGS_ROWS / (float)NUM_SET_ROWS;
    float scroll_bar_height_float = p * (float)SCREEN_HEIGHT;
    float scroll_bar_increment_float = ((float)SCREEN_HEIGHT - scroll_bar_height_float) / ((float)NUM_SET_ROWS - (float)NUMBER_OF_VISIBLE_SETTINGS_ROWS);

    settings_scroll_bar_height    = (uint8_t)scroll_bar_height_float + 1;
    settings_scroll_bar_increment = (uint8_t)scroll_bar_increment_float;


}

void displayTrigger() {

    display_trigger = true;

}

void displayDisplay() {

    displayHangTimerCheck();
    displayOffTimer(display_trigger);

    if(!display_trigger) {
        
        return;
    } else {
        display_trigger = false;
    }

    devicesDisplayClear();

    view_layer_t layer = viewGetLayer();
    view_page_t page = viewGetPage();
    bool p_lock = pLockIsEnabled();
    bool showing_encoder_mode = encoder_mode_show || display_hang_timer_active;

    if(page == PAGE_SETTINGS) {

        displayPageSettings();

    } else {

        if(layer == LAYER_SEQ) {

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
            case LAYER_USER_PARAM:
    
                displayParams(layer, page, p_lock);
                if(viewGetMode() == MODE_SEQ_WINDOW_SEL) showing_encoder_mode = false;
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