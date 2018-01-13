#include<stdio.h>
#include<assert.h>
#include <stdlib.h>

#include "queue.h"

struct t_queue* t_queue_init(){	
    struct t_queue *q = malloc(sizeof(struct t_queue));
    q->head = NULL;
    q->tail = NULL;
    return q;
} 

void t_queue_enqueue(struct t_queue *queue, int data){
    struct node *newNode = malloc(sizeof(struct node));
    assert(newNode != NULL);

    newNode->val = data;
    newNode->next = NULL;

    //if queue is empty-add to head, else add to tail
    if (queue->head == NULL){
        queue->head = newNode;
        queue->tail = queue->head;
    } else {
        queue->tail->next = newNode;
        queue->tail = queue->tail->next;
    }
}

//Removes element from head of queue
void t_queue_dequeue(struct t_queue *queue){
    assert(queue!=NULL);
    if(queue==NULL || queue->head==NULL)
        return ;

    struct node *t = queue->head;
    if(queue->head == queue->tail){
	queue->head = NULL;
        queue->tail = NULL;
	free(t);
        return;
    }

    queue->head = queue->head->next;
    //t->next = NULL;
    free(t);
    return;
}

bool t_queue_isEmpty(struct t_queue *queue){
    assert(queue!=NULL);
    if(queue->head==NULL)
        return true;

    return false;
}

void printQ(struct t_queue *queue) {
    if(queue != NULL && !t_queue_isEmpty(queue)) {
        struct node *current = queue->head;

        while(current != NULL) {
            printf("%d ", current->val);
            current = current->next;
        }
    }
    printf("\n");
}

int remove_node(struct t_queue *queue, int data) {
    if(queue == NULL || t_queue_isEmpty(queue)) {
        return -1;
    }

    if(queue->head->val == data) {
        t_queue_dequeue(queue);
    } else {
        struct node* prev = queue->head;
        struct node* curr = queue->head->next;

        while(prev!=NULL && curr!=NULL && curr->val != data) {
            prev = curr;
            curr = curr->next;
        }

        if(curr == NULL)
            return -1;
        if(curr!=NULL) {
            if(queue->tail == curr){
                queue->tail = prev;
                queue->tail->next = NULL;
                return curr->val;
            }
            prev->next = curr->next;
            curr->next = NULL;
            //free(curr);
        }
        return curr->val;
    }
    return -1;
}

bool is_present(struct t_queue *queue, int data) {
    if(queue == NULL || t_queue_isEmpty(queue))
        return false;

    struct node *curr = queue->head;

    while(curr != NULL && curr->val != data) {
        curr = curr->next;
    }

    if(curr == NULL)
        return false;

    return true;
}
