/*
 * This file was developed to test the working of the thread library.
 *
 * Author: Abhinay Agrawal
 */

#include "uthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void *thread_func(void *arg)
{
    int tid = uthread_self();
    
    if(tid==1){
	for(int i=0;i<5000;i++){
            if(i==500){
	    	uthread_suspend(2);
	    }
	}
    }else if(tid==2){
	for(int i=0;i<5000;i++){
	}
    }else{
	for(int i=0;i<5000;i++){
		if(i==800){
			uthread_resume(2);
		}
	}
    }
}

void *stub(void *arg) {
	(*thread_func)(arg);
	int tid = uthread_self();
        uthread_terminate(tid);
}

int main()
{
   int time_slice = 500;
   int nthreads;

   printf("\n---------------DEMO APPLICATION FOR UTHREAD LIBRARY---------------------");
   printf("\nEnter no. of threads:");
   scanf("%d", &nthreads);

   printf("\nRunning with time_slice = %d",time_slice);

    uthread_init(time_slice);
    int count[nthreads];
    char *status;

    for (int i = 1; i <= nthreads; i++) {
        count[i] = i;
        uthread_create(stub, NULL);
    }
    printf("thread created. scheduling now\n");

    for (int i = 1; i <= nthreads; i++) {
        uthread_join(i, (void **)status);
    }

    printf("Done..main thread exiting\n");

    return 0;
}
