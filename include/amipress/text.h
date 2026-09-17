#ifndef AMIPRESS_TEXT_H
#define AMIPRESS_TEXT_H

#include <stddef.h>

#define AMIPRESS_TEXT_OK 0
#define AMIPRESS_TEXT_ERR_ARGUMENT 1
#define AMIPRESS_TEXT_ERR_UNMAPPABLE 2
#define AMIPRESS_TEXT_ERR_NOSPACE 3

#define AMIPRESS_STYLE_BOLD 1U
#define AMIPRESS_STYLE_ITALIC 2U
#define AMIPRESS_STYLE_UNDERLINE 4U

enum amipress_encoding {
    AMIPRESS_ENCODING_LATIN1 = 1,
    AMIPRESS_ENCODING_AMIGAPL = 2,
    AMIPRESS_ENCODING_ISO_8859_2 = 3,
    AMIPRESS_ENCODING_WINDOWS_1250 = 4
};

enum amipress_base14_font {
    AMIPRESS_FONT_HELVETICA = 1,
    AMIPRESS_FONT_HELVETICA_BOLD,
    AMIPRESS_FONT_HELVETICA_OBLIQUE,
    AMIPRESS_FONT_HELVETICA_BOLD_OBLIQUE,
    AMIPRESS_FONT_TIMES_ROMAN,
    AMIPRESS_FONT_TIMES_BOLD,
    AMIPRESS_FONT_TIMES_ITALIC,
    AMIPRESS_FONT_TIMES_BOLD_ITALIC,
    AMIPRESS_FONT_COURIER,
    AMIPRESS_FONT_COURIER_BOLD,
    AMIPRESS_FONT_COURIER_OBLIQUE,
    AMIPRESS_FONT_COURIER_BOLD_OBLIQUE,
    AMIPRESS_FONT_SYMBOL,
    AMIPRESS_FONT_ZAPF_DINGBATS
};

const char *amipress_base14_name(enum amipress_base14_font font);
enum amipress_base14_font amipress_base14_style(
    enum amipress_base14_font regular_font, unsigned int style);
int amipress_base14_char_width(enum amipress_base14_font font,
    unsigned char character, unsigned int *width_1000);
int amipress_base14_text_width(enum amipress_base14_font font,
    const unsigned char *text, size_t text_len, int size_pt, int *width_pt);
int amipress_decode_byte(enum amipress_encoding encoding,
    unsigned char input, unsigned long *codepoint);
int amipress_decode_text(enum amipress_encoding encoding,
    const unsigned char *input, size_t input_len,
    unsigned long *output, size_t output_count, size_t *output_len);
int amipress_map_text(enum amipress_encoding encoding,
    const unsigned char *input, size_t input_len,
    unsigned char *output, size_t output_size, size_t *output_len);

#endif
