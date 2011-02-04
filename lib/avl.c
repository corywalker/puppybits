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

/*
  2009-05-21 Michael Zucchi

  Modified to remove traverser, tree object, allocations,
  and data pointer.
  Added find >= and <= functions.
  Renamed functions and types.

  2010-03-014 Michael Zucchi

  Merged parent/balance into bitfield, to reduce size of structure
  by 4 bytes.
  Added Move function.
*/
#include "avl.h"

#define NULL ((void *)0L)

#define getBalance(n) ((n)->Private & 0x07)
#define setBalance(n, v) ((n)->Private = ((n)->Private & ~7) | (v))

#define getParent(n) ((struct AVLNode *)((n)->Private & ~0x07))
#define setParent(n, v) ((n)->Private = ((n)->Private & 7) | (intptr_t)(v))

#define setBoth(n, p, b) ((n)->Private = (((intptr_t)p) & ~7) | ((b) & 0x7))

#define incBalance(n) ((n)->Private = (((n)->Private + 1) & 7) | ((n)->Private & ~7))
#define decBalance(n) ((n)->Private = (((n)->Private - 1) & 7) | ((n)->Private & ~7))

/* Search |tree| for an item matching |item|, and return it if found.
   Otherwise return |NULL|. */
struct AVLNode *
AVL_Find(struct AVLNode *p, const void *item, AVLKEYCOMP cmpkey)
{
	int dir;

	for (; p != NULL; p = p->Link[dir]) {
		int cmp = cmpkey(item, p);

		if (cmp == 0)
			break;

		dir = cmp > 0;
	}

	return (struct AVLNode *)p;
}

/* Find node >= item */
struct AVLNode *
AVL_FindNext(struct AVLNode *p, const void *item, AVLKEYCOMP cmpkey)
{
	int dir;
	struct AVLNode *l;

	if (p == NULL)
		return p;

	for (; p != NULL; p = p->Link[dir]) {
		int cmp = cmpkey(item, p);

		if (cmp == 0)
			return p;

		l = p;
		dir = cmp > 0;
	}

	if (dir == 0)
		return l;
	else
		return AVL_Next(l);
}

/* Find node <= item */
struct AVLNode *
AVL_FindPrev(struct AVLNode *p, const void *item, AVLKEYCOMP cmpkey)
{
	int dir;
	struct AVLNode *l;

	if (p == NULL)
		return p;

	for (; p != NULL; p = p->Link[dir]) {
		int cmp = cmpkey(item, p);

		if (cmp == 0)
			return p;

		l = p;
		dir = cmp > 0;
	}

	if (dir == 0)
		return AVL_Prev(l);
	else
		return l;
}

/* Inserts |item| into |tree| and returns a pointer to |item|'s address.
   If a duplicate item is found in the tree,
   returns a pointer to the duplicate without inserting |item|.
   Returns |NULL| in case of success. */
