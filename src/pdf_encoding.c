#include "amipress/pdf.h"

#include <stdlib.h>

int amipdf_text_encoded(struct amipdf *pdf, int x, int y,
    const unsigned char *text, size_t text_len, enum amipress_encoding encoding,
    int size_pt, unsigned int style)
{
    unsigned long *codepoints;
    char *safe;
    size_t decoded_len;
    size_t i;
    int rc;

    if (!pdf || (!text && text_len) || size_pt <= 0)
        return AMIPDF_ERR_ARGUMENT;
    if (text_len == 0)
        return amipdf_text_styled(pdf, x, y, "", size_pt, style);
    if (text_len > ((size_t)-1) / sizeof(*codepoints))
        return AMIPDF_ERR_MEMORY;

    codepoints = (unsigned long *)malloc(text_len * sizeof(*codepoints));
    if (!codepoints) return AMIPDF_ERR_MEMORY;
    safe = (char *)malloc(text_len + 1U);
    if (!safe) { free(codepoints); return AMIPDF_ERR_MEMORY; }

    rc = amipress_decode_text(encoding, text, text_len,
        codepoints, text_len, &decoded_len);
    if (rc != AMIPRESS_TEXT_OK) {
        free(safe); free(codepoints);
        return rc == AMIPRESS_TEXT_ERR_UNMAPPABLE ?
            AMIPDF_ERR_UNMAPPABLE : AMIPDF_ERR_ARGUMENT;
    }

    /* Base-14 fonts currently use their default PDF encoding.  Until a
       deliberate Differences/embedded-font layer exists, only the common
       printable ASCII repertoire is serialized.  This avoids silently
       emitting wrong glyphs for Latin-1 and Central-European codepoints. */
    for (i = 0; i < decoded_len; ++i) {
        if (codepoints[i] < 0x20UL || codepoints[i] > 0x7eUL) {
            free(safe); free(codepoints);
            return AMIPDF_ERR_UNMAPPABLE;
        }
        safe[i] = (char)codepoints[i];
    }
    safe[decoded_len] = '\0';
    rc = amipdf_text_styled(pdf, x, y, safe, size_pt, style);
    free(safe); free(codepoints);
    return rc;
}
