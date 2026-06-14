// Copyright (C) 2026 Cory Oleson <globeseq@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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

        if(synthUpdate()) {

            control_flag = true;
        }
    }

}

int main()
{
    stdio_init_all();

    bool clock_work = set_sys_clock_khz(192000, true);

    startupRun();

    multicore_launch_core1(core1_main);

    while(1) {

        bool set_control = control_flag;
        if(control_flag) control_flag = false;
        controlLoopUpdateByFlag(set_control);
        
    }

    return 0;
}
