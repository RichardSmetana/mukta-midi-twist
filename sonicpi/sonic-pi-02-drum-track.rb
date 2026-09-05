# sonic-pi-02-drum-track.rb
# =============================================================================
# Mukta MIDI Twist — drum track with LED feedback and encoder control
# =============================================================================
#
# Plays a 32-bar electronic drum arrangement in Sonic Pi and drives the
# controller LEDs via MIDI. Also listens for encoder CC and button notes
# from the hardware (bidirectional USB MIDI).
#
# Prerequisites:
#   - Firmware running; Sonic Pi → Preferences → I/O:
#       • Mukta MIDI Twist as MIDI **output** (LED notes)
#       • Mukta MIDI Twist as MIDI **input** (encoder / button messages)
#   - Adjust :midi_in / :midi_out below if port names differ
#
# Hardware mapping (default config.h):
#   Encoder 0 relative CC 17  → BPM ±4   (live_loop :midi_1)
#   Encoder 1 relative CC 19  → state ±1 (live_loop :midi_1)
#   Button 0 note 60          → stop, reset BPM/state
#   Button 1 note 61          → jump to bar 1
#   Button 2 note 62          → state +1 (max 4)
#   Button 3 note 63          → state −1 (min 0)
#
# LED output (when state > 0):
#   Note 45 on/off each 16th  → onboard red pulse
#   Notes 47 + ring value     → strip colours (see led1 / led2 rings)
#
# Arrangement (bars):
#   0–3   Intro · 4–11 Main groove · 12–19 Tribal · 20–23 Break · 24–31 Finale
# =============================================================================

use_debug false
use_midi_logging false
set_sched_ahead_time! 1

# --- Tempo & MIDI ports -------------------------------------------------------

set :bpm, 90
set :bpm_i, get(:bpm)
set :bpm_o, get(:bpm)
set :bpm_n, get(:bpm)

set :midi_in,  "/midi:mukta*"
set :midi_out, "mukta_midi_twist__0_"

# --- Drum patterns (16 steps per bar) -----------------------------------------

kick_a  = (ring 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0)
kick_b  = (ring 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0)
snare   = (ring 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0)
ghost   = (ring 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0)
hat     = (ring 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0)
openhat = (ring 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0)
tomlo   = (ring 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0)
tommid  = (ring 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0)
tomhi   = (ring 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0)
perc    = (ring 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1)
shaker  = (ring 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1)
poly3   = (ring 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1)

# --- Melody / harmony (alternate rings commented out) -------------------------

# synth1 = (ring :a3, :c3, :f3, :d3, :a2, :c2, :f2, :d2)
# melody = (ring :a4, :c5, :e5, :c5, :e5, :c5, :b4, :a4, ...)

# synth1 = (ring :a3, :g3, :f3, :e3, :a2, :g2, :f2, :e2)
# melody = (ring :a4, :c5, :e5, :b4, :g4, :b4, :d5, :b4, ...)

# synth1 = (ring :d3, :c3, :bb2, :c3, :d2, :c2, :bb1, :c2)
# melody = (ring :d4, :a4, :f4, :a4, :c4, :g4, :e4, :g4, ...)

# synth1 = (ring :a3, :c4, :f3, :d3, :e3, :g3, :c3, :d3)
# melody = (ring :a4, :e5, :b4, :c5, :c5, :g5, :e5, :d5, ...)

synth1 = (ring :e3, :g3, :d3, :a3, :e2, :g2, :d2, :a2)
melody = (ring :e4, :g4, :b4, :g4, :b4, :a4, :g4, :e4, :d4, :g4, :b4, :d5, :b4, :a4, :g4, :d4,
               :a4, :b4, :e5, :b4, :a4, :g4, :a4, :b4, :d5, :b4, :a4, :g4, :e4, :g4, :a4, :b4,
               :e5, :b4, :g4, :e4, :g4, :b4, :d5, :b4, :a4, :d5, :b4, :a4, :e5, :d5, :b4, :g4,
               :b4, :e5, :g5, :e5, :d5, :b4, :a4, :g4, :a4, :b4, :d5, :b4, :g4, :a4, :e4, :e4)

# --- LED step maps (values → MIDI note = 47 + value) --------------------------

led1 = (ring 1, 4, 7, 10, 3, 6, 9, 12, 1, 4, 7, 10, 2, 5, 8, 11)
led2 = (ring 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 9, 12, 0, 0, 0, 0)

