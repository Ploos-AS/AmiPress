#include "amipress/text.h"

static const char *const base14_names[] = {
    0,
    "Helvetica",
    "Helvetica-Bold",
    "Helvetica-Oblique",
    "Helvetica-BoldOblique",
    "Times-Roman",
    "Times-Bold",
    "Times-Italic",
    "Times-BoldItalic",
    "Courier",
    "Courier-Bold",
    "Courier-Oblique",
    "Courier-BoldOblique",
    "Symbol",
    "ZapfDingbats"
};

const char *amipress_base14_name(enum amipress_base14_font font)
{
    if ((int)font < (int)AMIPRESS_FONT_HELVETICA ||
        (int)font > (int)AMIPRESS_FONT_ZAPF_DINGBATS)
        return 0;
    return base14_names[(int)font];
}

enum amipress_base14_font amipress_base14_style(
    enum amipress_base14_font regular_font, unsigned int style)
{
    unsigned int face;

    face = style & (AMIPRESS_STYLE_BOLD | AMIPRESS_STYLE_ITALIC);
    if (regular_font == AMIPRESS_FONT_HELVETICA) {
        if (face == (AMIPRESS_STYLE_BOLD | AMIPRESS_STYLE_ITALIC))
            return AMIPRESS_FONT_HELVETICA_BOLD_OBLIQUE;
        if (face == AMIPRESS_STYLE_BOLD)
            return AMIPRESS_FONT_HELVETICA_BOLD;
        if (face == AMIPRESS_STYLE_ITALIC)
            return AMIPRESS_FONT_HELVETICA_OBLIQUE;
        return AMIPRESS_FONT_HELVETICA;
    }
    if (regular_font == AMIPRESS_FONT_TIMES_ROMAN) {
        if (face == (AMIPRESS_STYLE_BOLD | AMIPRESS_STYLE_ITALIC))
            return AMIPRESS_FONT_TIMES_BOLD_ITALIC;
        if (face == AMIPRESS_STYLE_BOLD)
            return AMIPRESS_FONT_TIMES_BOLD;
        if (face == AMIPRESS_STYLE_ITALIC)
            return AMIPRESS_FONT_TIMES_ITALIC;
        return AMIPRESS_FONT_TIMES_ROMAN;
    }
    if (regular_font == AMIPRESS_FONT_COURIER) {
        if (face == (AMIPRESS_STYLE_BOLD | AMIPRESS_STYLE_ITALIC))
            return AMIPRESS_FONT_COURIER_BOLD_OBLIQUE;
        if (face == AMIPRESS_STYLE_BOLD)
            return AMIPRESS_FONT_COURIER_BOLD;
        if (face == AMIPRESS_STYLE_ITALIC)
            return AMIPRESS_FONT_COURIER_OBLIQUE;
        return AMIPRESS_FONT_COURIER;
    }
    return regular_font;
}

int amipress_map_text(enum amipress_encoding encoding,
    const unsigned char *input, size_t input_len,
    unsigned char *output, size_t output_size, size_t *output_len)
{
    size_t i;

    if ((!input && input_len) || !output_len)
        return AMIPRESS_TEXT_ERR_ARGUMENT;
    if (input_len > output_size || (!output && input_len))
        return AMIPRESS_TEXT_ERR_NOSPACE;

    if (encoding != AMIPRESS_ENCODING_LATIN1)
        return AMIPRESS_TEXT_ERR_UNMAPPABLE;

    for (i = 0; i < input_len; ++i)
        output[i] = input[i];
    *output_len = input_len;
    return AMIPRESS_TEXT_OK;
}
