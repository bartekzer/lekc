# SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
# SPDX-License-Identifier: MPL-2.0

# Resources and inspirations that supported me throughout this project :
# https://www.cs.rochester.edu/u/nelson/courses/csc_173/grammars/parsing.html
# https://github.com/zesterer/chumsky
# https://keleshev.com/abstract-syntax-tree-an-example-in-c/

CC := gcc
CFLAGS := -std=gnu99 -Wall -Wextra -Wwrite-strings -Iinclude
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
LIB = lib/liblekc.a

TEST_SRC = $(wildcard tests/*.c)
TEST_BIN = $(TEST_SRC:%.c=%)

all: $(LIB)

$(LIB): $(OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

tests/%: tests/%.c $(LIB)
	$(CC) $< -Iinclude -Llib -llekc -o $@

test: $(TEST_BIN)
	@for test in $(TEST_BIN); do \
		echo "Running $$test"; \
		./$$test; \
	done

clean:
	rm -f $(OBJ) $(LIB) tests/test

.PHONY: all clean test
