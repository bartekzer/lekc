// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <stddef.h>

typedef struct {
    void* array;
    size_t elem_size;
    size_t size;
    size_t capacity;
} dynarray;

dynarray *create_dynarray(size_t, size_t);
void resize_dynarray(dynarray *, size_t);
void destroy_dynarray(dynarray *);
void push_dynarray(dynarray *, void*);
void *pop_dynarray(dynarray *);

#endif // DYNARRAY_H
