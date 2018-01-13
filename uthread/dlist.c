/*
 * This file contains all the function definitions for the doubly linked list
 * implementation.
 *
 * Author: Abhinay Agrawal
 */

//#include <types.h>
//#include <lib.h>
#include "dlist.h"
#include <assert.h>
#include <stdio.h>

// TODO: Delete the APIs which are of no use.

void
linkedlistnode_init(struct linkedlistnode *lln, void *t)
{
		assert(lln != NULL);
		assert(t != NULL);

		lln->lln_next = NULL;
		lln->lln_prev = NULL;
		lln->lln_self = t;
}

// Function to check whether everything is cleaned up or not
void
linkedlistnode_cleanup(struct linkedlistnode *lln)
{
		assert(lln != NULL);
		assert(lln->lln_next == NULL);
		assert(lln->lln_prev == NULL);
		assert(lln->lln_self != NULL);
}

void
linkedlist_init(struct linkedlist *ll)
{
		assert(ll != NULL);

		ll->ll_head.lln_next = &ll->ll_tail;
		ll->ll_head.lln_prev = NULL;
		ll->ll_tail.lln_next = NULL;
		ll->ll_tail.lln_prev = &ll->ll_head;
		ll->ll_head.lln_self = NULL;
		ll->ll_tail.lln_self = NULL;
		ll->ll_count = 0;
}

void
linkedlist_cleanup(struct linkedlist *ll)
{
		assert(ll != NULL);
		assert(ll->ll_head.lln_next == &ll->ll_tail);
		assert(ll->ll_head.lln_prev == NULL);
		assert(ll->ll_tail.lln_next == NULL);
		assert(ll->ll_tail.lln_prev == &ll->ll_head);
		assert(ll->ll_head.lln_self == NULL);
		assert(ll->ll_tail.lln_self == NULL);
		assert(linkedlist_isempty(ll));
		assert(ll->ll_count == 0);

		/* nothing (else) to do */
}

bool
linkedlist_isempty(struct linkedlist *ll)
{
		assert(ll != NULL);

		return (ll->ll_count == 0);
}

////////////////////////////////////////////////////////////
// internal

/*
 * Do insertion. Doesn't update _count.
 */
static void
linkedlist_insertafternode(struct linkedlistnode *onlist, struct linkedlistnode *addee)
{
		assert(addee->lln_prev == NULL);
		assert(addee->lln_next == NULL);

		addee->lln_prev = onlist;
		addee->lln_next = onlist->lln_next;
		addee->lln_prev->lln_next = addee;
		addee->lln_next->lln_prev = addee;
}

/*
 * Do insertion. Doesn't update ll_count.
 */
static void
linkedlist_insertbeforenode(struct linkedlistnode *addee, struct linkedlistnode *onlist)
{
		assert(addee->lln_prev == NULL);
		assert(addee->lln_next == NULL);

		addee->lln_prev = onlist->lln_prev;
		addee->lln_next = onlist;
		addee->lln_prev->lln_next = addee;
		addee->lln_next->lln_prev = addee;
}

/*
 * Do removal. Doesn't update ll_count.
 */
static void
linkedlist_removenode(struct linkedlistnode *lln)
{
		assert(lln != NULL);
		assert(lln->lln_prev != NULL);
		assert(lln->lln_next != NULL);

		lln->lln_prev->lln_next = lln->lln_next;
		lln->lln_next->lln_prev = lln->lln_prev;
		lln->lln_prev = NULL;
		lln->lln_next = NULL;
}

////////////////////////////////////////////////////////////
// public

void
linkedlist_addhead(struct linkedlist *ll, struct linkedlistnode *t)
{
		assert(ll != NULL);
		assert(t != NULL);

		linkedlist_insertafternode(&ll->ll_head, t);
		ll->ll_count++;
}

void
linkedlist_addtail(struct linkedlist *ll, struct linkedlistnode *t)
{
		assert(ll != NULL);
		assert(t != NULL);

		linkedlist_insertbeforenode(t, &ll->ll_tail);
		ll->ll_count++;
}

void *
linkedlist_remhead(struct linkedlist *ll)
{
		struct linkedlistnode *lln;

		assert(ll != NULL);

		lln = ll->ll_head.lln_next;
		if (lln->lln_next == NULL) {
				/* list was empty  */
				return NULL;
		}
		linkedlist_removenode(lln);
		assert(ll->ll_count > 0);
		ll->ll_count--;
		return lln->lln_self;
}

void *
linkedlist_remtail(struct linkedlist *ll)
{
		struct linkedlistnode *lln;

		assert(ll != NULL);

		lln = ll->ll_tail.lln_prev;
		if (lln->lln_prev == NULL) {
				/* list was empty  */
				return NULL;
		}
		linkedlist_removenode(lln);
		assert(ll->ll_count > 0);
		ll->ll_count--;
		return lln->lln_self;
}

void
linkedlist_insertafter(struct linkedlist *ll,
				struct linkedlistnode *onlist, struct linkedlistnode *addee)
{
		linkedlist_insertafternode(onlist, addee);
		ll->ll_count++;
}

void
linkedlist_insertbefore(struct linkedlist *ll,
				struct linkedlistnode *addee, struct linkedlistnode *onlist)
{
		linkedlist_insertbeforenode(addee, onlist);
		ll->ll_count++;
}

void
linkedlist_remove(struct linkedlist *ll, struct linkedlistnode *t)
{
		linkedlist_removenode(t);
		assert(ll->ll_count > 0);
		ll->ll_count--;
}
