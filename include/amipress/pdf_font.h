#ifndef AMIPRESS_PDF_FONT_H
#define AMIPRESS_PDF_FONT_H

#include <stddef.h>

#define AMIPRESS_PDF_FONT_OK 0
#define AMIPRESS_PDF_FONT_ERR_ARGUMENT 1
#define AMIPRESS_PDF_FONT_ERR_UNMAPPABLE 2
#define AMIPRESS_PDF_FONT_ERR_NOSPACE 3

/*
 * Dependency-free PDF font mapping layer.
 *
 * The baseline mapper targets the existing Base-14 / WinAnsi path.  It is
 * intentionally separate from source-text decoding: callers first decode
 * AmigaPL/ISO-8859-2/Windows-1250 to Unicode and then ask this layer whether
 * the selected PDF font path can represent each code point.
 */
int amipress_pdf_winansi_code(unsigned long codepoint, unsigned char *code);

int amipress_pdf_winansi_encode(const unsigned long *codepoints,
    size_t codepoint_count, unsigned char *output, size_t output_size,
    size_t *output_len);

#endif
