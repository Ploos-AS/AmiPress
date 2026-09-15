CC ?= cc
CFLAGS ?= -std=c89 -Wall -Wextra -Werror -pedantic
CPPFLAGS ?= -Iinclude

BUILD := build
TEST_DOCUMENT := $(BUILD)/test_document
TEST_PDF := $(BUILD)/test_pdf

.PHONY: all check clean

all: check

$(BUILD):
	mkdir -p $(BUILD)

$(TEST_DOCUMENT): $(BUILD) tests/test_document.c src/document.c include/amipress/document.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_document.c src/document.c

$(TEST_PDF): $(BUILD) tests/test_pdf.c src/pdf.c include/amipress/pdf.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_pdf.c src/pdf.c

check: $(TEST_DOCUMENT) $(TEST_PDF)
	./$(TEST_DOCUMENT)
	./$(TEST_PDF)

clean:
	rm -rf $(BUILD)
