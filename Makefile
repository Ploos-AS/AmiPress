CC ?= cc
CFLAGS ?= -std=c89 -Wall -Wextra -Werror -pedantic
CPPFLAGS ?= -Iinclude

BUILD := build
TEST := $(BUILD)/test_document

.PHONY: all check clean

all: check

$(BUILD):
	mkdir -p $(BUILD)

$(TEST): $(BUILD) tests/test_document.c src/document.c include/amipress/document.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ tests/test_document.c src/document.c

check: $(TEST)
	./$(TEST)

clean:
	rm -rf $(BUILD)
