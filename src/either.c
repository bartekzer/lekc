// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "either.h"
#include <stddef.h>

int is_left(Either *either)
{
    return either->right == NULL;
}

int is_right(Either *either)
{
    return either->left == NULL;
}