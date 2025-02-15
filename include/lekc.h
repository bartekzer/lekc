// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef LEKC_H
#define LEKC_H

#include <stdint.h>
#include "dynarray.h"

typedef struct Parser Parser;

struct Parser {
    uint16_t curr;
    uint16_t start;

    dynarray *input; 
    dynarray *output;
    dynarray *errors;

    void* (*next)(void *);
};

#endif // LEKC_H