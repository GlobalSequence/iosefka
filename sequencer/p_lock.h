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
