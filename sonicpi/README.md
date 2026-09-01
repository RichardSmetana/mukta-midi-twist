# Sonic Pi examples

Example buffers for testing **Mukta MIDI Twist** LED feedback from [Sonic Pi](https://sonic-pi.net/).

## Files

| File | Purpose |
|------|---------|
| [`sonic-pi-01-led-test.txt`](sonic-pi-01-led-test.txt) | Visual test — cycles onboard RGB + 4 strip pixels via MIDI notes |

## Setup

1. Flash the firmware and connect USB (see main [README](../README.md)).
2. In Sonic Pi: **Preferences → I/O → MIDI** → enable **Mukta MIDI Twist** as output.
3. Unplug/replug USB if the device does not appear.
4. Open `sonic-pi-01-led-test.txt`, copy all lines **from `use_bpm 60` onward** into a Sonic Pi buffer (comments starting with `#` are ignored by Sonic Pi).
5. Press **Run**.

## MIDI port name

The sketch uses:

```ruby
port: "mukta_midi_twist__0_"
```

Sonic Pi may show a slightly different name. In a buffer, run:

```ruby
puts midi_available_ports
```

Replace the port string in the script if yours differs (e.g. `mukta_midi_twist` without suffix).

## What `sonic-pi-01-led-test` does

- **Tempo:** 60 BPM (`use_bpm 60`).
- **Loop:** `live_loop :test1` runs forever.
- **`:index`:** alternates `0` and `1` each full cycle → base note **45** or **46** (onboard R/G channels).
- **Each cycle:** 4 steps (`i = 0..3`), one strip pixel highlighted per step.

Per step (`i`):

| Action | MIDI notes | Velocity |
|--------|------------|----------|
| 5× note on/off | `note1 + j*3` for `j = 0..4` | 0.2 |
| Strip pixel `i+1` RGB | `48 + i*3`, `49 + i*3`, `50 + i*3` | 0.6 |
| Timing | 0.25 s on, 0.25 s off | — |
| Audio | `:drum_cymbal_closed` (louder on first step) | amp 1.0 / 0.5 |

### Note map (must match `config.h`)

| Target | Notes |
|--------|-------|
| Onboard R / G / B | **45**, **46**, **47** |
| Strip pixel 1 R/G/B | **48**, **49**, **50** |
| Strip pixel 2 R/G/B | **51**, **52**, **53** |
| Strip pixel 3 R/G/B | **54**, **55**, **56** |
| Strip pixel 4 R/G/B | **57**, **58**, **59** |

All messages use **MIDI channel 1** (Sonic Pi default for `midi_note_on`).

## Troubleshooting

| Problem | Check |
|---------|--------|
| No LEDs | MIDI port name; device enabled in I/O prefs; `LED_MIDI_ENABLE` / `STRIP_MIDI_ENABLE` in `config.h` |
| Wrong colours on strip | `NEO_RGB` vs `NEO_GRB` in `leds.cpp` |
| No MIDI device | Firmware README — **USB CDC On Boot: Disabled**, USB replug |

## Adding more examples

Place new `.txt` or `.rb` files in this folder with a short description in this README.
