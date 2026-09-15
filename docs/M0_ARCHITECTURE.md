# AmiPress M0 Architecture

## Purpose

AmiPress is a reusable document rendering and publishing stack for classic
AmigaOS. PDF is the first major target, but the architecture deliberately
separates the document model from output backends.

## Layering

```text
Amiga applications
        |
        +-- printer.device integration
        +-- CLI
        +-- ARexx
        |
        v
+----------------------+
|   AmiPress document  |
|   and render core    |
+----------------------+
        |
        +-- PDF
        +-- PostScript/EPS
        +-- SVG
        +-- bitmap/IFF
        +-- future backends
```

## Compatibility

The baseline target is classic AmigaOS, with a 68000-compatible core wherever
possible. CPU-specific optimisations may be added for 68020, 68040 and 68060
builds without changing the public document model.

The project will distinguish between:

- features possible on all supported systems;
- features requiring newer printer.device versions;
- features requiring newer AmigaOS UI/toolkit components.

No dependency on modern AmigaOS components is allowed in the core rendering
library.

## Document model

The core will represent documents independently of their final encoding. The
initial model is expected to cover:

- documents and pages;
- physical page dimensions and margins;
- text runs;
- bitmap images;
- simple drawing primitives;
- metadata;
- output/resource references.

Backends consume this model and are responsible for format-specific encoding.

## printer.device boundary

The printer integration is an adapter. It captures the printer job and maps
printer text and graphics operations into the AmiPress document/rendering
pipeline. The PDF backend must not contain printer.device-specific policy.

This separation also permits direct CLI/ARexx publishing without pretending
that every document originated from a printer job.

## Output strategy

The first production backend will target conservative PDF suitable for old
Amiga systems and current desktop PDF readers. Additional backends can be
implemented against the same core interface.

Streaming is preferred where it materially reduces RAM requirements. Large
raster pages may still require bounded page buffers; the renderer must fail
cleanly rather than assume abundant contiguous memory.

## Testing strategy

M0 establishes the long-term test model:

1. host-side tests for deterministic document and encoding logic;
2. cross-compilation for classic 68k targets;
3. visible FS-UAE runtime qualification for Amiga-specific behaviour;
4. GitHub Actions automation for repeatable host/build checks and emulator
   tests where the runner environment permits them.

## Non-goals for M0

M0 does not implement a PDF writer, printer driver, viewer or preferences
editor. It establishes the boundaries so those components can be implemented
incrementally without coupling the whole project together.
