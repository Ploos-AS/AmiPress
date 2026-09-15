# M2 PDF Backend Status

M2 establishes the first real PDF backend and a deterministic host-side
serialization test.

## Implemented

- PDF 1.4 file header
- PDF indirect objects
- Object offset tracking
- Cross-reference table
- Trailer and `startxref`
- Catalog and Pages tree
- Page resources
- Built-in Helvetica Type 1 font resource
- Multiple independent pages
- Text content streams
- PDF string escaping for ASCII control/non-ASCII bytes
- A4/default and custom point page sizes
- Host-side serialization test

## Deliberately deferred

The following remain open for later M2/M3 work:

- embedded bitmap/image objects
- Flate compression
- document metadata
- richer font and character encoding support
- printer.device capture

The current backend is therefore a functional PDF 1.4 core, not yet the final
AmiPress printer replacement.

## Testing note

The repository contains `make check` coverage for the document core and PDF
serialization. Runtime execution should be performed in the normal CI and
local toolchain environment before claiming a release qualification.
