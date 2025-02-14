// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef LEKC_H
#define LEKC_H

#include <stdint.h>

/* #define IO_PARSER_T(I, O) \
    I ##input[];
    O ##output[];
    O ##(*next)(PState ps); */

/* typedef struct {
    void* (*next)(void *);
} Iterator; */

typedef struct PState PState;

/*
 * à fix.
 * Il faut un moyen qu'elle dépende de deux types génériques,
 * qu'on ne peut paradoxallement pas écrire ici même dans la
 * définition comme c'est tout l'enjeux des génériques
 */
struct PState {
    uint16_t curr;
    uint16_t start;

    void *input; 
    void *output;
    void* (*next)(void *);
};

#endif // LEKC_H