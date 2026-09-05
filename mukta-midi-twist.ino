/*
 * Mukta MIDI Twist — Arduino KY-040 MIDI controller firmware
 * Version: 0.2.0-dev (see VERSION / CHANGELOG.md)
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Richard Smetana
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Edit config.h for GPIOs and MIDI mapping.
 *
 * ESP32-S3 (Arduino IDE → Tools):
 *   USB Mode:        USB-OTG (TinyUSB)
 *   USB CDC On Boot: Disabled   ← required for MIDI on Windows / Sonic Pi
 *   Upload Mode:     USB-OTG CDC (TinyUSB)
 * GPIO (config.h): Enc0 42/41/40 · Enc1 39/38/37 · Enc2 4/5/6 · Enc3 15/16/17 · Strip 7
 * Upload: BOOT 2 s → USB JTAG port. Details: README.md
 */

#include "config.h"
#include "encoder.h"
#include "button.h"
#include "midi.h"
#include "midi_in.h"
#include "leds.h"

#if defined(CONFIG_IDF_TARGET_ESP32S3) && ARDUINO_USB_MODE
#error Select Tools -> USB Mode -> USB-OTG (TinyUSB) for ESP32-S3 native USB MIDI
#endif

#if defined(CONFIG_IDF_TARGET_ESP32S3) && !ARDUINO_USB_MODE
#include "USB.h"
#include "USBMIDI.h"

#if ARDUINO_USB_CDC_ON_BOOT
#warning USB CDC On Boot must be Disabled - with Enabled, Windows shows only a COM port and no MIDI (Espressif/TinyUSB composite limitation).
#endif

USBMIDI usbMidi("Mukta MIDI Twist");
#endif

void setup() {
  pinMode(0, INPUT_PULLUP);  // BOOT — long press triggers upload bootloader
  midiBegin();

  delay(100);

  ledsBegin();
  midiInBegin();
  encoderBegin();
  buttonBegin();
}

void loop() {
  midiPollUploadReset();
  midiInProcess();
  ledsUpdate();

  encoderUpdate();
  encoderSendMidi();
  buttonProcessMidi();
  midiFlushOutput();
}
