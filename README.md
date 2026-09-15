# AmiPress

A modern document rendering and publishing engine for classic AmigaOS.

AmiPress is designed as a reusable output stack rather than a single PDF
printer. The project will provide a common document model and rendering
pipeline with output backends such as PDF, PostScript, SVG and bitmap/image
formats, together with Amiga-native printer.device integration, CLI tools and
ARexx automation.

## Goals

- Run on classic Amiga hardware, starting with 68000 and scaling through 68060.
- Support Kickstart/AmigaOS 1.3+ where practical, with clear feature gates.
- Provide a small, reusable document and rendering core.
- Preserve real text whenever the selected output format supports it.
- Support Amiga printer.device as a virtual publishing target.
- Make output backends independent and extensible.
- Provide both programmatic and ARexx/CLI interfaces.
- Build and qualify reproducibly on modern CI and visible Amiga emulation.

## Planned outputs

- PDF
- PostScript / EPS
- SVG
- IFF/ILBM and other bitmap output
- PNG/JPEG where practical
- HTML/text-oriented publishing backends where useful

## M0

M0 establishes the project architecture and compatibility boundaries. No
production renderer is promised yet.

See `docs/M0_ARCHITECTURE.md` and `ROADMAP.md`.

## Status

M0 — architecture and project foundation.
