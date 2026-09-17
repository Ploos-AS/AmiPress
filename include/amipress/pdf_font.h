#ifndef AMIPRESS_PDF_FONT_H
#define AMIPRESS_PDF_FONT_H

#include <stddef.h>

#define AMIPRESS_PDF_FONT_OK 0
#define AMIPRESS_PDF_FONT_ERR_ARGUMENT 1
#define AMIPRESS_PDF_FONT_ERR_UNMAPPABLE 2
#define AMIPRESS_PDF_FONT_ERR_NOSPACE 3

#define AMIPRESS_PDF_FONT_PATH_WINANSI 1
#define AMIPRESS_PDF_FONT_PATH_CE 2

/*
 * Dependency-free PDF font mapping layer.
 *
 * Source encodings are decoded to Unicode before entering this layer.  The
 * WinAnsi path covers the existing Base-14 baseline.  The CE path assigns
 * stable private 8-bit PDF character codes to Central-European Unicode
 * glyphs.  A later PDF writer/font-program integration can use the returned
 * glyph names to build a /Differences encoding for an actually embedded or
 * otherwise guaranteed font program.
 */
int amipress_pdf_winansi_code(unsigned long codepoint, unsigned char *code);

int amipress_pdf_winansi_encode(const unsigned long *codepoints,
    size_t codepoint_count, unsigned char *output, size_t output_size,
    size_t *output_len);

int amipress_pdf_ce_code(unsigned long codepoint, unsigned char *code,
    const char **glyph_name);

int amipress_pdf_font_code(unsigned long codepoint, int *path,
    unsigned char *code, const char **glyph_name);

#endif
