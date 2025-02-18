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
    unsigned int index;
    unsigned int start;

    // size_t token_size; <-- reachable by input->elem_size
    // size_t node_size;  <-- reachable by output->elem_size

    iterator *input;
    dynarray *output;

    int (*eq)(void *, void *);

    /*.--.--.--.*/

    int success;
    union {
        Error error;
        void *ast;
    } result;
} Context;

typedef struct Combinator Combinator;

struct Combinator {
    char *name;
    Combinator *(*fn)(Context *, dynarray *combinators);
    dynarray *combinators;
};

Context new_ctxt(iterator *,
                 int (*)(void *, void *),
                 int);

#endif // LEKC_H