// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "lekc.h"
#include "dynarray.h"
#include "iterator.h"
#include <stdio.h>
#include <stdlib.h>

void
print_node(Node* node,
           int depth)
{
    if (!node) {
        for (int i = 0; i < depth; i++) printf("  ");
        printf("<NULL NODE>\n");
        return;
    }

    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    printf("%s (type: %s)", 
           node->name ? node->name : "<NO NAME>",
           node->terminal ? "terminal" : "non-terminal");
    
    if (node->terminal) {
        printf(" value: %d", *(int*)node->terminal);
    }
    printf("\n");

    if (node->children) {
        for (int i = 0; i < depth + 1; i++) printf("  ");
        printf("Children count: %zu\n", node->children->size);

        for (size_t i = 0; i < node->children->size; i++) {
            Node* child = GET_DYNARRAY(node->children, i, Node*);
            if (!child) {
                for (int j = 0; j < depth + 1; j++) printf("  ");
                printf("Child %zu: <NULL>\n", i);
                continue;
            }
            print_node(child, depth + 1);
        }
    }
}

int compare_int(void *a, void *b) {
    return *(int *)a == *(int *)b;
}

char *show_error(Error error)
{
    switch (error) {
        case A: return "A";
        case B: return "B";
        case C: return "C";
        case D: return "D";
        case E: return "E";
        case F: return "F";
        case G: return "G";
        case H: return "H";
        case I: return "I";
        case J: return "J";
        case K: return "K";
        case L: return "L";
        case M: return "M";
        case N: return "N";
        case O: return "O";
        case P: return "P";
        case Q: return "Q";
        case R: return "R";
        case S: return "S";
        case T: return "T";
        case U: return "U";
        case V: return "V";
        case W: return "W";
        case X: return "X";
        case Y: return "Y";
        case Z: return "Z";
        case AMBIGUOUS: return "AMBIGUOUS";
        case UNEXPECTED: return "UNEXPECTED";
        default: "UNKNOWN";
    }
}

int main() {
    int array[10] = { 0, 0, 1, 1, 0, 0, 1, 1, 1, 0 };
    iterator it = init(array, sizeof(int), 10);
    // for (; !is_end(&it); next(&it)) {
    //     printf("%d ", *(int*)it.ptr);
    // }

    Cursor cursor = new_cursor(&it);
    Context ctxt = {
        .input = &cursor,
        .eq = compare_int
    };

    ///////// 1 and 0

    int zero_value = 0;
    Combinator zero = new_combinator("zero", skip, &zero_value, NULL);

    int one_value = 1;
    Combinator one = new_combinator("one", skip, &one_value, NULL);

    ///////// 0 1

    dynarray *zero_one_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!zero_one_combinators) return 1;
    PUSH_DYNARRAY(zero_one_combinators, &zero);
    PUSH_DYNARRAY(zero_one_combinators, &one);
    Combinator c8 = new_combinator("c8", seq, NULL, zero_one_combinators);

    ///////// 1 0

    dynarray *one_zero_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!one_zero_combinators) return 1;
    PUSH_DYNARRAY(one_zero_combinators, &one);
    PUSH_DYNARRAY(one_zero_combinators, &zero);
    Combinator c7 = new_combinator("c7", seq, NULL, one_zero_combinators);

    ///////// (1 0 | 0 1)
    dynarray *c6_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!c6_combinators) return 1;
    PUSH_DYNARRAY(c6_combinators, &c7);
    PUSH_DYNARRAY(c6_combinators, &c8);
    Combinator c6 = new_combinator("c6", alt, NULL, c6_combinators);

    ///////// 1 1

    dynarray *c5_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!c5_combinators) return 1;
    PUSH_DYNARRAY(c5_combinators, &one);
    PUSH_DYNARRAY(c5_combinators, &one);
    Combinator c5 = new_combinator("c5", seq, NULL, c5_combinators);

    ///////// 0 0

    dynarray *c4_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!c4_combinators) return 1;
    PUSH_DYNARRAY(c4_combinators, &zero);
    PUSH_DYNARRAY(c4_combinators, &zero);
    Combinator c4 = new_combinator("c4", seq, NULL, c4_combinators);

    ///////// (1 1 | 0 0)

    dynarray *c3_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!c3_combinators) return 1;
    PUSH_DYNARRAY(c3_combinators, &c4);
    PUSH_DYNARRAY(c3_combinators, &c5);

    Combinator c3 = new_combinator("c3", alt, NULL, c3_combinators);

    //////// (1 1 | 0 0)*

    dynarray *c2_combinators = CREATE_DYNARRAY(1, Combinator*);
    if (!c2_combinators) return 1;
    PUSH_DYNARRAY(c2_combinators, &c3);
    PUSH_DYNARRAY(c2_combinators, &c4);

    Combinator c2 = new_combinator("c2", many, NULL, c2_combinators);

    //////// (1 1 | 0 0)* (1 0 | 0 1)

    dynarray *c1_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!c1_combinators) return 1;
    PUSH_DYNARRAY(c1_combinators, &c2);
    PUSH_DYNARRAY(c1_combinators, &c6);

    Combinator c1 = new_combinator("c1", seq, NULL, c1_combinators);

    //////////////////////////////

    Result r = c1.fn(&ctxt, &c1);

    if (r.success) {
        print_node(r.node, 0);
    } else {
        printf("Failed with error: %s\n", show_error(r.error));
    }

    return 0;
}