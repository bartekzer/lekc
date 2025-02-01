// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "error.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

void
add_err(state, message) 
    struct PState *state;
    const char *message;
{
    Error *error = malloc(sizeof(Error));
    error->pos.line = state->line;
    error->pos.col = state->col;
    error->message = message;
    error->next = state->errors;
    state->errors = error;
}

void
fr_err(errors) 
    Error *errors;
{
    while (errors) {
        Error *next = errors->next;
        free(errors);
        errors = next;
    }
}

void
pr_err(errors)
    Error *errors;
{
    while (errors) {
        printf("error at %lu:%lu : %s", errors->pos.col, errors->pos.line, errors->message);
        errors = errors->next;
    }
}