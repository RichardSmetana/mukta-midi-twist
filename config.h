#ifndef MIDICTRL_CONFIG_H
#define MIDICTRL_CONFIG_H

#include "types.h"

// -----------------------------------------------------------------------------
// Board / USB (ESP32-S3)
// Arduino IDE → Tools:
//   Board:           ESP32S3 Dev Module
//   USB Mode:        USB-OTG (TinyUSB)
//   USB CDC On Boot: Disabled  (pure USB MIDI — works on Windows / Sonic Pi)
//   Upload Mode:     USB-OTG CDC (TinyUSB)
// Upload: hold BOOT (GPIO 0) 2 s, then Upload in the IDE. Unplug/replug USB after.
// Auto-start after upload: copy platform.local.txt into the esp32 package (see README).
// Sonic Pi: "Mukta MIDI Twist" (Sound/audio devices, not COM port).
// -----------------------------------------------------------------------------
const unsigned long SERIAL_BAUD = 115200;

const uint8_t ENCODER_COUNT = 4;
const uint8_t BUTTON_COUNT = 4;

// -----------------------------------------------------------------------------
// GPIO — ESP32-S3 YD-ESP32-S3 (each pin unique; do not use 0, 19, 20, 45, 46, 48)
// Enc0–1: header J2 (SPI side)   Enc2–3: header J3   Strip: GPIO 7
// -----------------------------------------------------------------------------
// Encoder 0
const uint8_t ENC0_PIN_CLK = 42;
const uint8_t ENC0_PIN_DT = 41;
const uint8_t BTN0_PIN_SW = 40;

// Encoder 1
const uint8_t ENC1_PIN_CLK = 39;
const uint8_t ENC1_PIN_DT = 38;
const uint8_t BTN1_PIN_SW = 37;

// Encoder 2
const uint8_t ENC2_PIN_CLK = 4;
const uint8_t ENC2_PIN_DT = 5;
const uint8_t BTN2_PIN_SW = 6;

// Encoder 3
const uint8_t ENC3_PIN_CLK = 15;
const uint8_t ENC3_PIN_DT = 16;
const uint8_t BTN3_PIN_SW = 17;

// External WS2812 strip data pin
const uint8_t STRIP_PIN = 7;

// Compile-time uniqueness check for all GPIOs
constexpr bool midictrlPinsUnique() {
  constexpr uint8_t pins[] = {
      ENC0_PIN_CLK, ENC0_PIN_DT, BTN0_PIN_SW, ENC1_PIN_CLK, ENC1_PIN_DT, BTN1_PIN_SW,
      ENC2_PIN_CLK, ENC2_PIN_DT, BTN2_PIN_SW, ENC3_PIN_CLK, ENC3_PIN_DT, BTN3_PIN_SW, STRIP_PIN};
  constexpr unsigned n = sizeof(pins) / sizeof(pins[0]);
  for (unsigned i = 0; i < n; i++) {
    for (unsigned j = i + 1; j < n; j++) {
      if (pins[i] == pins[j]) {
        return false;
      }
    }
  }
  return true;
}
static_assert(midictrlPinsUnique(),
              "GPIO pin conflict in config.h — each CLK, DT, SW and STRIP_PIN must be unique");

// -----------------------------------------------------------------------------
// Onboard RGB LED (ESP32-S3 native rgbLedWrite / RGB_BUILTIN, GPIO 48)
// YD-ESP32-S3: solder bridge "RGB" must be closed.
// -----------------------------------------------------------------------------
const bool LED_MIDI_ENABLE = true;
const uint8_t NEOPIXEL_BRIGHTNESS = 255;  // brightness ceiling for onboard + strip (0–255)
const uint8_t LED_MIDI_CHANNEL = 1;       // 1–16; only this channel drives LEDs
const uint8_t LED_INIT_SPEED = 50;        // hello sequence speed (ms)
const uint8_t LED_INIT_BRIGHT = 31;       // hello sequence brightness

// Seconds without any incoming MIDI → all LEDs off. 0 = timeout disabled.
const uint16_t LED_IDLE_TIMEOUT_SEC = 5;

// Onboard LED — MIDI notes for R / G / B (starting at 45)
const uint8_t LED_NOTE_R = 45;
const uint8_t LED_NOTE_G = 46;
const uint8_t LED_NOTE_B = 47;

// -----------------------------------------------------------------------------
// External WS2812 strip (4 LEDs, GPIO 7) — Adafruit NeoPixel
// Layout: pixel 1–4 × RGB  →  12 MIDI notes
// -----------------------------------------------------------------------------
const bool STRIP_MIDI_ENABLE = true;
const uint8_t STRIP_LED_COUNT = 4;

