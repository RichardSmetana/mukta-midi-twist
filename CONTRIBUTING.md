# Contributing to Mukta MIDI Twist

Thanks for your interest in improving this project.

## Ground rules

- Keep changes focused and easy to review.
- Match the existing code style in `mukta-midi-twist.ino` (clear names, short ISRs, MIDI only from `loop()`).
- Do not commit build artifacts, IDE caches, or secrets.
- By contributing, you agree that your contributions are licensed under the **GNU GPL v3.0 or later** (see [LICENSE](LICENSE)).

## How to contribute

1. Fork the repository or work on a local branch.
2. Create a branch for your change, for example `feature/second-encoder`, `hw/gpio-remap`, or `fix/debounce`.
3. Test on real hardware when possible (encoder direction, CC range, note on/off, no stuck notes).
4. Open a pull request with a short description of **what** changed and **why**.
5. Update [README.md](README.md) and [README.de.md](README.de.md) when pins, MIDI maps, or setup steps change.
6. Note user-facing changes under **[Unreleased]** in [CHANGELOG.md](CHANGELOG.md).

## Releases / versioning

- Current development target: **0.2.0** (branch `develop`, version file [`VERSION`](VERSION) = `0.2.0-dev`).
- Stable releases live on `main` and are tagged `vX.Y.Z` (e.g. [`v0.1.0`](https://github.com/RichardSmetana/mukta-midi-twist/releases/tag/v0.1.0)).
- Before merging a release to `main`: move Unreleased notes into a dated `[X.Y.Z]` section, set `VERSION` to `X.Y.Z`, then tag and publish a GitHub Release.

## Development tips

- Prefer interrupt-safe patterns: `volatile` shared state, minimal ISR work, no `Serial` inside ISRs.
- Document new pins, MIDI defaults, or timing constants in the READMEs.
- If you add libraries, list them in the README and keep dependencies as small as possible.
- Hardware / pin changes belong primarily in [`config.h`](config.h).

## Reporting issues

Include:

- Board type (e.g. YD-ESP32-S3, ESP32S3 Dev Module)
- Wiring / pin numbers (or say “matches `config.h`”)
- Host OS and whether native USB MIDI is visible
- Expected vs actual MIDI behavior
