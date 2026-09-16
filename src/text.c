#include "amipress/text.h"

static const char *const base14_names[] = {
    0,
    "Helvetica", "Helvetica-Bold", "Helvetica-Oblique", "Helvetica-BoldOblique",
    "Times-Roman", "Times-Bold", "Times-Italic", "Times-BoldItalic",
    "Courier", "Courier-Bold", "Courier-Oblique", "Courier-BoldOblique",
    "Symbol", "ZapfDingbats"
};

static const unsigned short iso_8859_2_to_unicode[96] = {
    0x00a0,0x0104,0x02d8,0x0141,0x00a4,0x013d,0x015a,0x00a7,0x00a8,0x0160,0x015e,0x0164,0x0179,0x00ad,0x017d,0x017b,
    0x00b0,0x0105,0x02db,0x0142,0x00b4,0x013e,0x015b,0x02c7,0x00b8,0x0161,0x015f,0x0165,0x017a,0x02dd,0x017e,0x017c,
    0x0154,0x00c1,0x00c2,0x0102,0x00c4,0x0139,0x0106,0x00c7,0x010c,0x00c9,0x0118,0x00cb,0x011a,0x00cd,0x00ce,0x010e,
    0x0110,0x0143,0x0147,0x00d3,0x00d4,0x0150,0x00d6,0x00d7,0x0158,0x016e,0x00da,0x0170,0x00dc,0x00dd,0x0162,0x00df,
    0x0155,0x00e1,0x00e2,0x0103,0x00e4,0x013a,0x0107,0x00e7,0x010d,0x00e9,0x0119,0x00eb,0x011b,0x00ed,0x00ee,0x010f,
    0x0111,0x0144,0x0148,0x00f3,0x00f4,0x0151,0x00f6,0x00f7,0x0159,0x016f,0x00fa,0x0171,0x00fc,0x00fd,0x0163,0x02d9
};

static const unsigned short amigapl_to_unicode[96] = {
    0x00a0,0x00a1,0x00a2,0x00a3,0x00a4,0x00a5,0x00a6,0x00a7,0x00a8,0x00a9,0x00aa,0x00ab,0x00ac,0x00ad,0x00ae,0x00af,
    0x00b0,0x00b1,0x00b2,0x00b3,0x00b4,0x00b5,0x00b6,0x00b7,0x00b8,0x00b9,0x00ba,0x00bb,0x00bc,0x00bd,0x00be,0x00bf,
    0x00c0,0x00c1,0x0104,0x00c3,0x00c4,0x00c5,0x00c6,0x00c7,0x00c8,0x00c9,0x0106,0x0118,0x00cc,0x00cd,0x0141,0x0143,
    0x00d0,0x00d1,0x00d2,0x00d3,0x015a,0x00d5,0x00d6,0x00d7,0x00d8,0x00d9,0x0179,0x017b,0x00dc,0x00dd,0x00de,0x00df,
    0x00e0,0x00e1,0x0105,0x00e3,0x00e4,0x00e5,0x00e6,0x00e7,0x00e8,0x00e9,0x0107,0x0119,0x00ec,0x00ed,0x0142,0x0144,
    0x00f0,0x00f1,0x00f2,0x00f3,0x015b,0x00f5,0x00f6,0x00f7,0x00f8,0x00f9,0x017a,0x017c,0x00fc,0x00fd,0x00fe,0x00ff
};

static const unsigned short windows1250_to_unicode[32] = {
    0x20ac,0x0081,0x201a,0x0083,0x201e,0x2026,0x2020,0x2021,
    0x02c6,0x2030,0x0160,0x2039,0x015a,0x0164,0x017d,0x0179,
    0x0090,0x2018,0x2019,0x201c,0x201d,0x2022,0x2013,0x2014,
    0x02dc,0x2122,0x0161,0x203a,0x015b,0x0165,0x017e,0x017a
};

const char *amipress_base14_name(enum amipress_base14_font font)
{
    if ((int)font < (int)AMIPRESS_FONT_HELVETICA || (int)font > (int)AMIPRESS_FONT_ZAPF_DINGBATS)
        return 0;
    return base14_names[(int)font];
}

