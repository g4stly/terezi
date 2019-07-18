#include <terezi.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

/*\
 * Double Linked List
\*/
tz_dlist *tz_dlist_init(void (*delete)(void *data))
{
	tz_dlist *l = malloc(sizeof(tz_dlist));
	if (!l) die("tz_dlist_new(): malloc():");

	memset(l, '\0', sizeof(tz_dlist));
	l->destroy = delete;

	return l;
}

void tz_dlist_free(tz_dlist *l)
{
	if (!l) return;

	tz_dlist_node *prev;
	tz_dlist_node *n = l->tail;
	while (n) {
		prev = n->prev;
		tz_dlist_del(l, n);
		n = prev;
	}
	free(l);
}

tz_dlist_node *tz_dlist_ins_next(
	tz_dlist *l, 
	tz_dlist_node *prev, 
	void *data)
{
	if (!l || (!prev && l->size)) return NULL;

	tz_dlist_node *n = malloc(sizeof(tz_dlist_node));
	if (!n) die("tz_dlist_ins_next(): malloc():");
	memset(n, 0, sizeof(tz_dlist_node));

	n->data = data;
	n->prev = prev;
	if (n->prev) {
		n->next = n->prev->next;
		n->prev->next = n;
	}
	if (n->next) {
		n->next->prev = n;
	}

	if (!l->head || !n->prev) l->head = n;
	if (!l->tail || !n->next) l->tail = n;

	l->size++;
	return n;
}

tz_dlist_node *tz_dlist_ins_prev(
	tz_dlist *l,
	tz_dlist_node *next,
	void *data)
{
	if (!l || (!next && l->size)) return NULL;

	tz_dlist_node *n = malloc(sizeof(tz_dlist_node));
	if (!n) die("tz_dlist_ins_prev(): malloc():");
	memset(n, 0, sizeof(tz_dlist_node));

	n->data = data;
	n->next = next;
	if (n->next) {
		n->prev = n->next->prev;
		n->next->prev = n;
	}
	if (n->prev) {
		n->prev->next = n;
	}

	if (!l->head || !n->prev) l->head = n;
	if (!l->tail || !n->next) l->tail = n;

	l->size++;
	return n;
}

static void dlist_delete(tz_dlist *l, tz_dlist_node *n, int destroy)
{
	if (!l || !n) return;

	if (n->prev) n->prev->next = n->next;
	else l->head = n->next;

	if (n->next) n->next->prev = n->prev;
	else l->tail = n->prev;

	if (destroy && l->destroy) l->destroy(n->data);
	l->size--;
	free(n);
}

void tz_dlist_rm(tz_dlist *l, tz_dlist_node *n)
{
	dlist_delete(l, n, 0);
}

void tz_dlist_del(tz_dlist *l, tz_dlist_node *n)
{
	dlist_delete(l, n, 1);
}

/*\
 * Stack
\*/

tz_stack *tz_stack_init(void (*destroy)(void *data))
{
	return (tz_stack *)tz_dlist_init(destroy);
}

void tz_stack_free(tz_stack *s)
{
	tz_dlist_free((tz_dlist *)s);
}

int tz_stack_push(tz_stack *s, void *data)
{
	if (!s) return 0;
	tz_dlist *l = (tz_dlist *)s;
	return tz_dlist_ins_prev(l, l->head, data) ? 1 : 0;
}

void *tz_stack_peek(tz_stack *s)
{
	if (!s) return NULL;
	tz_dlist *l = (tz_dlist *)s;
	if (!l->head) return NULL;
	return l->head->data;
}

void *tz_stack_pop(tz_stack *s)
{
	if (!s) return NULL;
	tz_dlist *l = (tz_dlist *)s;
	if (!l->head) return NULL;

	void *rv = l->head->data;
	tz_dlist_rm(l, l->head);
	return rv;
}

/*\
 * Queue
\*/

tz_queue *tz_queue_init(void (*destroy)(void *data))
{
	return (tz_queue *)tz_dlist_init(destroy);
}

void tz_queue_free(tz_queue *q)
{
	tz_dlist_free((tz_dlist *)q);
}

int tz_queue_push(tz_queue *q, void *data)
{
	if (!q) return 0;
	tz_dlist *l = (tz_dlist *)q;
	return tz_dlist_ins_next(l, l->tail, data) ? 1 : 0;
}

void *tz_queue_peek(tz_queue *q)
{
	if (!q) return NULL;
	tz_dlist *l = (tz_dlist *)q;
	if (!l->head) return NULL;
	return l->head->data;
}

void *tz_queue_pop(tz_queue *q)
{
	if (!q) return NULL;
	tz_dlist *l = (tz_dlist *)q;
	if (!l->head) return NULL;

	void *rv = l->head->data;
	tz_dlist_rm(l, l->head);
	return rv;
}