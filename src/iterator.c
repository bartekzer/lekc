// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include <stdio.h>
#include "iterator.h"

iterator
init(void* ptr,
     size_t elem_size,
     size_t length)
{
    return (iterator){
        ptr,
        elem_size,
        length == 0 ? NULL : ptr + elem_size * length
    };
}

iterator*
next(iterator* it)
{
    if (!it || is_end(it)) {
        return it;
    }
    void* next_ptr = it->ptr + it->elem_size;
    if (next_ptr <= it->end) {
        it->ptr = next_ptr;
    }
    return it;
}

int
is_end(iterator* it)
{
    if (!it || !it->ptr || !it->end) {
        return 1;
    }
    return it->ptr >= it->end;
}