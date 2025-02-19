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

// typedef Context *(*Terminal)(Context *, void *);
// typedef Context *(*Nonterminal)(Context *, dynarray *);

// typedef struct {
//     int term;
//     union  {
//         Terminal terminal;
//         Nonterminal nonterminal; 
//     };
// } Lexeme; // go in the dynarray of nonterminals

// typedef struct Combinator Combinator;

// struct Combinator {
//     char *name;
//     Combinator *(*fn)(Context *, dynarray *combinators);
//     dynarray *combinators;
// };

typedef struct {
    char *name;
    int (*fn)(Context *, dynarray *combinators);
    dynarray *combinators;
} Combinator;

Context new_ctxt(iterator *,
                 int (*)(void *, void *),
                 int);

#define COMPARE(type, name, body) \
    static int compare_##name(void *_a, void *_b) { \
        const type *a = (const type*)_a; \
        const type *b = (const type*)_b; \
        body \
    }

#endif // LEKC_H