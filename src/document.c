#include "amipress/document.h"

#include <stdlib.h>
#include <string.h>

struct AmiPressPage {
    long width;
    long height;
    AmiPressObject *objects;
    size_t object_count;
    size_t object_capacity;
};

struct AmiPressDocument {
    AmiPressPage *pages;
    size_t page_count;
    size_t page_capacity;
};

static int grow_pages(AmiPressDocument *document)
{
    size_t capacity;
    AmiPressPage *pages;

    if (document->page_count < document->page_capacity) {
        return 0;
    }

    capacity = document->page_capacity == 0 ? 4 : document->page_capacity * 2;
    pages = (AmiPressPage *)realloc(document->pages,
                                    capacity * sizeof(AmiPressPage));
    if (pages == NULL) {
        return -1;
    }

    document->pages = pages;
    document->page_capacity = capacity;
    return 0;
}

static int grow_objects(AmiPressPage *page)
{
    size_t capacity;
    AmiPressObject *objects;

    if (page->object_count < page->object_capacity) {
        return 0;
    }

    capacity = page->object_capacity == 0 ? 8 : page->object_capacity * 2;
    objects = (AmiPressObject *)realloc(page->objects,
                                         capacity * sizeof(AmiPressObject));
    if (objects == NULL) {
        return -1;
    }

    page->objects = objects;
    page->object_capacity = capacity;
    return 0;
}

AmiPressDocument *amipress_document_create(void)
{
    AmiPressDocument *document;

    document = (AmiPressDocument *)calloc(1, sizeof(AmiPressDocument));
    return document;
}

void amipress_document_destroy(AmiPressDocument *document)
{
    size_t i;

    if (document == NULL) {
        return;
    }

    for (i = 0; i < document->page_count; ++i) {
        free(document->pages[i].objects);
    }
    free(document->pages);
    free(document);
}

AmiPressPage *amipress_document_add_page(AmiPressDocument *document,
                                          long width,
                                          long height)
{
    AmiPressPage *page;

    if (document == NULL || width <= 0 || height <= 0) {
        return NULL;
    }
    if (grow_pages(document) != 0) {
        return NULL;
    }

    page = &document->pages[document->page_count++];
    memset(page, 0, sizeof(*page));
    page->width = width;
    page->height = height;
    return page;
}

size_t amipress_document_page_count(const AmiPressDocument *document)
{
    return document == NULL ? 0 : document->page_count;
}

AmiPressPage *amipress_document_page(AmiPressDocument *document, size_t index)
{
    if (document == NULL || index >= document->page_count) {
        return NULL;
    }
    return &document->pages[index];
}

int amipress_page_add_object(AmiPressPage *page, const AmiPressObject *object)
{
    if (page == NULL || object == NULL) {
        return -1;
    }
    if (grow_objects(page) != 0) {
        return -1;
    }

    page->objects[page->object_count++] = *object;
    return 0;
}

size_t amipress_page_object_count(const AmiPressPage *page)
{
    return page == NULL ? 0 : page->object_count;
}

const AmiPressObject *amipress_page_object(const AmiPressPage *page,
                                            size_t index)
{
    if (page == NULL || index >= page->object_count) {
        return NULL;
    }
    return &page->objects[index];
}
