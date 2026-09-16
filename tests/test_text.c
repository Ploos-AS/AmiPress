#include "amipress/text.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "FAIL: %s\n", message);
    return 1;
}

int main(void)
{
    static const unsigned char latin1[] = {
        'A', 'm', 'i', 'P', 'r', 'e', 's', 's', ' ', 0xc6, 0xd8, 0xc5
    };
    unsigned char output[sizeof(latin1)];
    size_t output_len;

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

    if (amipress_map_text(AMIPRESS_ENCODING_AMIGAPL, latin1,
        sizeof(latin1), output, sizeof(output), &output_len) !=
        AMIPRESS_TEXT_ERR_UNMAPPABLE)
        return fail("unimplemented encoding must be explicit");

    puts("PASS: M3 Base-14 font model, style mapping and Latin-1 core");
    return 0;
}
