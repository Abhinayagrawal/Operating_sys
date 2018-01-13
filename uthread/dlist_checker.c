// Author: Abhinay Agrawal

#include <stdio.h>
#include <stdlib.h>
#include "dlist.h"

int main() {
    //Adding node to ll
    struct linkedlist *head = (struct linkedlist*)malloc(sizeof(struct linkedlist));
    linkedlist_init(head);

    int i = 1;
    struct linkedlistnode *temp1 = (struct linkedlistnode*) malloc(sizeof(struct linkedlistnode));
    linkedlistnode_init(temp1, &i);
    printf("val = %p\n", (void*)temp1->lln_self);
    linkedlist_addhead(head, temp1);

    i++;
    while(i<=5) {
        struct linkedlistnode *temp = (struct linkedlistnode*) malloc(sizeof(struct linkedlistnode));
        linkedlistnode_init(temp, &i);
        linkedlist_addtail(head, temp);
        printf("%d node added\n", i);
        i++;
    }
    return 0;
}

