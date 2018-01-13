#include<stdio.h>
#include<signal.h>
#include<sys/time.h>
#include<stdlib.h>
#include "queue.h"
void *thread_func(void *arg)
{
    //uthread_yield();
   // while(1){
	printf("\nIn thread func");
    //}
    return NULL;;
}

void *stub(void *arg) {
        (*thread_func)(arg);
        int tid = uthread_self();
        uthread_terminate(tid);
}

int main(){
	//uthread_init(1000);
  	//uthread_create(stub,(void*)NULL);

	struct t_queue *queue = t_queue_init();
	struct thread *thr1 = malloc(sizeof(struct thread));
	struct thread *thr2 = malloc(sizeof(struct thread));
	struct thread *thr3 = malloc(sizeof(struct thread));

	thr1->tid = 1;
	thr2->tid=2;
	thr3->tid =3;

	t_queue_enqueue(queue,thr1);
	t_queue_enqueue(queue,thr2);
	t_queue_enqueue(queue,thr3);

	printf("%d",is_present(queue,2));
	printf("%d",is_present(queue,3));
	printf("%d",is_present(queue,5));

	printQ(queue);

	remove_node(queue,2);
	printQ(queue);

	remove_node(queue,1);
	printQ(queue);
	return 0;
}
