// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include <stdio.h>
#include "iterator.h"

Iterator begin(void* ptr,
               size_t elem_size,
               size_t length)
{
    return (Iterator){ptr, elem_size, length == 0 ? NULL : ptr + elem_size * length};
}

Iterator* next(Iterator* it)
{
    it->ptr += it->elem_size;
    return it;
}

int is_end(Iterator* it)
{
    return *(char*)it->ptr == '\0' || (it->end && it->ptr == it->end);
}