struct AVLNode *
AVL_Insert(struct AVLNode **tree, struct AVLNode *item, AVLNODECOMP cmpnode)
{
	struct AVLNode *y;     /* Top node to update balance factor, and parent. */
	struct AVLNode *p, *q; /* Iterator, and parent. */
	struct AVLNode *n;     /* Newly inserted node. */
	struct AVLNode *w;     /* New root of rebalanced subtree. */
	int dir;                 /* Direction to descend. */

	//assert (tree != NULL && item != NULL);

	y = *tree;
	for (q = NULL, p = *tree; p != NULL; q = p, p = p->Link[dir]) {
		int cmp = cmpnode(item, p);

		if (cmp == 0)
			return p;

		dir = cmp > 0;
		
		//if (p->Balance != 0)
		if (getBalance(p) != 0)
			y = p;
	}

	n = item;
	n->Link[0] = n->Link[1] = NULL;
	//n->Parent = q;
	//n->Balance = 0;
	setBoth(n, q, 0);
	if (q != NULL)
		q->Link[dir] = n;
	else {
		*tree = n;
		return NULL;
	}

	for (p = n; p != y; p = q) {
		//q = p->Parent;
		q = getParent(p);
		dir = q->Link[0] != p;
		if (dir == 0)
			//q->Balance--;
			decBalance(q);
		else
			//q->Balance++;
			incBalance(q);
	}

	//if (y->Balance == -2) {
	if (getBalance(y) == -2) {
		struct AVLNode *x = y->Link[0];
		//if (x->Balance == -1) {
		if (getBalance(x) == -1) {
			w = x;
			y->Link[0] = x->Link[1];
			x->Link[1] = y;
			//x->Balance = y->Balance = 0;
			//x->Parent = y->Parent;
			//y->Parent = x;
			setBoth(x, getParent(y), 0);
			setBoth(y, x, 0);
			if (y->Link[0] != NULL)
				//y->Link[0]->Parent = y;
				setParent(y->Link[0], y);
		} else {
			//assert (x->Balance == +1);
			w = x->Link[1];
			x->Link[1] = w->Link[0];
			w->Link[0] = x;
			y->Link[0] = w->Link[1];
			w->Link[1] = y;
			//if (w->Balance == -1)
			//	x->Balance = 0, y->Balance = +1;
			//else if (w->Balance == 0)
			//	x->Balance = y->Balance = 0;
			//else /* |w->Balance == +1| */
			//	x->Balance = -1, y->Balance = 0;
			int b = getBalance(w);
			intptr_t yp = y->Private;
			if (b == -1) {
				setBoth(x, w, 0);
				setBoth(y, w, 1);
			} else if (b == 0) {
				setBoth(x, w, 0);
				setBoth(y, w, 0);
			} else {
				setBoth(x, w, -1);
				setBoth(y, w, 0);
			}
			//w->Balance = 0;
			//w->Parent = y->Parent;
			setBoth(w, yp, 0);
			//x->Parent = y->Parent = w;
			if (x->Link[1] != NULL)
				//x->Link[1]->Parent = x;
				setParent(x->Link[1], x);
			if (y->Link[0] != NULL)
				//y->Link[0]->Parent = y;
				setParent(y->Link[0], y);
		}
		//} else if (y->Balance == +2) {
	} else if (getBalance(y) == +2) {
		struct AVLNode *x = y->Link[1];
		//if (x->Balance == +1) {
		if (getBalance(x) == +1) {
			w = x;
			y->Link[1] = x->Link[0];
			x->Link[0] = y;
			//x->Balance = y->Balance = 0;
			//x->Parent = y->Parent;
			//y->Parent = x;
			setBoth(x, getParent(y), 0);
			setBoth(y, x, 0);
			if (y->Link[1] != NULL)
				//y->Link[1]->Parent = y;
				setParent(y->Link[1], y);
		} else {
			//assert (x->Balance == -1);
			w = x->Link[0];
			x->Link[0] = w->Link[1];
			w->Link[1] = x;
			y->Link[1] = w->Link[0];
			w->Link[0] = y;
			//if (w->Balance == +1)
			//	x->Balance = 0, y->Balance = -1;
			//else if (w->Balance == 0)
			//	x->Balance = y->Balance = 0;
			//else /* |w->Balance == -1| */
			//	x->Balance = +1, y->Balance = 0;
			int b  = getBalance(w);
			intptr_t yp = y->Private;
			if (b == 1) {
				setBoth(x, w, 0);
				setBoth(y, w, -1);
			} else if (b == 0) {
				setBoth(x, w, 0);
				setBoth(y, w, 0);
			} else {
				setBoth(x, w, 1);
				setBoth(y, w, 0);
			}
			//w->Balance = 0;
			//w->Parent = y->Parent;
			setBoth(w, yp, 0);
			//x->Parent = y->Parent = w;
			if (x->Link[0] != NULL)
				//x->Link[0]->Parent = x;
				setParent(x->Link[0], x);
			if (y->Link[1] != NULL)
				//y->Link[1]->Parent = y;
				setParent(y->Link[1], y);
		}
	} else
		return NULL;

	//if (w->Parent != NULL)
	//w->Parent->Link[y != w->Parent->Link[0]] = w;
	p = getParent(w);
	if (p != NULL)
		p->Link[y != p->Link[0]] = w;
	else
		*tree = w;

	return NULL;
}

