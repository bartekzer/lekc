// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "parser.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>

void
add_outp(state, data)
    struct PState *state;
    void *data;
{
    Output *output = malloc(sizeof(Output));
    output->data = data;
    output->next = state->output;
    state->output = output;
}

void
pr_outp(output) 
    Output *output;
{
    while (output) {
        printf("output: %p\n", output->data);
        output = output->next;
    }
}

void
fr_outp(output)
    Output *output;
{
    while (output) {
        Output *next = output->next;
        free(output);
        output = next;
    }
}

PState *
parser(input, next_fun)
    void* input;
    void *(*next_fun)(void **);
{
    PState *state = malloc(sizeof(PState));
    state->input = input;
    state->pos = 0;
    state->start = 0;
    state->errors = NULL;
    state->output = NULL;
    state->next = next_fun;
    return state;
}

PState *
then(state, pfun)
    struct PState *state;
    struct PState *(*pfun)(PState *);
{
    return pfun(state);
}

PState *
just(state, expected)
    struct PState *state;
    void *expected;
{
    void *current = state->next(&state->input);
    if (current == expected) {
        add_outp(state, current);
    } else {
        add_err(state, "Expected element not found");
    }
    return state;
}