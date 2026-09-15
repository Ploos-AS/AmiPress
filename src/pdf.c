#include "amipress/pdf.h"

#include <stdlib.h>
#include <string.h>

static int reserve_offsets(struct amipdf *pdf, size_t need)
{
    long *p;
    size_t cap;
    if (need <= pdf->offset_capacity) return AMIPDF_OK;
    cap = pdf->offset_capacity ? pdf->offset_capacity : 16;
    while (cap < need) cap *= 2;
    p = (long *)realloc(pdf->offsets, cap * sizeof(*p));
    if (!p) return AMIPDF_ERR_MEMORY;
    pdf->offsets = p;
    pdf->offset_capacity = cap;
    return AMIPDF_OK;
}

static int reserve_pages(struct amipdf *pdf, size_t need)
{
    struct amipdf_page_data *p;
    size_t cap;
    if (need <= pdf->page_capacity) return AMIPDF_OK;
    cap = pdf->page_capacity ? pdf->page_capacity : 4;
    while (cap < need) cap *= 2;
    p = (struct amipdf_page_data *)realloc(pdf->pages, cap * sizeof(*p));
    if (!p) return AMIPDF_ERR_MEMORY;
    memset(p + pdf->page_capacity, 0,
        (cap - pdf->page_capacity) * sizeof(*p));
    pdf->pages = p;
    pdf->page_capacity = cap;
    return AMIPDF_OK;
}

static int reserve_stream(struct amipdf *pdf, size_t extra)
{
    char *p;
    size_t need;
    size_t cap;
    need = pdf->stream_len + extra + 1;
    if (need <= pdf->stream_capacity) return AMIPDF_OK;
    cap = pdf->stream_capacity ? pdf->stream_capacity : 256;
    while (cap < need) cap *= 2;
    p = (char *)realloc(pdf->stream, cap);
    if (!p) return AMIPDF_ERR_MEMORY;
    pdf->stream = p;
    pdf->stream_capacity = cap;
    return AMIPDF_OK;
}

static int stream_append(struct amipdf *pdf, const char *s, size_t n)
{
    if (reserve_stream(pdf, n) != AMIPDF_OK) return AMIPDF_ERR_MEMORY;
    memcpy(pdf->stream + pdf->stream_len, s, n);
    pdf->stream_len += n;
    pdf->stream[pdf->stream_len] = '\0';
    return AMIPDF_OK;
}

static int stream_printf(struct amipdf *pdf, const char *format, int a, int b)
{
    char buf[160];
    int n;
    n = sprintf(buf, format, a, b);
    if (n < 0 || (size_t)n >= sizeof(buf)) return AMIPDF_ERR_IO;
    return stream_append(pdf, buf, (size_t)n);
}

static int next_object(struct amipdf *pdf)
{
    int number;
    number = (int)pdf->offset_count;
    ++pdf->offset_count;
    return number;
}

static int object_begin(struct amipdf *pdf, int number)
{
    if (reserve_offsets(pdf, (size_t)number + 1) != AMIPDF_OK)
        return AMIPDF_ERR_MEMORY;
    pdf->offsets[number] = ftell(pdf->out);
    if (pdf->offsets[number] < 0) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "%d 0 obj\n", number) < 0) return AMIPDF_ERR_IO;
    return AMIPDF_OK;
}

static int pdf_escape_stream(struct amipdf *pdf, const char *text)
{
    const unsigned char *p;
    char esc[5];
    p = (const unsigned char *)text;
    while (*p) {
        if (*p == '(' || *p == ')' || *p == '\\') {
            if (stream_append(pdf, "\\", 1) != AMIPDF_OK) return AMIPDF_ERR_MEMORY;
        }
        if (*p < 32 || *p > 126) {
            sprintf(esc, "\\%03o", (unsigned int)*p);
            if (stream_append(pdf, esc, 4) != AMIPDF_OK) return AMIPDF_ERR_MEMORY;
        } else if (stream_append(pdf, (const char *)p, 1) != AMIPDF_OK) {
            return AMIPDF_ERR_MEMORY;
        }
        ++p;
    }
    return AMIPDF_OK;
}

static int write_header(struct amipdf *pdf)
{
    if (fputs("%PDF-1.4\n%\xE2\xE3\xCF\xD3\n", pdf->out) == EOF)
        return AMIPDF_ERR_IO;
    return AMIPDF_OK;
}

int amipdf_init(struct amipdf *pdf, FILE *out)
{
    if (!pdf || !out) return AMIPDF_ERR_ARGUMENT;
    memset(pdf, 0, sizeof(*pdf));
    pdf->out = out;
    pdf->page_width_pt = 595;
    pdf->page_height_pt = 842;
    pdf->offset_count = 1;
    pdf->pages_obj = next_object(pdf);
    pdf->catalog_obj = next_object(pdf);
    pdf->font_obj = next_object(pdf);
    return write_header(pdf);
}

int amipdf_set_page_size(struct amipdf *pdf, int width_pt, int height_pt)
{
    if (!pdf || !pdf->out || width_pt <= 0 || height_pt <= 0)
        return AMIPDF_ERR_ARGUMENT;
    if (pdf->page_count != 0) return AMIPDF_ERR_STATE;
    pdf->page_width_pt = width_pt;
    pdf->page_height_pt = height_pt;
    return AMIPDF_OK;
}

