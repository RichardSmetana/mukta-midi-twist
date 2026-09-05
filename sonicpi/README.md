# Sonic Pi examples

Example buffers for **Mukta MIDI Twist** — LED tests and a full drum performance with bidirectional MIDI control via [Sonic Pi](https://sonic-pi.net/).

All scripts are plain Ruby. Copy the file contents into a Sonic Pi buffer (or open the `.rb` file) and press **Run**. Lines starting with `#` are comments and are ignored by Sonic Pi.

## Files

| File | Type | Description |
|------|------|-------------|
| [`sonic-pi-01-led-test.rb`](sonic-pi-01-led-test.rb) | Output only | Simple LED visual test — cycles onboard RGB + strip pixels |
| [`sonic-pi-02-drum-track.rb`](sonic-pi-02-drum-track.rb) | Input + output | 32-bar drum track with melody; LEDs follow the beat; encoders/buttons control playback |

## Setup

1. Flash the firmware and connect USB (see main [README](../README.md)).
2. Sonic Pi → **Preferences → I/O → MIDI**:
   - **Output:** enable **Mukta MIDI Twist** (required for LED feedback).
   - **Input:** enable **Mukta MIDI Twist** (required for `sonic-pi-02-drum-track.rb` only).
3. Unplug/replug USB if the device does not appear.
4. Open a script, copy into a buffer if needed, press **Run**.

### MIDI port names

Default output port in the scripts:

```ruby
"mukta_midi_twist__0_"
```

Sonic Pi may show a slightly different name. In a buffer, run:

```ruby
puts midi_available_ports
```

Update the port string in the script if yours differs. In `sonic-pi-02-drum-track.rb`, edit:

```ruby
set :midi_in,  "/midi:mukta*"      # wildcard input match
set :midi_out, "mukta_midi_twist__0_"  # exact output name
```

---

## `sonic-pi-01-led-test.rb`

Minimal test — no hardware input required.

| Setting | Value |
|---------|-------|
| Tempo | 60 BPM |
| Loop | `live_loop :test1` (runs forever) |
| Onboard base notes | **45** or **46** (`:index` toggles each cycle) |
| Strip | One pixel per step, notes **48–59** (R/G/B per pixel) |
| Timing | 0.25 s on, 0.25 s off per step |
| Audio | `:drum_cymbal_closed` (louder on first step) |

---

## `sonic-pi-02-drum-track.rb`

Full performance piece with **bidirectional MIDI**.

### Arrangement (32 bars)

| Bars | Section |
|------|---------|
| 0–3 | Intro |
| 4–11 | Main groove |
| 12–19 | Tribal |
| 20–23 | Break |
| 24–31 | Finale |

Default start tempo: **90 BPM**. Swing timing after the intro.

### LED output (MIDI → controller)

| MIDI note | Effect |
|-----------|--------|
| **45** | Onboard red — toggles every 16th step |
| **47 + offset** | Strip colours from `led1` / `led2` rings (offset 1→48, 4→51, 7→54, …) |

Strip LED patterns run only when `state > 0`.

### Hardware input (controller → Sonic Pi)

Maps to default [`config.h`](../config.h):

| Control | MIDI | Action in script |
|---------|------|------------------|
| Encoder 0 (relative) | CC **17**, val 1 / 127 | BPM ±4 |
| Encoder 1 (relative) | CC **19**, val 1 / 127 | `state` ±1 (range 0–4) |
| Button 0 | Note **60** | Stop — reset BPM and `state` to 0 |
| Button 1 | Note **61** | Jump to bar 1 |
| Button 2 | Note **62** | `state` +1 (max 4) |
| Button 3 | Note **63** | `state` −1 (min 0) |

Monitor the log panel for `BPM`, `STATE`, `BAR`, and `STOP` messages.

### State variable

| `state` | Behaviour (summary) |
|---------|---------------------|
| 0 | LEDs off (strip patterns disabled); bar forced to 80 |
| 1–3 | Strip LED patterns active; normal bar progression |
| 4 | At bar 32 → stop loop |

---

## MIDI note map (LED feedback)

Must match [`config.h`](../config.h). All on **channel 1**.

| Target | Notes |
|--------|-------|
| Onboard R / G / B | **45**, **46**, **47** |
| Strip pixel 1 R/G/B | **48**, **49**, **50** |
| Strip pixel 2 R/G/B | **51**, **52**, **53** |
| Strip pixel 3 R/G/B | **54**, **55**, **56** |
| Strip pixel 4 R/G/B | **57**, **58**, **59** |

Velocity sets brightness (0.0–1.0 in Sonic Pi → 0–127 on the device).

---

## Troubleshooting

| Problem | Check |
|---------|--------|
| No LEDs | MIDI output port name; device enabled in I/O prefs; `LED_MIDI_ENABLE` / `STRIP_MIDI_ENABLE` in `config.h` |
| Encoders/buttons ignored | MIDI **input** enabled; `:midi_in` wildcard matches your port |
| Wrong strip colours | `NEO_RGB` vs `NEO_GRB` in `leds.cpp` |
| No MIDI device | **USB CDC On Boot: Disabled**; USB replug (see main README) |

---

## Adding more examples

Add new `.rb` files in this folder with a header comment block and a row in the table above.

Scripts in this folder are part of the Mukta MIDI Twist project and licensed under **GNU GPL v3.0 or later** (see [LICENSE](../LICENSE)).
