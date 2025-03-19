// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef LEKC_H
#define LEKC_H

#include <stdint.h>
#include "dynarray.h"
#include "iterator.h"

typedef struct {
    iterator *src;
    size_t index;
} Cursor;

typedef struct {
    const char *name;
    void *terminal;
    dynarray *children;
} Node;

typedef struct {
    Cursor *input;
    int (*eq)(void *, void *);
    Node* parent_node;
    const char* error;
} Context;

typedef struct Combinator Combinator;

struct Combinator {
    const char *name;
    int (*fn)(Context *, Combinator *);
    void *terminal;
    dynarray *combinators;
};

void debug_node(const Node*, int);
Combinator build(const char *,
                 int (*)(Context *, Combinator *),
                 void *,
                 dynarray *);
Cursor new_cursor(iterator *);
int seq(Context *, Combinator *);
int alt(Context *, Combinator *);
int many(Context *, Combinator *);
int many1(Context *, Combinator *);
int optional(Context *, Combinator *);
int skip(Context *, Combinator *);

#endif // LEKC_H
