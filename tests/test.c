// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "lekc.h"
#include "dynarray.h"
#include "iterator.h"
#include <stdio.h>
#include <stdlib.h>

COMPARE(int, int, {
    return *a == *b;
})


int main()
{
    // dynarray *array = CREATE_DYNARRAY(3, int);
    // if (!array)
    //     return 1;

    // PUSH_DYNARRAY(array, 1);
    // PUSH_DYNARRAY(array, 2);
    // PUSH_DYNARRAY(array, 3);
    // PUSH_DYNARRAY(array, 4);

    // int popped_value = POP_DYNARRAY(array, int);
    // printf("Popped value: %d\n", popped_value);

    // int int_arr[4] = {1, 2, 3, 4};
    // iterator it = begin(int_arr, sizeof(int), 4);
    
    // FOR_EACH(it) {
    //     printf("%d\n", *(int *)it.ptr);
    // }

    // int x = x + 5;
    // printf("x: %d\n", x);

    // destroy_dynarray(array);

    int int_arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    iterator input = begin(int_arr, sizeof(int), 10);

    Context ctxt = new_ctxt(&input, compare_int, sizeof(double));

    dynarray *seq_combinators = CREATE_DYNARRAY(2, Combinator);
    
    Combinator seq = {
        .name = "seq",
        .fn = seq_fn,
        .combinators = CREATE_DYNARRAY()
    }

    return 0;
}
