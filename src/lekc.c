// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "lekc.h"
#include "dynarray.h"
#include "iterator.h"
#include <stdlib.h>
#include <string.h>



Cursor new_cursor(iterator *src)
{
    return (Cursor){
        .src = src,
        .index = 0
    };
}

Context new_ctxt(iterator *input,
                 int (*eq)(void *, void *))
{
    Cursor cursor = new_cursor(input);
    return (Context){
        .input = &cursor,
        .eq = eq
    };
}

Result seq(Context *ctxt,
           Combinator *self)
{   
    // va aller dans `children` du Node retourné dans le result si ça marche
    dynarray *v = create_dynarray(5, sizeof(Node*));
    if (!v) {
        return (Result){
            .success = 0,
            .error = UNEXPECTED
        };
    }

    dynarray *combinators = self->combinators;
    for (size_t i = 0; i < combinators->size; i++) {
        Combinator *combinator = GET_DYNARRAY(combinators, i, Combinator*);
        Result res = combinator->fn(ctxt, combinator);

        if (res.success) { // c'est passé pour ce combinator
            PUSH_DYNARRAY(v, res.node);
        } else {
            destroy_dynarray(v);
            return (Result){
                .success = 0,
                .error = res.error
            };
        }
    }

    Node *node = malloc(sizeof(Node));
    if (!node) {
        destroy_dynarray(v);
        return (Result){
            .success = 0,
            .error = UNEXPECTED
        };
    }

    *node = (Node){
        .name = "seq",
        .terminal = self->terminal, // devrait être NULL normalement
        .children = v
    };

    return (Result){
        .success = 1,
        .node = node
    };
}


Result alt(Context *ctxt,
           Combinator *self)
{
    dynarray *combinators = self->combinators;
    for (size_t i = 0; i < combinators->size; i++) {
        Context *new_ctxt = malloc(sizeof(Context));
        if (!new_ctxt) {
            return (Result){
                .success = 0,
                .error = UNEXPECTED
            };
        }
        memcpy(new_ctxt, ctxt, sizeof(Context));
        Combinator *combinator = GET_DYNARRAY(combinators, i, Combinator*);
        Result res = combinator->fn(new_ctxt, combinator);
        
        if (res.success) {
            ctxt = new_ctxt;

            Node *node = malloc(sizeof(Node));
            if (!node) {
                free(new_ctxt);
                return (Result){
                    .success = 0,
                    .error = UNEXPECTED
                };
            }

            *node = (Node){
                .name = "alt",
                .terminal = self->terminal, // devrait être NULL normalement
                .children = CREATE_DYNARRAY(2, sizeof(Node*))
            };

            if (!node->children) {
                free(node);
                free(new_ctxt);
                return (Result){
                    .success = 0,
                    .error = UNEXPECTED
                };
            }

            PUSH_DYNARRAY(node->children, res.node);
            return (Result) {
                .success = 1,
                .node = node
            };
        } else {
            free(new_ctxt);

            if (i == combinators->size - 1) {
                return (Result){
                    .success = 0,
                    .error = res.error
                };
            }
        }
    }
}   


Result many(Context *ctxt,
            Combinator *self)
{
    dynarray *v = create_dynarray(5, sizeof(Node*));
    if (!v) {
        return (Result){
            .success = 0,
            .error = UNEXPECTED
        };
    }

    dynarray *combinators = self->combinators;
    Combinator *combinator = GET_DYNARRAY(combinators, 0, Combinator*);

    if (!combinator) {
        destroy_dynarray(v);
        return (Result){
            .success = 0,
            .error = UNEXPECTED
        };
    }

    Result res = combinator->fn(ctxt, combinator);

    while (res.success) {
        PUSH_DYNARRAY(v, res.node);
        res = combinator->fn(ctxt, combinator);
    }

    Node *node = malloc(sizeof(Node));
    if (!node) {
        destroy_dynarray(v);
        return (Result){
            .success = 0,
            .error = UNEXPECTED
        };
    }
    *node = (Node){
        .name = "many",
        .terminal = self->terminal,
        .children = v
    };

    return (Result){
        .success = 1,
        .node = node
    };
}
















/*
 * For the moment, all combinators act as if
 * only 2 combinators had been passed to them,
 * so that we can pop from the `combinators` without
 * further complication. Soon they will accept
 * an arbitrary number in accordance with their
 * dynamic nature.
 */

// int seq_fn(Combinator* combinator,
//            Context *ctxt)
// {
//     if (ctxt->success) {
//         Combinator *p2 = POP_DYNARRAY(combinator->combinators, Combinator);
//         Combinator *p1 = POP_DYNARRAY(combinator->combinators, Combinator);

//         if (!p1->fn(ctxt, p1->combinators)) {
//             PUSH_DYNARRAY(ctxt->output, ctxt->result.ast);
//         } else {
//             return 1;
//         }
        
//         if (!p2->fn(ctxt, p2->combinators)) {
//             PUSH_DYNARRAY(ctxt->output, ctxt->result.ast);
//         } else {
//             return 1;
//         }
        
//         return 0;

//     } else {
//         return 1;
//     }
// }

// // for now works like a `or` combinator
// int alt_fn(Combinator *combinator,
//            Context *ctxt)
// {
//     if (ctxt->success) {
//         Combinator *p2 = POP_DYNARRAY(combinator->combinators, Combinator);
//         Combinator *p1 = POP_DYNARRAY(combinator->combinators, Combinator);

//         if (!p1->fn(ctxt, p1->combinators)) {
//             PUSH_DYNARRAY(ctxt->output, ctxt->result.ast);
//             return 0;
//         }

//         if (p2->fn(ctxt, p2->combinators)) {
//             return 1;
//         } else {
//             PUSH_DYNARRAY(ctxt->output, ctxt->result.ast);
//             return 0;
//         }
//     } else {
//         return 1;
//     }
// }

// int many_fn(Combinator *combinator,
//             Context *ctxt)
// {
//     if (ctxt->success) {
//         Combinator *p = POP_DYNARRAY(combinator->combinators, Combinator);

//         dynarray *v = create_dynarray(20, sizeof(ctxt->output->elem_size));
        
//         while (!p->fn(ctxt, p->combinators)) {
//             PUSH_DYNARRAY(v, ctxt->result.ast);
//         }

//         PUSH_DYNARRAY(ctxt->output, v);
//         destroy_dynarray(v);
//         return 0; // there will always be at least 0 successes
//     } else {
//         return 1;
//     }
// }

// int many1_fn(Combinator *combinator,
//              Context *ctxt)
// {
//     if (ctxt->success) {
//         Combinator *p = POP_DYNARRAY(combinator->combinators, Combinator);

//         dynarray *v = create_dynarray(20, sizeof(ctxt->output->elem_size));
        
//         while (!p->fn(ctxt, p->combinators)){
//             PUSH_DYNARRAY(v, ctxt->result.ast);
//         }

//         if (!v->size) {
//             destroy_dynarray(v);
//             return 1;
//         }

//         PUSH_DYNARRAY(ctxt->output, v);
//         destroy_dynarray(v);
//         return 0;
//     } else {
//         return 1;
//     }
// }

// Context new_ctxt(iterator *input,
//                  int (*eq)(void *, void *),
//                  int node_size)
// {
//     return (Context){
//         .index = 0,
//         .start = 0,
//         .input = input,
//         .output = create_dynarray(100, node_size),
//         .eq = eq,
//         .success = 1,
//         .result = {}
//     };
// }