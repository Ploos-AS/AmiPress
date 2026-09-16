# M2 PDF Backend Status

M2 establishes the first real PDF backend and deterministic host-side
serialization coverage.

## Status

**M2 host qualification: PASS**

GitHub Actions CI run #5 (`35099365958`) passed on commit
`22e3e1e33cc8c60ecf5d878f90852a9a00a61c21`.

This is host qualification only. Classic Amiga builds and visible AmigaOS /
FS-UAE runtime qualification remain part of the later M9 qualification phase.

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
- Binary-safe host-side serialization tests for text, images, FlateDecode and metadata

## Host qualification

The GitHub-hosted Ubuntu runner performs a clean strict C89 build with
`-Wall -Wextra -Werror -pedantic`, runs `make check`, then rebuilds from a
clean tree through `make all`.

The passing qualification covers both the document model test and the PDF
serialization test. The PDF test validates the PDF 1.4 header, Catalog and
Pages tree, text from two pages, RGB image XObject and resources,
`/FlateDecode`, Info metadata, xref/startxref and EOF structure.

## Remaining for later milestones

- richer font and character encoding support (M3)
- graphics/layout primitives (M4)
- printer.device capture (M5)
- additional output backends (M6)
- classic Amiga CPU/OS and visible FS-UAE qualification (M9)

The current backend is a functional lightweight PDF 1.4 core suitable as the
foundation for the AmiPress printer replacement.
