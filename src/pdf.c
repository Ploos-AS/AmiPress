#include "amipress/pdf.h"

#include <stdlib.h>
#include <string.h>

static int reserve_offsets(struct amipdf *pdf, size_t need)
{
    long *p; size_t cap;
    if (need <= pdf->offset_capacity) return AMIPDF_OK;
    cap = pdf->offset_capacity ? pdf->offset_capacity : 16;
    while (cap < need) cap *= 2;
    p = (long *)realloc(pdf->offsets, cap * sizeof(*p));
    if (!p) return AMIPDF_ERR_MEMORY;
    pdf->offsets = p; pdf->offset_capacity = cap; return AMIPDF_OK;
}

static int reserve_pages(struct amipdf *pdf, size_t need)
{
    struct amipdf_page_data *p; size_t cap;
    if (need <= pdf->page_capacity) return AMIPDF_OK;
    cap = pdf->page_capacity ? pdf->page_capacity : 4;
    while (cap < need) cap *= 2;
    p = (struct amipdf_page_data *)realloc(pdf->pages, cap * sizeof(*p));
    if (!p) return AMIPDF_ERR_MEMORY;
    memset(p + pdf->page_capacity, 0, (cap - pdf->page_capacity) * sizeof(*p));
    pdf->pages = p; pdf->page_capacity = cap; return AMIPDF_OK;
}

static int reserve_images(struct amipdf *pdf, size_t need)
{
    struct amipdf_image *p; size_t cap;
    if (need <= pdf->image_capacity) return AMIPDF_OK;
    cap = pdf->image_capacity ? pdf->image_capacity : 4;
    while (cap < need) cap *= 2;
    p = (struct amipdf_image *)realloc(pdf->images, cap * sizeof(*p));
    if (!p) return AMIPDF_ERR_MEMORY;
    memset(p + pdf->image_capacity, 0, (cap - pdf->image_capacity) * sizeof(*p));
    pdf->images = p; pdf->image_capacity = cap; return AMIPDF_OK;
}

static int reserve_stream(struct amipdf *pdf, size_t extra)
{
    char *p; size_t need, cap;
    need = pdf->stream_len + extra + 1;
    if (need <= pdf->stream_capacity) return AMIPDF_OK;
    cap = pdf->stream_capacity ? pdf->stream_capacity : 256;
    while (cap < need) cap *= 2;
    p = (char *)realloc(pdf->stream, cap);
    if (!p) return AMIPDF_ERR_MEMORY;
    pdf->stream = p; pdf->stream_capacity = cap; return AMIPDF_OK;
}

static int stream_append(struct amipdf *pdf, const char *s, size_t n)
{
    if (reserve_stream(pdf, n) != AMIPDF_OK) return AMIPDF_ERR_MEMORY;
    memcpy(pdf->stream + pdf->stream_len, s, n);
    pdf->stream_len += n; pdf->stream[pdf->stream_len] = '\0'; return AMIPDF_OK;
}

static int stream_printf(struct amipdf *pdf, const char *format, int a, int b)
{
    char buf[160]; int n;
    n = sprintf(buf, format, a, b);
    if (n < 0 || (size_t)n >= sizeof(buf)) return AMIPDF_ERR_IO;
    return stream_append(pdf, buf, (size_t)n);
}

static int next_object(struct amipdf *pdf)
{
    int number = (int)pdf->offset_count; ++pdf->offset_count; return number;
}

static int object_begin(struct amipdf *pdf, int number)
{
    if (reserve_offsets(pdf, (size_t)number + 1) != AMIPDF_OK) return AMIPDF_ERR_MEMORY;
    pdf->offsets[number] = ftell(pdf->out);
    if (pdf->offsets[number] < 0) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "%d 0 obj\n", number) < 0) return AMIPDF_ERR_IO;
    return AMIPDF_OK;
}