# --- Playback state -----------------------------------------------------------

set :led1_o, 0
set :led2_o, 0
set :state, 0
set :bar, 0
set :bar_o, 1

# --- Main song loop -----------------------------------------------------------

live_loop :song do
  bpm = get(:bpm_n)
  state = get(:state)

  bar = get(:bar) + 1
  if bar >= 32
    if state == 3
      bar = 4
    else
      if state == 4
        bar = 80
        stop
      else
        bar = 0
      end
    end
  end

  bar = 0 if bar > 0 if state == 1
  bar = 80 if state == 0

  set :bar, bar

  if bpm != get(:bpm_o)
    set :bpm_o, bpm
    set :bpm, bpm
  end
  use_bpm get(:bpm)

  intro   = bar < 4
  groove  = bar >= 4 && bar < 12
  tribal  = bar >= 12 && bar < 20
  brk     = bar >= 20 && bar < 24
  finale  = bar >= 24 && bar < 32

  if bar != get(:bar_o)
    set :bar_o, bar
    if bar == 80
      puts "bar: off"
    else
      puts "bar: #{bar}"
    end
  end

  synthnote = synth1[bar / 2]
  if groove or tribal or finale
    if bar % 2 == 0
      with_fx :reverb, room: 0.8, mix: 0.45 do
        use_synth :prophet
        play synthnote, amp: rrand(0.05, 0.2), attack: 0.5, sustain: 1.5, release: 2.5, cutoff: 70
        play synthnote + 7, amp: 0.045, attack: 0.8, sustain: 1, release: 2.8, cutoff: 60
      end
    end
  end

  if [4, 12, 24].include?(bar)
    sample :drum_splash_hard, amp: 0.7
  end

  current_kick = bar.even? ? kick_a : kick_b

  16.times do |i|
    # --- LED MIDI out ---
    if i.even?
      midi_note_on 45, vel_f: 0.2, port: get(:midi_out)
    else
      midi_note_off 45, port: get(:midi_out)
    end

    midi_note_off 47 + get(:led1_o), vel_f: 0.3, port: get(:midi_out) if get(:led1_o) != 0
    midi_note_off 47 + get(:led2_o), vel_f: 0.3, port: get(:midi_out) if get(:led2_o) != 0
    midi_note_on 47 + led1[i], vel_f: 0.2, port: get(:midi_out) if led1[i] != 0 if get(:state) > 0
    midi_note_on 47 + led2[i], vel_f: 0.2, port: get(:midi_out) if led2[i] != 0 if get(:state) > 0
    set :led1_o, led1[i]
    set :led2_o, led2[i]

    # --- Melody ---
    melbar = bar % 16
    if groove or tribal or finale or brk
      if i % 4 == 0
        note_index = melbar * 4 + i / 4
        with_fx :reverb, room: 0.85, mix: 0.5 do
          with_fx :echo, phase: 0.375, decay: 1.5, mix: 0.15 do
            if groove
              synth :pretty_bell, note: melody[note_index], amp: 0.09 * 0.33, attack: 0.02, release: 0.7
            else
              synth :pretty_bell, note: melody[note_index], amp: 0.09, attack: 0.02, release: 0.7
            end
          end
        end
      end
    end

    # --- Kick ---
    unless intro && bar < 2
      unless brk && bar == 22
        if current_kick[i] == 1
          amp = i == 0 ? 1.35 : 1.05
          sample :bd_haus, amp: amp if bar <= 31
        end
      end
    end

    # --- Snare ---
    unless intro && bar == 0
      if snare[i] == 1
        sample :sn_dolf, amp: 0.75 if bar <= 31
      end
    end

    # --- Ghost snare ---
    if !intro && ghost[i] == 1
      if one_in(3)
        sample :sn_dolf, amp: rrand(0.10, 0.20) if bar <= 31
      end
    end

    # --- Closed hi-hat ---
    unless brk && bar == 21
      if hat[i] == 1
        hatamp = if i % 4 == 0
                   0.28
                 else
                   0.16
                 end
        sample :drum_cymbal_closed, amp: hatamp, finish: 0.08 if bar <= 31
      end
    end

    # --- Open hi-hat ---
    if !intro && openhat[i] == 1
      if bar.odd?
        sample :drum_cymbal_open, amp: 0.28, finish: 0.18
      end
    end

    # --- Toms ---
    if !intro && tomlo[i] == 1
      sample :drum_tom_lo_soft, amp: rrand(0.45, 0.65), pan: -0.35 if bar <= 31
    end
    if tribal || finale
      if tommid[i] == 1
        sample :drum_tom_mid_soft, amp: rrand(0.35, 0.55), pan: 0
      end
    end
    if tribal || finale
      if tomhi[i] == 1
        sample :drum_tom_hi_soft, amp: rrand(0.30, 0.48), pan: 0.35
      end
    end

    # --- Percussion ---
    if groove || tribal || finale
      if perc[i] == 1
        with_fx :reverb, room: 0.4, mix: 0.20 do
          sample :perc_snap, amp: 0.18, pan: rrand(-0.3, 0.3)
        end
      end
    end

    # --- Shaker ---
    if tribal || finale
      if shaker[i] == 1
        sample :elec_tick, amp: rrand(0.05, 0.12), rate: rrand(1.25, 1.55), pan: rrand(-0.5, 0.5)
      end
    end

    # --- Cowbell (finale) ---
    if finale
      if i == 3 || i == 11
        sample :drum_cowbell, amp: 0.16, rate: 0.9
      end
    end

    # --- Polyrhythmic percussion (tribal) ---
    if tribal
      if poly3[i] == 1
        sample :elec_tick, amp: 0.08, rate: 0.8, pan: 0.45
      end
    end

    # --- Tom call & response (finale) ---
    if finale
      if i == 2
        sample :drum_tom_lo_soft, amp: 0.5, pan: -0.4
      end
      if i == 7
        sample :drum_tom_hi_soft, amp: 0.42, pan: 0.4
      end
    end

    # --- Random extras ---
    if tribal || finale
      if one_in(40)
        sample :perc_snap, amp: 0.12, rate: rrand(0.7, 1.3)
      end
    end

    # --- Tom fill every 4 bars ---
    if bar % 4 == 3 && i >= 12
      if i == 12
        sample :drum_tom_lo_hard, amp: 0.65, pan: -0.4
      end
      if i == 13
        sample :drum_tom_mid_hard, amp: 0.6, pan: -0.15
      end
      if i == 14
        sample :drum_tom_hi_hard, amp: 0.58, pan: 0.2
      end
      if i == 15
        sample :drum_tom_hi_hard, amp: 0.75, pan: 0.45
      end
    end

    # --- Big ending fill ---
    if bar == 31
      if i >= 8
        tom = choose([:drum_tom_lo_soft, :drum_tom_mid_soft, :drum_tom_hi_soft])
        sample tom, amp: 0.45 + ((i - 8) * 0.04), pan: rrand(-0.5, 0.5)
      end
      if i == 15
        sample :drum_splash_hard, amp: 0.85
      end
    end

    # --- Swing timing ---
    if i.even?
      unless intro
        sleep 0.21
      else
        sleep 0.25
      end
    else
      unless intro
        sleep 0.28
      else
        sleep 0.25
      end
    end
  end
