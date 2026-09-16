# M2 Host Qualification

## Result

**PASS**

M2 PDF Backend host qualification passed in GitHub Actions CI run #5.

- Run ID: `35099365958`
- Qualified commit: `22e3e1e33cc8c60ecf5d878f90852a9a00a61c21`
- Workflow: `.github/workflows/ci.yml`
- Job: `Host qualification`
- Runner: GitHub-hosted `ubuntu-latest`
- Result: `success`

## Qualification procedure

The workflow performs:

1. repository checkout;
2. compiler and GNU Make version reporting;
3. `make clean`;
4. `make check`;
5. another `make clean`;
6. `make all` from a clean tree.

The host build uses the repository defaults:

```text
-std=c89 -Wall -Wextra -Werror -pedantic
```

## Covered tests

### Document core

`tests/test_document.c` validates the basic AmiPress document/page/object
model.

### PDF backend

`tests/test_pdf.c` creates a two-page PDF containing text, a 2x2 RGB image and
Document Info metadata. Its binary-safe validation checks:

- PDF 1.4 header;
- Catalog object;
- two-page Pages tree;
- text from both pages;
- image XObject;
- DeviceRGB colour space;
- FlateDecode filter;
- image page resource;
- Title, Author, Creator and Producer metadata;
- trailer Info reference;
- xref table;
- startxref;
- EOF marker.

The binary-safe search is significant because embedded Flate/image streams may
contain NUL bytes and therefore cannot be validated correctly with ordinary
C string searches across the entire generated PDF.

## Qualification boundary

This report records **host qualification**, not final classic Amiga runtime
qualification. M9 retains the CPU, AmigaOS and visible FS-UAE qualification
matrix, including 68000/68020/68040/68060 and the targeted AmigaOS versions.

## Outcome

M2 is accepted as the host-qualified PDF backend baseline. Development can
proceed to M3 Text and Encoding while classic Amiga runtime qualification is
kept for the dedicated qualification phase.
