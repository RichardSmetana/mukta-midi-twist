# Contributing to midictrl

Thanks for your interest in improving this project.

## Ground rules

- Keep changes focused and easy to review.
- Match the existing code style in `midictrl.ino` (clear names, short ISRs, MIDI only from `loop()`).
- Do not commit build artifacts, IDE caches, or secrets.
- By contributing, you agree that your contributions are licensed under the **GNU GPL v3.0** (see [LICENSE](LICENSE)).

## How to contribute

1. Fork the repository (after it is published on GitHub) or work on a local branch.
2. Create a branch for your change, for example `feature/second-encoder` or `fix/debounce`.
3. Test on real hardware when possible (encoder direction, CC range, note on/off, no stuck notes).
4. Open a pull request with a short description of **what** changed and **why**.

## Development tips

- Prefer interrupt-safe patterns: `volatile` shared state, minimal ISR work, no `Serial` inside ISRs.
- Document new pins, MIDI defaults, or timing constants in [README.md](README.md).
- If you add libraries, list them in the README and keep dependencies as small as possible.

## Reporting issues

Include:

- Board type (e.g. Mega 2560, Uno)
- Wiring / pin numbers
- Host OS and serial↔MIDI bridge (baud rate)
- Expected vs actual MIDI behavior
