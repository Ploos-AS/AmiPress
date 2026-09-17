#include "amipress/pdf_font.h"

#include <stdio.h>
#include <string.h>

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

static int expect_ce(unsigned long cp, const char *expected_name)
{
    unsigned char code = 0;
    const char *name = 0;
    int path = 0;
    int rc = amipress_pdf_font_code(cp, &path, &code, &name);
    if (rc != AMIPRESS_PDF_FONT_OK || path != AMIPRESS_PDF_FONT_PATH_CE ||
        !name || strcmp(name, expected_name) != 0 || code < 0x80U) {
        fprintf(stderr, "CE mapping failed: U+%04lx rc=%d path=%d\n", cp, rc, path);
        return 1;
    }
    return 0;
}

int main(void)
{
    static const char differences_prefix[] = "[128 /Abreve /abreve /Aogonek";
    unsigned long input[4];
    unsigned char output[4];
    char differences[1024];
    char tiny[8];
    size_t output_len = 0;
    size_t differences_len = 0;
    unsigned char code = 0;
    unsigned long cp = 0;
    const char *name = 0;
    int path = 0;
    int rc;

    if (expect_code(0x0041UL, 0x41U)) return 1;
    if (expect_code(0x00e9UL, 0xe9U)) return 1;
    if (expect_code(0x20acUL, 0x80U)) return 1;
    if (expect_code(0x0160UL, 0x8aU)) return 1;
    if (expect_code(0x017eUL, 0x9eU)) return 1;

    rc = amipress_pdf_winansi_code(0x0104UL, &code);
    if (rc != AMIPRESS_PDF_FONT_ERR_UNMAPPABLE) return 1;
    if (expect_ce(0x0104UL, "Aogonek")) return 1;
    if (expect_ce(0x0105UL, "aogonek")) return 1;
    if (expect_ce(0x0141UL, "Lslash")) return 1;
    if (expect_ce(0x0142UL, "lslash")) return 1;
    if (expect_ce(0x015aUL, "Sacute")) return 1;
    if (expect_ce(0x017bUL, "Zdotaccent")) return 1;
    if (expect_ce(0x010cUL, "Ccaron")) return 1;
    if (expect_ce(0x0165UL, "tcaron")) return 1;

    rc = amipress_pdf_ce_glyph(0U, &code, &cp, &name);
    if (rc != AMIPRESS_PDF_FONT_OK || code != 0x80U || cp != 0x0102UL ||
        strcmp(name, "Abreve") != 0) return 1;
    if (amipress_pdf_ce_glyph_count() < 40U) return 1;

    rc = amipress_pdf_ce_differences(differences, sizeof(differences),
        &differences_len);
    if (rc != AMIPRESS_PDF_FONT_OK || differences_len != strlen(differences) ||
        strncmp(differences, differences_prefix, strlen(differences_prefix)) != 0 ||
        strstr(differences, "/Lslash") == 0 ||
        strstr(differences, "/Zdotaccent") == 0 ||
        differences[differences_len - 1U] != ']') {
        fprintf(stderr, "Differences serialization failed\n");
        return 1;
    }
    rc = amipress_pdf_ce_differences(tiny, sizeof(tiny), &differences_len);
    if (rc != AMIPRESS_PDF_FONT_ERR_NOSPACE) {
        fprintf(stderr, "short Differences buffer was not rejected\n");
        return 1;
    }

    rc = amipress_pdf_font_code(0x20acUL, &path, &code, &name);
    if (rc != AMIPRESS_PDF_FONT_OK || path != AMIPRESS_PDF_FONT_PATH_WINANSI ||
        code != 0x80U || name != 0) return 1;
    rc = amipress_pdf_font_code(0x2603UL, &path, &code, &name);
    if (rc != AMIPRESS_PDF_FONT_ERR_UNMAPPABLE) return 1;

    input[0] = 0x0041UL;
    input[1] = 0x20acUL;
    input[2] = 0x00e9UL;
    input[3] = 0x017eUL;
    rc = amipress_pdf_winansi_encode(input, 4U, output, sizeof(output), &output_len);
    if (rc != AMIPRESS_PDF_FONT_OK || output_len != 4U ||
        output[0] != 0x41U || output[1] != 0x80U ||
        output[2] != 0xe9U || output[3] != 0x9eU) return 1;
    rc = amipress_pdf_winansi_encode(input, 4U, output, 3U, &output_len);
    if (rc != AMIPRESS_PDF_FONT_ERR_NOSPACE) return 1;

    puts("pdf font mapper tests passed");
    return 0;
}
