/*  lists.c: Double-linked list implementation (aka 'exec lists')

    Copyright (C) 2010 Michael Zucchi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "lists.h"

void NewList(struct List *l)
{
	l->Head = (struct Node *)&l->Tail;
	l->Tail = 0;
	l->TailPred = (struct Node *)&l->Head;
}

void AddHead(struct List *l, struct Node *n)
{
	n->Succ = l->Head;
	n->Pred = (struct Node *)&l->Head;
	l->Head->Pred = n;
	l->Head = n;
}

void AddTail(struct List *l, struct Node *n)
{
	n->Pred = l->TailPred;
	n->Succ = (struct Node *)&l->Tail;
	l->TailPred->Succ = n;
	l->TailPred = n;
}

#if 0
void Enqueue(struct List *l, struct Node *n)
{
	struct Node *sw, *sn;

	sw = l->Head;
	sn = sw->Succ;
	while (sn) {
		if (sw->Pri < n->Pri) {
			sw->Pred->Succ = n;
			n->Pred = sw->Pred;
			n->Succ = sw;
			sw->Pred = n;
			return;
		}
		sw = sn;
		sn = sn->Succ;
	}
	AddTail(l, n);
}

void EnqueueF(struct List *l, struct Node *n, int (*cmp)(const void *a, const void *b))
{
	struct Node *sw, *sn;

	sw = l->Head;
	sn = sw->Succ;
	while (sn) {
		if (cmp(sw, n) < 0) {
			sw->Pred->Succ = n;
			n->Pred = sw->Pred;
			n->Succ = sw;
			sw->Pred = n;
			return;
		}
		sw = sn;
		sn = sn->Succ;
	}
	AddTail(l, n);
}
#endif

void Remove(struct Node *n)
{
	n->Pred->Succ = n->Succ;
	n->Succ->Pred = n->Pred;
}

struct Node *RemHead(struct List *l)
{
	struct Node *n = l->Head;

	if (n->Succ != 0L) {
		n->Succ->Pred = (struct Node *)&l->Head;
		l->Head = n->Succ;
		return n;
	} else
		return 0L;
}

struct Node *RemTail(struct List *l)
{
	struct Node *n = l->Tail;

	if (n->Pred != 0L) {
		n->Pred->Succ = (struct Node *)&l->TailPred;
		l->Tail = n->Pred;
		return n;
	} else
		return 0L;
}

#ifdef EmptyList
#undef EmptyList
#endif

int EmptyList(struct List *l)
{
	return l->TailPred == (struct Node *)l;
}

#if 0
int strcmp(const char *a, const char *b)
{
	while (*a && *a == *b) {
		a++;
		b++;
	}

	return (((int)*a) & 0xff) - (((int)*b) & 0xff);
}

struct Node *FindName(struct List *l, const char *name)
{
	struct Node *nw, *nn;

	nw = l->Head;
	nn = nw->Succ;
	while (nn) {
		if (nw->Name && __builtin_strcmp(name, nw->Name) == 0)
			return nw;
		nw = nn;
		nn = nn->Succ;
	}

	return NULL;
}
#endif
