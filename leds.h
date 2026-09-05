// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Richard Smetana

#ifndef MIDICTRL_LEDS_H
#define MIDICTRL_LEDS_H

#include <stdint.h>

void ledsBegin();
void ledsHandleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void ledsHandleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void ledsNotifyMidiActivity();  // call on any received MIDI message
void ledsUpdate();

#endif
