CC ?= cc
CFLAGS ?= -std=c89 -Wall -Wextra -Werror -pedantic
CPPFLAGS ?= -Iinclude

BUILD := build
TEST_DOCUMENT := $(BUILD)/test_document
TEST_PDF := $(BUILD)/test_pdf
TEST_TEXT := $(BUILD)/test_text
TEST_PDF_FONT := $(BUILD)/test_pdf_font

.PHONY: all check clean

all: check

$(BUILD):
	mkdir -p $(BUILD)

$(TEST_DOCUMENT): $(BUILD) tests/test_document.c src/document.c include/amipress/document.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_document.c src/document.c

$(TEST_PDF): $(BUILD) tests/test_pdf.c src/pdf.c src/pdf_encoding.c src/text.c include/amipress/pdf.h include/amipress/text.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_pdf.c src/pdf.c src/pdf_encoding.c src/text.c

$(TEST_TEXT): $(BUILD) tests/test_text.c src/text.c include/amipress/text.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_text.c src/text.c

$(TEST_PDF_FONT): $(BUILD) tests/test_pdf_font.c src/pdf_font.c include/amipress/pdf_font.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_pdf_font.c src/pdf_font.c

check: $(TEST_DOCUMENT) $(TEST_PDF) $(TEST_TEXT) $(TEST_PDF_FONT)
	./$(TEST_DOCUMENT)
	./$(TEST_PDF)
	./$(TEST_TEXT)
	./$(TEST_PDF_FONT)

clean:
	rm -rf $(BUILD)
