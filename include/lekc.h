// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef LEKC_H
#define LEKC_H

#include <stdint.h>
#include "dynarray.h"
#include "iterator.h"

typedef enum {
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    AMBIGUOUS,
    UNEXPECTED
} Error;

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
    int success;
    union {
        Error error;
        Node *node;
    };
} Result;

typedef struct {
    Cursor *input;
    int (*eq)(void *, void *);
} Context;

typedef struct Combinator Combinator;

struct Combinator {
    const char *name;
    Result (*fn)(Context *, Combinator *);
    void *terminal;
    dynarray *combinators;
};

Combinator new_combinator(const char *,
                          Result (*)(Context *, Combinator *),
                          void *,
                          dynarray *);
Cursor new_cursor(iterator *);
Context new_ctxt(iterator *,
                 int (*)(void *, void *));

Result seq(Context *, Combinator *);
Result alt(Context *, Combinator *);
Result many(Context *, Combinator *); 
Result many1(Context *, Combinator *);
Result optional(Context *, Combinator *);
Result skip(Context *, Combinator *);

#define COMPARE(type, name, body) \
    static int compare_##name(void *_a, void *_b) { \
        const type *a = (const type*)_a; \
        const type *b = (const type*)_b; \
        body \
    }

#endif // LEKC_H