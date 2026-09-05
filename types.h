// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Richard Smetana

#ifndef MIDICTRL_TYPES_H
#define MIDICTRL_TYPES_H

#include <stdint.h>

enum NoteAction : uint8_t {
  NOTE_NONE = 0,
  NOTE_ON = 1,
  NOTE_OFF = 2
};

enum LedChannel : uint8_t {
  LED_CH_R = 0,
  LED_CH_G = 1,
  LED_CH_B = 2
};

#endif
