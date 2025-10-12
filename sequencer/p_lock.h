#ifndef P_LOCK_H
#define P_LOCK_H

#include <stdio.h>
#include "pico/stdlib.h"

#define P_LOCK_NOTE_ADJUST_OFFSET 13

#ifdef __cplusplus
extern "C"
{
#endif

void pLockEnable(bool enable);
void pLockEngage(bool engage);
void pLockSetOrder(uint8_t index);
void pLockSetNote(uint8_t index, uint8_t value);
void pLockSetNoteDistance(int8_t value);
void pLockResetOrder(uint8_t index);
void pLockResetOrderAll();

bool pLockIsEnabled();
bool pLockIsEngaged();
uint8_t pLockGetNote(uint8_t index);
uint8_t pLockGetNoteDistanceUnsigned(uint8_t index);
int8_t pLockGetOrder(uint8_t index);
int8_t pLockGetOrderFirst();

#ifdef __cplusplus
}
#endif

#endif
