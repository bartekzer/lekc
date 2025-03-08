// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "lekc.h"
#include "dynarray.h"
#include "iterator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Combinator
build(const char *name,
      int (*fn)(Context *, Combinator *),
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

int
seq(Context *ctxt,
    Combinator *self)
{
    printf("\n\n%s(seq)\n", self->name);

    if (ctxt->error) return 0;

    if (!ctxt || !self || !self->combinators) {
        // return (Result){
        //     .success = 0,
        //     .error = A
        // };
        ctxt->error = A;
        return 0;
    }

    // Rajouter la sauvegarde des indices au cas où
    // size_t index_copy = ctxt->input->index;
    // void *ptr_copy = ctxt->input->src->ptr;

    // dynarray *v = create_dynarray(5, sizeof(Node*));
    // if (!v) {
    //     return (Result){
    //         .success = 0,
    //         .error = B
    //     };
    // }


    //////////
    Node *old_parent = ctxt->parent_node;
    Node *new_parent = malloc(sizeof(Node));
    if (!new_parent) {
        ctxt->error = B;
        return 0;
    }
    new_parent->name = self->name;
    new_parent->terminal = self->terminal;
    new_parent->children = CREATE_DYNARRAY(5, sizeof(Node*));
    PUSH_DYNARRAY(old_parent->children, new_parent);
    ctxt->parent_node = new_parent;
    //////////


    dynarray *combinators = self->combinators;
    for (size_t i = 0; i < combinators->size; i++) {
        // ctxt->input->index = index_copy;
        // ctxt->input->src->ptr = ptr_copy;
        Combinator *combinator = GET_DYNARRAY(combinators, i, Combinator*);
        if (!combinator) {
            ctxt->error = C;
            return 0;
        }
        if (!combinator->fn(ctxt, combinator)) {
            ctxt->error = D;
            return 0;
        }

        // Combinator *combinator = GET_DYNARRAY(combinators, i, Combinator*);
        // Result res = combinator->fn(ctxt, combinator);

        // if (!res.success) {
        //     destroy_dynarray(v);
        //     return res;
        // }

        // PUSH_DYNARRAY(v, res.node);
    }

    ctxt->parent_node = old_parent;
    return 1;

    // Node *node = malloc(sizeof(Node));
    // if (!node) {
    //     destroy_dynarray(v);
    //     return (Result){
    //         .success = 0,
    //         .error = C
    //     };
    // }

    // *node = (Node){
    //     .name = self->name,
    //     .terminal = self->terminal,
    //     .children = v
    // };

    // return (Result){
    //     .success = 1,
    //     .node = node
    // };
}

int
alt(Context *ctxt,
    Combinator *self)
{
    printf("\n\n%s(alt)\n", self->name);

    if (ctxt->error) return 0;

    if (!ctxt || !self || !self->combinators) {
        ctxt->error = E; // impossible car ctxt n'est justement pas initialisé
        return 0;
    }

    size_t index_copy = ctxt->input->index;
    void *ptr_copy = ctxt->input->src->ptr;


    //////////
    Node *old_parent = ctxt->parent_node;
    Node *new_parent = malloc(sizeof(Node));
    if (!new_parent) {
        ctxt->error = F;
        return 0;
    }
    new_parent->name = self->name;
    new_parent->terminal = self->terminal;
    new_parent->children = CREATE_DYNARRAY(5, sizeof(Node*));
    PUSH_DYNARRAY(old_parent->children, new_parent);
    ctxt->parent_node = new_parent;
    //////////


    dynarray *combinators = self->combinators;

    for (size_t i = 0; i < combinators->size; i++) {
        ctxt->input->index = index_copy;
        ctxt->input->src->ptr = ptr_copy;

        Combinator *combinator = GET_DYNARRAY(combinators, i, Combinator*);
        
        if (combinator->fn(ctxt, combinator)) {
            ctxt->parent_node = old_parent;
            return 1;
            // Node *node = malloc(sizeof(Node));

            // if (!node) {
            //     return (Result){
            //         .success = 0,
            //         .error = E
            //     };
            // }

            // *node = (Node){
            //     .name = self->name,
            //     .terminal = self->terminal,
            //     .children = CREATE_DYNARRAY(1, sizeof(Node*))
            // };

            // if (!node->children) {
            //     free(node);
            //     return (Result){
            //         .success = 0,
            //         .error = F
            //     };
            // }

            // PUSH_DYNARRAY(node->children, res.node);

            // return (Result) {
            //     .success = 1,
            //     .node = node
            // };
        }
        // nécessaire de créer un nouveau dynarray ?
        // typiquement, seq peut foirer au dernier combinator
        ctxt->parent_node->children = CREATE_DYNARRAY(5, sizeof(Node*));
        /* else {
            if (i == combinators->size - 1) {
                return res;
            }
        }*/
    }

    ctxt->error = G;
    return 0;
}

int
many(Context *ctxt,
     Combinator *self)
{
    printf("\n\n%s(many)\n", self->name);

    if (ctxt->error) return 0;

    // inutile je pense car ça ajoute directement dans parent_node->children
    // dynarray *v = create_dynarray(5, sizeof(Node*));
    // if (!v) {
    //     ctxt->error = H;
    //     return 0;
    // }

    dynarray *combinators = self->combinators;
    Combinator *combinator = GET_DYNARRAY(combinators, 0, Combinator*);
    if (!combinator) {
        ctxt->error = H;
        return 0;
    }

    // size_t index_copy = ctxt->input->index;
    // void *saved_ptr = ctxt->input->src->ptr;


    //////////
    Node *old_parent = ctxt->parent_node;
    Node *new_parent = malloc(sizeof(Node));
    if (!new_parent) {
        ctxt->error = I;
        return 0;
    }
    new_parent->name = self->name;
    new_parent->terminal = self->terminal;
    new_parent->children = CREATE_DYNARRAY(5, sizeof(Node*));
    PUSH_DYNARRAY(old_parent->children, new_parent);
    ctxt->parent_node = new_parent;
    //////////


    // Result res;
    while (!is_end(ctxt->input->src)) {
        size_t curr_index = ctxt->input->index;
        void *curr_ptr = ctxt->input->src->ptr;
        
        // res = combinator->fn(ctxt, combinator);

        if (!combinator->fn(ctxt, combinator)) {
            ctxt->input->index = curr_index;
            ctxt->input->src->ptr = curr_ptr;
            break;
        }
    }

    // Node *node = malloc(sizeof(Node));
    // if (!node) {
    //     destroy_dynarray(v);
    //     ctxt->input->index = index_copy;
    //     ctxt->input->src->ptr = saved_ptr;
    //     return (Result){
    //         .success = 0,
    //         .error = J
    //     };
    // }
    // *node = (Node){
    //     .name = self->name,
    //     .terminal = self->terminal,
    //     .children = v
    // };

    // return (Result){
    //     .success = 1,
    //     .node = node
    // };
    ctxt->error = 0;
    ctxt->parent_node = old_parent;
    return 1;
}

int
optional(Context *ctxt,
         Combinator *self)
{
    printf("\n\n%s(optional)\n", self->name);

    if (ctxt->error) return 0;

    dynarray *combinators = self->combinators;

    Combinator *combinator = GET_DYNARRAY(combinators, 0, Combinator*);
    if (!combinator) {
        ctxt->error = J;
        return 0;
    }


    //////////
    Node *old_parent = ctxt->parent_node;
    Node *new_parent = malloc(sizeof(Node));
    if (!new_parent) {
        ctxt->error = K;
        return 0;
    }
    new_parent->name = self->name;
    new_parent->terminal = self->terminal;
    new_parent->children = CREATE_DYNARRAY(1, sizeof(Node*));
    PUSH_DYNARRAY(old_parent->children, new_parent);
    ctxt->parent_node = new_parent;
    //////////


    // Result res = combinator->fn(ctxt, combinator);
    if (!combinator->fn(ctxt, combinator)) {
        ctxt->error = L;
        return 0;
    }

    ctxt->error = 0;
    ctxt->parent_node = old_parent;
    return 1;
}

int
skip(Context *ctxt,
     Combinator *self)
{   
    printf("\n\n%s(skip)\n", self->name);

    if (ctxt->error) return 0;

    if (!ctxt || !self || !self->terminal) {
        ctxt->error = M;
        return 0;
    }

    iterator *it = ctxt->input->src;

    if (is_end(it)) {
        printf("Error N");
        ctxt->error = N;
        return 0;
    }

    int *curr = it->ptr;

    printf("expected: %d\n", *(int *)self->terminal);
    printf("curr: %d\n", *(int *)curr);
    printf("eq: %d\n", ctxt->eq(self->terminal, curr));
    if (ctxt->eq(self->terminal, curr)) {
        printf("is end : %d\n", !is_end(it));
        next(it);
        ctxt->input->index++;
        printf("index: %ld\n", ctxt->input->index);

        printf("Success");
        // return (Result){
        //     .success = 1,
        //     .node = NULL,
        // };
        return 1;
    }

    ctxt->error = O;
    return 0;
}