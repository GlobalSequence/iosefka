#ifndef EVENT_PROCESSOR_H
#define EVENT_PROCESSOR_H

#include <stdio.h>
#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C"
{
#endif

bool eventProcessorCheckForActionsOnStartup();
bool eventProcessorUpdate();

#ifdef __cplusplus
}
#endif

#endif