const uint8_t STRIP1_NOTE_R = 48;
const uint8_t STRIP1_NOTE_G = 49;
const uint8_t STRIP1_NOTE_B = 50;

const uint8_t STRIP2_NOTE_R = 51;
const uint8_t STRIP2_NOTE_G = 52;
const uint8_t STRIP2_NOTE_B = 53;

const uint8_t STRIP3_NOTE_R = 54;
const uint8_t STRIP3_NOTE_G = 55;
const uint8_t STRIP3_NOTE_B = 56;

const uint8_t STRIP4_NOTE_R = 57;
const uint8_t STRIP4_NOTE_G = 58;
const uint8_t STRIP4_NOTE_B = 59;

// -----------------------------------------------------------------------------
// Shared encoder mechanics
// -----------------------------------------------------------------------------
const int8_t ENC_STEPS_PER_DETENT = 4;
const unsigned int ENC_BOUNCE_US = 300;

// -----------------------------------------------------------------------------
// Encoder 0 — behavior & MIDI
// -----------------------------------------------------------------------------
const bool ENC0_INVERT_DIRECTION = false;

const bool ENC0_ABSOLUTE_ENABLE = true;
const uint8_t ENC0_ABS_MIDI_CHANNEL = 1;
const uint8_t ENC0_ABS_CC_NUMBER = 16;
const uint8_t ENC0_ABS_START = 64;
const uint8_t ENC0_ABS_MIN = 0;
const uint8_t ENC0_ABS_MAX = 127;

const bool ENC0_RELATIVE_ENABLE = true;
const uint8_t ENC0_REL_MIDI_CHANNEL = 1;
const uint8_t ENC0_REL_CC_NUMBER = 17;
const uint8_t ENC0_REL_PLUS_VALUE = 1;
const uint8_t ENC0_REL_MINUS_VALUE = 127;

// -----------------------------------------------------------------------------
// Encoder 1 — behavior & MIDI
// -----------------------------------------------------------------------------
const bool ENC1_INVERT_DIRECTION = false;

const bool ENC1_ABSOLUTE_ENABLE = true;
const uint8_t ENC1_ABS_MIDI_CHANNEL = 1;
const uint8_t ENC1_ABS_CC_NUMBER = 18;
const uint8_t ENC1_ABS_START = 64;
const uint8_t ENC1_ABS_MIN = 0;
const uint8_t ENC1_ABS_MAX = 127;

const bool ENC1_RELATIVE_ENABLE = true;
const uint8_t ENC1_REL_MIDI_CHANNEL = 1;
const uint8_t ENC1_REL_CC_NUMBER = 19;
const uint8_t ENC1_REL_PLUS_VALUE = 1;
const uint8_t ENC1_REL_MINUS_VALUE = 127;

// -----------------------------------------------------------------------------
// Encoder 2 — behavior & MIDI
// -----------------------------------------------------------------------------
const bool ENC2_INVERT_DIRECTION = false;

const bool ENC2_ABSOLUTE_ENABLE = true;
const uint8_t ENC2_ABS_MIDI_CHANNEL = 1;
const uint8_t ENC2_ABS_CC_NUMBER = 20;
const uint8_t ENC2_ABS_START = 64;
const uint8_t ENC2_ABS_MIN = 0;
const uint8_t ENC2_ABS_MAX = 127;

const bool ENC2_RELATIVE_ENABLE = true;
const uint8_t ENC2_REL_MIDI_CHANNEL = 1;
const uint8_t ENC2_REL_CC_NUMBER = 21;
const uint8_t ENC2_REL_PLUS_VALUE = 1;
const uint8_t ENC2_REL_MINUS_VALUE = 127;

// -----------------------------------------------------------------------------
// Encoder 3 — behavior & MIDI
// -----------------------------------------------------------------------------
const bool ENC3_INVERT_DIRECTION = false;

const bool ENC3_ABSOLUTE_ENABLE = true;
const uint8_t ENC3_ABS_MIDI_CHANNEL = 1;
const uint8_t ENC3_ABS_CC_NUMBER = 22;
const uint8_t ENC3_ABS_START = 64;
const uint8_t ENC3_ABS_MIN = 0;
const uint8_t ENC3_ABS_MAX = 127;

const bool ENC3_RELATIVE_ENABLE = true;
const uint8_t ENC3_REL_MIDI_CHANNEL = 1;
const uint8_t ENC3_REL_CC_NUMBER = 23;
const uint8_t ENC3_REL_PLUS_VALUE = 1;
const uint8_t ENC3_REL_MINUS_VALUE = 127;

