/*
 * @author: Aarti Sundrarajan, Abhinay Agrawal, Anushree Jagrawal
 *
 * This file contains the code for consumers
 */

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 512

int main(int argc, char* argv[]) {
    if (argc != 3) {
       printf("error - usage: consumer <id> <number-of-items>\n");
      exit(1);
    }

    int id = atoi(argv[1]);
    int total_items = atoi(argv[2]);
    char buf[BUF_SIZE];

    int num = open("/dev/scullBuffer0", O_RDONLY);
    if (num == -1) {
        perror("error:  Open failed for consumer.");
        exit(1);
    }

    int i = 0;

    while(i < total_items) {
        int status = read(num, &buf, BUF_SIZE);

        if (status == -1) {
            printf("Consumer %d: Read failed.\n", id);
	    break;
        } else if (status == 0) {
            printf("Consumer %d: Buffer is empty and no producer available\n", id);
            break;
        } else {
            printf("Consumer %d, Iteration %d: %s\n", id,i, buf);
        }
        ++i;
	sleep(1);
    }

    printf("\nConsumer %d: Total number of items read is %d\n", id, i);
    close(num);
    exit(0);
}

