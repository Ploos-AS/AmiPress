#include "amipress/document.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    AmiPressDocument *document;
    AmiPressPage *page;
    AmiPressObject object;
    const AmiPressObject *stored;

    document = amipress_document_create();
    assert(document != NULL);
    assert(amipress_document_page_count(document) == 0);

    page = amipress_document_add_page(document, 595, 842);
    assert(page != NULL);
    assert(amipress_document_page_count(document) == 1);
    assert(amipress_document_page(document, 0) == page);
    assert(amipress_document_page(document, 1) == NULL);

    object.type = AMIPRESS_OBJECT_TEXT;
    object.bounds.x = 72;
    object.bounds.y = 72;
    object.bounds.width = 240;
    object.bounds.height = 24;
    object.data.text.text = "AmiPress M1";
    object.data.text.font = "Helvetica";
    object.data.text.size = 12;
    object.data.text.flags = AMIPRESS_TEXT_BOLD;

    assert(amipress_page_add_object(page, &object) == 0);
    assert(amipress_page_object_count(page) == 1);

    stored = amipress_page_object(page, 0);
    assert(stored != NULL);
    assert(stored->type == AMIPRESS_OBJECT_TEXT);
    assert(stored->bounds.x == 72);
    assert(stored->data.text.size == 12);
    assert(stored->data.text.flags == AMIPRESS_TEXT_BOLD);
    assert(amipress_page_object(page, 1) == NULL);

    amipress_document_destroy(document);
    puts("PASS: AmiPress document model");
    return 0;
}
