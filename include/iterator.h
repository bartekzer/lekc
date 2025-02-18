// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef iterator_H
#define iterator_H

#include <stddef.h>

typedef struct
{
    void* ptr;
    size_t elem_size;
    void* end;
} iterator;

iterator begin(void*, size_t, size_t);
iterator* next(iterator*);
int is_end(iterator*);

#define FOR_EACH(it) for (; !is_end(&it); next(&it))

#endif // iterator_H 