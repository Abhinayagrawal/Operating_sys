#include <stdlib.h>
#include <assert.h>
#include "queue.h"

#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <ucontext.h>

int time_interval;   //number of user threads 

struct t_queue *threadQ;
struct t_queue *suspendQ;

typedef unsigned long address_t;
int next_tid = 0;
ucontext_t main_context;

struct sigaction scheduler;
struct itimerval timer;

/* Function definitions */
void schedule(int sig);
int uthread_init (int time_slice) {
    printf("\nIn %s : Setting up ....",__FUNCTION__);
    time_interval = time_slice;
    	
    //Initializing lists
    threadQ = t_queue_init();
    suspendQ = t_queue_init();

    struct thread* main_thread = malloc(sizeof(struct thread));
    getcontext(&main_thread->ucontext);
    main_thread->tid = next_tid++;
    printf("\nAssigned id %d to main thread",main_thread->tid);
    t_queue_enqueue(threadQ,main_thread);

    scheduler.sa_handler = schedule;
    //scheduler.sa_flags = SA_RESTART;
    sigemptyset(&scheduler.sa_mask);
    sigaddset(&scheduler.sa_mask, SIGVTALRM);
    if (sigaction(SIGVTALRM, &scheduler, NULL) != 0) {
            return -1;
    }

    //setting up the timer
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = time_slice;
    timer.it_interval = timer.it_value;
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) != 0) {
            return -1;
    }

    printf("\nExiting %s : Set up complete",__FUNCTION__);
}

void schedule(int sig){
//	printf("\nIn %s:Going to switch",__FUNCTION__);
	uthread_yield();	
}

int uthread_create (void *(*start_routine)(void*), void* arg) {
    
    sigprocmask(SIG_BLOCK, &scheduler.sa_mask, NULL);
    printf("\nIn %s,%d",__FUNCTION__,__LINE__);
    address_t sp, pc;
    
    // Crete a new thread now
    struct thread *new_thread = (struct thread *)malloc (sizeof(struct thread));
    assert(new_thread != NULL);

    new_thread->tid = next_tid++;
    printf("\nIn %s:Creating thread %d",__FUNCTION__,new_thread->tid);
    //set stack related values.
    new_thread->ucontext.uc_stack.ss_size = STACK_SIZE;
    new_thread->ucontext.uc_stack.ss_sp = new_thread->stack;
    sp = (address_t)new_thread->stack + STACK_SIZE - sizeof(int); 
    pc = (address_t)start_routine;

    if (getcontext(&new_thread->ucontext) == -1) {
        sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
	return FAIL;
    }
    new_thread->sp = new_thread->stack;
    new_thread->arg = arg;
    //set program counter as the thread's start routine function.
    new_thread->ucontext.uc_mcontext.gregs[REG_RIP] = pc;
    new_thread->ucontext.uc_mcontext.gregs[REG_RSP] = sp;
    sigemptyset(&new_thread->ucontext.uc_sigmask);
    new_thread->state = READY;

    //Adding the new thread to the thread queue
    assert(new_thread != NULL);
    t_queue_enqueue(threadQ, new_thread);

    printf("\nThread %d created successfully. Exiting %s",new_thread->tid,__FUNCTION__);
    sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
    return new_thread->tid;
}

int uthread_self(void){
	return threadQ->head->thread_t->tid;
}

int uthread_yield(void){
    sigprocmask(SIG_BLOCK, &scheduler.sa_mask, NULL);
    //printf("\nIn %s,%d",__FUNCTION__,__LINE__);
    volatile int flag = 0;

    if (threadQ == NULL || t_queue_isEmpty(threadQ) == true) {
        sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
        return FAIL;
    }

    //Save context of current running thread and change its state to READY
    if (getcontext(&threadQ->head->thread_t->ucontext) == -1) {
        sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
        return FAIL;
    }
    
    int prev_tid = threadQ->head->thread_t->tid; 
    if (flag == 1){
        sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
        return SUCCESS;
    }
    flag = 1;
    if(threadQ->head->thread_t->state == RUNNING)
        threadQ->head->thread_t->state = READY;

    t_queue_circleHead(threadQ);

    //get next ready thread from thread list
    while (t_queue_isEmpty(threadQ) != true 
            && threadQ->head->thread_t->state != READY) {
        t_queue_circleHead(threadQ);
    }
    if (t_queue_isEmpty(threadQ)) {
        sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
        return FAIL;
    }

    threadQ->head->thread_t->state = RUNNING;

    int curr_tid = threadQ->head->thread_t->tid;
    printf("\nThread %d is yielding to thread %d\n",prev_tid, curr_tid);


    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) != 0) {
        sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
        return FAIL;
    }

    sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
    if (setcontext(&threadQ->head->thread_t->ucontext) == -1) {
        return FAIL;
    }
    
    return SUCCESS;
}