/*
  Moves avl node 'p' to new location 'item'.  Returns new item 'item'.
  Note that p and item must sort to the same location in the tree, yet not necessarily
  compare equal.  Use with caution.
*/
struct AVLNode *
AVL_Move(struct AVLNode **table, struct AVLNode *p, struct AVLNode *item)
{
	item->Link[0] = p->Link[0];
	item->Link[1] = p->Link[1];
	//item->Parent = p->Parent;
	//item->Balance = p->Balance;
	item->Private = p->Private;
	//if (p->Parent->Link[0] == p)
	//	p->Parent->Link[0] = item;
	//else
	//	p->Parent->Link[1] = item;
	struct AVLNode *a = getParent(p);
	if (a->Link[0] == p)
		a->Link[0] = item;
	else
		a->Link[1] = item;
	if (p->Link[0])
		//	p->Link[0]->Parent = item;
		setParent(p->Link[0], item);
	if (p->Link[1])
		//p->Link[1]->Parent = item;
		setParent(p->Link[1], item);

	return item;
}

/* Inserts |item| into |table|, replacing any duplicate item.
   Returns |NULL| if |item| was inserted without replacing a duplicate,
   or if a memory allocation error occurred.
   Otherwise, returns the item that was replaced. */
struct AVLNode *
AVL_Replace(struct AVLNode **table, struct AVLNode *item, AVLNODECOMP cmpnode)
{
	struct AVLNode *p = AVL_Insert(table, item, cmpnode);

	if (p) {
		item->Link[0] = p->Link[0];
		item->Link[1] = p->Link[1];
		//item->Parent = p->Parent;
		//item->Balance = p->Balance;
		item->Private = p->Private;
		//if (p->Parent->Link[0] == p)
		//	p->Parent->Link[0] = item;
		//else
		//	p->Parent->Link[1] = item;
		struct AVLNode *a = getParent(p);
		if (a->Link[0] == p)
			a->Link[0] = item;
		else
			a->Link[1] = item;
		if (p->Link[0])
		//	p->Link[0]->Parent = item;
			setParent(p->Link[0], item);
		if (p->Link[1])
			//p->Link[1]->Parent = item;
			setParent(p->Link[1], item);
	}

	return p;
}

/* Deletes from |tree| and returns an item matching |item|.
   Returns a null pointer if no matching item found. */
