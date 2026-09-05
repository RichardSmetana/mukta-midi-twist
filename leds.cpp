// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Richard Smetana

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "leds.h"

struct BuiltinNoteMap {
  uint8_t note;
  LedChannel channel;
};

struct StripNoteMap {
  uint8_t note;
  uint8_t pixelIndex;
  LedChannel channel;
};

static uint8_t builtinLevels[3];
static uint8_t stripLevels[STRIP_LED_COUNT][3];
static bool builtinDirty = false;
static bool stripDirty = false;
static unsigned long lastMidiActivityMs = 0;
static bool idleTimeoutFired = false;

static Adafruit_NeoPixel strip(STRIP_LED_COUNT, STRIP_PIN, NEO_RGB + NEO_KHZ800);

static const BuiltinNoteMap BUILTIN_NOTE_MAP[] = {
  { LED_NOTE_R, LED_CH_R },
  { LED_NOTE_G, LED_CH_G },
  { LED_NOTE_B, LED_CH_B },
};

static const StripNoteMap STRIP_NOTE_MAP[] = {
  { STRIP1_NOTE_R, 0, LED_CH_R },
  { STRIP1_NOTE_G, 0, LED_CH_G },
  { STRIP1_NOTE_B, 0, LED_CH_B },
  { STRIP2_NOTE_R, 1, LED_CH_R },
  { STRIP2_NOTE_G, 1, LED_CH_G },
  { STRIP2_NOTE_B, 1, LED_CH_B },
  { STRIP3_NOTE_R, 2, LED_CH_R },
  { STRIP3_NOTE_G, 2, LED_CH_G },
  { STRIP3_NOTE_B, 2, LED_CH_B },
  { STRIP4_NOTE_R, 3, LED_CH_R },
  { STRIP4_NOTE_G, 3, LED_CH_G },
  { STRIP4_NOTE_B, 3, LED_CH_B },
};

static const uint8_t BUILTIN_NOTE_MAP_COUNT =
  (uint8_t)(sizeof(BUILTIN_NOTE_MAP) / sizeof(BUILTIN_NOTE_MAP[0]));
static const uint8_t STRIP_NOTE_MAP_COUNT =
  (uint8_t)(sizeof(STRIP_NOTE_MAP) / sizeof(STRIP_NOTE_MAP[0]));

static uint8_t velocityToLevel(uint8_t velocity) {
  if (velocity == 0) {
    return 0;
  }
  return (uint8_t)(((uint16_t)velocity * 255U) / 127U);
}

static uint8_t scaleBrightness(uint8_t level) {
  return (uint8_t)(((uint16_t)level * NEOPIXEL_BRIGHTNESS) / 255U);
}

static void applyBuiltin() {
  rgbLedWrite(RGB_BUILTIN, scaleBrightness(builtinLevels[LED_CH_R]), scaleBrightness(builtinLevels[LED_CH_G]),
              scaleBrightness(builtinLevels[LED_CH_B]));
}

static void applyStripPixel(uint8_t pixelIndex) {
  if (pixelIndex >= STRIP_LED_COUNT) {
    return;
  }
  strip.setPixelColor(pixelIndex, scaleBrightness(stripLevels[pixelIndex][LED_CH_R]),
                      scaleBrightness(stripLevels[pixelIndex][LED_CH_G]),
                      scaleBrightness(stripLevels[pixelIndex][LED_CH_B]));
}

static bool setBuiltinLevel(uint8_t note, uint8_t level) {
  bool found = false;
  for (uint8_t i = 0; i < BUILTIN_NOTE_MAP_COUNT; i++) {
    if (BUILTIN_NOTE_MAP[i].note != note) {
      continue;
    }
    const uint8_t ch = (uint8_t)BUILTIN_NOTE_MAP[i].channel;
    if (builtinLevels[ch] != level) {
      builtinLevels[ch] = level;
      builtinDirty = true;
    }
    found = true;
  }
  return found;
}

static bool setStripLevel(uint8_t note, uint8_t level) {
  if (!STRIP_MIDI_ENABLE) {
    return false;
  }
  bool found = false;
  for (uint8_t i = 0; i < STRIP_NOTE_MAP_COUNT; i++) {
    if (STRIP_NOTE_MAP[i].note != note) {
      continue;
    }
    const uint8_t px = STRIP_NOTE_MAP[i].pixelIndex;
    const uint8_t ch = (uint8_t)STRIP_NOTE_MAP[i].channel;
    if (px >= STRIP_LED_COUNT || ch > LED_CH_B) {
      continue;
    }
    if (stripLevels[px][ch] != level) {
      stripLevels[px][ch] = level;
      applyStripPixel(px);
      stripDirty = true;
    }
    found = true;
  }
  return found;
}

