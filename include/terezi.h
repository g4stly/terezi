#ifndef terezi_pyrope_h_
#define terezi_pyrope_h_

/*\
 * Double Linked List
\*/
typedef struct dlist_node {
	void *data;
	struct dlist_node *next;
	struct dlist_node *prev;
} tz_dlist_node;

typedef struct dlist {
	int size;

	int (*match)(const void *key1, const void *key2);
	void (*destroy)(void *data);

	tz_dlist_node *head;
	tz_dlist_node *tail;
} tz_dlist;

tz_dlist *tz_dlist_init(void (*destroy)(void *data));
void tz_dlist_free(tz_dlist *list);

tz_dlist_node *tz_dlist_ins_next(
	tz_dlist *list, 
	tz_dlist_node *node, 
	void *data);
tz_dlist_node *tz_dlist_ins_prev(
	tz_dlist *list, 
	tz_dlist_node *node, 
	void *data);

// won't call destroy
void tz_dlist_rm(tz_dlist *list, tz_dlist_node *node);

// WILL call destroy on data
void tz_dlist_del(tz_dlist *list, tz_dlist_node *node);

/*\
 * Stack
\*/

typedef tz_dlist tz_stack;

tz_stack *tz_stack_init(void (*destroy)(void *data));
void tz_stack_free(tz_stack *stack);

int tz_stack_push(tz_stack *stack, void *data);
void *tz_stack_peek(tz_stack *stack);
void *tz_stack_pop(tz_stack *stack);

/*\
 * Queue
\*/

typedef tz_dlist tz_queue;

tz_queue *tz_queue_init(void (*destroy)(void *data));
void tz_queue_free(tz_queue *queue);

int tz_queue_push(tz_queue *queue, void *data);
void *tz_queue_peek(tz_queue *queue);
void *tz_queue_pop(tz_queue *queue);
#endif