enum amipress_base14_font amipress_base14_style(enum amipress_base14_font regular_font, unsigned int style)
{
    unsigned int face = style & (AMIPRESS_STYLE_BOLD | AMIPRESS_STYLE_ITALIC);
    if (regular_font == AMIPRESS_FONT_HELVETICA) {
        if (face == (AMIPRESS_STYLE_BOLD | AMIPRESS_STYLE_ITALIC)) return AMIPRESS_FONT_HELVETICA_BOLD_OBLIQUE;
        if (face == AMIPRESS_STYLE_BOLD) return AMIPRESS_FONT_HELVETICA_BOLD;
        if (face == AMIPRESS_STYLE_ITALIC) return AMIPRESS_FONT_HELVETICA_OBLIQUE;
        return AMIPRESS_FONT_HELVETICA;
    }
    if (regular_font == AMIPRESS_FONT_TIMES_ROMAN) {
        if (face == (AMIPRESS_STYLE_BOLD | AMIPRESS_STYLE_ITALIC)) return AMIPRESS_FONT_TIMES_BOLD_ITALIC;
        if (face == AMIPRESS_STYLE_BOLD) return AMIPRESS_FONT_TIMES_BOLD;
        if (face == AMIPRESS_STYLE_ITALIC) return AMIPRESS_FONT_TIMES_ITALIC;
        return AMIPRESS_FONT_TIMES_ROMAN;
    }
    if (regular_font == AMIPRESS_FONT_COURIER) {
        if (face == (AMIPRESS_STYLE_BOLD | AMIPRESS_STYLE_ITALIC)) return AMIPRESS_FONT_COURIER_BOLD_OBLIQUE;
        if (face == AMIPRESS_STYLE_BOLD) return AMIPRESS_FONT_COURIER_BOLD;
        if (face == AMIPRESS_STYLE_ITALIC) return AMIPRESS_FONT_COURIER_OBLIQUE;
        return AMIPRESS_FONT_COURIER;
    }
    return regular_font;
}

int amipress_decode_byte(enum amipress_encoding encoding, unsigned char input, unsigned long *codepoint)
{
    if (!codepoint) return AMIPRESS_TEXT_ERR_ARGUMENT;
    if (input < 0x80U) { *codepoint = (unsigned long)input; return AMIPRESS_TEXT_OK; }
    if (encoding == AMIPRESS_ENCODING_LATIN1) { *codepoint = (unsigned long)input; return AMIPRESS_TEXT_OK; }
    if (encoding == AMIPRESS_ENCODING_AMIGAPL) {
        if (input < 0xa0U) { *codepoint = (unsigned long)input; return AMIPRESS_TEXT_OK; }
        *codepoint = (unsigned long)amigapl_to_unicode[input - 0xa0U]; return AMIPRESS_TEXT_OK;
    }
    if (encoding == AMIPRESS_ENCODING_ISO_8859_2) {
        if (input < 0xa0U) { *codepoint = (unsigned long)input; return AMIPRESS_TEXT_OK; }
        *codepoint = (unsigned long)iso_8859_2_to_unicode[input - 0xa0U]; return AMIPRESS_TEXT_OK;
    }
    if (encoding == AMIPRESS_ENCODING_WINDOWS_1250) {
        if (input < 0xa0U) {
            if (input >= 0x80U && input <= 0x9fU) *codepoint = (unsigned long)windows1250_to_unicode[input - 0x80U];
            else *codepoint = (unsigned long)input;
        } else {
            *codepoint = (unsigned long)iso_8859_2_to_unicode[input - 0xa0U];
        }
        return AMIPRESS_TEXT_OK;
    }
    return AMIPRESS_TEXT_ERR_UNMAPPABLE;
}

int amipress_map_text(enum amipress_encoding encoding, const unsigned char *input, size_t input_len,
    unsigned char *output, size_t output_size, size_t *output_len)
{
    size_t i;
    unsigned long cp;
    if ((!input && input_len) || !output_len) return AMIPRESS_TEXT_ERR_ARGUMENT;
    if (input_len > output_size || (!output && input_len)) return AMIPRESS_TEXT_ERR_NOSPACE;
    for (i = 0; i < input_len; ++i) {
        if (amipress_decode_byte(encoding, input[i], &cp) != AMIPRESS_TEXT_OK) return AMIPRESS_TEXT_ERR_UNMAPPABLE;
        if (cp > 0xffUL) return AMIPRESS_TEXT_ERR_UNMAPPABLE;
        output[i] = (unsigned char)cp;
    }
    *output_len = input_len;
    return AMIPRESS_TEXT_OK;
}
