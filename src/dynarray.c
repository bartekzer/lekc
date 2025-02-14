// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "dynarray.h"

dynarray *create_dynarray(size_t cap,
                          size_t elem_size)
{
    dynarray* da = malloc(sizeof(dynarray));
    if (!da) return NULL;

    da->array = malloc(cap * elem_size);
    if (!da->array) {
        free(da);
        return NULL;
    }

    da->size = 0;
    da->capacity = cap;
    da->elem_size = elem_size;

    return da;
}

void resize_dynarray(dynarray *da,
                     size_t new_cap)
{
    /* dynarray *new_da = malloc(sizeof(dynarray));
    if (!new_da) return NULL;

    new_da->array = malloc(new_cap * da->elem_size);
    if (!new_da->array) {
        free(new_da);
        return NULL;
    } */
    void *new_arr = malloc(new_cap * da->elem_size);

    /* for (size_t i = 0; i < da->size; i++) {
        new_da->array[i] = da->array[i];
    } */
    memcpy(new_arr, da->array, da->size * da->elem_size);

    free(da->array);

    da->array = new_arr;
    da->capacity = new_cap;
}

void destroy_dynarray(dynarray *da)
{
    free(da->array);
    free(da);
}

void push_dynarray(dynarray *da,
                   void *elem)
{
    if (da->size == da->capacity) {
        resize_dynarray(da, da->capacity * 2);
    }
    // da->array[da->size] = elem;
    memcpy((char *)da->array + da->size * da->elem_size, elem, da->elem_size);
    da->size++;
}

void *pop_dynarray(dynarray *da)
{
    if (da->size == 0) return NULL;

    return da->array[--da->size];
}