static int pdf_escape_stream(struct amipdf *pdf, const char *text)
{
    const unsigned char *p = (const unsigned char *)text; char esc[5];
    while (*p) {
        if (*p == '(' || *p == ')' || *p == '\\')
            if (stream_append(pdf, "\\", 1) != AMIPDF_OK) return AMIPDF_ERR_MEMORY;
        if (*p < 32 || *p > 126) {
            sprintf(esc, "\\%03o", (unsigned int)*p);
            if (stream_append(pdf, esc, 4) != AMIPDF_OK) return AMIPDF_ERR_MEMORY;
        } else if (stream_append(pdf, (const char *)p, 1) != AMIPDF_OK) return AMIPDF_ERR_MEMORY;
        ++p;
    }
    return AMIPDF_OK;
}

static unsigned long adler32(const unsigned char *data, size_t len)
{
    unsigned long a = 1, b = 0; size_t i;
    for (i = 0; i < len; ++i) { a = (a + data[i]) % 65521UL; b = (b + a) % 65521UL; }
    return (b << 16) | a;
}

/* Valid zlib/Flate stream using stored DEFLATE blocks.  This keeps the
 * backend dependency-free on classic Amiga targets while still producing
 * standards-compliant /FlateDecode image streams. */
static unsigned char *flate_store(const unsigned char *data, size_t len, size_t *out_len)
{
    unsigned char *out; size_t blocks, n, pos, remaining, block; unsigned long ad;
    blocks = len ? (len + 65534U) / 65535U : 1U;
    if (len > ((size_t)-1) - 6U - blocks * 5U) return NULL;
    n = 2U + len + blocks * 5U + 4U;
    out = (unsigned char *)malloc(n);
    if (!out) return NULL;
    out[0] = 0x78; out[1] = 0x01; pos = 2; remaining = len;
    if (!len) { out[pos++] = 1; out[pos++] = 0; out[pos++] = 0xff; out[pos++] = 0xff; }
    while (remaining) {
        block = remaining > 65535U ? 65535U : remaining;
        out[pos++] = (block == remaining) ? 1U : 0U;
        out[pos++] = (unsigned char)(block & 255U); out[pos++] = (unsigned char)(block >> 8);
        out[pos++] = (unsigned char)(~block & 255U); out[pos++] = (unsigned char)((~block >> 8) & 255U);
        memcpy(out + pos, data + (len - remaining), block); pos += block; remaining -= block;
    }
    ad = adler32(data, len);
    out[pos++] = (unsigned char)(ad >> 24); out[pos++] = (unsigned char)(ad >> 16);
    out[pos++] = (unsigned char)(ad >> 8); out[pos++] = (unsigned char)ad;
    *out_len = pos; return out;
}

static int write_header(struct amipdf *pdf)
{
    return fputs("%PDF-1.4\n%\xE2\xE3\xCF\xD3\n", pdf->out) == EOF ? AMIPDF_ERR_IO : AMIPDF_OK;
}

int amipdf_init(struct amipdf *pdf, FILE *out)
{
    if (!pdf || !out) return AMIPDF_ERR_ARGUMENT;
    memset(pdf, 0, sizeof(*pdf)); pdf->out = out;
    pdf->page_width_pt = 595; pdf->page_height_pt = 842; pdf->offset_count = 1;
    pdf->pages_obj = next_object(pdf); pdf->catalog_obj = next_object(pdf); pdf->font_obj = next_object(pdf);
    return write_header(pdf);
}

int amipdf_set_page_size(struct amipdf *pdf, int width_pt, int height_pt)
{
    if (!pdf || !pdf->out || width_pt <= 0 || height_pt <= 0) return AMIPDF_ERR_ARGUMENT;
    if (pdf->page_count != 0) return AMIPDF_ERR_STATE;
    pdf->page_width_pt = width_pt; pdf->page_height_pt = height_pt; return AMIPDF_OK;
}

int amipdf_begin_page(struct amipdf *pdf)
{
    if (!pdf || !pdf->out || pdf->current_stream_open) return AMIPDF_ERR_STATE;
    if (reserve_pages(pdf, pdf->page_count + 1) != AMIPDF_OK) return AMIPDF_ERR_MEMORY;
    pdf->pages[pdf->page_count].page_object = next_object(pdf);
    pdf->pages[pdf->page_count].contents_object = next_object(pdf);
    pdf->stream_len = 0; pdf->current_stream_open = 1; return AMIPDF_OK;
}

