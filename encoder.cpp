#include <Arduino.h>
#include "config.h"
#include "encoder.h"
#include "midi.h"

// Valid quadrature transitions only; invalid/bounce -> 0
static const int8_t ENC_TABLE[16] = {
    0, -1, 1, 0,
    1, 0, 0, -1,
    -1, 0, 0, 1,
    0, 1, -1, 0};

struct EncoderConfig {
  uint8_t pinClk;
  uint8_t pinDt;
  bool invertDirection;
  bool absEnable;
  uint8_t absChannel;
  uint8_t absCc;
  uint8_t absStart;
  uint8_t absMin;
  uint8_t absMax;
  bool relEnable;
  uint8_t relChannel;
  uint8_t relCc;
  uint8_t relPlus;
  uint8_t relMinus;
};

struct EncoderRuntime {
  uint8_t grayPrev;
  int8_t stepAccum;
  uint8_t absValue;
  uint8_t lastSentAbs;
  int16_t relStepsPending;
};

static const EncoderConfig ENC_CFG[ENCODER_COUNT] = {
    {
        ENC0_PIN_CLK, ENC0_PIN_DT, ENC0_INVERT_DIRECTION,
        ENC0_ABSOLUTE_ENABLE, ENC0_ABS_MIDI_CHANNEL, ENC0_ABS_CC_NUMBER,
        ENC0_ABS_START, ENC0_ABS_MIN, ENC0_ABS_MAX,
        ENC0_RELATIVE_ENABLE, ENC0_REL_MIDI_CHANNEL, ENC0_REL_CC_NUMBER,
        ENC0_REL_PLUS_VALUE, ENC0_REL_MINUS_VALUE,
    },
    {
        ENC1_PIN_CLK, ENC1_PIN_DT, ENC1_INVERT_DIRECTION,
        ENC1_ABSOLUTE_ENABLE, ENC1_ABS_MIDI_CHANNEL, ENC1_ABS_CC_NUMBER,
        ENC1_ABS_START, ENC1_ABS_MIN, ENC1_ABS_MAX,
        ENC1_RELATIVE_ENABLE, ENC1_REL_MIDI_CHANNEL, ENC1_REL_CC_NUMBER,
        ENC1_REL_PLUS_VALUE, ENC1_REL_MINUS_VALUE,
    },
    {
        ENC2_PIN_CLK, ENC2_PIN_DT, ENC2_INVERT_DIRECTION,
        ENC2_ABSOLUTE_ENABLE, ENC2_ABS_MIDI_CHANNEL, ENC2_ABS_CC_NUMBER,
        ENC2_ABS_START, ENC2_ABS_MIN, ENC2_ABS_MAX,
        ENC2_RELATIVE_ENABLE, ENC2_REL_MIDI_CHANNEL, ENC2_REL_CC_NUMBER,
        ENC2_REL_PLUS_VALUE, ENC2_REL_MINUS_VALUE,
    },
    {
        ENC3_PIN_CLK, ENC3_PIN_DT, ENC3_INVERT_DIRECTION,
        ENC3_ABSOLUTE_ENABLE, ENC3_ABS_MIDI_CHANNEL, ENC3_ABS_CC_NUMBER,
        ENC3_ABS_START, ENC3_ABS_MIN, ENC3_ABS_MAX,
        ENC3_RELATIVE_ENABLE, ENC3_REL_MIDI_CHANNEL, ENC3_REL_CC_NUMBER,
        ENC3_REL_PLUS_VALUE, ENC3_REL_MINUS_VALUE,
    },
};

static EncoderRuntime encRt[ENCODER_COUNT];

static void encoderOnDetent(uint8_t index, int8_t direction) {
  const EncoderConfig &cfg = ENC_CFG[index];
  EncoderRuntime &rt = encRt[index];

  if (direction > 0) {
    if (cfg.absEnable && rt.absValue < cfg.absMax) {
      rt.absValue++;
    }
    if (cfg.relEnable) {
      rt.relStepsPending++;
    }
  } else {
    if (cfg.absEnable && rt.absValue > cfg.absMin) {
      rt.absValue--;
    }
    if (cfg.relEnable) {
      rt.relStepsPending--;
    }
  }
}

static void encoderPoll(uint8_t index) {
  const EncoderConfig &cfg = ENC_CFG[index];
  EncoderRuntime &rt = encRt[index];

  const uint8_t clk = (uint8_t)digitalRead(cfg.pinClk);
  const uint8_t dt = (uint8_t)digitalRead(cfg.pinDt);
  const uint8_t curr = (uint8_t)((clk << 1) | dt);

  if (curr == rt.grayPrev) {
    return;
  }

  const uint8_t tableIdx = (uint8_t)((rt.grayPrev << 2) | curr);
  int8_t step = ENC_TABLE[tableIdx & 0x0F];
  rt.grayPrev = curr;

  if (step == 0) {
    return;
  }

  if (cfg.invertDirection) {
    step = (int8_t)(-step);
  }

  rt.stepAccum = (int8_t)(rt.stepAccum + step);

  if (rt.stepAccum >= ENC_STEPS_PER_DETENT) {
    rt.stepAccum = 0;
    encoderOnDetent(index, 1);
  } else if (rt.stepAccum <= -ENC_STEPS_PER_DETENT) {
    rt.stepAccum = 0;
    encoderOnDetent(index, -1);
  }
}

static void encoderSendOne(uint8_t index) {
  const EncoderConfig &cfg = ENC_CFG[index];
  EncoderRuntime &rt = encRt[index];

  if (cfg.absEnable && rt.absValue != rt.lastSentAbs) {
    rt.lastSentAbs = rt.absValue;
    midiControlChange(cfg.absChannel, cfg.absCc, rt.absValue);
  }

  if (cfg.relEnable) {
    while (rt.relStepsPending > 0) {
      midiControlChange(cfg.relChannel, cfg.relCc, cfg.relPlus);
      rt.relStepsPending--;
    }
    while (rt.relStepsPending < 0) {
      midiControlChange(cfg.relChannel, cfg.relCc, cfg.relMinus);
      rt.relStepsPending++;
    }
  }
}

void encoderBegin() {
  for (uint8_t i = 0; i < ENCODER_COUNT; i++) {
    const EncoderConfig &cfg = ENC_CFG[i];
    EncoderRuntime &rt = encRt[i];

    pinMode(cfg.pinClk, INPUT_PULLUP);
    pinMode(cfg.pinDt, INPUT_PULLUP);

    const uint8_t clk = (uint8_t)digitalRead(cfg.pinClk);
    const uint8_t dt = (uint8_t)digitalRead(cfg.pinDt);
    rt.grayPrev = (uint8_t)((clk << 1) | dt);
    rt.stepAccum = 0;
    rt.absValue = cfg.absStart;
    rt.lastSentAbs = cfg.absStart;
    rt.relStepsPending = 0;
  }
}

void encoderUpdate() {
  for (uint8_t i = 0; i < ENCODER_COUNT; i++) {
    encoderPoll(i);
  }
}

void encoderSendMidi() {
  for (uint8_t i = 0; i < ENCODER_COUNT; i++) {
    encoderSendOne(i);
  }
}
