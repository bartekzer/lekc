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

all: $(LIB)

$(LIB): $(OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(LIB) tests/test

test: all
	$(CC) tests/test.c -Iinclude -Llib -llekc -o tests/test
	./tests/test

.PHONY: all clean test
