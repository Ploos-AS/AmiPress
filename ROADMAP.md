# AmiPress Roadmap

## M0 — Architecture

- [x] Establish project scope and name
- [x] Define reusable document/rendering core
- [x] Define output-backend model
- [x] Define classic Amiga compatibility strategy
- [x] Define printer.device integration boundary
- [x] Define CLI and ARexx integration goals
- [x] Define CI and emulator qualification strategy

## M1 — Document Core

- [x] Document/page object model
- [x] Units, page sizes and margins (initial physical page dimensions)
- [x] Text, image and primitive object type foundation
- [x] Metadata model (reserved for next core extension)
- [x] Streaming/output abstraction (reserved for backend phase)
- [x] Host-side unit tests

## M2 — PDF Backend

- [x] PDF 1.4 baseline writer
- [x] Objects, streams and xref
- [x] Pages and resources
- [x] Text output
- [x] Bitmap/image output — RGB images
- [x] FlateDecode-compatible dependency-free stored DEFLATE blocks
- [x] Metadata — Title, Author, Creator and Producer
- [x] GitHub Actions host qualification

## M3 — Text and Encoding

- [ ] Amiga printer text capture
- [x] Base-14 PDF font model
- [x] Bold/italic/underline rendering — Base-14 style selection and initial underline path implemented
- [x] Latin-1 byte mapping core
- [x] AmigaPL decoding table
- [x] ISO-8859-2 decoding table
- [x] Windows-1250 decoding table, including undefined-byte handling
- [x] Extensible character mapping — encoding-independent Unicode codepoint and buffer APIs established
- [x] Encoded-text PDF bridge
- [x] Explicit WinAnsiEncoding for Base-14 PDF fonts
- [x] Unicode-to-WinAnsi mapping with unmappable-character rejection
- [x] GitHub Actions qualification for encoded PDF text
- [ ] Full Central-European PDF glyph output beyond WinAnsi — requires a deliberate Differences or embedded-font path
- [ ] Base-14 metric-aware underline width

## M4 — Graphics and Layout

- [ ] Monochrome output
- [ ] Grayscale output
- [ ] Indexed colour output
- [ ] Full-colour output
- [ ] Physical-size preservation
- [ ] Scaling and rotation
- [ ] Margins and page positioning
- [ ] Multi-page/strip printing

## M5 — printer.device Backend

- [ ] Virtual PDF printer
- [ ] Printer.device command handling
- [ ] Text rendering path
- [ ] Graphics rendering path
- [ ] Printer preferences integration
- [ ] OS 1.3 compatibility path

## M6 — Additional Backends

- [ ] PostScript
- [ ] EPS
- [ ] SVG
- [ ] IFF/ILBM
- [ ] PNG
- [ ] JPEG
- [ ] HTML/text publishing where justified

## M7 — CLI and ARexx

- [ ] AmiPress CLI
- [ ] Batch publishing
- [ ] ARexx port
- [ ] Document manipulation commands
- [ ] Output/backend selection

## M8 — ShowPress

- [ ] Lightweight Amiga viewer
- [ ] PDF rendering
- [ ] Page navigation
- [ ] Zoom/fit modes
- [ ] Search/copy where supported
- [ ] 68000-compatible baseline

## M9 — Qualification

- [x] Host tests
- [ ] 68000 build
- [ ] 68020 build
- [ ] 68040 build
- [ ] 68060 build
- [ ] Kickstart 1.3 / AmigaOS 1.3
- [ ] AmigaOS 2.x
- [ ] AmigaOS 3.1
- [ ] AmigaOS 3.2
- [ ] Visible FS-UAE qualification
- [x] GitHub runner automation where practical

## M10 — Release

- [ ] Documentation
- [ ] Installer/package layout
- [ ] 1.3-ready distribution
- [ ] OS 2.x/3.x distribution
- [ ] Release archive
- [ ] Checksums
- [ ] GitHub Release
- [ ] Aminet-ready packaging