int uthread_join(int tid, void **retval){
    sigprocmask(SIG_BLOCK, &scheduler.sa_mask, NULL);
    
    int curr = uthread_self();
    //Suspend current running thread, till thread tid finishes
    if (tid <= 0 || tid > next_tid-1){
        sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
        return FAIL;
    }

    while (1) {
        assert(threadQ != NULL);
	
	//If thread tid is either waiting or suspended, wait. If not, break and continue execution
        //struct node *temp = threadQ->head;

	if(!is_present(threadQ,tid) && !is_present(suspendQ,tid))
		break;
	printf("\nThread %d is waiting for thread %d to join. Going to yield ....",curr,tid);
        threadQ->head->thread_t->state = READY;
        if(uthread_yield() == FAIL) {
    	    sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
            return FAIL;
        }
    }

        printf("\nThread %d is joint thread  %d. Exiting %s ....",tid,curr,__FUNCTION__);
    sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
    return SUCCESS;
}

int uthread_terminate(int tid){
    printf("\nIn %s: Thread %d will now terminate",__FUNCTION__,tid);
    sigprocmask(SIG_BLOCK, &scheduler.sa_mask, NULL);
    struct thread *removet = t_queue_dequeue(threadQ);
    removet->ucontext.uc_stack.ss_sp = NULL;
    removet->sp = NULL;
    free(removet);
    
    printf("\nIn %s: Thread %d terminated",__FUNCTION__,tid);
    if(threadQ == NULL || t_queue_isEmpty(threadQ) == true){
	//NOTE: This block is never executed
        setcontext(&main_context);
        sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
        return -1;
    }
    int next_tid =threadQ->head->thread_t->tid;
    printf("\nIn %s: Will now switch to thread %d",__FUNCTION__,next_tid);
    threadQ->head->thread_t->state = RUNNING;
    
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) != 0) {
        sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
        return -1;
    }

    sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
    if (setcontext(&threadQ->head->thread_t->ucontext) == -1) {
        return FAIL;
    }
    return SUCCESS;
}


int uthread_suspend (int tid) {
    printf("\n%s for thread %d",__FUNCTION__,tid);
    sigprocmask(SIG_BLOCK, &scheduler.sa_mask, NULL);
    assert(threadQ != NULL);
    assert(suspendQ != NULL);

    //search for particular thread in threadQ
    struct node *head = threadQ->head, *t_nxt;
    struct thread *t_cur = head->thread_t;
    volatile int flag = 0;

    if (t_cur->tid == tid) {
	 struct thread *suspended_thread = remove_node(threadQ,tid);

        if (t_cur->state == RUNNING) {
            if (getcontext(&t_cur->ucontext) == -1) {
                sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
                return FAIL;
            }
            
            if (flag == 1) {
                sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
                return SUCCESS;
            }
            flag = 1;
        }

        t_cur->state = SUSPEND;
        t_queue_enqueue(suspendQ, t_cur);

        while (t_queue_isEmpty(threadQ) != true
                && threadQ->head->thread_t->state != READY) {
            t_queue_circleHead(threadQ);
        }
        if (t_queue_isEmpty(threadQ)) {
            sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
            return FAIL;
        }

        if (setitimer(ITIMER_VIRTUAL, &timer, NULL) != 0) {
            sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
            return FAIL;
        }

        threadQ->head->thread_t->state = RUNNING;
        sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
        if (setcontext(&threadQ->head->thread_t->ucontext) == -1) {
            return FAIL;
        }

        return SUCCESS;
    } else {
	int result = FAIL;
	struct thread *suspended_thread = remove_node(threadQ,tid);
	if(suspended_thread!=NULL){
		suspended_thread->state = SUSPEND;
		t_queue_enqueue(suspendQ,suspended_thread);
		result = SUCCESS;
	}
        sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
	return result;
	/*
        while (head != NULL && head->next != NULL && head->next->thread_t->tid != tid) {
            head = head->next;
        }

        if (head == NULL || head->next == NULL) {
            printf("Thread with tid = %d not found \n", tid);
            sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
            return  FAIL;
        }

        t_nxt = head->next;
        head->next = t_nxt->next;
        t_nxt->next = NULL;

        if (getcontext(&t_nxt->thread_t->ucontext) == -1) {
            sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
            return FAIL;
        }

        if (flag == 1) {
            sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
            return SUCCESS;
        }

        flag = 1;
        t_nxt->thread_t->state = SUSPEND;
        t_queue_enqueue(suspendQ, t_nxt->thread_t);
        sigprocmask(SIG_UNBLOCK, &scheduler.sa_mask, NULL);
        return SUCCESS;*/
    }
}

int uthread_resume(int tid){
	printf("\n%s for thread %d",__FUNCTION__,tid);
	assert(suspendQ!=NULL);
	struct thread *thread_to_resume = remove_node(suspendQ,tid);

	if(thread_to_resume == NULL){
		printf("\nError in resuming thread %d: Thread was not suspended",tid);
		return -1;
	}
	thread_to_resume->state = READY;	
	t_queue_enqueue(threadQ,thread_to_resume);
	return 0;
}
