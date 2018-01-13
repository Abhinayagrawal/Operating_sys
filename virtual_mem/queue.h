typedef enum {false = 0, true = 1} bool;
struct node {
	int val;
	struct node *next;
};

struct t_queue {
	struct node *head;
	struct node *tail;
	int size;
};

struct t_queue* t_queue_init(void);

void t_queue_enqueue(struct t_queue *queue, int data);

void t_queue_dequeue(struct t_queue *queue);

bool t_queue_isEmpty(struct t_queue *queue);

void printQ(struct t_queue *queue);

int remove_node(struct t_queue *queue, int data);

bool is_present(struct t_queue *queue, int data);
