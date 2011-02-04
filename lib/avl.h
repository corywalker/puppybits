/* Produced by texiweb from libavl.w. */

/* libavl - library for manipulation of binary trees.
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004 Free Software
   Foundation, Inc.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA.
*/

#ifndef AVL_H
#define AVL_H 1

#include <stdint.h>

/* An AVL tree node. */
struct AVLNode {
	struct AVLNode *Link[2];   /* Subtrees. */
	intptr_t Private;          /* actually Parent pointer and balance factor bitfield */
};

typedef int AVLKEYCOMP(const void *keyb, const struct AVLNode *nodea);
typedef int AVLNODECOMP(const struct AVLNode *AVL_a, const struct AVLNode *AVL_b);

struct AVLNode *AVL_Find    (struct AVLNode *tree, const void *item, AVLKEYCOMP cmpkey);
struct AVLNode *AVL_FindNext(struct AVLNode *tree, const void *item, AVLKEYCOMP cmpkey);
struct AVLNode *AVL_FindPrev(struct AVLNode *tree, const void *item, AVLKEYCOMP cmpkey);
struct AVLNode *AVL_Insert  (struct AVLNode **tree, struct AVLNode *item, AVLNODECOMP cmpnode);
struct AVLNode *AVL_Move    (struct AVLNode **table, struct AVLNode *p, struct AVLNode *item);
struct AVLNode *AVL_Replace (struct AVLNode **tree, struct AVLNode *item, AVLNODECOMP cmpnode);
struct AVLNode *AVL_Remove  (struct AVLNode **tree, struct AVLNode *node);
struct AVLNode *AVL_First   (struct AVLNode *root);
struct AVLNode *AVL_Last    (struct AVLNode *root);
struct AVLNode *AVL_Next    (struct AVLNode *node);
struct AVLNode *AVL_Prev    (struct AVLNode *node);

#endif
