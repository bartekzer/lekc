// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "lekc.h"
#include "dynarray.h"
#include "iterator.h"
#include <stdio.h>
#include <stdlib.h>


int main()
{
    dynarray *array = CREATE_DYNARRAY(3, int);
    if (!array)
        return 1;

    PUSH_DYNARRAY(array, 1);
    PUSH_DYNARRAY(array, 2);
    PUSH_DYNARRAY(array, 3);
    PUSH_DYNARRAY(array, 4);

    int popped_value = POP_DYNARRAY(array, int);
    printf("Popped value: %d\n", popped_value);

    int int_arr[4] = {1, 2, 3, 4};
    Iterator it = begin(int_arr, sizeof(int), 4);
    
    FOR_EACH(it) {
        printf("%d\n", *(int *)it.ptr);
    }

    destroy_dynarray(array);

    return 0;
}
