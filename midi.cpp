// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Richard Smetana

#include "midi.h"
#include "config.h"

#if defined(CONFIG_IDF_TARGET_ESP32S3) && !ARDUINO_USB_MODE
#include "USB.h"
#include "USBMIDI.h"
#include "esp32-hal-tinyusb.h"

static void midiInitUsbStack() {
  static bool started = false;
  if (started) {
    return;
  }
  started = true;

  // Same init as the working esp32s3-test sketch — one USB.begin() in setup().
  // Requires USB CDC On Boot: Disabled (app_main must not start USB early).
  usbMidi.begin();
  USB.begin();
}
#endif

static uint8_t midiStatusChannel(uint8_t status, uint8_t channel) {
  if (channel < 1) {
    channel = 1;
  } else if (channel > 16) {
    channel = 16;
  }
  return (uint8_t)(status | (channel - 1));
}

void midiBegin() {
#if defined(CONFIG_IDF_TARGET_ESP32S3) && !ARDUINO_USB_MODE
  midiInitUsbStack();
#endif
}

void midiPollUploadReset() {
#if defined(CONFIG_IDF_TARGET_ESP32S3) && !ARDUINO_USB_MODE
  static unsigned long holdStartMs = 0;
  static bool triggered = false;

  if (digitalRead(0) == LOW) {
    if (holdStartMs == 0) {
      holdStartMs = millis();
    } else if (!triggered && (millis() - holdStartMs) >= 2000) {
      triggered = true;
      usb_persist_restart(RESTART_BOOTLOADER);
    }
  } else {
    holdStartMs = 0;
    triggered = false;
  }
#endif
}

void midiControlChange(uint8_t channel, uint8_t cc, uint8_t value) {
#if defined(CONFIG_IDF_TARGET_ESP32S3) && !ARDUINO_USB_MODE
  usbMidi.controlChange(cc & 0x7F, value & 0x7F, channel);
#else
  Serial.write(midiStatusChannel(0xB0, channel));
  Serial.write(cc & 0x7F);
  Serial.write(value & 0x7F);
#endif
}

void midiNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
#if defined(CONFIG_IDF_TARGET_ESP32S3) && !ARDUINO_USB_MODE
  usbMidi.noteOn(note & 0x7F, velocity & 0x7F, channel);
#else
  Serial.write(midiStatusChannel(0x90, channel));
  Serial.write(note & 0x7F);
  Serial.write(velocity & 0x7F);
#endif
}

void midiNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
#if defined(CONFIG_IDF_TARGET_ESP32S3) && !ARDUINO_USB_MODE
  usbMidi.noteOff(note & 0x7F, velocity & 0x7F, channel);
#else
  Serial.write(midiStatusChannel(0x80, channel));
  Serial.write(note & 0x7F);
  Serial.write(velocity & 0x7F);
#endif
}

void midiSendNoteAction(NoteAction action, uint8_t channel, uint8_t note,
                       uint8_t onVelocity, uint8_t offVelocity) {
  if (action == NOTE_ON) {
    midiNoteOn(channel, note, onVelocity);
  } else if (action == NOTE_OFF) {
    midiNoteOff(channel, note, offVelocity);
  }
}

void midiFlushOutput() {
#if defined(CONFIG_IDF_TARGET_ESP32S3) && !ARDUINO_USB_MODE
#else
  Serial.flush();
#endif
}

#if defined(CONFIG_IDF_TARGET_ESP32S3) && !ARDUINO_USB_MODE
bool midiReadPacket(midiEventPacket_t *packet) {
  return usbMidi.readPacket(packet);
}
#endif
