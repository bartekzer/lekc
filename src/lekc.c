// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "lekc.h"
#include "dynarray.h"
#include "iterator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define UNEXPECTED_MSG "Err: Unexpected token"
#define NULL_MSG "Err: Null pointer"

const char *
failed_err_msg(const char *name)
{
    int len = strlen(name) + 24;
    char error_buffer[len];
    snprintf(error_buffer, len, "Err: Combinator %s failed", name);
    return strdup(error_buffer);
}

const char *
memory_err_msg(size_t size)
{
    char error_buffer[256];
    snprintf(error_buffer, 256, "Err: Allocation %zu bytes", size);
    return strdup(error_buffer);
}

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

static void print_indent(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
}

static void print_value(void* value) {
    if (!value) {
        printf("null");
        return;
    }

    printf("%d", *(int*)value);
}

static int is_node_valid(const Node* node) {
    if (!node) return 0;
    if (!node->name) return 0;
    if (!node->children) return 0;
    return 1;
}

void debug_node(const Node* node, int depth) {
    if (depth > 100) {
        printf("Error: Maximum depth exceeded\n");
        return;
    }

    if (!is_node_valid(node)) {
        print_indent(depth);
        printf("(invalid/freed node)\n");
        return;
    }

    print_indent(depth);
    printf("Node '%s' {\n", node->name);

    print_indent(depth + 1);
    printf("terminal: ");
    print_value(node->terminal);
    printf("\n");

    if (node->children) {
        if (node->children->size > 0) {
            print_indent(depth + 1);
            printf("children [%zu]: [\n", node->children->size);

            for (size_t i = 0; i < node->children->size; i++) {
                Node* child = NULL;

                // Vérification sécurisée de l'accès au tableau
                if (i * node->children->elem_size <
                    node->children->size * node->children->elem_size) {
                    memcpy(&child,
                           (char*)node->children->array + (i * node->children->elem_size),
                           sizeof(Node*));
                }

                if (is_node_valid(child)) {
                    debug_node(child, depth + 2);
                } else {
                    print_indent(depth + 2);
                    printf("(invalid child at index %zu)\n", i);
                }
            }

            print_indent(depth + 1);
            printf("]\n");
        } else {
            print_indent(depth + 1);
            printf("children: [empty]\n");
        }
    } else {
        print_indent(depth + 1);
        printf("children: null\n");
    }

    print_indent(depth);
    printf("}\n");
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

    if (!self || !self->combinators) {
        ctxt->error = NULL_MSG;
        ctxt->success = 0;
        return 0;
    }

    size_t index_copy = ctxt->input->index;
    void *ptr_copy = ctxt->input->src->ptr;

    Node *old_parent = ctxt->parent_node;
    Node *new_parent = malloc(sizeof(Node));
    if (!new_parent) {
        ctxt->error = memory_err_msg(sizeof(Node));
        ctxt->success = 0;
        return 0;
    }
    new_parent->name = self->name;
    new_parent->terminal = self->terminal;
    new_parent->children = CREATE_DYNARRAY(5, sizeof(Node*));
    if (!new_parent->children) {
        free(new_parent);
        ctxt->error = memory_err_msg(sizeof(dynarray));
        ctxt->success = 0;
        return 0;
    }
    ctxt->parent_node = new_parent;

    dynarray *combinators = self->combinators;

    for (size_t i = 0; i < combinators->size; i++) {
        Combinator *combinator = GET_DYNARRAY(combinators, i, Combinator*);

        if (!combinator) {
            ctxt->input->index = index_copy;
            ctxt->input->src->ptr = ptr_copy;
            ctxt->error = NULL_MSG;
            ctxt->success = 0;

            free(new_parent->children); // ?
            free(new_parent); // ?

            ctxt->parent_node = old_parent; // ?
            return 0;
        }

        if (!combinator->fn(ctxt, combinator)) {
            ctxt->input->index = index_copy;
            ctxt->input->src->ptr = ptr_copy;

            free(new_parent->children);
            free(new_parent);

            ctxt->parent_node = old_parent;
            return 0;
        }
    }

    PUSH_DYNARRAY(old_parent->children, new_parent);
    ctxt->parent_node = old_parent;
    ctxt->error = NULL; // ?
    ctxt->success = 1;
    return 1;
}

