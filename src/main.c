#include <stdio.h>
#include <terezi.h>
#include <assert.h>
#include "util.h"

void test_dlist();
void test_stack();
void test_queue();

int main(int argc, char **argv)
{
	test_dlist();
	test_stack();
	test_queue();
	return 0;
}

void test_dlist()
{
	const char *message1 = "hello";
	const char *message2 = ", ";
	const char *message3 = "world!";

	tz_dlist *l;
	tz_dlist_node *a;
	tz_dlist_node *b;
	tz_dlist_node *c;

	l = tz_dlist_init(NULL);
	assert(l);

	a = tz_dlist_ins_next(l, NULL, (void *)message1);
	assert(l->head == a);
	assert(l->tail == a);
	assert(l->size == 1);

	b = tz_dlist_ins_next(l, a, (void *)message3);
	assert(l->size == 2);
	assert(l->tail == b);

	assert(a->next == b);
	assert(!a->prev);
	assert(b->prev == a);
	assert(!b->next);

	c = tz_dlist_ins_prev(l, b, (void *)message2);
	assert(l->size == 3);
	assert(l->head == a);
	assert(l->tail == b);

	assert(!a->prev);
	assert(a->next == c);

	assert(c->prev == a);
	assert(c->next == b);

	assert(b->prev == c);
	assert(!b->next);

	a = l->head;
	printf("test_dlist(): ");
	while (a) {
		printf("%s", (char *)a->data);
		a = a->next;
	}
	tz_dlist_free(l);
	printf("\n");
}

void test_stack()
{
	const char *message1 = "he";
	const char *message2 = "l";
	const char *message3 = "o, world!";
	const char *message4 = "**junk**";

	tz_stack *s = tz_stack_init(NULL);
	assert(s);

	assert(tz_stack_push(s, (void *)message3));
	assert(tz_stack_push(s, (void *)message4));
	assert(tz_stack_push(s, (void *)message1));
	assert(tz_stack_push(s, (void *)message2));
	assert(tz_stack_push(s, (void *)message4));

	assert(tz_stack_pop(s) == (void *)message4);

	printf("test_stack(): %s%s%s", 
		(char *)tz_stack_pop(s), 
		(char *)tz_stack_pop(s), 
		(char *)tz_stack_peek(s));
	
	assert(tz_stack_pop(s) == (void *)message4);

	printf("%s\n", (char *)tz_stack_pop(s));

	tz_stack_free(s);
}

void test_queue()
{
	const char *message1 = "he";
	const char *message2 = "l";
	const char *message3 = "o, world!";
	const char *message4 = "**junk**";

	tz_queue *q = tz_queue_init(NULL);
	assert(tz_queue_push(q, (void *)message1));
	assert(tz_queue_push(q, (void *)message4));
	assert(tz_queue_push(q, (void *)message3));
	assert(tz_queue_push(q, (void *)message2));

	printf("test_queue(): %s", (char *)tz_queue_pop(q));
	assert(tz_queue_pop(q) == (void *)message4);
	printf("%s%s%s\n", 
		(char *)tz_queue_pop(q), 
		(char *)tz_queue_peek(q), 
		(char *)tz_queue_pop(q));
}