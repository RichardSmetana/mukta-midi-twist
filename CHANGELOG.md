# Changelog

All notable changes to **Mukta MIDI Twist** are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project aims to follow [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

Development toward **0.2.0** (see branch `develop`). Version string in [`VERSION`](VERSION): `0.2.0-dev`.

### Planned

- Full firmware / hardware rewrite (breaking changes expected)
- Update wiring docs, GPIO map, and Sonic Pi examples to match new hardware
- Keep GPL-3.0-or-later licensing

### Added

- *(fill in during development)*

### Changed

- *(fill in during development)*

### Fixed

- *(fill in during development)*

### Removed

- *(fill in during development)*

<!--
When releasing 0.2.0:
1. Move Unreleased notes into ## [0.2.0] - YYYY-MM-DD
2. Set VERSION to 0.2.0
3. Tag v0.2.0 on main after merge
4. Create GitHub Release from CHANGELOG notes
-->

## [0.1.0] - 2026-09-05

First public release of the Mukta MIDI Twist firmware and documentation.

### Added

- ESP32-S3 USB MIDI firmware for four KY-040 encoders with push buttons
- Absolute and relative MIDI CC modes per encoder (`config.h`)
- Momentary and latch button note mapping
- MIDI receive → onboard RGB (GPIO 48) and WS2812 strip (GPIO 7, 4 pixels)
- Velocity-controlled LED brightness and idle timeout
- `platform.local.txt` for post-upload watchdog reset on ESP32-S3
- English README and German [`README.de.md`](README.de.md)
- Project logo and breadboard / wiring diagram images
- Sonic Pi examples: LED visual test and bidirectional drum track
- Contributing guide and code of conduct
- Full project licensing under **GPL-3.0-or-later** (source, docs, logo, images)

### Changed

- Wiring diagram labels switched to fully English
- Documentation and Sonic Pi scripts formatted and expanded
- SPDX / GPL headers added across source and example files

### Fixed

- Native USB MIDI setup for Windows / Sonic Pi (`USB CDC On Boot: Disabled`)
- Strip color order (`NEO_RGB`) and GPIO map for current hardware

[Unreleased]: https://github.com/RichardSmetana/mukta-midi-twist/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/RichardSmetana/mukta-midi-twist/releases/tag/v0.1.0