struct AVLNode *
AVL_Remove(struct AVLNode **tree, struct AVLNode *p)
{
	struct AVLNode *q; /* Parent of |p|. */
	int dir;       /* Side of |q| on which |p| is linked. */

	//q = p->Parent;
	q = getParent(p);
	if (q == NULL) {
		q = (struct AVLNode *)tree;
		dir = 0;
	} else {
		dir = q->Link[0] != p;
	}

	if (p->Link[1] == NULL) {
		q->Link[dir] = p->Link[0];
		if (q->Link[dir] != NULL)
			//q->Link[dir]->Parent = p->Parent;
			setParent(q->Link[dir], getParent(p));
	} else {
		struct AVLNode *r = p->Link[1];
		if (r->Link[0] == NULL) {
			r->Link[0] = p->Link[0];
			q->Link[dir] = r;
			//r->Parent = p->Parent;
			if (r->Link[0] != NULL)
				//r->Link[0]->Parent = r;
				setParent(r->Link[0], r);
			//r->Balance = p->Balance;
			r->Private = p->Private;
			q = r;
			dir = 1;
		} else {
			struct AVLNode *s = r->Link[0];
			while (s->Link[0] != NULL)
				s = s->Link[0];
			//r = s->Parent;
			r = getParent(s);
			r->Link[0] = s->Link[1];
			s->Link[0] = p->Link[0];
			s->Link[1] = p->Link[1];
			q->Link[dir] = s;
			if (s->Link[0] != NULL)
				//s->Link[0]->Parent = s;
				setParent(s->Link[0], s);
			//s->Link[1]->Parent = s;
			setParent(s->Link[1], s);
			//s->Parent = p->Parent;
			if (r->Link[0] != NULL)
				//r->Link[0]->Parent = r;
				setParent(r->Link[0], r);
			//s->Balance = p->Balance;
			s->Private = p->Private;
			q = r;
			dir = 0;
		}
	}
	//tree->AVL_alloc->libavl_free (tree->AVL_alloc, p);

	while (q != (struct AVLNode *)tree) {
		struct AVLNode *y = q;

		//if (y->Parent != NULL)
		//	q = y->Parent;
		if (getParent(y) != NULL)
			q = getParent(y);
		else
			q = (struct AVLNode *)tree;

		if (dir == 0) {
			dir = q->Link[0] != y;
			//y->Balance++;
			incBalance(y);
			//if (y->Balance == +1)
			if (getBalance(y) == +1)
				break;
			//else if (y->Balance == +2) {
			else if (getBalance(y) == +2) {
				struct AVLNode *x = y->Link[1];
				//if (x->Balance == -1) {
				if (getBalance(x) == -1) {
					struct AVLNode *w;

					//assert (x->Balance == -1);
					w = x->Link[0];
					x->Link[0] = w->Link[1];
					w->Link[1] = x;
					y->Link[1] = w->Link[0];
					w->Link[0] = y;
					//if (w->Balance == +1)
					//	x->Balance = 0, y->Balance = -1;
					//else if (w->Balance == 0)
					//	x->Balance = y->Balance = 0;
					//else /* |w->Balance == -1| */
					//	x->Balance = +1, y->Balance = 0;
					int b = getBalance(w);
					intptr_t yp = y->Private;
					if (b == +1) {
						setBoth(x, w, 0);
						setBoth(y, w, -1);
					} else if (b == 0) {
						setBoth(x, w, 0);
						setBoth(y, w, 0);
					} else {
						setBoth(x, w, +1);
						setBoth(y, w, 0);
					}
					//w->Balance = 0;
					//w->Parent = y->Parent;
					setBoth(w, yp, 0);
					//x->Parent = y->Parent = w;
					if (x->Link[0] != NULL)
						//x->Link[0]->Parent = x;
						setParent(x->Link[0], x);
					if (y->Link[1] != NULL)
						//y->Link[1]->Parent = y;
						setParent(y->Link[1], y);
					q->Link[dir] = w;
				} else {
					y->Link[1] = x->Link[0];
					x->Link[0] = y;
					intptr_t yp = y->Private;
					//x->Parent = y->Parent;
					//y->Parent = x;
					if (y->Link[1] != NULL)
						//y->Link[1]->Parent = y;
						setParent(y->Link[1], y);
					q->Link[dir] = x;
					//if (x->Balance == 0) {
					//	x->Balance = -1;
					//	y->Balance = +1;
					//	break;
					//} else {
					//	x->Balance = y->Balance = 0;
					//	y = x;
					//}
					if (getBalance(x) == 0) {
						setBoth(x, yp, -1);
						setBoth(y, x, +1);
						break;
					} else {
						setBoth(x, yp, 0);
						setBoth(y, x, 0);
						y = x;
					}
				}
			}
		} else {
			dir = q->Link[0] != y;
			//y->Balance--;
			decBalance(y);
			//if (y->Balance == -1)
			if (getBalance(y) == -1)
				break;
			//else if (y->Balance == -2) {
			else if (getBalance(y) == -2) {
				struct AVLNode *x = y->Link[0];

				//if (x->Balance == +1) {
				if (getBalance(x) == +1) {
					struct AVLNode *w;

					//assert (x->Balance == +1);
					w = x->Link[1];
					x->Link[1] = w->Link[0];
					w->Link[0] = x;
					y->Link[0] = w->Link[1];
					w->Link[1] = y;
					//if (w->Balance == -1)
					//	x->Balance = 0, y->Balance = +1;
					//else if (w->Balance == 0)
					//	x->Balance = y->Balance = 0;
					//else /* |w->Balance == +1| */
					//	x->Balance = -1, y->Balance = 0;
					int b = getBalance(w);
					intptr_t yp = y->Private;
					if (b == -1) {
						setBoth(x, w, 0);
						setBoth(y, w, +1);
					} else if (b == 0) {
						setBoth(x, w, 0);
						setBoth(y, w, 0);
					} else {
						setBoth(x, w, -1);
						setBoth(y, w, 0);
					}
					//w->Balance = 0;
					//w->Parent = y->Parent;
					setBoth(w, yp, 0);
					//x->Parent = y->Parent = w;
					if (x->Link[1] != NULL)
						//x->Link[1]->Parent = x;
						setParent(x->Link[1], x);
					if (y->Link[0] != NULL)
						//y->Link[0]->Parent = y;
						setParent(y->Link[0], y);
					q->Link[dir] = w;
				} else {
					y->Link[0] = x->Link[1];
					x->Link[1] = y;
					intptr_t yp = y->Private;
					//x->Parent = y->Parent;
					//y->Parent = x;
					if (y->Link[0] != NULL)
						//y->Link[0]->Parent = y;
						setParent(y->Link[0], y);
					q->Link[dir] = x;
					//if (x->Balance == 0) {
					//	x->Balance = +1;
					//	y->Balance = -1;
					//	break;
					//} else {
					//	x->Balance = y->Balance = 0;
					//	y = x;
					//}
					if (getBalance(x) == 0) {
						setBoth(x, yp, +1);
						setBoth(y, x, -1);
						break;
					} else {
						setBoth(x, yp, 0);
						setBoth(y, x, 0);
						y = x;
					}
				}
			}
		}
	}

	return p;

}