int amipdf_text(struct amipdf *pdf, int x, int y, const char *text)
{
    int rc;
    if (!pdf || !pdf->current_stream_open || !text) return AMIPDF_ERR_STATE;
    rc = stream_printf(pdf, "BT /F1 12 Tf %d %d Td (", x, y); if (rc != AMIPDF_OK) return rc;
    rc = pdf_escape_stream(pdf, text); if (rc != AMIPDF_OK) return rc;
    return stream_append(pdf, ") Tj ET\n", 8);
}

int amipdf_image_rgb(struct amipdf *pdf, int x, int y, int width, int height, const unsigned char *rgb)
{
    struct amipdf_image *image; size_t bytes;
    char buf[192]; int n;
    if (!pdf || !pdf->current_stream_open || !rgb || width <= 0 || height <= 0) return AMIPDF_ERR_ARGUMENT;
    if ((size_t)width > ((size_t)-1) / (size_t)height) return AMIPDF_ERR_ARGUMENT;
    bytes = (size_t)width * (size_t)height;
    if (bytes > ((size_t)-1) / 3U) return AMIPDF_ERR_ARGUMENT;
    bytes *= 3U;
    if (reserve_images(pdf, pdf->image_count + 1) != AMIPDF_OK) return AMIPDF_ERR_MEMORY;
    image = &pdf->images[pdf->image_count];
    image->data = (unsigned char *)malloc(bytes);
    if (!image->data) return AMIPDF_ERR_MEMORY;
    memcpy(image->data, rgb, bytes); image->data_len = bytes; image->width = width; image->height = height;
    image->components = 3; image->object = next_object(pdf);
    ++pdf->image_count;
    n = sprintf(buf, "q %d 0 0 %d %d %d cm /Im%d Do Q\n", width, height, x, y, (int)pdf->image_count);
    if (n < 0 || (size_t)n >= sizeof(buf)) return AMIPDF_ERR_IO;
    return stream_append(pdf, buf, (size_t)n);
}

int amipdf_end_page(struct amipdf *pdf)
{
    char *copy; struct amipdf_page_data *page;
    if (!pdf || !pdf->current_stream_open) return AMIPDF_ERR_STATE;
    page = &pdf->pages[pdf->page_count]; copy = (char *)malloc(pdf->stream_len + 1);
    if (!copy) return AMIPDF_ERR_MEMORY;
    if (pdf->stream_len) memcpy(copy, pdf->stream, pdf->stream_len);
    copy[pdf->stream_len] = '\0'; page->stream = copy; page->stream_len = pdf->stream_len;
    ++pdf->page_count; pdf->stream_len = 0; pdf->current_stream_open = 0; return AMIPDF_OK;
}

static int write_page(struct amipdf *pdf, size_t index)
{
    struct amipdf_page_data *page = &pdf->pages[index]; size_t i;
    if (object_begin(pdf, page->contents_object) != AMIPDF_OK) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "<< /Length %lu >>\nstream\n", (unsigned long)page->stream_len) < 0) return AMIPDF_ERR_IO;
    if (page->stream_len && fwrite(page->stream, 1, page->stream_len, pdf->out) != page->stream_len) return AMIPDF_ERR_IO;
    if (fputs("endstream\nendobj\n", pdf->out) == EOF) return AMIPDF_ERR_IO;
    if (object_begin(pdf, page->page_object) != AMIPDF_OK) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "<< /Type /Page /Parent %d 0 R /MediaBox [0 0 %d %d] /Resources << /Font << /F1 %d 0 R >>",
        pdf->pages_obj, pdf->page_width_pt, pdf->page_height_pt, pdf->font_obj) < 0) return AMIPDF_ERR_IO;
    for (i = 0; i < pdf->image_count; ++i)
        if (fprintf(pdf->out, " /XObject << /Im%lu %d 0 R >>", (unsigned long)(i + 1), pdf->images[i].object) < 0) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, " >> /Contents %d 0 R >>\nendobj\n", page->contents_object) < 0) return AMIPDF_ERR_IO;
    return AMIPDF_OK;
}

