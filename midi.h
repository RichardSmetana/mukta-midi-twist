#ifndef MIDICTRL_MIDI_H
#define MIDICTRL_MIDI_H

#include <Arduino.h>
#include "types.h"

void midiBegin();
void midiPollUploadReset();
void midiControlChange(uint8_t channel, uint8_t cc, uint8_t value);
void midiNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
void midiNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
void midiSendNoteAction(NoteAction action, uint8_t channel, uint8_t note,
                        uint8_t onVelocity, uint8_t offVelocity);
void midiFlushOutput();

#if defined(CONFIG_IDF_TARGET_ESP32S3) && !ARDUINO_USB_MODE
#include "USBMIDI.h"
extern USBMIDI usbMidi;
bool midiReadPacket(midiEventPacket_t *packet);
#endif

#endif
