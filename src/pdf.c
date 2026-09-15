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

static int object_begin(struct amipdf *pdf, int number)
{
    if (reserve_offsets(pdf, (size_t)number + 1) != AMIPDF_OK)
        return AMIPDF_ERR_MEMORY;
    pdf->offsets[number] = ftell(pdf->out);
    if (pdf->offsets[number] < 0) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "%d 0 obj\n", number) < 0) return AMIPDF_ERR_IO;
    return AMIPDF_OK;
}

static int next_object(struct amipdf *pdf)
{
    return (int)pdf->offset_count++;
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
    if (write_header(pdf) != AMIPDF_OK) return AMIPDF_ERR_IO;
    return AMIPDF_OK;
}

int amipdf_set_page_size(struct amipdf *pdf, int width_pt, int height_pt)
{
    if (!pdf || !pdf->out || width_pt <= 0 || height_pt <= 0)
        return AMIPDF_ERR_ARGUMENT;
    if (pdf->pages_count != 0) return AMIPDF_ERR_STATE;
    pdf->page_width_pt = width_pt;
    pdf->page_height_pt = height_pt;
    return AMIPDF_OK;
}

int amipdf_begin_page(struct amipdf *pdf)
{
    int rc;
    if (!pdf || !pdf->out || pdf->current_stream_open) return AMIPDF_ERR_STATE;
    pdf->current_page_obj = next_object(pdf);
    pdf->current_contents_obj = next_object(pdf);
    rc = object_begin(pdf, pdf->current_page_obj);
    if (rc != AMIPDF_OK) return rc;
    if (fprintf(pdf->out,
        "<< /Type /Page /Parent %d 0 R /MediaBox [0 0 %d %d] /Resources << /Font << /F1 %d 0 R >> >> /Contents %d 0 R >>\nendobj\n",
        pdf->pages_obj, pdf->page_width_pt, pdf->page_height_pt,
        pdf->font_obj, pdf->current_contents_obj) < 0) return AMIPDF_ERR_IO;

    rc = object_begin(pdf, pdf->current_contents_obj);
    if (rc != AMIPDF_OK) return rc;
    if (fputs("<< /Length ", pdf->out) == EOF) return AMIPDF_ERR_IO;
    pdf->current_stream_start = ftell(pdf->out);
    if (pdf->current_stream_start < 0) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "0000000000 >>\nstream\n") < 0) return AMIPDF_ERR_IO;
    pdf->current_stream_start = ftell(pdf->out);
    if (pdf->current_stream_start < 0) return AMIPDF_ERR_IO;
    pdf->current_stream_open = 1;
    return AMIPDF_OK;
}

static int pdf_escape(FILE *out, const char *text)
{
    const unsigned char *p = (const unsigned char *)text;
    while (*p) {
        if (*p == '(' || *p == ')' || *p == '\\') {
            if (fputc('\\', out) == EOF) return AMIPDF_ERR_IO;
        }
        if (*p < 32 || *p > 126) {
            if (fprintf(out, "\\%03o", (unsigned int)*p) < 0) return AMIPDF_ERR_IO;
        } else if (fputc(*p, out) == EOF) {
            return AMIPDF_ERR_IO;
        }
        ++p;
    }
    return AMIPDF_OK;
}

int amipdf_text(struct amipdf *pdf, int x, int y, const char *text)
{
    int rc;
    if (!pdf || !pdf->current_stream_open || !text) return AMIPDF_ERR_STATE;
    if (fprintf(pdf->out, "BT /F1 12 Tf %d %d Td (", x, y) < 0)
        return AMIPDF_ERR_IO;
    rc = pdf_escape(pdf->out, text);
    if (rc != AMIPDF_OK) return rc;
    if (fputs(") Tj ET\n", pdf->out) == EOF) return AMIPDF_ERR_IO;
    return AMIPDF_OK;
}

int amipdf_end_page(struct amipdf *pdf)
{
    long end;
    long length;
    long save;
    if (!pdf || !pdf->current_stream_open) return AMIPDF_ERR_STATE;
    end = ftell(pdf->out);
    if (end < 0) return AMIPDF_ERR_IO;
    length = end - pdf->current_stream_start;
    if (fputs("endstream\nendobj\n", pdf->out) == EOF) return AMIPDF_ERR_IO;

    save = ftell(pdf->out);
    if (save < 0) return AMIPDF_ERR_IO;
    if (fseek(pdf->out, pdf->offsets[pdf->current_contents_obj], SEEK_SET) != 0)
        return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "%d 0 obj\n<< /Length %ld >>\nstream\n", pdf->current_contents_obj, length) < 0)
        return AMIPDF_ERR_IO;
    if (fseek(pdf->out, save, SEEK_SET) != 0) return AMIPDF_ERR_IO;
    pdf->current_stream_open = 0;
    ++pdf->pages_count;
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

    if (object_begin(pdf, pdf->pages_obj) != AMIPDF_OK) return AMIPDF_ERR_IO;
    if (fprintf(pdf->out, "<< /Type /Pages /Count %d /Kids [", pdf->pages_count) < 0)
        return AMIPDF_ERR_IO;
    for (i = 4; i < pdf->offset_count; ++i) {
        if (i == (size_t)pdf->font_obj || i == (size_t)pdf->pages_obj || i == (size_t)pdf->catalog_obj)
            continue;
        if (fprintf(pdf->out, "%lu 0 R ", (unsigned long)i) < 0) return AMIPDF_ERR_IO;
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
    if (!pdf) return;
    free(pdf->offsets);
    memset(pdf, 0, sizeof(*pdf));
}
