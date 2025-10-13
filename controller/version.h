#ifndef VERSION_H
#define VERSION_H

#include <stdio.h>
#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

bool versionWriteVersionToCleanStorage();
void versionUpdate();
// void versionMigratePresetv0_0_0_to_0_1_0();

#ifdef __cplusplus
}
#endif

#endif
