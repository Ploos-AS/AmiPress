# M3 Central-European PDF Font Strategy

## Purpose

AmiPress already decodes Latin-1, AmigaPL, ISO-8859-2 and Windows-1250 to Unicode code points and can serialize the WinAnsi subset through the PDF Base-14 path. Central-European characters outside WinAnsi must not be silently substituted or emitted with an incorrect encoding.

This document defines the next M3 implementation boundary without requiring changes to the active PDF writer while that file is being worked on elsewhere.

## Requirements

1. Preserve the current Unicode code-point boundary after source decoding.
2. Keep the existing Base-14/WinAnsi path for characters that are representable there.
3. Return an explicit unmappable result when no selected PDF font path can represent a code point.
4. Add a second PDF font path for Central-European glyphs rather than pretending WinAnsi covers them.
5. Keep the implementation usable on classic 68000 Amigas with bounded memory use and no mandatory heavyweight runtime dependency.
6. Do not embed or redistribute proprietary font files in the repository.

## Preferred implementation

Use an embedded Type 1 or TrueType font only when the user supplies a redistributable/compatible font at build or runtime, with subsetting where practical. The document/render core should deal in Unicode code points; the PDF backend owns code-point-to-glyph mapping and PDF font encoding.

For the dependency-free baseline, add a PDF `/Differences` encoding only for glyphs that are available in a selected standard-compatible font program. A `/Differences` dictionary alone does not create missing glyphs, so it must never be used to claim Central-European support from a Base-14 viewer font that does not guarantee those glyphs.

## Planned API boundary

A future font mapper should expose operations equivalent to:

- test whether a Unicode code point is representable by the selected PDF font;
- map a Unicode code point to an 8-bit PDF character code or embedded-font glyph identifier;
- return the advance width in 1/1000 em units;
- expose the PDF resource information needed by the writer.

The existing `amipress_decode_byte()` and `amipress_decode_text()` APIs remain the source-decoding layer and should not acquire PDF-specific behavior.

## Qualification cases

Qualification must cover at least:

- ASCII and Latin-1 regression through WinAnsi;
- Windows-1250 Euro sign through WinAnsi;
- Polish A-ogonek/a-ogonek;
- L-stroke/l-stroke;
- S-acute/s-acute;
- Z-dot/z-dot;
- representative Czech/Slovak accented letters from ISO-8859-2/Windows-1250;
- explicit rejection when the selected font cannot represent a requested code point;
- round-trip PDF inspection confirming the declared encoding/font resource matches emitted character codes.

## Status

Architecture defined. Implementation and qualification remain pending. This does not mark full Central-European PDF glyph output complete.
