// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef LEKC_H
#define LEKC_H

#include <stdint.h>
#include "dynarray.h"
#include "iterator.h"

typedef enum {
    A = 1,
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

// à supprimer au profit d'un retour de booléen
// typedef struct {
//     int success;
//     union {
//         Error error;
//         Node *node;
//     };
// } Result;

typedef struct {
    Cursor *input;
    int (*eq)(void *, void *);
    Node* parent_node;
    Error error;
} Context;

typedef struct Combinator Combinator;

struct Combinator {
    const char *name;
    int (*fn)(Context *, Combinator *); // changer ici aussi par un `bool`
    void *terminal;
    dynarray *combinators;
};

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