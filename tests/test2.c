// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "lekc.h"
#include "dynarray.h"
#include "iterator.h"
#include <stdio.h>
#include <stdlib.h>

int compare_char(void *a, void *b)
{
    return *(char *)a == *(char *)b;
}

int
main()
{
    char *input = "aabbca";
    iterator it = init(input, sizeof(char), 6);

    Cursor cursor = new_cursor(&it);

    Node *root = malloc(sizeof(Node));
    if (!root) return 1;
    root->name = "root";
    root->terminal = NULL;
    root->children = CREATE_DYNARRAY(10, Node*);
    if (!root->children) return 1;

    Context context = {
        .input = &cursor,
        .eq = &compare_char,
        .success = 1,
        .parent_node = root
    };

    // a
    char a_value = 'a';
    Combinator a = build("a", skip, &a_value, NULL);

    // b
    char b_value = 'b';
    Combinator b = build("b", skip, &b_value, NULL);

    // c
    char c_value = 'c';
    Combinator c = build("c", skip, &c_value, NULL);

    // aa
    dynarray *aa_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!aa_combinators) return 1;
    PUSH_DYNARRAY(aa_combinators, &a);
    PUSH_DYNARRAY(aa_combinators, &a);
    Combinator aa = build("aa", seq, NULL, aa_combinators);

    // bb
    dynarray *bb_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!bb_combinators) return 1;
    PUSH_DYNARRAY(bb_combinators, &b);
    PUSH_DYNARRAY(bb_combinators, &b);
    Combinator bb = build("bb", seq, NULL, bb_combinators);

    // cc
    dynarray *cc_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!cc_combinators) return 1;
    PUSH_DYNARRAY(cc_combinators, &c);
    PUSH_DYNARRAY(cc_combinators, &c);
    Combinator cc = build("cc", seq, NULL, cc_combinators);

    // ab
    dynarray *ab_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!ab_combinators) return 1;
    PUSH_DYNARRAY(ab_combinators, &a);
    PUSH_DYNARRAY(ab_combinators, &b);
    Combinator ab = build("ab", seq, NULL, ab_combinators);

    // bc
    dynarray *bc_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!bc_combinators) return 1;
    PUSH_DYNARRAY(bc_combinators, &b);
    PUSH_DYNARRAY(bc_combinators, &c);
    Combinator bc = build("bc", seq, NULL, bc_combinators);

    // ca
    dynarray *ca_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!ca_combinators) return 1;
    PUSH_DYNARRAY(ca_combinators, &c);
    PUSH_DYNARRAY(ca_combinators, &a);
    Combinator ca = build("ca", seq, NULL, ca_combinators);

    // (ab | bc | ca)
    dynarray *abc_combinators = CREATE_DYNARRAY(3, Combinator*);
    if (!abc_combinators) return 1;
    PUSH_DYNARRAY(abc_combinators, &ab);
    PUSH_DYNARRAY(abc_combinators, &bc);
    PUSH_DYNARRAY(abc_combinators, &ca);
    Combinator abbcca = build("(ab | bc | ca)", alt, NULL, abc_combinators);

    // (aa | bb | cc)
    dynarray *aabbcc_combinators = CREATE_DYNARRAY(3, Combinator*);
    if (!aabbcc_combinators) return 1;
    PUSH_DYNARRAY(aabbcc_combinators, &aa);
    PUSH_DYNARRAY(aabbcc_combinators, &bb);
    PUSH_DYNARRAY(aabbcc_combinators, &cc);
    Combinator aabbcc = build("(aa | bb | cc)", alt, NULL, aabbcc_combinators);

    // (aa | bb | cc) | (ab | bc | ca)
    dynarray *aabbcc_abbcca_combinators = CREATE_DYNARRAY(2, Combinator*);
    if (!aabbcc_abbcca_combinators) return 1;
    PUSH_DYNARRAY(aabbcc_abbcca_combinators, &aabbcc);
    PUSH_DYNARRAY(aabbcc_abbcca_combinators, &abbcca);
    Combinator aabbcc_abbcca = build("(aa | bb | cc) | (ab | bc | ca)", alt, NULL, aabbcc_abbcca_combinators);

    // ((aa | bb | cc) | (ab | bc | ca))*
    dynarray *aabbcc_abbcca_combinators_star = CREATE_DYNARRAY(1, Combinator*);
    if (!aabbcc_abbcca_combinators_star) return 1;
    PUSH_DYNARRAY(aabbcc_abbcca_combinators_star, &aabbcc_abbcca);
    Combinator aabbcc_abbcca_star = build("((aa | bb | cc) | (ab | bc | ca))*", many, NULL, aabbcc_abbcca_combinators_star);

    aabbcc_abbcca_star.fn(&context, &aabbcc_abbcca_star);

    printf("%s\n", context.error);
    debug_node(context.parent_node, 0);

    return 0;
}
