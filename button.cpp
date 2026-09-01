#include <Arduino.h>
#include "config.h"
#include "button.h"
#include "midi.h"

struct ButtonConfig {
  uint8_t pinSw;
  bool momEnable;
  bool latchEnable;
  uint8_t momChannel;
  uint8_t momNote;
  uint8_t momOnVel;
  uint8_t momOffVel;
  NoteAction momPress;
  NoteAction momRelease;
  uint8_t latchChannel;
  uint8_t latchNote;
  uint8_t latchOnVel;
  uint8_t latchOffVel;
};

struct ButtonState {
  bool latchNoteIsOn;
  byte swStable;
  byte lastReading;
  unsigned long lastChangeMs;
};

static const ButtonConfig BTN_CFG[BUTTON_COUNT] = {
    {
        BTN0_PIN_SW, BTN0_MOMENTARY_ENABLE, BTN0_LATCH_ENABLE,
        BTN0_MOM_MIDI_CHANNEL, BTN0_MOM_NOTE, BTN0_MOM_NOTE_ON_VELOCITY,
        BTN0_MOM_NOTE_OFF_VELOCITY, BTN0_MOM_PRESS_ACTION, BTN0_MOM_RELEASE_ACTION,
        BTN0_LATCH_MIDI_CHANNEL, BTN0_LATCH_NOTE, BTN0_LATCH_NOTE_ON_VELOCITY,
        BTN0_LATCH_NOTE_OFF_VELOCITY,
    },
    {
        BTN1_PIN_SW, BTN1_MOMENTARY_ENABLE, BTN1_LATCH_ENABLE,
        BTN1_MOM_MIDI_CHANNEL, BTN1_MOM_NOTE, BTN1_MOM_NOTE_ON_VELOCITY,
        BTN1_MOM_NOTE_OFF_VELOCITY, BTN1_MOM_PRESS_ACTION, BTN1_MOM_RELEASE_ACTION,
        BTN1_LATCH_MIDI_CHANNEL, BTN1_LATCH_NOTE, BTN1_LATCH_NOTE_ON_VELOCITY,
        BTN1_LATCH_NOTE_OFF_VELOCITY,
    },
    {
        BTN2_PIN_SW, BTN2_MOMENTARY_ENABLE, BTN2_LATCH_ENABLE,
        BTN2_MOM_MIDI_CHANNEL, BTN2_MOM_NOTE, BTN2_MOM_NOTE_ON_VELOCITY,
        BTN2_MOM_NOTE_OFF_VELOCITY, BTN2_MOM_PRESS_ACTION, BTN2_MOM_RELEASE_ACTION,
        BTN2_LATCH_MIDI_CHANNEL, BTN2_LATCH_NOTE, BTN2_LATCH_NOTE_ON_VELOCITY,
        BTN2_LATCH_NOTE_OFF_VELOCITY,
    },
    {
        BTN3_PIN_SW, BTN3_MOMENTARY_ENABLE, BTN3_LATCH_ENABLE,
        BTN3_MOM_MIDI_CHANNEL, BTN3_MOM_NOTE, BTN3_MOM_NOTE_ON_VELOCITY,
        BTN3_MOM_NOTE_OFF_VELOCITY, BTN3_MOM_PRESS_ACTION, BTN3_MOM_RELEASE_ACTION,
        BTN3_LATCH_MIDI_CHANNEL, BTN3_LATCH_NOTE, BTN3_LATCH_NOTE_ON_VELOCITY,
        BTN3_LATCH_NOTE_OFF_VELOCITY,
    },
};

static ButtonState btnState[BUTTON_COUNT];

static void handleButtonEdge(uint8_t index, byte state) {
  const ButtonConfig &cfg = BTN_CFG[index];
  ButtonState &st = btnState[index];
  const bool pressed = (state == LOW);

  if (cfg.momEnable) {
    const NoteAction action = pressed ? cfg.momPress : cfg.momRelease;
    midiSendNoteAction(action, cfg.momChannel, cfg.momNote, cfg.momOnVel, cfg.momOffVel);
  }

  if (cfg.latchEnable && pressed) {
    st.latchNoteIsOn = !st.latchNoteIsOn;
    midiSendNoteAction(st.latchNoteIsOn ? NOTE_ON : NOTE_OFF, cfg.latchChannel, cfg.latchNote,
                       cfg.latchOnVel, cfg.latchOffVel);
  }
}

void buttonBegin() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    const ButtonConfig &cfg = BTN_CFG[i];
    ButtonState &st = btnState[i];

    pinMode(cfg.pinSw, INPUT_PULLUP);

    st.swStable = digitalRead(cfg.pinSw);
    st.lastReading = st.swStable;
    st.lastChangeMs = millis();
    st.latchNoteIsOn = false;
  }
}

void buttonProcessMidi() {
  const unsigned long now = millis();

  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    const ButtonConfig &cfg = BTN_CFG[i];
    ButtonState &st = btnState[i];
    const byte reading = digitalRead(cfg.pinSw);

    if (reading != st.lastReading) {
      st.lastReading = reading;
      st.lastChangeMs = now;
    }

    if ((now - st.lastChangeMs) < SW_STABLE_MS) {
      continue;
    }

    if (reading == st.swStable) {
      continue;
    }

    st.swStable = reading;
    handleButtonEdge(i, reading);
  }
}
