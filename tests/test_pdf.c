#include "amipress/pdf.h"

#include <stdio.h>
#include <string.h>

static int fail(const char *message) { fprintf(stderr, "FAIL: %s\n", message); return 1; }
static int contains_bytes(const unsigned char *buf, size_t len, const char *needle)
{
    size_t needle_len = strlen(needle); size_t i;
    if (needle_len == 0) return 1; if (needle_len > len) return 0;
    for (i = 0; i <= len - needle_len; ++i) if (memcmp(buf + i, needle, needle_len) == 0) return 1;
    return 0;
}
static int check_file(FILE *fp)
{
    unsigned char buf[16384]; size_t n; long size;
    if (fseek(fp, 0, SEEK_END) != 0) return fail("cannot seek to end of generated PDF");
    size = ftell(fp); if (size < 0 || size > (long)sizeof(buf)) return fail("generated PDF size is invalid or exceeds test buffer");
    if (fseek(fp, 0, SEEK_SET) != 0) return fail("cannot rewind generated PDF");
    n = fread(buf, 1, (size_t)size, fp); if (n != (size_t)size) return fail("cannot read complete generated PDF");
    if (n < 9 || memcmp(buf, "%PDF-1.4\n", 9) != 0) return fail("missing PDF 1.4 header");
    if (!contains_bytes(buf,n,"/Type /Catalog") || !contains_bytes(buf,n,"/Type /Pages /Count 2")) return fail("missing PDF structure");
    if (!contains_bytes(buf,n,"Hello AmiPress") || !contains_bytes(buf,n,"Encoded ASCII")) return fail("missing text");
    if (!contains_bytes(buf,n,"Styled text") || !contains_bytes(buf,n,"/BaseFont /Helvetica-BoldOblique") || !contains_bytes(buf,n,"BT /F4 18 Tf")) return fail("missing styled text");
    if (!contains_bytes(buf,n," m ") || !contains_bytes(buf,n," l S")) return fail("missing underline path");
    if (!contains_bytes(buf,n,"Second page")) return fail("missing second-page text");
    if (!contains_bytes(buf,n,"/Subtype /Image") || !contains_bytes(buf,n,"/ColorSpace /DeviceRGB") || !contains_bytes(buf,n,"/Filter /FlateDecode") || !contains_bytes(buf,n,"/XObject << /Im1")) return fail("missing image resources");
    if (!contains_bytes(buf,n,"/Title (AmiPress M2 Test)") || !contains_bytes(buf,n,"/Author (Ploos-AS)") || !contains_bytes(buf,n,"/Creator (AmiPress test suite)") || !contains_bytes(buf,n,"/Producer (AmiPress PDF backend)") || !contains_bytes(buf,n," /Info ")) return fail("missing metadata");
    if (!contains_bytes(buf,n,"xref\n") || !contains_bytes(buf,n,"startxref\n") || !contains_bytes(buf,n,"%%EOF")) return fail("missing PDF trailer");
    return 0;
}
int main(void)
{
    struct amipdf pdf; FILE *fp; int rc;
    static const unsigned char image[12] = {255,0,0,0,255,0,0,0,255,255,255,0};
    static const unsigned char encoded_ascii[] = "Encoded ASCII";
    static const unsigned char cp1250_non_ascii[] = {0xa5};
    fp = tmpfile(); if (!fp) return fail("tmpfile failed");
    rc = amipdf_init(&pdf, fp); if (rc != AMIPDF_OK) return fail("amipdf_init failed");
    if (amipdf_set_metadata(&pdf,"AmiPress M2 Test","Ploos-AS","AmiPress test suite","AmiPress PDF backend") != AMIPDF_OK) return fail("metadata failed");
    if (amipdf_set_page_size(&pdf,612,792) != AMIPDF_OK || amipdf_begin_page(&pdf) != AMIPDF_OK) return fail("page setup failed");
    if (amipdf_text(&pdf,72,720,"Hello AmiPress") != AMIPDF_OK) return fail("text failed");
    if (amipdf_text_encoded(&pdf,72,700,encoded_ascii,sizeof(encoded_ascii)-1,AMIPRESS_ENCODING_WINDOWS_1250,12,0U) != AMIPDF_OK) return fail("encoded ASCII bridge failed");
    if (amipdf_text_encoded(&pdf,72,690,cp1250_non_ascii,sizeof(cp1250_non_ascii),AMIPRESS_ENCODING_WINDOWS_1250,12,0U) != AMIPDF_ERR_UNMAPPABLE) return fail("unsafe Base-14 codepoint was not rejected");
    if (amipdf_text_styled(&pdf,72,680,"Styled text",18,AMIPRESS_STYLE_BOLD|AMIPRESS_STYLE_ITALIC|AMIPRESS_STYLE_UNDERLINE) != AMIPDF_OK) return fail("styled text failed");
    if (amipdf_image_rgb(&pdf,72,500,2,2,image) != AMIPDF_OK || amipdf_end_page(&pdf) != AMIPDF_OK) return fail("first page finish failed");
    if (amipdf_begin_page(&pdf) != AMIPDF_OK || amipdf_text(&pdf,72,720,"Second page") != AMIPDF_OK || amipdf_end_page(&pdf) != AMIPDF_OK) return fail("second page failed");
    if (amipdf_finish(&pdf) != AMIPDF_OK) return fail("amipdf_finish failed");
    if (check_file(fp) != 0) return 1;
    amipdf_dispose(&pdf); fclose(fp);
    puts("PASS: PDF encoded-text bridge, styles, image, Flate and metadata");
    return 0;
}
