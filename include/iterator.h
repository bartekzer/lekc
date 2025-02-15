// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef ITERATOR_H
#define ITERATOR_H

#include <stddef.h>

typedef struct
{
    void* ptr;
    size_t elem_size;
    void* end;
} Iterator;

Iterator begin(void*, size_t, size_t);
Iterator* next(Iterator*);
int is_end(Iterator*);

#define FOR_EACH(it) for (; !is_end(&it); next(&it))

#endif // ITERATOR_H 