end

# --- MIDI in: encoders (relative CC) -------------------------------------------

live_loop :midi_1 do
  use_real_time

  id, val = sync get(:midi_in) + "/control_change"
  if id == 17
    set :bpm_n, get(:bpm_n) + 4 if val == 1
    set :bpm_n, get(:bpm_n) - 4 if val == 127
    puts "BPM #{get(:bpm_n)}"
  end
  if id == 19
    set :state, get(:state) + 1 if get(:state) < 4 if val == 1
    set :state, get(:state) - 1 if get(:state) > 0 if val == 127
    puts "STATE #{get(:state)}"
  end
end

# --- MIDI in: buttons (momentary notes) ---------------------------------------

live_loop :midi_2 do
  use_real_time

  id, val = sync get(:midi_in) + "/note_on"
  if id == 60
    puts "STOP"
    set :state, 0
    set :bpm_n, get(:bpm_i)
    set :bpm_o, get(:bpm_i)
    set :bpm, get(:bpm_i)
    puts "BPM #{get(:bpm_n)}"
    puts "STATE #{get(:state)}"
  end
  if id == 61
    set :bar, 1
    puts "BAR 1"
  end
  if id == 62
    set :state, get(:state) + 1 if get(:state) < 4
    puts "START + STATE #{get(:state)}"
  end
  if id == 63
    set :state, get(:state) - 1 if get(:state) > 0
    puts "START - STATE #{get(:state)}"
  end
end
