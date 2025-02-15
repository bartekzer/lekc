// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef LEKC_H
#define LEKC_H

#include <stdint.h>

/* typedef struct {
    void* (*next)(void *);
} Iterator; */

typedef struct PState PState;

struct PState {
    uint16_t curr;
    uint16_t start;

    void *input; 
    void *output;
    void* (*next)(void *);
};

#endif // LEKC_H