int amipdf_begin_page(struct amipdf *pdf)
{
    if (!pdf || !pdf->out || pdf->current_stream_open) return AMIPDF_ERR_STATE;
    if (reserve_pages(pdf, pdf->page_count + 1) != AMIPDF_OK)
        return AMIPDF_ERR_MEMORY;
    pdf->pages[pdf->page_count].page_object = next_object(pdf);
    pdf->pages[pdf->page_count].contents_object = next_object(pdf);
    pdf->stream_len = 0;
    pdf->current_stream_open = 1;
    return AMIPDF_OK;
}

int amipdf_text(struct amipdf *pdf, int x, int y, const char *text)
{
    int rc;
    if (!pdf || !pdf->current_stream_open || !text) return AMIPDF_ERR_STATE;
    rc = stream_printf(pdf, "BT /F1 12 Tf %d %d Td (", x, y);
    if (rc != AMIPDF_OK) return rc;
    rc = pdf_escape_stream(pdf, text);
    if (rc != AMIPDF_OK) return rc;
    return stream_append(pdf, ") Tj ET\n", 8);
}

int amipdf_end_page(struct amipdf *pdf)
{
    char *copy;
    struct amipdf_page_data *page;
    if (!pdf || !pdf->current_stream_open) return AMIPDF_ERR_STATE;
    page = &pdf->pages[pdf->page_count];
    copy = (char *)malloc(pdf->stream_len + 1);
    if (!copy) return AMIPDF_ERR_MEMORY;
    if (pdf->stream_len != 0) memcpy(copy, pdf->stream, pdf->stream_len);
    copy[pdf->stream_len] = '\0';
    page->stream = copy;
    page->stream_len = pdf->stream_len;
    ++pdf->page_count;
    pdf->stream_len = 0;
    pdf->current_stream_open = 0;
    return AMIPDF_OK;
}

static int write_page(struct amipdf *pdf, size_t index)
{
    struct amipdf_page_data *page;
    page = &pdf->pages[index];
    if (object_begin(pdf, page->contents_object) != AMIPDF_OK) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "<< /Length %lu >>\nstream\n", (unsigned long)page->stream_len) < 0)
        return AMIPDF_ERR_IO;
    if (page->stream_len != 0 && fwrite(page->stream, 1, page->stream_len, pdf->out) != page->stream_len)
        return AMIPDF_ERR_IO;
    if (fputs("endstream\nendobj\n", pdf->out) == EOF) return AMIPDF_ERR_IO;
    if (object_begin(pdf, page->page_object) != AMIPDF_OK) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out,
        "<< /Type /Page /Parent %d 0 R /MediaBox [0 0 %d %d] /Resources << /Font << /F1 %d 0 R >> >> /Contents %d 0 R >>\nendobj\n",
        pdf->pages_obj, pdf->page_width_pt, pdf->page_height_pt,
        pdf->font_obj, page->contents_object) < 0) return AMIPDF_ERR_IO;
    return AMIPDF_OK;
}

int amipdf_finish(struct amipdf *pdf)
{
    long xref;
    size_t i;
    if (!pdf || !pdf->out || pdf->current_stream_open) return AMIPDF_ERR_STATE;
    if (object_begin(pdf, pdf->font_obj) != AMIPDF_OK) return AMIPDF_ERR_IO;
    if (fputs("<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n", pdf->out) == EOF)
        return AMIPDF_ERR_IO;
    for (i = 0; i < pdf->page_count; ++i) {
        if (write_page(pdf, i) != AMIPDF_OK) return AMIPDF_ERR_IO;
    }
    if (object_begin(pdf, pdf->pages_obj) != AMIPDF_OK) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "<< /Type /Pages /Count %lu /Kids [", (unsigned long)pdf->page_count) < 0)
        return AMIPDF_ERR_IO;
    for (i = 0; i < pdf->page_count; ++i) {
        if (fprintf(pdf->out, "%d 0 R ", pdf->pages[i].page_object) < 0) return AMIPDF_ERR_IO;
    }
    if (fputs("] >>\nendobj\n", pdf->out) == EOF) return AMIPDF_ERR_IO;
    if (object_begin(pdf, pdf->catalog_obj) != AMIPDF_OK) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "<< /Type /Catalog /Pages %d 0 R >>\nendobj\n", pdf->pages_obj) < 0)
        return AMIPDF_ERR_IO;
    xref = ftell(pdf->out);
    if (xref < 0) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "xref\n0 %lu\n0000000000 65535 f \n", (unsigned long)pdf->offset_count) < 0)
        return AMIPDF_ERR_IO;
    for (i = 1; i < pdf->offset_count; ++i) {
        if (fprintf(pdf->out, "%010ld 00000 n \n", pdf->offsets[i]) < 0) return AMIPDF_ERR_IO;
    }
    if (fprintf(pdf->out, "trailer\n<< /Size %lu /Root %d 0 R >>\nstartxref\n%ld\n%%%%EOF\n",
        (unsigned long)pdf->offset_count, pdf->catalog_obj, xref) < 0) return AMIPDF_ERR_IO;
    return fflush(pdf->out) == 0 ? AMIPDF_OK : AMIPDF_ERR_IO;
}

void amipdf_dispose(struct amipdf *pdf)
{
    size_t i;
    if (!pdf) return;
    for (i = 0; i < pdf->page_count; ++i) free(pdf->pages[i].stream);
    free(pdf->pages);
    free(pdf->offsets);
    free(pdf->stream);
    memset(pdf, 0, sizeof(*pdf));
}
