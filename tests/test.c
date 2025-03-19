// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "lekc.h"
#include "dynarray.h"
#include "iterator.h"
#include <stdio.h>
#include <stdlib.h>

int compare_int(void *a, void *b) {
    return *(int *)a == *(int *)b;
}

int main() {
    int array[10] = { 0, 0, 0, 1, 0, 0, 1, 1, 0, 1 };
    iterator it = init(array, sizeof(int), 10);

    Cursor cursor = new_cursor(&it);

    Node* root = malloc(sizeof(Node));
    if (!root) return 1;
    root->name = "root";
    root->terminal = NULL;
    root->children = CREATE_DYNARRAY(10, Node*);
    if (!root->children) return 1;

    Context ctxt = {
        .input = &cursor,
        .eq = compare_int,
        .parent_node = root,
        .error = NULL
    };

    ///////// 1 0

    int zero_value = 0;
    Combinator zero = build("zero", skip, &zero_value, NULL);

    int one_value = 1;
    Combinator one = build("one", skip, &one_value, NULL);

    ///////// 0 1

    dynarray *zero_one_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!zero_one_combinators) return 1;
    PUSH_DYNARRAY(zero_one_combinators, &zero);
    PUSH_DYNARRAY(zero_one_combinators, &one);
    Combinator c8 = build("c8", seq, NULL, zero_one_combinators);

    ///////// 1 0

    dynarray *one_zero_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!one_zero_combinators) return 1;
    PUSH_DYNARRAY(one_zero_combinators, &one);
    PUSH_DYNARRAY(one_zero_combinators, &zero);
    Combinator c7 = build("c7", seq, NULL, one_zero_combinators);

    ///////// (1 0 | 0 1)
    dynarray *c6_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!c6_combinators) return 1;
    PUSH_DYNARRAY(c6_combinators, &c7);
    PUSH_DYNARRAY(c6_combinators, &c8);
    Combinator c6 = build("c6", alt, NULL, c6_combinators);

    ///////// 1 1

    dynarray *c5_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!c5_combinators) return 1;
    PUSH_DYNARRAY(c5_combinators, &one);
    PUSH_DYNARRAY(c5_combinators, &one);
    Combinator c5 = build("c5", seq, NULL, c5_combinators);

    ///////// 0 0

    dynarray *c4_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!c4_combinators) return 1;
    PUSH_DYNARRAY(c4_combinators, &zero);
    PUSH_DYNARRAY(c4_combinators, &zero);
    Combinator c4 = build("c4", seq, NULL, c4_combinators);

    ///////// (1 1 | 0 0)

    dynarray *c3_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!c3_combinators) return 1;
    PUSH_DYNARRAY(c3_combinators, &c5);
    PUSH_DYNARRAY(c3_combinators, &c4);

    Combinator c3 = build("c3", alt, NULL, c3_combinators);

    //////// (1 1 | 0 0)*

    dynarray *c2_combinators = CREATE_DYNARRAY(1, Combinator*);
    if (!c2_combinators) return 1;
    PUSH_DYNARRAY(c2_combinators, &c3);

    Combinator c2 = build("c2", many, NULL, c2_combinators);

    //////// (1 1 | 0 0)* (1 0 | 0 1)

    dynarray *c1_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!c1_combinators) return 1;
    PUSH_DYNARRAY(c1_combinators, &c2);
    PUSH_DYNARRAY(c1_combinators, &c6);

    Combinator c1 = build("c1", seq, NULL, c1_combinators);

    //////////////////////////////

    c1.fn(&ctxt, &c1);


    printf("%s\n", ctxt.error);
    debug_node(ctxt.parent_node, 0);

    return 0;
}
