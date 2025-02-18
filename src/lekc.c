// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "lekc.h"
#include "dynarray.h"
#include "iterator.h"

Context new_ctxt(iterator *input,
                 int (*eq)(void *, void *),
                 int node_size)
{
    return (Context){
        .index = 0,
        .start = 0,
        .input = input,
        .output = create_dynarray(100, node_size),
        .eq = eq,
        .success = 1,
        .result = {}
    };
}