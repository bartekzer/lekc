// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef LEKC_H
#define LEKC_H

#include <stdint.h>
#include "dynarray.h"
#include "iterator.h"

typedef enum {
    AMBIGUOUS,
    UNEXPECTED
} Error;

typedef struct {
    iterator *src;
    size_t index;
    // src.elem_size (size of token)
} Cursor;

typedef struct {
    const char *name;
    void *terminal;
    dynarray *children;
    // children.elem_size (size of a node)
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

// typedef Result (*CombinatorFn)(Context *, Combinator *);

typedef struct Combinator Combinator;

struct Combinator {
    Result (*fn)(Context *, Combinator *);
    void *terminal;
    dynarray *combinators;
};

Cursor new_cursor(iterator *);
Context new_ctxt(iterator *,
                 int (*)(void *, void *));

Result seq(Context *, Combinator *);
Result alt(Context *, Combinator *);
Result many(Context *, Combinator *); 
Result many1(Context *, Combinator *);
Result optional(Context *, Combinator *);
Result read(Context *, Combinator *);

// typedef struct {
//     Cursor *input;
//     Node *output;

//     int (*eq)(void *, void *);

//     int success;
//     union {
//         Error error;
//         void *node;
//     } result;
// } Context;

// typedef struct {
//     void *data;
//     int (*fn)(Context *, dynarray *combinators);
//     dynarray *combinators;
// } Combinator;

// Context new_ctxt(iterator *,
//                  int (*)(void *, void *),
//                  int);

// #define COMPARE(type, name, body) \
//     static int compare_##name(void *_a, void *_b) { \
//         const type *a = (const type*)_a; \
//         const type *b = (const type*)_b; \
//         body \
//     }

#endif // LEKC_H