static int write_image(struct amipdf *pdf, struct amipdf_image *image)
{
    unsigned char *compressed; size_t clen;
    compressed = flate_store(image->data, image->data_len, &clen);
    if (!compressed) return AMIPDF_ERR_MEMORY;
    if (object_begin(pdf, image->object) != AMIPDF_OK) { free(compressed); return AMIPDF_ERR_IO; }
    if (fprintf(pdf->out, "<< /Type /XObject /Subtype /Image /Width %d /Height %d /ColorSpace /DeviceRGB /BitsPerComponent 8 /Filter /FlateDecode /Length %lu >>\nstream\n",
        image->width, image->height, (unsigned long)clen) < 0) { free(compressed); return AMIPDF_ERR_IO; }
    if (fwrite(compressed, 1, clen, pdf->out) != clen) { free(compressed); return AMIPDF_ERR_IO; }
    free(compressed);
    return fputs("\nendstream\nendobj\n", pdf->out) == EOF ? AMIPDF_ERR_IO : AMIPDF_OK;
}

int amipdf_finish(struct amipdf *pdf)
{
    long xref; size_t i;
    if (!pdf || !pdf->out || pdf->current_stream_open) return AMIPDF_ERR_STATE;
    if (object_begin(pdf, pdf->font_obj) != AMIPDF_OK) return AMIPDF_ERR_IO;
    if (fputs("<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n", pdf->out) == EOF) return AMIPDF_ERR_IO;
    for (i = 0; i < pdf->image_count; ++i) if (write_image(pdf, &pdf->images[i]) != AMIPDF_OK) return AMIPDF_ERR_IO;
    for (i = 0; i < pdf->page_count; ++i) if (write_page(pdf, i) != AMIPDF_OK) return AMIPDF_ERR_IO;
    if (object_begin(pdf, pdf->pages_obj) != AMIPDF_OK) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "<< /Type /Pages /Count %lu /Kids [", (unsigned long)pdf->page_count) < 0) return AMIPDF_ERR_IO;
    for (i = 0; i < pdf->page_count; ++i) if (fprintf(pdf->out, "%d 0 R ", pdf->pages[i].page_object) < 0) return AMIPDF_ERR_IO;
    if (fputs("] >>\nendobj\n", pdf->out) == EOF) return AMIPDF_ERR_IO;
    if (object_begin(pdf, pdf->catalog_obj) != AMIPDF_OK) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "<< /Type /Catalog /Pages %d 0 R >>\nendobj\n", pdf->pages_obj) < 0) return AMIPDF_ERR_IO;
    xref = ftell(pdf->out); if (xref < 0) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "xref\n0 %lu\n0000000000 65535 f \n", (unsigned long)pdf->offset_count) < 0) return AMIPDF_ERR_IO;
    for (i = 1; i < pdf->offset_count; ++i) if (fprintf(pdf->out, "%010ld 00000 n \n", pdf->offsets[i]) < 0) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "trailer\n<< /Size %lu /Root %d 0 R >>\nstartxref\n%ld\n%%%%EOF\n", (unsigned long)pdf->offset_count, pdf->catalog_obj, xref) < 0) return AMIPDF_ERR_IO;
    return fflush(pdf->out) == 0 ? AMIPDF_OK : AMIPDF_ERR_IO;
}

void amipdf_dispose(struct amipdf *pdf)
{
    size_t i; if (!pdf) return;
    for (i = 0; i < pdf->page_count; ++i) free(pdf->pages[i].stream);
    for (i = 0; i < pdf->image_count; ++i) free(pdf->images[i].data);
    free(pdf->images); free(pdf->pages); free(pdf->offsets); free(pdf->stream); memset(pdf, 0, sizeof(*pdf));
}
