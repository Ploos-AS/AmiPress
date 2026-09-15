#ifndef AMIPRESS_DOCUMENT_H
#define AMIPRESS_DOCUMENT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct AmiPressDocument AmiPressDocument;
typedef struct AmiPressPage AmiPressPage;

typedef struct AmiPressRect {
    long x;
    long y;
    long width;
    long height;
} AmiPressRect;

typedef enum AmiPressObjectType {
    AMIPRESS_OBJECT_TEXT = 1,
    AMIPRESS_OBJECT_IMAGE = 2,
    AMIPRESS_OBJECT_LINE = 3,
    AMIPRESS_OBJECT_RECT = 4
} AmiPressObjectType;

typedef struct AmiPressText {
    const char *text;
    const char *font;
    unsigned long size;
    unsigned long flags;
} AmiPressText;

typedef struct AmiPressObject {
    AmiPressObjectType type;
    AmiPressRect bounds;
    union {
        AmiPressText text;
    } data;
} AmiPressObject;

#define AMIPRESS_TEXT_BOLD      0x0001UL
#define AMIPRESS_TEXT_ITALIC    0x0002UL
#define AMIPRESS_TEXT_UNDERLINE 0x0004UL

AmiPressDocument *amipress_document_create(void);
void amipress_document_destroy(AmiPressDocument *document);

AmiPressPage *amipress_document_add_page(AmiPressDocument *document,
                                          long width,
                                          long height);
size_t amipress_document_page_count(const AmiPressDocument *document);
AmiPressPage *amipress_document_page(AmiPressDocument *document, size_t index);

int amipress_page_add_object(AmiPressPage *page, const AmiPressObject *object);
size_t amipress_page_object_count(const AmiPressPage *page);
const AmiPressObject *amipress_page_object(const AmiPressPage *page,
                                            size_t index);

#ifdef __cplusplus
}
#endif

#endif
