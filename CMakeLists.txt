# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(iosefka C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(iosefka iosefka.cpp 
common/config.h
common/globals.h
common/test.h
common/utilities.h
common/ring_buffer.h

controller/control_loop.h
controller/control_loop.cpp
controller/display.h
controller/display.cpp
controller/encoder_map.h  
controller/encoder_map.cpp
controller/event_processor.h
controller/event_processor.cpp
controller/midi.h
controller/midi.cpp
controller/note_stack.h
controller/note_stack.cpp
controller/note_queue.h  
controller/note_queue.cpp
controller/settings.h
controller/settings.cpp
controller/parameter.h  
controller/parameter.cpp
controller/responsive_analog_read.h
controller/responsive_analog_read.cpp
controller/startup.h
controller/startup.cpp
controller/storage.h  
controller/storage.cpp
controller/switch_reader.h  
controller/switch_reader.cpp
controller/switch_actions.h  
controller/switch_actions.cpp
controller/ui.h
controller/ui.cpp
controller/version.h  
controller/version.cpp
controller/view.h
controller/view.cpp

devices/audio.h  
devices/audio.c
devices/devices.h
devices/devices.cpp
devices/eeprom.h
devices/eeprom.c
devices/gpio.h
devices/gpio.c
devices/peripheral_config.h
devices/pin_assignments.h
devices/cd4051.h
devices/cd4051.c
devices/mcp230xx.h
devices/mcp230xx.c
devices/encoder.h
devices/encoder.c
devices/pwm.h
devices/pwm.cpp
devices/ssd1306.h
devices/ssd1306.cpp

graphics/graphics.h
graphics/graphics.c
graphics/frameBuffer/FrameBuffer.h
graphics/frameBuffer/FrameBuffer.cpp
graphics/textRenderer/TextRenderer.h
graphics/textRenderer/TextRenderer.cpp
graphics/shapeRenderer/ShapeRenderer.h
graphics/shapeRenderer/ShapeRenderer.cpp

sequencer/pattern_generator.h
sequencer/pattern_generator.cpp
sequencer/p_lock.h  
sequencer/p_lock.cpp
sequencer/sequencer.h
sequencer/sequencer.cpp
sequencer/seq_poll.h
sequencer/seq_poll.cpp

synth/cv.h
synth/delay.h
synth/envelope.h
synth/lfo.h
synth/mod_matrix.h
synth/mod_matrix.cpp
synth/oscillator.h
synth/oscillator.cpp
synth/patch.h
synth/voice.h
synth/voice.cpp
synth/voice2.h
synth/voice2.cpp
synth/synth.h  
synth/synth.cpp

)

# Generate PIO header
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/devices/pio_rotary_encoder.pio)
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/devices/spi.pio)

pico_set_program_name(iosefka "iosefka")
pico_set_program_version(iosefka "0.1")


# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(iosefka 0)
pico_enable_stdio_usb(iosefka 1)

# Add the standard library to the build
target_link_libraries(iosefka
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(iosefka PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(iosefka 
        hardware_spi
        hardware_i2c
        hardware_dma
        hardware_pio
        hardware_timer
        hardware_adc
        hardware_uart
        hardware_pwm
        hardware_clocks
        pico_multicore
        )

pico_add_extra_outputs(iosefka)