static bool setChannelLevel(uint8_t note, uint8_t level) {
  const bool builtin = setBuiltinLevel(note, level);
  const bool strip = setStripLevel(note, level);
  return builtin || strip;
}

static void ledsClearAll() {
  bool anyBuiltin = false;
  for (uint8_t i = 0; i < 3; i++) {
    if (builtinLevels[i] != 0) {
      anyBuiltin = true;
    }
    builtinLevels[i] = 0;
  }
  if (anyBuiltin) {
    builtinDirty = true;
  }

  if (STRIP_MIDI_ENABLE) {
    bool anyStrip = false;
    for (uint8_t px = 0; px < STRIP_LED_COUNT; px++) {
      for (uint8_t ch = 0; ch < 3; ch++) {
        if (stripLevels[px][ch] != 0) {
          anyStrip = true;
        }
        stripLevels[px][ch] = 0;
      }
      applyStripPixel(px);
    }
    if (anyStrip) {
      stripDirty = true;
    }
  }
}

void ledsBegin() {
  for (uint8_t k = 0; k <= 8; k++) {
    rgbLedWrite(RGB_BUILTIN, ((k & 4) != 0) ? LED_INIT_BRIGHT : 0, ((k & 1) != 0) ? LED_INIT_BRIGHT : 0,
                ((k & 2) != 0) ? LED_INIT_BRIGHT : 0);
    delay(LED_INIT_SPEED);
  }

  builtinLevels[0] = 0;
  builtinLevels[1] = 0;
  builtinLevels[2] = 0;
  applyBuiltin();
  builtinDirty = false;

  if (STRIP_MIDI_ENABLE) {
    strip.begin();
    strip.setBrightness(NEOPIXEL_BRIGHTNESS);
    strip.clear();

    for (uint8_t k = 0; k <= 8; k++) {
      for (uint8_t px = 0; px < STRIP_LED_COUNT; px++) {
        strip.setPixelColor(px, ((k & 4) != 0) ? LED_INIT_BRIGHT : 0, ((k & 1) != 0) ? LED_INIT_BRIGHT : 0,
                            ((k & 2) != 0) ? LED_INIT_BRIGHT : 0);
        strip.show();
        delay(LED_INIT_SPEED);
      }
    }

    for (uint8_t px = 0; px < STRIP_LED_COUNT; px++) {
      stripLevels[px][0] = 0;
      stripLevels[px][1] = 0;
      stripLevels[px][2] = 0;
    }
    strip.clear();
    strip.show();
    stripDirty = false;
  }

  lastMidiActivityMs = millis();
  idleTimeoutFired = false;
}

void ledsNotifyMidiActivity() {
  lastMidiActivityMs = millis();
  idleTimeoutFired = false;
}

void ledsHandleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
  if (!LED_MIDI_ENABLE) {
    return;
  }
  if (channel != LED_MIDI_CHANNEL) {
    return;
  }
  if (velocity == 0) {
    ledsHandleNoteOff(channel, note, 0);
    return;
  }
  setChannelLevel(note, velocityToLevel(velocity));
}

void ledsHandleNoteOff(uint8_t channel, uint8_t note, uint8_t /*velocity*/) {
  if (!LED_MIDI_ENABLE) {
    return;
  }
  if (channel != LED_MIDI_CHANNEL) {
    return;
  }
  setChannelLevel(note, 0);
}

void ledsUpdate() {
  if (LED_MIDI_ENABLE && LED_IDLE_TIMEOUT_SEC > 0 && !idleTimeoutFired) {
    const unsigned long timeoutMs = (unsigned long)LED_IDLE_TIMEOUT_SEC * 1000UL;
    if ((millis() - lastMidiActivityMs) >= timeoutMs) {
      ledsClearAll();
      idleTimeoutFired = true;
    }
  }

  if (builtinDirty) {
    applyBuiltin();
    builtinDirty = false;
  }

  if (stripDirty) {
    strip.show();
    stripDirty = false;
  }
}
