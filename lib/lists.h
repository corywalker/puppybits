/*  lists.h: double-linked list implementation (aka 'exec lists')

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
#ifndef _LISTS_H
#define _LISTS_H

struct Node {
	struct Node *Succ;
	struct Node *Pred;
};

struct List {
	struct Node *Head;
	struct Node *Tail;
	struct Node *TailPred;
};

#define INIT_LIST(l) { (struct Node *)&(l).Tail, (void *)0, (struct Node *)&l }

void NewList(struct List *l);
void AddHead(struct List *l, struct Node *n);
void AddTail(struct List *l, struct Node *n);
//void Enqueue(struct List *l, struct Node *n);
void Remove(struct Node *n);
//struct Node *FindName(struct List *, const char *);

struct Node *RemHead(struct List *l);
#define EmptyList(l) ((l)->TailPred == (struct Node *)(l))

//void EnqueueF(struct List *l, struct Node *n, int (*cmp)(const void *a, const void *b));

#endif
