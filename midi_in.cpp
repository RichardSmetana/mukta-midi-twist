// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Richard Smetana

#include <Arduino.h>
#include "midi_in.h"
#include "midi.h"
#include "leds.h"

#if defined(CONFIG_IDF_TARGET_ESP32S3) && !ARDUINO_USB_MODE

static void midiDispatchMessage(uint8_t status, uint8_t data1, uint8_t data2) {
  const uint8_t cmd = (uint8_t)(status & 0xF0);
  const uint8_t channel = (uint8_t)((status & 0x0F) + 1);  // 1–16

  ledsNotifyMidiActivity();

  if (cmd == 0x90) {
    if (data2 == 0) {
      ledsHandleNoteOff(channel, data1, data2);
    } else {
      ledsHandleNoteOn(channel, data1, data2);
    }
  } else if (cmd == 0x80) {
    ledsHandleNoteOff(channel, data1, data2);
  }
}

void midiInBegin() {}

void midiInProcess() {
  midiEventPacket_t packet;
  while (midiReadPacket(&packet)) {
    midiDispatchMessage(packet.byte1, packet.byte2, packet.byte3);
  }
}

#else

// Minimal MIDI parser (Note On / Note Off + running status) for serial transport
static uint8_t midiStatus = 0;
static uint8_t midiData[2];
static uint8_t midiDataExpected = 0;
static uint8_t midiDataIndex = 0;

static uint8_t midiDataBytesForStatus(uint8_t status) {
  const uint8_t cmd = (uint8_t)(status & 0xF0);
  switch (cmd) {
    case 0x80:  // Note Off
    case 0x90:  // Note On
    case 0xA0:  // Poly Aftertouch
    case 0xB0:  // Control Change
    case 0xE0:  // Pitch Bend
      return 2;
    case 0xC0:  // Program Change
    case 0xD0:  // Channel Aftertouch
      return 1;
    default:
      return 0;
  }
}

static void midiDispatchMessage() {
  const uint8_t cmd = (uint8_t)(midiStatus & 0xF0);
  const uint8_t channel = (uint8_t)((midiStatus & 0x0F) + 1);  // 1–16

  ledsNotifyMidiActivity();

  if (cmd == 0x90) {
    ledsHandleNoteOn(channel, midiData[0], midiData[1]);
  } else if (cmd == 0x80) {
    ledsHandleNoteOff(channel, midiData[0], midiData[1]);
  }
}

static void midiHandleByte(uint8_t b) {
  if (b >= 0xF8) {
    return;
  }

  if (b >= 0xF0) {
    midiStatus = 0;
    midiDataExpected = 0;
    midiDataIndex = 0;
    return;
  }

  if (b & 0x80) {
    midiStatus = b;
    midiDataExpected = midiDataBytesForStatus(midiStatus);
    midiDataIndex = 0;
    if (midiDataExpected == 0) {
      midiStatus = 0;
    }
    return;
  }

  if (midiStatus == 0 || midiDataExpected == 0) {
    return;
  }

  midiData[midiDataIndex++] = b;
  if (midiDataIndex >= midiDataExpected) {
    midiDispatchMessage();
    midiDataIndex = 0;
  }
}

void midiInBegin() {
  midiStatus = 0;
  midiDataExpected = 0;
  midiDataIndex = 0;
}

void midiInProcess() {
  while (Serial.available() > 0) {
    midiHandleByte((uint8_t)Serial.read());
  }
}

#endif
