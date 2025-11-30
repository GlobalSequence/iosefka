#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"

#include "../controller/startup.h"
#include "../controller/control_loop.h"
#include "../synth/synth.h"
#include "devices/audio.h"

#include "../common/test.h"

volatile bool control_flag;

void core1_main() {

    startupRunCore1();

    while(1) {

        // if(!control_flag) {

        //     control_flag = synthUpdate();

        // }

        if(synthUpdate()) control_flag = true;

    }

}

int main()
{
    stdio_init_all();

    bool clock_work = set_sys_clock_khz(192000, true);

    // sleep_ms(4000);

    startupRun();

    multicore_launch_core1(core1_main);

/*     sleep_ms(2000);
    extern char __flash_binary_start;  // defined in linker script
    extern char __flash_binary_end;    // defined in linker script
    uintptr_t start = (uintptr_t) &__flash_binary_start;
    uintptr_t end = (uintptr_t) &__flash_binary_end;
    printf("Binary starts at %08x and ends at %08x, size is %08x\n", start, end, end-start); 
*/

    while(1) {

        bool set_control = control_flag;
        if(control_flag) control_flag = false;
        controlLoopUpdateByFlag(set_control);
        
    }

    return 0;
}
