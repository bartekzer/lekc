// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "lekc.h"
#include "dynarray.h"
#include "iterator.h"

/*
 * For the moment, all combinators act as if
 * only 2 combinators had been passed to them,
 * so that we can pop from the `combinators` without
 * further complication. Soon they will accept
 * an arbitrary number in accordance with their
 * dynamic nature.
 */

int seq_fn(Context *ctxt,
           dynarray *combinators)
{
    if (ctxt->success) {
        Combinator *p2 = POP_DYNARRAY(combinators, Combinator);
        Combinator *p1 = POP_DYNARRAY(combinators, Combinator);

        if (!p1->fn(ctxt, p1->combinators)) {
            PUSH_DYNARRAY(ctxt->output, ctxt->result.ast);
        } else {
            return 1;
        }
        
        if (!p2->fn(ctxt, p2->combinators)) {
            PUSH_DYNARRAY(ctxt->output, ctxt->result.ast);
        } else {
            return 1;
        }
        
        return 0;

    } else {
        return 1;
    }
}

// for now works like a `or` combinator
int alt_fn(Context *ctxt,
           dynarray *combinators)
{
    if (ctxt->success) {
        Combinator *p2 = POP_DYNARRAY(combinators, Combinator);
        Combinator *p1 = POP_DYNARRAY(combinators, Combinator);

        if (!p1->fn(ctxt, p1->combinators)) {
            PUSH_DYNARRAY(ctxt->output, ctxt->result.ast);
            return 0;
        }

        if (p2->fn(ctxt, p2->combinators)) {
            return 1;
        } else {
            PUSH_DYNARRAY(ctxt->output, ctxt->result.ast);
            return 0;
        }
    } else {
        return 1;
    }
}

int many_fn(Context *ctxt,
            dynarray *combinators)
{
    if (ctxt->success) {
        Combinator *p = POP_DYNARRAY(combinators, Combinator);

        dynarray *v = create_dynarray(20, sizeof(ctxt->output->elem_size));
        
        while (!p->fn(ctxt, p->combinators)) {
            PUSH_DYNARRAY(v, ctxt->result.ast);
        }

        PUSH_DYNARRAY(ctxt->output, v);
        destroy_dynarray(v);
        return 0; // there will always be at least 0 successes
    } else {
        return 1;
    }
}

int many1_fn(Context *ctxt,
             dynarray *combinators)
{
    if (ctxt->success) {
        Combinator *p = POP_DYNARRAY(combinators, Combinator);

        dynarray *v = create_dynarray(20, sizeof(ctxt->output->elem_size));
        
        while (!p->fn(ctxt, p->combinators)){
            PUSH_DYNARRAY(v, ctxt->result.ast);
        }

        if (!v->size) {
            destroy_dynarray(v);
            return 1;
        }

        PUSH_DYNARRAY(ctxt->output, v);
        destroy_dynarray(v);
        return 0;
    } else {
        return 1;
    }
}

Context new_ctxt(iterator *input,
                 int (*eq)(void *, void *),
                 int node_size)
{
    return (Context){
        .index = 0,
        .start = 0,
        .input = input,
        .output = create_dynarray(100, node_size),
        .eq = eq,
        .success = 1,
        .result = {}
    };
}