// -----------------------------------------------------------------------------
// Button 0 — behavior & MIDI (momentary and latch can both be true)
// -----------------------------------------------------------------------------
const bool BTN0_MOMENTARY_ENABLE = true;
const bool BTN0_LATCH_ENABLE = true;

const uint8_t BTN0_MOM_MIDI_CHANNEL = 1;
const uint8_t BTN0_MOM_NOTE = 60;
const uint8_t BTN0_MOM_NOTE_ON_VELOCITY = 100;
const uint8_t BTN0_MOM_NOTE_OFF_VELOCITY = 0;
const NoteAction BTN0_MOM_PRESS_ACTION = NOTE_ON;
const NoteAction BTN0_MOM_RELEASE_ACTION = NOTE_OFF;

const uint8_t BTN0_LATCH_MIDI_CHANNEL = 1;
const uint8_t BTN0_LATCH_NOTE = 68;
const uint8_t BTN0_LATCH_NOTE_ON_VELOCITY = 100;
const uint8_t BTN0_LATCH_NOTE_OFF_VELOCITY = 0;

// -----------------------------------------------------------------------------
// Button 1 — behavior & MIDI
// -----------------------------------------------------------------------------
const bool BTN1_MOMENTARY_ENABLE = true;
const bool BTN1_LATCH_ENABLE = true;

const uint8_t BTN1_MOM_MIDI_CHANNEL = 1;
const uint8_t BTN1_MOM_NOTE = 61;
const uint8_t BTN1_MOM_NOTE_ON_VELOCITY = 100;
const uint8_t BTN1_MOM_NOTE_OFF_VELOCITY = 0;
const NoteAction BTN1_MOM_PRESS_ACTION = NOTE_ON;
const NoteAction BTN1_MOM_RELEASE_ACTION = NOTE_OFF;

const uint8_t BTN1_LATCH_MIDI_CHANNEL = 1;
const uint8_t BTN1_LATCH_NOTE = 69;
const uint8_t BTN1_LATCH_NOTE_ON_VELOCITY = 100;
const uint8_t BTN1_LATCH_NOTE_OFF_VELOCITY = 0;

// -----------------------------------------------------------------------------
// Button 2 — behavior & MIDI
// -----------------------------------------------------------------------------
const bool BTN2_MOMENTARY_ENABLE = true;
const bool BTN2_LATCH_ENABLE = true;

const uint8_t BTN2_MOM_MIDI_CHANNEL = 1;
const uint8_t BTN2_MOM_NOTE = 62;
const uint8_t BTN2_MOM_NOTE_ON_VELOCITY = 100;
const uint8_t BTN2_MOM_NOTE_OFF_VELOCITY = 0;
const NoteAction BTN2_MOM_PRESS_ACTION = NOTE_ON;
const NoteAction BTN2_MOM_RELEASE_ACTION = NOTE_OFF;

const uint8_t BTN2_LATCH_MIDI_CHANNEL = 1;
const uint8_t BTN2_LATCH_NOTE = 70;
const uint8_t BTN2_LATCH_NOTE_ON_VELOCITY = 100;
const uint8_t BTN2_LATCH_NOTE_OFF_VELOCITY = 0;

// -----------------------------------------------------------------------------
// Button 3 — behavior & MIDI
// -----------------------------------------------------------------------------
const bool BTN3_MOMENTARY_ENABLE = true;
const bool BTN3_LATCH_ENABLE = true;

const uint8_t BTN3_MOM_MIDI_CHANNEL = 1;
const uint8_t BTN3_MOM_NOTE = 63;
const uint8_t BTN3_MOM_NOTE_ON_VELOCITY = 100;
const uint8_t BTN3_MOM_NOTE_OFF_VELOCITY = 0;
const NoteAction BTN3_MOM_PRESS_ACTION = NOTE_ON;
const NoteAction BTN3_MOM_RELEASE_ACTION = NOTE_OFF;

const uint8_t BTN3_LATCH_MIDI_CHANNEL = 1;
const uint8_t BTN3_LATCH_NOTE = 71;
const uint8_t BTN3_LATCH_NOTE_ON_VELOCITY = 100;
const uint8_t BTN3_LATCH_NOTE_OFF_VELOCITY = 0;

// -----------------------------------------------------------------------------
// Button debounce (milliseconds, polled in loop)
// -----------------------------------------------------------------------------
const uint8_t SW_STABLE_MS = 25;

#endif
