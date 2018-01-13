#include "uthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int nthreads;
int total_points_in_circle;
int points_per_thread;

/* For each thread this function is run to calculate the number of points 
 * inside a circle inscribed in a square to calculate the value of Pi using 
 * Monte Carlo method.
 */
void *thread_func(void *arg)
{
   int thread_in_circle = 0;
   for(int i = 0; i < points_per_thread; i++){
       double x = (double)rand() / (double)RAND_MAX;
       double y = (double)rand() / (double)RAND_MAX;
       
       if(x*x + y*y < 1)
           thread_in_circle++;
   	
	printf("\nThread %d at iteration %d",uthread_self(),i);
   }
   printf("No. of point inside circle for thread %d is:%d",uthread_self(),thread_in_circle);
   total_points_in_circle += thread_in_circle;
   
   printf("Current no of points in circle by thread %d is:%d",uthread_self(),total_points_in_circle);
}

void *stub(void *arg) {
	(*thread_func)(arg);
	int tid = uthread_self();
	printf("Termination starts : tid = %d\n", tid);
        uthread_terminate(tid);
	printf("\nTermination ends : tid = %d\n", tid);
}

/* This is a simple Pi calculation application demonstrating usage of the 
 * uthread library.
 */
int main(int argc, char *argv[])
{
    int total_points;
    printf("\n<----------Pi Calculation using Monte Carlo method--------->\n");
    printf("Enter no. of points (in range of integer):");
    scanf("%d", &total_points);
    printf("Enter no. of threads:");
    scanf("%d", &nthreads);
    points_per_thread = total_points / nthreads;

    uthread_init(500);
    char *status;
    int i;
    for (i = 1; i <= nthreads; i++) {   
        uthread_create(stub, NULL);
    }

    for (i = 1; i <= nthreads; i++) {
        uthread_join(i, (void **)status);
    }
    
    printf("\nPi: %f", (4. * (double)total_points_in_circle) / ((double)points_per_thread * nthreads));
    return 0;
}
