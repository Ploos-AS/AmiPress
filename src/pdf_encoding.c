#include "amipress/pdf.h"

#include <stdlib.h>

static int unicode_to_winansi(unsigned long codepoint, unsigned char *out)
{
    static const unsigned long high[32] = {
        0x20acUL, 0xffffUL, 0x201aUL, 0x0192UL, 0x201eUL, 0x2026UL, 0x2020UL, 0x2021UL,
        0x02c6UL, 0x2030UL, 0x0160UL, 0x2039UL, 0x0152UL, 0xffffUL, 0x017dUL, 0xffffUL,
        0xffffUL, 0x2018UL, 0x2019UL, 0x201cUL, 0x201dUL, 0x2022UL, 0x2013UL, 0x2014UL,
        0x02dcUL, 0x2122UL, 0x0161UL, 0x203aUL, 0x0153UL, 0xffffUL, 0x017eUL, 0x0178UL
    };
    size_t i;

    if (!out) return AMIPDF_ERR_ARGUMENT;
    if ((codepoint >= 0x20UL && codepoint <= 0x7eUL) ||
        (codepoint >= 0xa0UL && codepoint <= 0xffUL)) {
        *out = (unsigned char)codepoint;
        return AMIPDF_OK;
    }
    for (i = 0; i < 32U; ++i) {
        if (high[i] == codepoint) {
            *out = (unsigned char)(0x80U + i);
            return AMIPDF_OK;
        }
    }
    return AMIPDF_ERR_UNMAPPABLE;
}

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
    if (text_len > ((size_t)-1) / sizeof(*codepoints) || text_len == (size_t)-1)
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

    for (i = 0; i < decoded_len; ++i) {
        unsigned char encoded;
        rc = unicode_to_winansi(codepoints[i], &encoded);
        if (rc != AMIPDF_OK || encoded == 0U) {
            free(safe); free(codepoints);
            return AMIPDF_ERR_UNMAPPABLE;
        }
        safe[i] = (char)encoded;
    }
    safe[decoded_len] = '\0';
    rc = amipdf_text_styled(pdf, x, y, safe, size_pt, style);
    free(safe); free(codepoints);
    return rc;
}
