/*
 * Author: Abhinay Agrawal
 */

#include<assert.h>
//#include "uthread.h"
#include "queue.h"
#include <stdlib.h>

struct t_queue* t_queue_init(){	
    struct t_queue *q = malloc(sizeof(struct t_queue));
    q->head = NULL;
    q->tail = NULL;
    return q;
} 

void t_queue_enqueue(struct t_queue *queue, struct thread *new_thread){
    struct node *newNode = malloc(sizeof(struct node));
    assert(newNode != NULL);
    assert(new_thread != NULL);

    newNode->thread_t = new_thread;
    newNode->next = NULL;

    //if queue is empty-add to head, else add to tail
    if (queue->head == NULL){
        queue->head = newNode;
        queue->tail = newNode;
    } else {
        queue->tail->next = newNode;
        queue->tail = queue->tail->next;
    }
    //printf("tid = %d, %d, %s \n", queue->tail->thread_t->tid, __LINE__, __FUNCTION__);
}

//Removes element from head of queue
struct thread* t_queue_dequeue(struct t_queue *queue){
    if(queue==NULL || queue->head==NULL)
        return NULL;

    struct node *t = queue->head;
    if(queue->head == queue->tail){
	queue->head = NULL;
        queue->tail = NULL;
	return t->thread_t;
    }

    queue->head = queue->head->next;
    
    return t->thread_t;
}

bool t_queue_isEmpty(struct t_queue *queue){
    assert(queue!=NULL);
    if(queue->head==NULL)
        return true;

    return false;
}

int t_queue_circleHead(struct t_queue *queue){
    if(t_queue_isEmpty(queue))
        return -1;

    struct thread* currentThread = t_queue_dequeue(queue);

    if(currentThread == NULL)
        return -1;

    t_queue_enqueue(queue, currentThread);
}

void printQ(struct t_queue *queue){
    if(queue == NULL)
        printf("\n Queue is NULL in %s\n", __FUNCTION__);
    else if(t_queue_isEmpty(queue))
        printf("\nQueue is empty %s\n", __FUNCTION__);
    else{
        struct node *current = queue->head;

        while(current != NULL) {
            printf("tid = %d, %s \n", current->thread_t->tid, __FUNCTION__);
            current = current->next;
        }
    }
}

struct thread* remove_node(struct t_queue *queue, int tid){
	struct thread *removed_thread;
	if(queue == NULL || t_queue_isEmpty(queue)){
		return NULL;
	}

	if(queue->head->thread_t->tid == tid){
		removed_thread = t_queue_dequeue(queue);
	}else{
		struct node* prev = queue->head;
		struct node* curr = queue->head->next;
	
		while(prev!=NULL && curr!=NULL && curr->thread_t->tid!=tid){
			prev = curr;
			curr = curr->next;
		}

		if(curr == NULL)
			return NULL;
		if(curr!=NULL){
			prev->next = curr->next;
			curr->next = NULL;
			removed_thread = curr->thread_t;
		}
	}

	return removed_thread;
}

bool is_present(struct t_queue *queue, int tid){
	if(queue == NULL || t_queue_isEmpty(queue))
		return false;

	struct node *curr = queue->head;

	while(curr!=NULL && curr->thread_t->tid!=tid){
		curr = curr->next;
	}

	if(curr == NULL)
		return false;

	return true;
}
