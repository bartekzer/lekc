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

void debug_node(const Node* node, int depth) {
    if (!node) {
        print_indent(depth);
        printf("(null node)\n");
        return;
    }

    print_indent(depth);
    printf("Node '%s' {\n", node->name ? node->name : "(unnamed)");

    print_indent(depth + 1);
    printf("terminal: ");
    print_value(node->terminal);
    printf("\n");

    if (node->children && node->children->size > 0) {
        print_indent(depth + 1);
        printf("children: [\n");

        for (size_t i = 0; i < node->children->size; i++) {
            Node* child = NULL;
            memcpy(&child, (char*)node->children->array + (i * node->children->elem_size), sizeof(Node*));
            debug_node(child, depth + 2);
        }

        print_indent(depth + 1);
        printf("]\n");
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
    // printf("\n\n%s(seq)\n", self->name);

    if (!self || !self->combinators) {
        ctxt->error = NULL_MSG;
        return 0;
    }

    size_t index_copy = ctxt->input->index;
    void *ptr_copy = ctxt->input->src->ptr;

    Node *old_parent = ctxt->parent_node;
    Node *new_parent = malloc(sizeof(Node));
    if (!new_parent) {
        ctxt->error = memory_err_msg(sizeof(Node));
        return 0;
    }
    new_parent->name = self->name;
    new_parent->terminal = self->terminal;
    new_parent->children = CREATE_DYNARRAY(5, sizeof(Node*));
    PUSH_DYNARRAY(old_parent->children, new_parent);
    ctxt->parent_node = new_parent;

    dynarray *combinators = self->combinators;

    for (size_t i = 0; i < combinators->size; i++) {
        Combinator *combinator = GET_DYNARRAY(combinators, i, Combinator*);

        if (!combinator) {
            ctxt->input->index = index_copy;
            ctxt->input->src->ptr = ptr_copy;
            ctxt->error = NULL_MSG;

            ctxt->parent_node = old_parent;
            return 0;
        }

        if (!combinator->fn(ctxt, combinator)) {
            ctxt->input->index = index_copy;
            ctxt->input->src->ptr = ptr_copy;
            ctxt->error = failed_err_msg(combinator->name);

            ctxt->parent_node = old_parent;
            return 0;
        }
    }

    ctxt->parent_node = old_parent;
    return 1;
}

int
alt(Context *ctxt,
    Combinator *self)
{
    // printf("\n\n%s(alt)\n", self->name);

    if (!self || !self->combinators) {
        ctxt->error = NULL_MSG;
        return 0;
    }

    size_t index_copy = ctxt->input->index;
    void *ptr_copy = ctxt->input->src->ptr;

    Node *old_parent = ctxt->parent_node;
    Node *new_parent = malloc(sizeof(Node));
    if (!new_parent) {
        ctxt->error = memory_err_msg(sizeof(Node));
        return 0;
    }
    new_parent->name = self->name;
    new_parent->terminal = self->terminal;
    new_parent->children = CREATE_DYNARRAY(5, sizeof(Node*));
    PUSH_DYNARRAY(old_parent->children, new_parent);
    ctxt->parent_node = new_parent;

    dynarray *combinators = self->combinators;

    for (size_t i = 0; i < combinators->size; i++) {
        Combinator *combinator = GET_DYNARRAY(combinators, i, Combinator*);

        if (combinator->fn(ctxt, combinator)) {
            ctxt->parent_node = old_parent;
            return 1;
        }

        ctxt->input->index = index_copy;
        ctxt->input->src->ptr = ptr_copy;
        ctxt->parent_node->children = CREATE_DYNARRAY(5, sizeof(Node*));
    }

    ctxt->error = failed_err_msg(self->name);
    ctxt->parent_node = old_parent;
    return 0;
}

int
many(Context *ctxt,
     Combinator *self)
{
    // printf("\n\n%s(many)\n", self->name);

    // TODO: Force user to provide only one combinator
    dynarray *combinators = self->combinators;
    Combinator *combinator = GET_DYNARRAY(combinators, 0, Combinator*);
    if (!combinator) {
        ctxt->error = NULL_MSG;
        return 0;
    }

    Node *old_parent = ctxt->parent_node;
    Node *new_parent = malloc(sizeof(Node));
    if (!new_parent) {
        ctxt->error = memory_err_msg(sizeof(Node));
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

    ctxt->error = NULL;
    ctxt->parent_node = old_parent;
    return 1;
}

int
optional(Context *ctxt,
         Combinator *self)
{
    // printf("\n\n%s(optional)\n", self->name);

    dynarray *combinators = self->combinators;

    // TODO: Force user to provide only one combinator
    Combinator *combinator = GET_DYNARRAY(combinators, 0, Combinator*);
    if (!combinator) {
        ctxt->error = NULL_MSG;
        return 0;
    }

    Node *old_parent = ctxt->parent_node;
    Node *new_parent = malloc(sizeof(Node));
    if (!new_parent) {
        ctxt->error = memory_err_msg(sizeof(Node));
        return 0;
    }
    new_parent->name = self->name;
    new_parent->terminal = self->terminal;
    new_parent->children = CREATE_DYNARRAY(1, sizeof(Node*));
    PUSH_DYNARRAY(old_parent->children, new_parent);
    ctxt->parent_node = new_parent;

    int res = combinator->fn(ctxt, combinator);
    ctxt->parent_node = old_parent;
    return res;
}

int
skip(Context *ctxt,
     Combinator *self)
{
    // printf("\n\n%s(skip)\n", self->name);

    if (!self || !self->terminal) {
        ctxt->error = NULL_MSG;
        return 0;
    }

    iterator *it = ctxt->input->src;

    if (is_end(it)) {
        ctxt->error = "Err: Unexpected end of input";
        return 0;
    }

    int *curr = it->ptr;

    if (ctxt->eq(self->terminal, curr)) {
        next(it);
        ctxt->input->index++;
        return 1;
    }

    ctxt->error = UNEXPECTED_MSG;
    return 0;
}
