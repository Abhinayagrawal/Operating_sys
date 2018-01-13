#include "queue.h"
#include <stdio.h>

int main() {
    struct t_queue *Q = t_queue_init();

    t_queue_enqueue(Q, 0);
    remove_node(Q, 0);
    t_queue_enqueue(Q, 0);
    t_queue_enqueue(Q, 1);
    remove_node(Q, 1);
    t_queue_enqueue(Q, 1);
    t_queue_enqueue(Q, 2);


    printQ(Q);

    
    return 0;
}
