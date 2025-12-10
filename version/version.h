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
void versionTest();
void versionStorageTest();

void versionParameterHotfix();

#ifdef __cplusplus
}
#endif

#endif
