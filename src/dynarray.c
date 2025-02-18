// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "dynarray.h"

dynarray *create_dynarray(size_t cap,
                          size_t elem_size)
{
    dynarray *da = malloc(sizeof(dynarray));
    if (!da)
        return NULL;

    da->array = malloc(cap * elem_size);
    if (!da->array)
    {
        free(da);
        return NULL;
    }

    da->size = 0;
    da->capacity = cap;
    da->elem_size = elem_size;

    return da;
}

int resize_dynarray(dynarray *da,
                    size_t new_cap)
{
    void *new_arr = realloc(da->array, new_cap * da->elem_size);
    if (!new_arr)
        return 1;

    da->array = new_arr;
    da->capacity = new_cap;

    return 0;
}

void destroy_dynarray(dynarray *da)
{
    free(da->array);
    free(da);
}

int push_dynarray(dynarray *da,
                  void *elem)
{
    if (da->size == da->capacity)
        if (resize_dynarray(da, da->capacity * 2))
            return 1;

    memcpy((char *)da->array + da->size * da->elem_size, elem, da->elem_size);
    da->size++;

    return 0;
}

void *pop_dynarray(dynarray *da)
{
    if (da->size == 0)
        return NULL;

    da->size--;
    return (char *)da->array + da->size * da->elem_size;

}