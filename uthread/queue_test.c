/*
 * This file was developed to test the working of the queue implementation and
 * its integration with thread library.
 *
 * Author: Abhinay Agrawal
 */

#include<stdio.h>
//#include "queue.h"
#include<stdlib.h>
#include<assert.h>

#ifndef __USE_GNU
#define __USE_GNU
#endif
#include<ucontext.h>
#define STACK_SIZE 1024

typedef unsigned long address_t;
//struct t_queue *queue;

struct thread {
    int tid;            //thread ID
    ucontext_t ucontext;
    char stack[STACK_SIZE];
    //uthread_state state;
    void* (*entry_func)(void*);
    //void *arg;
    //void *sp;
};

struct thread *t1;// = malloc(sizeof(struct thread));
struct thread *t2;// = malloc(sizeof(struct thread));
//struct thread* current;
//struct thread *thread_l;

void switchThreads();
/*void handle(int n)
{
    puts("Bail");
    exit(1);
}*/
void p(){
	printf("\n\nHello from Thread 1\n\n");
	switchThreads();
        usleep(1000000);
}

void q(){
	printf("\n\nHello from Thread 2");
	switchThreads();
        usleep(1000000);
}

static int current_thread = 0;
void switchThreads(){
	printf("\n\nIn switchThreads");
	volatile int flag = 0;
        int ret_val;
	if(current_thread == 0)
	    ret_val = getcontext(&t1->ucontext);
	else
	    ret_val = getcontext(&t2->ucontext);

	printf("\n\nSwitching thread..");
	if(flag == 1) {
		return;
	}
	current_thread = 1 - current_thread;
	flag = 1;
	if (current_thread == 1)
		setcontext(&t2->ucontext);
	else
		setcontext(&t1->ucontext);
}

void setup(){
        t1 = (struct thread*)malloc(sizeof(struct thread));
        t2 = (struct thread*)malloc(sizeof(struct thread));

	address_t sp1, pc1, sp2, pc2;
	t1->tid =1;
	t2->tid=2;
    
        t1->ucontext.uc_stack.ss_size = STACK_SIZE;
        t1->ucontext.uc_stack.ss_sp = t1->stack;
	sp1 = (address_t)t1->stack + STACK_SIZE - sizeof(int);
	pc1 = (address_t)p;
	
	getcontext(&t1->ucontext);
        t1->ucontext.uc_mcontext.gregs[REG_RIP] = pc1;
	t1->ucontext.uc_mcontext.gregs[REG_RSP] = sp1;
	sigemptyset(&t1->ucontext.uc_sigmask);
        //t1->state = READY;


        t2->ucontext.uc_stack.ss_size = STACK_SIZE;
        t2->ucontext.uc_stack.ss_sp = t2->stack;
	sp2 = (address_t)t2->stack + STACK_SIZE - sizeof(int);
	pc2 = (address_t)q;
	
	getcontext(&t2->ucontext);
        t2->ucontext.uc_mcontext.gregs[REG_RIP] = pc2;
	t2->ucontext.uc_mcontext.gregs[REG_RSP] = sp2;
	sigemptyset(&t2->ucontext.uc_sigmask);
        //t2->state = READY;
#if 0
        thread_l[0] = malloc(sizeof(struct thread));
	thread_l[1] = malloc(sizeof(struct thread));
	assert(t1!=NULL);
	assert(t2!=NULL);
	t1->tid =1;
	t2->tid=2;

	getcontext(&t1->ucontext);
	address_t sp,pc;
	sp = (address_t)t1->stack + STACK_SIZE - sizeof(int);
	pc = (address_t)p;
	t1->ucontext.uc_mcontext.gregs[REG_RIP] = pc;
	t1->ucontext.uc_mcontext.gregs[REG_RSP] = sp;
	sigemptyset(&t1->ucontext.uc_sigmask);
//	t_queue_enqueue(queue,t1);
	
	getcontext(&t2->ucontext);
	sp = (address_t)t2->stack + STACK_SIZE - sizeof(int);
	pc = (address_t)q;
	t2->ucontext.uc_mcontext.gregs[REG_RIP] = pc;
	t2->ucontext.uc_mcontext.gregs[REG_RSP] = sp;
	sigemptyset(&t2->ucontext.uc_sigmask);
//	t_queue_enqueue(queue,t2);
//	current = t1;
	setcontext(&t1->ucontext);
	
        t_queue_enqueue(queue,t1);
	t_queue_enqueue(queue,t2);
	printf("%d",queue->head->thread_t->tid);
	t_queue_dequeue(queue);
	t_queue_enqueue(queue,t1);
	printf("%d",queue->head->thread_t->tid);
	t_queue_circleHead(queue);

	printf("%d",queue->head->thread_t->tid);
#endif
#if 0	
        thread_l[0].tid=1;
	address_t sp,pc;
	sp = (address_t)thread_l[0].stack + STACK_SIZE - sizeof(int);
	pc = (address_t)p;
	
        getcontext(&thread_l[0].ucontext);
	thread_l[0].ucontext.uc_mcontext.gregs[REG_RIP] = pc;
	thread_l[0].ucontext.uc_mcontext.gregs[REG_RSP] = sp;
	sigemptyset(&thread_l[0].ucontext.uc_sigmask);
	
	thread_l[1].tid=2;
	sp = (address_t)thread_l[1].stack + STACK_SIZE - sizeof(int);
	pc = (address_t)q;
	getcontext(&thread_l[1].ucontext);
	thread_l[1].ucontext.uc_mcontext.gregs[REG_RIP] = pc;
	thread_l[1].ucontext.uc_mcontext.gregs[REG_RSP] = sp;
	sigemptyset(&thread_l[1].ucontext.uc_sigmask);
#endif
}


int main(){
//	assert(thread_l[0] != NULL);
//	signal(SIGSEGV, handle);
	setup();
	setcontext(&t1->ucontext);
	printf("\nMain done");
	return 0;
}
