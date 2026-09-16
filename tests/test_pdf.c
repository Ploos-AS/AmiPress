#include "amipress/pdf.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "FAIL: %s\n", message);
    return 1;
}

static int contains_bytes(const unsigned char *buf, size_t len,
    const char *needle)
{
    size_t needle_len;
    size_t i;

    needle_len = strlen(needle);
    if (needle_len == 0)
        return 1;
    if (needle_len > len)
        return 0;
    for (i = 0; i <= len - needle_len; ++i) {
        if (memcmp(buf + i, needle, needle_len) == 0)
            return 1;
    }
    return 0;
}

static int check_file(FILE *fp)
{
    unsigned char buf[16384];
    size_t n;
    long size;

    if (fseek(fp, 0, SEEK_END) != 0)
        return fail("cannot seek to end of generated PDF");
    size = ftell(fp);
    if (size < 0 || size > (long)sizeof(buf))
        return fail("generated PDF size is invalid or exceeds test buffer");
    if (fseek(fp, 0, SEEK_SET) != 0)
        return fail("cannot rewind generated PDF");
    n = fread(buf, 1, (size_t)size, fp);
    if (n != (size_t)size)
        return fail("cannot read complete generated PDF");

    if (n < 9 || memcmp(buf, "%PDF-1.4\n", 9) != 0)
        return fail("missing PDF 1.4 header");
    if (!contains_bytes(buf, n, "/Type /Catalog"))
        return fail("missing Catalog object");
    if (!contains_bytes(buf, n, "/Type /Pages /Count 2"))
        return fail("missing two-page Pages tree");
    if (!contains_bytes(buf, n, "Hello AmiPress"))
        return fail("missing first-page text");
    if (!contains_bytes(buf, n, "Second page"))
        return fail("missing second-page text");
    if (!contains_bytes(buf, n, "/Subtype /Image"))
        return fail("missing image XObject");
    if (!contains_bytes(buf, n, "/ColorSpace /DeviceRGB"))
        return fail("missing RGB image colorspace");
    if (!contains_bytes(buf, n, "/Filter /FlateDecode"))
        return fail("missing FlateDecode image filter");
    if (!contains_bytes(buf, n, "/XObject << /Im1"))
        return fail("missing Im1 page resource");
    if (!contains_bytes(buf, n, "/Title (AmiPress M2 Test)"))
        return fail("missing Title metadata");
    if (!contains_bytes(buf, n, "/Author (Ploos-AS)"))
        return fail("missing Author metadata");
    if (!contains_bytes(buf, n, "/Creator (AmiPress test suite)"))
        return fail("missing Creator metadata");
    if (!contains_bytes(buf, n, "/Producer (AmiPress PDF backend)"))
        return fail("missing Producer metadata");
    if (!contains_bytes(buf, n, "/Info "))
        return fail("missing trailer Info reference");
    if (!contains_bytes(buf, n, "xref\n"))
        return fail("missing xref table");
    if (!contains_bytes(buf, n, "startxref\n"))
        return fail("missing startxref");
    if (!contains_bytes(buf, n, "%%EOF"))
        return fail("missing EOF marker");
    return 0;
}

int main(void)
{
    struct amipdf pdf;
    FILE *fp;
    int rc;
    static const unsigned char image[12] = {
        255, 0, 0, 0, 255, 0,
        0, 0, 255, 255, 255, 0
    };

    fp = tmpfile();
    if (!fp)
        return fail("tmpfile failed");
    rc = amipdf_init(&pdf, fp);
    if (rc != AMIPDF_OK)
        return fail("amipdf_init failed");
    if (amipdf_set_metadata(&pdf, "AmiPress M2 Test", "Ploos-AS",
        "AmiPress test suite", "AmiPress PDF backend") != AMIPDF_OK)
        return fail("amipdf_set_metadata failed");
    if (amipdf_set_page_size(&pdf, 612, 792) != AMIPDF_OK)
        return fail("amipdf_set_page_size failed");
    if (amipdf_begin_page(&pdf) != AMIPDF_OK)
        return fail("first amipdf_begin_page failed");
    if (amipdf_text(&pdf, 72, 720, "Hello AmiPress") != AMIPDF_OK)
        return fail("first amipdf_text failed");
    if (amipdf_image_rgb(&pdf, 72, 500, 2, 2, image) != AMIPDF_OK)
        return fail("amipdf_image_rgb failed");
    if (amipdf_end_page(&pdf) != AMIPDF_OK)
        return fail("first amipdf_end_page failed");
    if (amipdf_begin_page(&pdf) != AMIPDF_OK)
        return fail("second amipdf_begin_page failed");
    if (amipdf_text(&pdf, 72, 720, "Second page") != AMIPDF_OK)
        return fail("second amipdf_text failed");
    if (amipdf_end_page(&pdf) != AMIPDF_OK)
        return fail("second amipdf_end_page failed");
    if (amipdf_finish(&pdf) != AMIPDF_OK)
        return fail("amipdf_finish failed");
    if (check_file(fp) != 0)
        return 1;

    amipdf_dispose(&pdf);
    fclose(fp);
    puts("PASS: PDF 1.4 text, RGB image, Flate and metadata serialization");
    return 0;
}
