# M2 PDF Backend Status

M2 establishes the first real PDF backend and deterministic host-side
serialization coverage.

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
- Embedded RGB bitmap/image XObjects
- FlateDecode image streams using a small dependency-free zlib/DEFLATE stored-block encoder
- Document Info metadata: Title, Author, Creator and Producer
- `/Info` trailer reference
- Host-side serialization tests for text, images, compression and metadata

## Remaining for later milestones

- richer font and character encoding support (M3)
- graphics/layout primitives (M4)
- printer.device capture (M5)
- additional output backends (M6)

The current backend is a functional lightweight PDF 1.4 core suitable as the
foundation for the AmiPress printer replacement.

## Testing note

The repository contains `make check` coverage for the document core and PDF
serialization. Runtime execution should be performed in the normal CI and
local toolchain environment before claiming release qualification.
