# sonic-pi-01-led-test.rb
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Richard Smetana
# =============================================================================
# Mukta MIDI Twist — LED visual test
# =============================================================================
#
# Sends MIDI notes to onboard RGB (GPIO 48) and WS2812 strip (GPIO 7).
# Note map: see ../config.h (onboard 45–47, strip 48–59).
#
# Prerequisites:
#   - Firmware running on Mukta MIDI Twist
#   - Sonic Pi → Preferences → I/O → enable device as MIDI output
#   - Set MIDI_PORT below to match puts midi_available_ports
#
# Behaviour:
#   - 60 BPM, live_loop :test1
#   - :index alternates 0/1 → base note 45 or 46 (onboard R/G)
#   - Each cycle: 4 strip steps, cymbal + MIDI flash per pixel, 0.25 s on/off
# =============================================================================

MIDI_PORT = "mukta_midi_twist__0_"

use_bpm 60

set :flag, false
set :index, 0

live_loop :test1 do
  index = get(:index)
  note1 = 45 + index

  4.times do |i|
    sample :drum_cymbal_closed, amp: (i == 0 ? 1.0 : 0.5)

    5.times do |j|
      midi_note_on note1 + j * 3, vel_f: 0.2, port: MIDI_PORT
    end
    midi_note_on 48 + i * 3, vel_f: 0.6, port: MIDI_PORT
    midi_note_on 49 + i * 3, vel_f: 0.6, port: MIDI_PORT
    midi_note_on 50 + i * 3, vel_f: 0.6, port: MIDI_PORT

    sleep 0.25

    5.times do |j|
      midi_note_off note1 + j * 3, port: MIDI_PORT
    end
    midi_note_off 48 + i * 3, port: MIDI_PORT
    midi_note_off 49 + i * 3, port: MIDI_PORT
    midi_note_off 50 + i * 3, port: MIDI_PORT

    sleep 0.25
  end

  set :index, get(:index) + 1
  set :index, 0 if get(:index) >= 2
end
