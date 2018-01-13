/*
 * This header file contains all the structures and function declarations
 * for the doubly linked list implentation.
 *
 * Author: Abhinay Agrawal
 */

// TODO: remove the non-suing functions

#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

typedef enum {false = 0, true = 1} bool;

struct linkedlistnode {
	struct linkedlistnode *lln_prev;
	struct linkedlistnode *lln_next;
	void *lln_self;
};

struct linkedlist {
	struct linkedlistnode ll_head;
	struct linkedlistnode ll_tail;
	unsigned ll_count;
};

/* Initialize and clean up a linked list node. */
void linkedlistnode_init(struct linkedlistnode *lln, void *self);
void linkedlistnode_cleanup(struct linkedlistnode *lln);

/* Initialize and clean up a linked list. Must be empty at cleanup. */
void linkedlist_init(struct linkedlist *ll);
void linkedlist_cleanup(struct linkedlist *ll);

/* Check if it's empty */
bool linkedlist_isempty(struct linkedlist *tl);

/* Add and remove: at ends */
void linkedlist_addhead(struct linkedlist *ll, struct linkedlistnode *t);
void linkedlist_addtail(struct linkedlist *ll, struct linkedlistnode *t);
void *linkedlist_remhead(struct linkedlist *ll);
void *linkedlist_remtail(struct linkedlist *ll);

/* Add and remove: in middle. (LL is needed to maintain ->ll_count.) */
void linkedlist_insertafter(struct linkedlist *ll,
		struct linkedlistnode *onlist, struct linkedlistnode *addee);
void linkedlist_insertbefore(struct linkedlist *ll,
		struct linkedlistnode *addee, struct linkedlistnode *onlist);
void linkedlist_remove(struct linkedlist *ll, struct linkedlistnode *t);

#endif /* _LINKEDLIST_H_ */
