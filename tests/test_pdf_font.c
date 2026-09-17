#include "amipress/pdf_font.h"

#include <stdio.h>

static int expect_code(unsigned long cp, unsigned int expected)
{
    unsigned char code = 0;
    int rc = amipress_pdf_winansi_code(cp, &code);
    if (rc != AMIPRESS_PDF_FONT_OK || code != (unsigned char)expected) {
        fprintf(stderr, "mapping failed: U+%04lx rc=%d code=%u expected=%u\n",
            cp, rc, (unsigned int)code, expected);
        return 1;
    }
    return 0;
}

int main(void)
{
    unsigned long input[4];
    unsigned char output[4];
    size_t output_len = 0;
    unsigned char code = 0;
    int rc;

    if (expect_code(0x0041UL, 0x41U)) return 1;
    if (expect_code(0x00e9UL, 0xe9U)) return 1;
    if (expect_code(0x20acUL, 0x80U)) return 1;
    if (expect_code(0x0160UL, 0x8aU)) return 1;
    if (expect_code(0x017eUL, 0x9eU)) return 1;

    rc = amipress_pdf_winansi_code(0x0104UL, &code);
    if (rc != AMIPRESS_PDF_FONT_ERR_UNMAPPABLE) {
        fprintf(stderr, "A-ogonek must remain unmappable in WinAnsi baseline\n");
        return 1;
    }

    input[0] = 0x0041UL;
    input[1] = 0x20acUL;
    input[2] = 0x00e9UL;
    input[3] = 0x017eUL;
    rc = amipress_pdf_winansi_encode(input, 4U, output, sizeof(output), &output_len);
    if (rc != AMIPRESS_PDF_FONT_OK || output_len != 4U ||
        output[0] != 0x41U || output[1] != 0x80U ||
        output[2] != 0xe9U || output[3] != 0x9eU) {
        fprintf(stderr, "buffer mapping failed\n");
        return 1;
    }

    rc = amipress_pdf_winansi_encode(input, 4U, output, 3U, &output_len);
    if (rc != AMIPRESS_PDF_FONT_ERR_NOSPACE) {
        fprintf(stderr, "short output buffer was not rejected\n");
        return 1;
    }

    puts("pdf font mapper tests passed");
    return 0;
}
