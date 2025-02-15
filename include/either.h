// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#ifndef EITHER_H
#define EITHER_H

typedef union {
    void *left;
    void *right;
} Either;

int is_left(Either *);
int is_right(Either *);

#endif // EITHER_H