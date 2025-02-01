// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stddef.h>

struct Error;

typedef struct
{
    void *data;
    struct Output *next;
} Output;

typedef struct PState
{
    void *input;
    size_t index;

    size_t line;
    size_t col;

    size_t curr;
    size_t start;

    struct Error *errors;
    Output *output;
    
    void *(*next)(void **input);
} PState;

PState *parser (void* input, void *(*next_fn)(void **));
PState *just   (PState *pstate, void *expected);
PState *then   (PState *state, PState *(*pfun)(PState *));

void add_outp  (struct PState *state, void *data);
void pr_outp   (Output *output);
void fr_outp   (Output *output);

#endif // PARSER_H