#ifndef AMIPRESS_PDF_H
#define AMIPRESS_PDF_H

#include <stddef.h>
#include <stdio.h>

#define AMIPDF_OK 0
#define AMIPDF_ERR_ARGUMENT 1
#define AMIPDF_ERR_IO 2
#define AMIPDF_ERR_MEMORY 3
#define AMIPDF_ERR_STATE 4

struct amipdf_page_data {
    int page_object;
    int contents_object;
    char *stream;
    size_t stream_len;
};

struct amipdf_image {
    int object;
    unsigned char *data;
    size_t data_len;
    int width;
    int height;
    int components;
};

struct amipdf {
    FILE *out;
    long *offsets;
    size_t offset_count;
    size_t offset_capacity;
    struct amipdf_page_data *pages;
    size_t page_count;
    size_t page_capacity;
    struct amipdf_image *images;
    size_t image_count;
    size_t image_capacity;
    int page_width_pt;
    int page_height_pt;
    int pages_obj;
    int catalog_obj;
    int font_obj;
    int info_obj;
    char *title;
    char *author;
    char *stream;
    size_t stream_len;
    size_t stream_capacity;
    int current_stream_open;
};

int amipdf_init(struct amipdf *pdf, FILE *out);
int amipdf_set_page_size(struct amipdf *pdf, int width_pt, int height_pt);
int amipdf_set_title(struct amipdf *pdf, const char *title);
int amipdf_set_author(struct amipdf *pdf, const char *author);
int amipdf_begin_page(struct amipdf *pdf);
int amipdf_text(struct amipdf *pdf, int x, int y, const char *text);
int amipdf_image_rgb(struct amipdf *pdf, int x, int y, int width, int height,
    const unsigned char *rgb);
int amipdf_end_page(struct amipdf *pdf);
int amipdf_finish(struct amipdf *pdf);
void amipdf_dispose(struct amipdf *pdf);

#endif
