#include "amipress/pdf.h"

#include <stdio.h>
#include <string.h>

static int check_file(FILE *fp)
{
    char buf[16384]; size_t n; long size;
    if (fseek(fp, 0, SEEK_END) != 0) return 1;
    size = ftell(fp); if (size < 0 || size >= (long)sizeof(buf)) return 1;
    if (fseek(fp, 0, SEEK_SET) != 0) return 1;
    n = fread(buf, 1, (size_t)size, fp); if (n != (size_t)size) return 1;
    buf[n] = '\0';
    if (strncmp(buf, "%PDF-1.4\n", 9) != 0) return 1;
    if (strstr(buf, "/Type /Catalog") == NULL) return 1;
    if (strstr(buf, "/Type /Pages /Count 2") == NULL) return 1;
    if (strstr(buf, "Hello AmiPress") == NULL) return 1;
    if (strstr(buf, "Second page") == NULL) return 1;
    if (strstr(buf, "/Subtype /Image") == NULL) return 1;
    if (strstr(buf, "/ColorSpace /DeviceRGB") == NULL) return 1;
    if (strstr(buf, "/Filter /FlateDecode") == NULL) return 1;
    if (strstr(buf, "/XObject << /Im1") == NULL) return 1;
    if (strstr(buf, "/Title (AmiPress M2 Test)") == NULL) return 1;
    if (strstr(buf, "/Author (Ploos-AS)") == NULL) return 1;
    if (strstr(buf, "/Creator (AmiPress test suite)") == NULL) return 1;
    if (strstr(buf, "/Producer (AmiPress PDF backend)") == NULL) return 1;
    if (strstr(buf, "/Info ") == NULL) return 1;
    if (strstr(buf, "xref\n") == NULL) return 1;
    if (strstr(buf, "startxref\n") == NULL) return 1;
    if (strstr(buf, "%%EOF") == NULL) return 1;
    return 0;
}

int main(void)
{
    struct amipdf pdf; FILE *fp; int rc;
    static const unsigned char image[12] = {
        255, 0, 0, 0, 255, 0,
        0, 0, 255, 255, 255, 0
    };
    fp = tmpfile(); if (!fp) return 1;
    rc = amipdf_init(&pdf, fp); if (rc != AMIPDF_OK) return 1;
    if (amipdf_set_metadata(&pdf, "AmiPress M2 Test", "Ploos-AS",
        "AmiPress test suite", "AmiPress PDF backend") != AMIPDF_OK) return 1;
    if (amipdf_set_page_size(&pdf, 612, 792) != AMIPDF_OK) return 1;
    if (amipdf_begin_page(&pdf) != AMIPDF_OK) return 1;
    if (amipdf_text(&pdf, 72, 720, "Hello AmiPress") != AMIPDF_OK) return 1;
    if (amipdf_image_rgb(&pdf, 72, 500, 2, 2, image) != AMIPDF_OK) return 1;
    if (amipdf_end_page(&pdf) != AMIPDF_OK) return 1;
    if (amipdf_begin_page(&pdf) != AMIPDF_OK) return 1;
    if (amipdf_text(&pdf, 72, 720, "Second page") != AMIPDF_OK) return 1;
    if (amipdf_end_page(&pdf) != AMIPDF_OK) return 1;
    if (amipdf_finish(&pdf) != AMIPDF_OK) return 1;
    if (check_file(fp) != 0) return 1;
    amipdf_dispose(&pdf); fclose(fp);
    puts("PASS: PDF 1.4 text, RGB image, Flate and metadata serialization");
    return 0;
}