int
alt(Context *ctxt,
    Combinator *self)
{
    printf("\n\n%s(alt)\n", self->name);

    if (!self || !self->combinators) {
        ctxt->error = NULL_MSG;
        ctxt->success = 0;
        return 0;
    }

    size_t index_copy = ctxt->input->index;
    void *ptr_copy = ctxt->input->src->ptr;

    Node *old_parent = ctxt->parent_node;
    Node *new_parent = malloc(sizeof(Node));
    if (!new_parent) {
        ctxt->error = memory_err_msg(sizeof(Node));
        ctxt->success = 0;
        return 0;
    }
    new_parent->name = self->name;
    new_parent->terminal = self->terminal;
    new_parent->children = CREATE_DYNARRAY(5, sizeof(Node*));
    if (!new_parent->children) {
        free(new_parent);
        ctxt->error = memory_err_msg(sizeof(dynarray));
        ctxt->success = 0;
        return 0;
    }

    ctxt->parent_node = new_parent;

    dynarray *combinators = self->combinators;

    for (size_t i = 0; i < combinators->size; i++) {
        Combinator *combinator = GET_DYNARRAY(combinators, i, Combinator*);

        if (!combinator) {
            ctxt->input->index = index_copy;
            ctxt->input->src->ptr = ptr_copy;
            ctxt->error = NULL_MSG;
            ctxt->success = 0;

            free(new_parent->children); // ?
            free(new_parent); // ?

            ctxt->parent_node = old_parent; // ?
            return 0;
        }

        if (combinator->fn(ctxt, combinator)) {
            PUSH_DYNARRAY(old_parent->children, new_parent);
            ctxt->parent_node = old_parent;
            ctxt->error = NULL; // ?
            ctxt->success = 1;
            return 1;
        }

        ctxt->input->index = index_copy;
        ctxt->input->src->ptr = ptr_copy;
        // cleanup_dynarray(new_parent->new_parent); // err
        // free(ctxt->parent_node->children);
        // ctxt->parent_node->children = CREATE_DYNARRAY(5, sizeof(Node*)); // ?
        new_parent->children->size = 0;
    }

    // ctxt->error = failed_err_msg(self->name);
    ctxt->parent_node = old_parent;
    ctxt->success = 0;

    free(new_parent->children);
    free(new_parent);
    return 0;
}

int
many(Context *ctxt,
     Combinator *self)
{
    printf("\n\n%s(many)\n", self->name);

    // TODO: Force user to provide only one combinator
    dynarray *combinators = self->combinators;
    Combinator *combinator = GET_DYNARRAY(combinators, 0, Combinator*);
    if (!combinator) {
        ctxt->error = NULL_MSG;
        ctxt->success = 0;
        return 0;
    }

    Node *old_parent = ctxt->parent_node;
    Node *new_parent = malloc(sizeof(Node));
    if (!new_parent) {
        ctxt->error = memory_err_msg(sizeof(Node));
        ctxt->success = 0;
        return 0;
    }
    new_parent->name = self->name;
    new_parent->terminal = self->terminal;
    new_parent->children = CREATE_DYNARRAY(5, sizeof(Node*));
    PUSH_DYNARRAY(old_parent->children, new_parent);
    ctxt->parent_node = new_parent;

    while (!is_end(ctxt->input->src)) {
        size_t curr_index = ctxt->input->index;
        void *curr_ptr = ctxt->input->src->ptr;

        if (!combinator->fn(ctxt, combinator)) {
            ctxt->input->index = curr_index;
            ctxt->input->src->ptr = curr_ptr;
            break;
        }
    }

    ctxt->parent_node = old_parent;
    ctxt->success = 1;
    return 1;
}

int
optional(Context *ctxt,
         Combinator *self)
{
    printf("\n\n%s(optional)\n", self->name);

    dynarray *combinators = self->combinators;

    // TODO: Force user to provide only one combinator
    Combinator *combinator = GET_DYNARRAY(combinators, 0, Combinator*);
    if (!combinator) {
        ctxt->error = NULL_MSG;
        ctxt->success = 0;
        return 0;
    }

    Node *old_parent = ctxt->parent_node;
    Node *new_parent = malloc(sizeof(Node));
    if (!new_parent) {
        ctxt->error = memory_err_msg(sizeof(Node));
        ctxt->success = 0;
        return 0;
    }
    new_parent->name = self->name;
    new_parent->terminal = self->terminal;
    new_parent->children = CREATE_DYNARRAY(1, sizeof(Node*));
    PUSH_DYNARRAY(old_parent->children, new_parent);
    ctxt->parent_node = new_parent;

    if (!combinator->fn(ctxt, combinator)) {
        free(new_parent->children);
        free(new_parent);
        ctxt->error = NULL;
    }

    ctxt->parent_node = old_parent;
    ctxt->success = 1;
    return 1;
}

int
skip(Context *ctxt,
     Combinator *self)
{
    printf("\n\n%s(skip)\n", self->name);

    if (!self || !self->terminal) {
        ctxt->error = NULL_MSG;
        ctxt->success = 0;
        return 0;
    }

    iterator *it = ctxt->input->src;

    if (is_end(it)) {
        ctxt->error = "Err: Unexpected end of input";
        ctxt->success = 0;
        return 0;
    }

    int *curr = it->ptr;

    printf("expected: %d\n", *(int *)self->terminal);
    printf("curr: %d\n", *(int *)curr);
    printf("Are equal ? %d\n", ctxt->eq(self->terminal, curr));
    if (ctxt->eq(self->terminal, curr)) {
        next(it);
        ctxt->input->index++;
        ctxt->success = 1; // ?
        ctxt->error = NULL; // ?
        printf("Skip worked");
        return 1;
    }

    printf("Skip failed");
    ctxt->error = UNEXPECTED_MSG;
    ctxt->success = 0;
    return 0;
}
