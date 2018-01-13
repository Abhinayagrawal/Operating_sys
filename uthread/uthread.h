#include<stdio.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include<ucontext.h>
#include<sys/time.h>

typedef enum uthread_state{READY, RUNNING, SUSPEND, FINISHED} uthread_state;

#define SUCCESS 1
#define FAIL 0
#define MAX_THREADS 64
#define STACK_SIZE 8096

/*TODO: set this var to true when first initialized. Used for setting up alarm on first initialization
*/
//bool is_init = FALSE;

/*
 * thread control block structure
 * TODO: Do we need a separate structure to expose for the user containing just tid??
 */

struct thread {
    int tid;		//thread ID
    ucontext_t ucontext;
    char stack[STACK_SIZE];
    void *sp;
    uthread_state state;
    void* (*entry_func)(void*);
    void *arg;
};



/*
 * Function declarations
 */

int uthread_init (int time_slice);

int uthread_create (void *(*start_routine)(void*), void* arg);

int uthread_self (void);

int uthread_terminate(int tid);

void schedule_first_thread(void);

int uthread_yield(void);

int uthread_join(int tid, void **retval);

int uthread_suspend(int tid);

int uthread_resume(int tid);
