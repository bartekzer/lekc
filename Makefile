# SPDX-FileCopyrightText: 2024 Łukasz Bartkiewicz <lukasku@proton.me>
# SPDX-License-Identifier: MPL-2.0

CC = gcc
CFLAGS = -Iinclude
SRC = src/lekc.c
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
	$(CC) tests/test.c -Iinclude -Llib -llekc -o test 

.PHONY: all clean test