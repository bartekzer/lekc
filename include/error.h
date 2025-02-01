// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef ERROR_H
#define ERROR_H

#include "utils.h"
#include <stddef.h>

struct PState;

typedef struct Error
{
    Position pos;
    const char *message;
    struct Error *next;
} Error;

void add_err (struct PState *state, const char *message);
void fr_err  (Error *errors);
void pr_err  (Error *errors);

#endif // ERROR_H