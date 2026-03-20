#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "../common/globals.h"
#include "../controller/view.h"

#define NUMBER_OF_PARAM_ROWS 3
#define NUMBER_OF_PARAM_COLS 5
#define PARAM_HEADER_Y 2


typedef enum graphic_layer_header_t {
    G_LAYER_HEAD_SYNTH_1,
    G_LAYER_HEAD_SYNTH_2,
    G_LAYER_HEAD_ENV,
    G_LAYER_HEAD_LFO,
    G_LAYER_HEAD_NONE,

    NUM_G_LAYER_HEADERS
} graphic_layer_header_t;

typedef enum graphic_step_icon_t {
    STEP_ICON_ACTIVE,
    STEP_ICON_INACTIVE,
    STEP_ICON_UNWRITTEN,

    NUM_STEP_ICONS
} graphic_step_icon_t;

typedef enum graphic_encoder_t {
    GRAPHIC_ENCODER_DECIMAL,
    GRAPHIC_ENCODER_MOD_DST,

    NUM_GRAPHIC_ENCODERS
} graphic_encoder_t;

typedef enum settings_string_t {
    STRING_CHANNEL,
    STRING_VELOCITY,
    STRING_MOD_WHEEL,
    STRING_AFTERTOUCH,
    STRING_CLOCK,
    STRING_TRANSPORT,
    STRING_PROGRAM_CHANGE,
    STRING_OUT,
    STRING_THRU,
    STRING_PRESET,
    STRING_PITCH_BEND_DEPTH,
    STRING_LATCH,
    STRING_JUMP,
    STRING_NAME,
    STRING_LEGATO,
    STRING_OSC_DUO,
    STRING_VCA_VELOCITY,
    STRING_MOD_SOURCE,
    STRING_INIT_PATCH,
    STRING_SCOPE,

    NUM_SETTINGS_STRINGS

} settings_string_t;

typedef enum storage_string_t {

    STRING_SAVE,
    STRING_LOAD

} storage_string_t;

typedef enum char_id_t {
    CHAD_ID_NONE,
    CHAR_ID_A,
    CHAR_ID_B,
    CHAR_ID_C,
    CHAR_ID_D,
    CHAR_ID_E,
    CHAR_ID_F,
    CHAR_ID_G,
    CHAR_ID_H,
    CHAR_ID_I,
    CHAR_ID_J,
    CHAR_ID_K,
    CHAR_ID_L,
    CHAR_ID_M,
    CHAR_ID_N,
    CHAR_ID_O,
    CHAR_ID_P,
    CHAR_ID_Q,
    CHAR_ID_R,
    CHAR_ID_S,
    CHAR_ID_T,
    CHAR_ID_U,
    CHAR_ID_V,
    CHAR_ID_W,
    CHAR_ID_X,
    CHAR_ID_Y,
    CHAR_ID_Z,

    CHAR_ID_0,
    CHAR_ID_1,
    CHAR_ID_2,
    CHAR_ID_3,
    CHAR_ID_4,
    CHAR_ID_5,
    CHAR_ID_6,
    CHAR_ID_7,
    CHAR_ID_8,
    CHAR_ID_9,

    CHAR_ID_DOT,
    CHAR_ID_DASH,

    NUM_CHAR_IDS
} char_id_t;


#ifdef __cplusplus
extern "C"
{
#endif

const char* graphicsGetLogo();

const char* graphicsGetHexString(uint8_t index);
const char* graphicsGetDecString(int index);
const char* graphicsGetSeqNote(uint8_t index);
const char* graphicsGetSeqOctave(uint8_t index);
const char* graphicsGetSeqTranspose(uint8_t index);
const char* graphicsGetSeqDirection(uint8_t index);
const char* graphicsGetSeqScale(uint8_t index);
const char* graphicsGetSeqClkDiv(uint8_t index);
const char* graphicsGetSeqRepeat(uint8_t index);
const char* graphicsGetMidiNote(uint8_t index);
const char* graphicsGetFilterType(uint8_t index);
const char* graphicsGetLfoShape(uint8_t index);
const char* graphicsGetVcaMode(uint8_t index);
const char* graphicsGetResetSync(uint8_t index);
const char* graphicsPrintNothing(uint8_t index);
const char* graphicsGetSettingsString(settings_string_t string);
const char* graphicsGetDoubleDash(uint8_t x);
const char* graphicsGetCharFromID(uint8_t index);
const char* graphicsBuildCharString(const uint8_t* ids, size_t count, char* outBuffer);
const char* graphicsGetScopeDiv(uint8_t value);

const char* graphicsGetParamHeader(view_layer_t l, uint8_t index);
unsigned char* graphicsGetStepIcon(graphic_step_icon_t i);
unsigned char* graphicsGetSettingsIcon(bool global);
unsigned char* graphicsGetPlayIcon();
const char* graphicsGetModDst(mod_destinations_t dst);
const char* graphicsGetUserParamString(mod_destinations_t dst);
const char* graphicsGetEncoderMode(view_mode_t mode);
const char* graphicsGetOnOff(bool on);
const char* graphicsGetStorageTypeString(storage_string_t string);
const char* graphicsGetStorageActiveString(storage_string_t string);

const char* graphicsGetLayerName(view_layer_t layer);
const char* graphicsGetPageName(view_page_t page);

const char* graphicsGetModSource(mod_sources_t src);


#ifdef __cplusplus
}
#endif


#endif