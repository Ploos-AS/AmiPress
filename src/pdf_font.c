#include "amipress/pdf_font.h"

#include <stdio.h>
#include <string.h>

#define UNDEFINED_CODEPOINT 0xffffUL

struct ce_glyph {
    unsigned long codepoint;
    const char *name;
};

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

static const struct ce_glyph ce_glyphs[] = {
    {0x0102UL,"Abreve"},{0x0103UL,"abreve"},
    {0x0104UL,"Aogonek"},{0x0105UL,"aogonek"},
    {0x0106UL,"Cacute"},{0x0107UL,"cacute"},
    {0x010cUL,"Ccaron"},{0x010dUL,"ccaron"},
    {0x010eUL,"Dcaron"},{0x010fUL,"dcaron"},
    {0x0110UL,"Dcroat"},{0x0111UL,"dcroat"},
    {0x0118UL,"Eogonek"},{0x0119UL,"eogonek"},
    {0x011aUL,"Ecaron"},{0x011bUL,"ecaron"},
    {0x0139UL,"Lacute"},{0x013aUL,"lacute"},
    {0x013dUL,"Lcaron"},{0x013eUL,"lcaron"},
    {0x0141UL,"Lslash"},{0x0142UL,"lslash"},
    {0x0143UL,"Nacute"},{0x0144UL,"nacute"},
    {0x0147UL,"Ncaron"},{0x0148UL,"ncaron"},
    {0x0150UL,"Ohungarumlaut"},{0x0151UL,"ohungarumlaut"},
    {0x0154UL,"Racute"},{0x0155UL,"racute"},
    {0x0158UL,"Rcaron"},{0x0159UL,"rcaron"},
    {0x015aUL,"Sacute"},{0x015bUL,"sacute"},
    {0x015eUL,"Scedilla"},{0x015fUL,"scedilla"},
    {0x0162UL,"Tcedilla"},{0x0163UL,"tcedilla"},
    {0x0164UL,"Tcaron"},{0x0165UL,"tcaron"},
    {0x016eUL,"Uring"},{0x016fUL,"uring"},
    {0x0170UL,"Uhungarumlaut"},{0x0171UL,"uhungarumlaut"},
    {0x0179UL,"Zacute"},{0x017aUL,"zacute"},
    {0x017bUL,"Zdotaccent"},{0x017cUL,"zdotaccent"},
    {0x02c7UL,"caron"},{0x02d8UL,"breve"},
    {0x02d9UL,"dotaccent"},{0x02dbUL,"ogonek"},
    {0x02ddUL,"hungarumlaut"}
};

static size_t ce_count(void)
{
    return sizeof(ce_glyphs) / sizeof(ce_glyphs[0]);
}

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
        if (winansi_high[i] != UNDEFINED_CODEPOINT && winansi_high[i] == codepoint) {
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
    if ((!codepoints && codepoint_count) || !output_len) return AMIPRESS_PDF_FONT_ERR_ARGUMENT;
    if (codepoint_count > output_size || (!output && codepoint_count)) return AMIPRESS_PDF_FONT_ERR_NOSPACE;
    for (i = 0; i < codepoint_count; ++i) {
        rc = amipress_pdf_winansi_code(codepoints[i], &output[i]);
        if (rc != AMIPRESS_PDF_FONT_OK) return rc;
    }
    *output_len = codepoint_count;
    return AMIPRESS_PDF_FONT_OK;
}

size_t amipress_pdf_ce_glyph_count(void)
{
    return ce_count();
}

int amipress_pdf_ce_glyph(size_t index, unsigned char *code,
    unsigned long *codepoint, const char **glyph_name)
{
    if (!code || !codepoint || !glyph_name) return AMIPRESS_PDF_FONT_ERR_ARGUMENT;
    if (index >= ce_count()) return AMIPRESS_PDF_FONT_ERR_UNMAPPABLE;
    *code = (unsigned char)(0x80U + (unsigned int)index);
    *codepoint = ce_glyphs[index].codepoint;
    *glyph_name = ce_glyphs[index].name;
    return AMIPRESS_PDF_FONT_OK;
}

int amipress_pdf_ce_code(unsigned long codepoint, unsigned char *code,
    const char **glyph_name)
{
    size_t i;
    if (!code || !glyph_name) return AMIPRESS_PDF_FONT_ERR_ARGUMENT;
    for (i = 0; i < ce_count(); ++i) {
        if (ce_glyphs[i].codepoint == codepoint) {
            *code = (unsigned char)(0x80U + (unsigned int)i);
            *glyph_name = ce_glyphs[i].name;
            return AMIPRESS_PDF_FONT_OK;
        }
    }
    return AMIPRESS_PDF_FONT_ERR_UNMAPPABLE;
}

int amipress_pdf_font_code(unsigned long codepoint, int *path,
    unsigned char *code, const char **glyph_name)
{
    int rc;
    if (!path || !code || !glyph_name) return AMIPRESS_PDF_FONT_ERR_ARGUMENT;
    rc = amipress_pdf_winansi_code(codepoint, code);
    if (rc == AMIPRESS_PDF_FONT_OK) {
        *path = AMIPRESS_PDF_FONT_PATH_WINANSI;
        *glyph_name = 0;
        return AMIPRESS_PDF_FONT_OK;
    }
    rc = amipress_pdf_ce_code(codepoint, code, glyph_name);
    if (rc == AMIPRESS_PDF_FONT_OK) {
        *path = AMIPRESS_PDF_FONT_PATH_CE;
        return AMIPRESS_PDF_FONT_OK;
    }
    return rc;
}

int amipress_pdf_ce_differences(char *output, size_t output_size,
    size_t *output_len)
{
    size_t i;
    size_t used;
    size_t name_len;
    int n;
    if (!output_len) return AMIPRESS_PDF_FONT_ERR_ARGUMENT;

    used = 0U;
    if (output && output_size) output[0] = '\0';

    n = sprintf(0, "");
    (void)n;

    /* Codes are contiguous, so one starting code is sufficient. */
    if (!output || output_size < 5U) return AMIPRESS_PDF_FONT_ERR_NOSPACE;
    memcpy(output, "[128", 4U);
    used = 4U;

    for (i = 0; i < ce_count(); ++i) {
        name_len = strlen(ce_glyphs[i].name);
        if (used > output_size || name_len > output_size - used ||
            output_size - used <= name_len + 2U)
            return AMIPRESS_PDF_FONT_ERR_NOSPACE;
        output[used++] = ' ';
        output[used++] = '/';
        memcpy(output + used, ce_glyphs[i].name, name_len);
        used += name_len;
    }
    if (output_size - used < 2U) return AMIPRESS_PDF_FONT_ERR_NOSPACE;
    output[used++] = ']';
    output[used] = '\0';
    *output_len = used;
    return AMIPRESS_PDF_FONT_OK;
}
