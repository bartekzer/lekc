// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "lekc.h"
#include "dynarray.h"
#include "iterator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Combinator
new_combinator(const char *name,
               Result (*fn)(Context *, Combinator *),
               void *terminal,
               dynarray *combinators)
{
    return (Combinator){
        .name = name,
        .fn = fn,
        .terminal = terminal,
        .combinators = combinators
    };
}

Cursor
new_cursor(iterator *src)
{
    return (Cursor){
        .src = src,
        .index = 0
    };
}

Result
seq(Context *ctxt,
    Combinator *self)
{
    printf("\n\n%s(seq)\n", self->name);
    if (!ctxt || !self || !self->combinators) {
        return (Result){
            .success = 0,
            .error = A
        };
    }

    dynarray *v = create_dynarray(5, sizeof(Node*));
    if (!v) {
        return (Result){
            .success = 0,
            .error = B
        };
    }

    dynarray *combinators = self->combinators;
    for (size_t i = 0; i < combinators->size; i++) {
        Combinator *combinator = GET_DYNARRAY(combinators, i, Combinator*);
        Result res = combinator->fn(ctxt, combinator);

        if (!res.success) {
            destroy_dynarray(v);
            return res;
        }

        PUSH_DYNARRAY(v, res.node);
    }

    Node *node = malloc(sizeof(Node));
    if (!node) {
        destroy_dynarray(v);
        return (Result){
            .success = 0,
            .error = C
        };
    }

    *node = (Node){
        .name = self->name,
        .terminal = self->terminal,
        .children = v
    };

    return (Result){
        .success = 1,
        .node = node
    };
}

Result
alt(Context *ctxt,
    Combinator *self)
{
    printf("\n\n%s(alt)\n", self->name);
    if (!ctxt || !self || !self->combinators) {
        return (Result){
            .success = 0,
            .error = D
        };
    }

    size_t index_copy = ctxt->input->index;
    void *ptr_copy = ctxt->input->src->ptr;

    dynarray *combinators = self->combinators;

    for (size_t i = 0; i < combinators->size; i++) {
        ctxt->input->index = index_copy;
        ctxt->input->src->ptr = ptr_copy;

        Combinator *combinator = GET_DYNARRAY(combinators, i, Combinator*);
        Result res = combinator->fn(ctxt, combinator);
        
        if (res.success) {
            Node *node = malloc(sizeof(Node));

            if (!node) {
                return (Result){
                    .success = 0,
                    .error = E
                };
            }

            *node = (Node){
                .name = self->name,
                .terminal = self->terminal,
                .children = CREATE_DYNARRAY(1, sizeof(Node*))
            };

            if (!node->children) {
                free(node);
                return (Result){
                    .success = 0,
                    .error = F
                };
            }

            PUSH_DYNARRAY(node->children, res.node);

            return (Result) {
                .success = 1,
                .node = node
            };
        } 
        /*else {
            if (i == combinators->size - 1) {
                return res;
            }
        }*/
    }

    return (Result){
        .success = 0,
        .error = G
    };
}

Result
many(Context *ctxt,
     Combinator *self)
{
    printf("\n\n%s(many)\n", self->name);

    dynarray *v = create_dynarray(5, sizeof(Node*));
    if (!v) {
        return (Result){
            .success = 0,
            .error = H
        };
    }

    dynarray *combinators = self->combinators;
    Combinator *combinator = GET_DYNARRAY(combinators, 0, Combinator*);
    if (!combinator) {
        destroy_dynarray(v);
        return (Result){
            .success = 0,
            .error = I
        };
    }

    size_t index_copy = ctxt->input->index;
    void *saved_ptr = ctxt->input->src->ptr;

    Result res;
    while (!is_end(ctxt->input->src)) {
        size_t curr_index = ctxt->input->index;
        void *curr_ptr = ctxt->input->src->ptr;
        
        res = combinator->fn(ctxt, combinator);

        if (!res.success) {
            ctxt->input->index = curr_index;
            ctxt->input->src->ptr = curr_ptr;
            break;
        }

        PUSH_DYNARRAY(v, res.node);
    }

    Node *node = malloc(sizeof(Node));
    if (!node) {
        destroy_dynarray(v);
        ctxt->input->index = index_copy;
        ctxt->input->src->ptr = saved_ptr;
        return (Result){
            .success = 0,
            .error = J
        };
    }
    *node = (Node){
        .name = self->name,
        .terminal = self->terminal,
        .children = v
    };

    return (Result){
        .success = 1,
        .node = node
    };
}

Result
optional(Context *ctxt,
         Combinator *self)
{
    printf("\n\n%s(optional)\n", self->name);

    dynarray *combinators = self->combinators;
    Combinator *combinator = GET_DYNARRAY(combinators, 0, Combinator*);

    if (!combinator) {
        return (Result){
            .success = 0,
            .error = K
        };
    }
    
    Result res = combinator->fn(ctxt, combinator);
    if (res.success) {
        Node *node = malloc(sizeof(Node));
        if (!node) {
            return (Result){
                .success = 0,
                .error = L
            };
        }

        dynarray* children = CREATE_DYNARRAY(1, Node*);
        if (!children) {
            free(node);
            return (Result){
                .success = 0,
                .error = M
            };
        }

        PUSH_DYNARRAY(children, res.node);

        *node = (Node){
            .name = self->name,
            .terminal = self->terminal,
            .children = children
        };

        return (Result){
            .success = 1,
            .node = node
        };
    } else {
        return (Result){
            .success = 1,
            .error = res.error
        };
    }
}

Result
skip(Context *ctxt,
     Combinator *self)
{   
    printf("\n\n%s(skip)\n", self->name);
    if (!ctxt || !self || !self->terminal) {
        return (Result){
            .success = 0,
            .error = N
        };
    }

    iterator *it = ctxt->input->src;

    if (is_end(it)) {
        printf("Error O"); // unexpected end of input
        return (Result){
            .success = 0,
            .error = O
        };
    }

    int *curr = it->ptr;

    printf("expected: %d\n", *(int *)self->terminal);
    printf("curr: %d\n", *(int *)curr);
    printf("eq: %d\n", ctxt->eq(self->terminal, curr));
    if (ctxt->eq(self->terminal, curr)) {
        printf("is end : %d\n", !is_end(it));
        next(it);
        ctxt->input->index++;
        printf("index: %d\n", ctxt->input->index);

        printf("Success");
        return (Result){
            .success = 1,
            .node = NULL,
        };
    }

    printf("Error P");
    return (Result){
        .success = 0,
        .error = P
    };
}