/* Initializes |trav| for |tree|.
   Returns data item in |tree| with the least value,
   or |NULL| if |tree| is empty. */
struct AVLNode *
AVL_First(struct AVLNode *root)
{
	if (root) {
		while (root->Link[0] != NULL)
			root = root->Link[0];
	}
	return root;
}

/* Initializes |trav| for |tree|.
   Returns data item in |tree| with the greatest value,
   or |NULL| if |tree| is empty. */
struct AVLNode *
AVL_Last(struct AVLNode *root)
{
	if (root) {
		while (root->Link[1] != NULL)
			root = root->Link[1];
	}
	return root;
}

/* Returns the next data item in inorder
   within the tree being traversed with |trav|,
   or if there are no more data items returns |NULL|. */
struct AVLNode *
AVL_Next(struct AVLNode *node)
{
	if (node->Link[1] == NULL)	{
		struct AVLNode *q, *p; /* Current node and its child. */
		//for (p = node, q = p->Parent; ; p = q, q = q->Parent)
		for (p = node, q = getParent(p); ; p = q, q = getParent(q))
			if (q == NULL || p == q->Link[0])
				return q;
	} else {
		node = node->Link[1];
		while (node->Link[0] != NULL)
			node = node->Link[0];
		return node;
	}
}

/* Returns the previous data item in inorder
   within the tree being traversed with |trav|,
   or if there are no more data items returns |NULL|. */
struct AVLNode *
AVL_Prev(struct AVLNode *node)
{
	if (node->Link[0] == NULL)	{
		struct AVLNode *q, *p; /* Current node and its child. */
		//for (p = node, q = p->Parent; ; p = q, q = q->Parent)
		for (p = node, q = getParent(p); ; p = q, q = getParent(q))
			if (q == NULL || p == q->Link[1])
				return q;
	} else {
		node = node->Link[0];
		while (node->Link[1] != NULL)
			node = node->Link[1];
		return node;
	}
}
