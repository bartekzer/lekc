// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <stddef.h>

typedef struct
{
    void *array;
    size_t elem_size;
    size_t size;
    size_t capacity;
} dynarray;

dynarray *create_dynarray(size_t, size_t);
int resize_dynarray(dynarray *, size_t);
void destroy_dynarray(dynarray *);
int push_dynarray(dynarray *, void *);
void *pop_dynarray(dynarray *);

#define CREATE_DYNARRAY(cap, type) create_dynarray(cap, sizeof(type))
#define PUSH_DYNARRAY(da, elem) do { \
        typeof(elem) _elem = (elem); \
        push_dynarray(da, &_elem);   \
    } while (0)
#define POP_DYNARRAY(da, type) (*(type *)pop_dynarray(da))

#endif // DYNARRAY_H
