#include "amipress/pdf.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message)
{
    fprintf(stderr, "FAIL: %s\n", message);
    return 1;
}

static int check_file(FILE *fp)
{
    char buf[16384];
    size_t n;
    long size;

    if (fseek(fp, 0, SEEK_END) != 0)
        return fail("cannot seek to end of generated PDF");
    size = ftell(fp);
    if (size < 0 || size >= (long)sizeof(buf))
        return fail("generated PDF size is invalid or exceeds test buffer");
    if (fseek(fp, 0, SEEK_SET) != 0)
        return fail("cannot rewind generated PDF");
    n = fread(buf, 1, (size_t)size, fp);
    if (n != (size_t)size)
        return fail("cannot read complete generated PDF");
    buf[n] = '\0';

    if (strncmp(buf, "%PDF-1.4\n", 9) != 0)
        return fail("missing PDF 1.4 header");
    if (strstr(buf, "/Type /Catalog") == NULL)
        return fail("missing Catalog object");
    if (strstr(buf, "/Type /Pages /Count 2") == NULL)
        return fail("missing two-page Pages tree");
    if (strstr(buf, "Hello AmiPress") == NULL)
        return fail("missing first-page text");
    if (strstr(buf, "Second page") == NULL)
        return fail("missing second-page text");
    if (strstr(buf, "/Subtype /Image") == NULL)
        return fail("missing image XObject");
    if (strstr(buf, "/ColorSpace /DeviceRGB") == NULL)
        return fail("missing RGB image colorspace");
    if (strstr(buf, "/Filter /FlateDecode") == NULL)
        return fail("missing FlateDecode image filter");
    if (strstr(buf, "/XObject << /Im1") == NULL)
        return fail("missing Im1 page resource");
    if (strstr(buf, "/Title (AmiPress M2 Test)") == NULL)
        return fail("missing Title metadata");
    if (strstr(buf, "/Author (Ploos-AS)") == NULL)
        return fail("missing Author metadata");
    if (strstr(buf, "/Creator (AmiPress test suite)") == NULL)
        return fail("missing Creator metadata");
    if (strstr(buf, "/Producer (AmiPress PDF backend)") == NULL)
        return fail("missing Producer metadata");
    if (strstr(buf, "/Info ") == NULL)
        return fail("missing trailer Info reference");
    if (strstr(buf, "xref\n") == NULL)
        return fail("missing xref table");
    if (strstr(buf, "startxref\n") == NULL)
        return fail("missing startxref");
    if (strstr(buf, "%%EOF") == NULL)
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
