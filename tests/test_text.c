#include "amipress/text.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "FAIL: %s\n", message);
    return 1;
}

static int expect_codepoint(enum amipress_encoding encoding,
    unsigned char input, unsigned long expected)
{
    unsigned long codepoint;
    if (amipress_decode_byte(encoding, input, &codepoint) != AMIPRESS_TEXT_OK)
        return 0;
    return codepoint == expected;
}

int main(void)
{
    static const unsigned char latin1[] = {
        'A', 'm', 'i', 'P', 'r', 'e', 's', 's', ' ', 0xc6, 0xd8, 0xc5
    };
    unsigned char output[sizeof(latin1)];
    size_t output_len;
    unsigned long codepoint;

    if (strcmp(amipress_base14_name(AMIPRESS_FONT_HELVETICA),
        "Helvetica") != 0)
        return fail("Helvetica Base-14 name");
    if (strcmp(amipress_base14_name(AMIPRESS_FONT_TIMES_ROMAN),
        "Times-Roman") != 0)
        return fail("Times-Roman Base-14 name");
    if (strcmp(amipress_base14_name(AMIPRESS_FONT_COURIER),
        "Courier") != 0)
        return fail("Courier Base-14 name");
    if (strcmp(amipress_base14_name(AMIPRESS_FONT_SYMBOL),
        "Symbol") != 0)
        return fail("Symbol Base-14 name");
    if (strcmp(amipress_base14_name(AMIPRESS_FONT_ZAPF_DINGBATS),
        "ZapfDingbats") != 0)
        return fail("ZapfDingbats Base-14 name");

    if (amipress_base14_style(AMIPRESS_FONT_HELVETICA,
        AMIPRESS_STYLE_BOLD | AMIPRESS_STYLE_ITALIC) !=
        AMIPRESS_FONT_HELVETICA_BOLD_OBLIQUE)
        return fail("Helvetica bold italic mapping");
    if (amipress_base14_style(AMIPRESS_FONT_TIMES_ROMAN,
        AMIPRESS_STYLE_ITALIC) != AMIPRESS_FONT_TIMES_ITALIC)
        return fail("Times italic mapping");
    if (amipress_base14_style(AMIPRESS_FONT_COURIER,
        AMIPRESS_STYLE_BOLD) != AMIPRESS_FONT_COURIER_BOLD)
        return fail("Courier bold mapping");

    output_len = 0;
    if (amipress_map_text(AMIPRESS_ENCODING_LATIN1, latin1,
        sizeof(latin1), output, sizeof(output), &output_len) !=
        AMIPRESS_TEXT_OK)
        return fail("Latin-1 mapping returned error");
    if (output_len != sizeof(latin1) ||
        memcmp(output, latin1, sizeof(latin1)) != 0)
        return fail("Latin-1 mapping changed bytes");

    if (!expect_codepoint(AMIPRESS_ENCODING_LATIN1, 0xc6, 0x00c6UL))
        return fail("Latin-1 Unicode decode");

    if (!expect_codepoint(AMIPRESS_ENCODING_AMIGAPL, 0xc2, 0x0104UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_AMIGAPL, 0xca, 0x0106UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_AMIGAPL, 0xce, 0x0141UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_AMIGAPL, 0xd4, 0x015aUL) ||
        !expect_codepoint(AMIPRESS_ENCODING_AMIGAPL, 0xda, 0x0179UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_AMIGAPL, 0xdb, 0x017bUL) ||
        !expect_codepoint(AMIPRESS_ENCODING_AMIGAPL, 0xe2, 0x0105UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_AMIGAPL, 0xea, 0x0107UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_AMIGAPL, 0xee, 0x0142UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_AMIGAPL, 0xf4, 0x015bUL) ||
        !expect_codepoint(AMIPRESS_ENCODING_AMIGAPL, 0xfa, 0x017aUL) ||
        !expect_codepoint(AMIPRESS_ENCODING_AMIGAPL, 0xfb, 0x017cUL))
        return fail("AmigaPL Polish character decoding");

    if (!expect_codepoint(AMIPRESS_ENCODING_ISO_8859_2, 0xa1, 0x0104UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_ISO_8859_2, 0xa3, 0x0141UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_ISO_8859_2, 0xac, 0x0179UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_ISO_8859_2, 0xb1, 0x0105UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_ISO_8859_2, 0xb3, 0x0142UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_ISO_8859_2, 0xbc, 0x017aUL))
        return fail("ISO-8859-2 character decoding");

    if (!expect_codepoint(AMIPRESS_ENCODING_WINDOWS_1250, 0x80, 0x20acUL) ||
        !expect_codepoint(AMIPRESS_ENCODING_WINDOWS_1250, 0x8c, 0x015aUL) ||
        !expect_codepoint(AMIPRESS_ENCODING_WINDOWS_1250, 0xa1, 0x02c7UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_WINDOWS_1250, 0xa5, 0x0104UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_WINDOWS_1250, 0xaf, 0x017bUL) ||
        !expect_codepoint(AMIPRESS_ENCODING_WINDOWS_1250, 0xb9, 0x0105UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_WINDOWS_1250, 0xbf, 0x017cUL) ||
        !expect_codepoint(AMIPRESS_ENCODING_WINDOWS_1250, 0xc0, 0x0154UL) ||
        !expect_codepoint(AMIPRESS_ENCODING_WINDOWS_1250, 0xff, 0x02d9UL))
        return fail("Windows-1250 character decoding");

    if (amipress_decode_byte(AMIPRESS_ENCODING_WINDOWS_1250,
        0x81, &codepoint) != AMIPRESS_TEXT_ERR_UNMAPPABLE ||
        amipress_decode_byte(AMIPRESS_ENCODING_WINDOWS_1250,
        0x83, &codepoint) != AMIPRESS_TEXT_ERR_UNMAPPABLE ||
        amipress_decode_byte(AMIPRESS_ENCODING_WINDOWS_1250,
        0x88, &codepoint) != AMIPRESS_TEXT_ERR_UNMAPPABLE ||
        amipress_decode_byte(AMIPRESS_ENCODING_WINDOWS_1250,
        0x90, &codepoint) != AMIPRESS_TEXT_ERR_UNMAPPABLE ||
        amipress_decode_byte(AMIPRESS_ENCODING_WINDOWS_1250,
        0x98, &codepoint) != AMIPRESS_TEXT_ERR_UNMAPPABLE)
        return fail("Windows-1250 undefined bytes");

    puts("PASS: M3 Base-14 and all planned single-byte decoding cores");
    return 0;
}
