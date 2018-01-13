/*
 * @author: Aarti Sundararajan, Abhinay Agrawal, Anushree Jagrawal
 * 
 * THis file contains the code for producer.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 512

int main(int argc, char* argv[]) {

    if(argc != 3) {
        printf("error - usage: producer <id> <number-of-items> \n");
        exit(1);
    }
	
    int id = atoi(argv[1]);
    int total_items = atoi(argv[2]);
    char buf[BUF_SIZE];

    int num = open("/dev/scullBuffer0", O_WRONLY);
    if (num == -1) {
        perror("error: Open failed for producer.");
        exit(1);
    }

    int i = 0;

    while(i < total_items) {
        memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d", i+1);
        int status = write(num, buf, BUF_SIZE);
        
        if (status == -1) {
            printf("Producer %d : Write failed.", id);
	    break;
        } else if (status == 0) {
            printf("Producer %d : Buffer is full and no consumer available\n", id);
            break;
        } else {
	    printf("Producer %d, Iteration %d: %s\n",id, i, buf);
        }
        ++i;
	sleep(1);
    }

    printf("\nProducer %d: Total number of items produced is %d\n", id, i);
    close(num);
    exit(0);
}
