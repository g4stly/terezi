#include <stdio.h>
#include <terezi.h>
#include <assert.h>
#include "util.h"

void test_dlist();
void test_stack();
void test_queue();
void test_table();
void test_btree();

int main(int argc, char **argv)
{
	test_dlist();
	test_stack();
	test_queue();
	test_table();
	test_btree();
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
	tz_queue_free(q);
}

void test_table()
{
	tz_table *t = tz_table_init(NULL, NULL);
	assert(t);

	assert(tz_table_store(t, "part1", "hello"));
	assert(tz_table_store(t, "part2", ", "));
	assert(tz_table_store(t, "junk!!!", "this is some junk!!"));
	assert(tz_table_store(t, "part3", "world!"));
	assert(!tz_table_store(t, "part2", "this won't insert!"));

	assert(tz_table_rm(t, "junk!!!"));
	assert(!tz_table_fetch(t, "junk!!!"));

	printf("test_table(): %s%s%s\n",
		(char *)tz_table_fetch(t, "part1"), 
		(char *)tz_table_fetch(t, "part2"),
		(char *)tz_table_fetch(t, "part3"));

	tz_table_free(t);
}

void speak_and_destroy(void *data)
{
	printf((char *)data);
}

void test_btree()
{
	tz_btree *t = tz_btree_init(NULL, speak_and_destroy);
	assert(t);

	tz_btree_node *node = tz_btree_ins_left(t, NULL, "hello");
	assert(node && t->size == 1);
	assert(!tz_btree_ins_left(t, NULL, "junk"));

	node = tz_btree_ins_left(t, node, ", ");
	assert(node && t->size == 2);

	tz_btree_node *delete_me;
	delete_me = tz_btree_ins_right(t, node, "world!\n");
	assert(delete_me && t->size == 3);

	assert((node = tz_btree_ins_left(t, delete_me, "test_")));
	assert(tz_btree_ins_left(t, node, "btr"));
	assert(tz_btree_ins_right(t, node, "ee(): "));

	assert(tz_btree_del_left(t, delete_me) && t->size == 3);
	tz_btree_free(t);
}
