#include "amipress/pdf_font.h"

#define UNDEFINED_CODEPOINT 0xffffUL

static const unsigned long winansi_high[32] = {
    0x20acUL,UNDEFINED_CODEPOINT,0x201aUL,0x0192UL,
    0x201eUL,0x2026UL,0x2020UL,0x2021UL,
    0x02c6UL,0x2030UL,0x0160UL,0x2039UL,
    0x0152UL,UNDEFINED_CODEPOINT,0x017dUL,UNDEFINED_CODEPOINT,
    UNDEFINED_CODEPOINT,0x2018UL,0x2019UL,0x201cUL,
    0x201dUL,0x2022UL,0x2013UL,0x2014UL,
    0x02dcUL,0x2122UL,0x0161UL,0x203aUL,
    0x0153UL,UNDEFINED_CODEPOINT,0x017eUL,0x0178UL
};

int amipress_pdf_winansi_code(unsigned long codepoint, unsigned char *code)
{
    unsigned int i;
    if (!code) return AMIPRESS_PDF_FONT_ERR_ARGUMENT;

    if ((codepoint >= 0x20UL && codepoint <= 0x7eUL) ||
        (codepoint >= 0xa0UL && codepoint <= 0xffUL)) {
        *code = (unsigned char)codepoint;
        return AMIPRESS_PDF_FONT_OK;
    }

    for (i = 0U; i < 32U; ++i) {
        if (winansi_high[i] != UNDEFINED_CODEPOINT &&
            winansi_high[i] == codepoint) {
            *code = (unsigned char)(0x80U + i);
            return AMIPRESS_PDF_FONT_OK;
        }
    }

    return AMIPRESS_PDF_FONT_ERR_UNMAPPABLE;
}

int amipress_pdf_winansi_encode(const unsigned long *codepoints,
    size_t codepoint_count, unsigned char *output, size_t output_size,
    size_t *output_len)
{
    size_t i;
    int rc;

    if ((!codepoints && codepoint_count) || !output_len)
        return AMIPRESS_PDF_FONT_ERR_ARGUMENT;
    if (codepoint_count > output_size || (!output && codepoint_count))
        return AMIPRESS_PDF_FONT_ERR_NOSPACE;

    for (i = 0; i < codepoint_count; ++i) {
        rc = amipress_pdf_winansi_code(codepoints[i], &output[i]);
        if (rc != AMIPRESS_PDF_FONT_OK) return rc;
    }

    *output_len = codepoint_count;
    return AMIPRESS_PDF_FONT_OK;
}
