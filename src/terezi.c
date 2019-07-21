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

/*\
 * HashTable
\*/

struct key_value {
	void *key;
	void *value;
};

static unsigned int default_hash(const void *key)
{
	/*\
	 * Credit for hash algorithm to P. J. Weinberger
	 * referenced in `Compilers: Principles, Techniques, and Tools`
	 * by Alfred V. Aho, Ravi Sethi, and Jeffrey D. Ullman
	\*/

	if (!key) return 0;

	unsigned int tmp;
	unsigned int rv = 0;
	const char *c = key;

	while (*c != '\0') {
		rv = (rv << 4) + (*c);
		if ((tmp = (rv & 0xf0000000))) {
			rv = rv ^ (tmp >> 24);
			rv = rv ^ tmp;
		}
		c++;
	}

	return rv;
}

tz_table *tz_table_init(
	int (*match)(const void *key1, const void *key2),
	void (*destroy)(void *data))
{
	return tz_table_init_custom(
		TZ_TABLE_DEFAULT_LENGTH,
		default_hash,
		match, destroy);
}

tz_table *tz_table_init_custom(
	unsigned int length,
	unsigned int (*h)(const void *key),
	int (*match)(const void *key1, const void *key2),
	void (*destroy)(void *data))
{
	tz_table *t = malloc(sizeof(tz_table));
	if (!t) die("tz_table_init_custom(): malloc():");
	memset(t, 0, sizeof(tz_table));

	t->h = h;
	t->match = match;
	t->length = length;
	t->destroy = destroy;

	t->array = malloc(sizeof(tz_dlist) * t->length);
	if (!t->array) die("tz_table_init_custom(): malloc():");
	memset(t->array, 0, sizeof(tz_dlist) * t->length);

	for (int i = 0; i < t->length; i++) {
		t->array[i].destroy = t->destroy;
	}

	return t;
}

void tz_table_free(tz_table *t)
{
	if (!t) return;

	tz_dlist *l;
	tz_dlist_node *n;
	tz_dlist_node *prev;
	for (int i = 0; i < t->length; i++) {
		l = &t->array[i];
		if (!l) continue;

		n = l->tail;
		while (n) {
			prev = n->prev;

			// their_destory_function(their data)
			if (l->destroy) l->destroy(
				((struct key_value *)n->data)->value
			);

			// free(the copied key + our key_value *)
			free(((struct key_value *)n->data)->key);
			free(n->data);

			// manage list pointers
			tz_dlist_rm(&t->array[i], n);

			n = prev;
		}
	}
	free(t->array);
	free(t);
}

int tz_table_store(tz_table *t, const char *key, void *data)
{
	if (!t || !t->array || !key) return 0;
	if (tz_table_fetch(t, key)) return 0;

	unsigned int index = t->h(key) % t->length;
	tz_dlist *l = &t->array[index];
	tz_dlist_node *n = l->head;
	if (n) while (n->next) n = n->next;

	struct key_value *kv = malloc(sizeof(struct key_value));
	if (!kv) die("tz_table_store: malloc():");
	memset(kv, 0, sizeof(struct key_value));

	size_t buf_size = sizeof(char) * TZ_TABLE_MAX_KEY_LENGTH + 1;
	kv->key = malloc(buf_size);
	if (!kv->key) die("tz_table_store: malloc():");
	memset(kv->key, 0, buf_size);

	strncpy(kv->key, key, buf_size - 1);
	kv->value = data;

	return tz_dlist_ins_next(l, n, kv) ? ++t->size : 0;
}

void *tz_table_fetch(tz_table *t, const char *key)
{
	if (!t || !t->array || !key) return NULL;

	unsigned int index = t->h(key) % t->length;
	tz_dlist *l = &t->array[index];
	tz_dlist_node *n = l->head;
	struct key_value *kv;

	while (n) {
		kv = n->data;
		if (!kv) {
			n = n->next;
			continue;
		}
		if (!strncmp(kv->key, key, TZ_TABLE_MAX_KEY_LENGTH)) {
			break;
		}
		n = n->next;
	}

	return n ? kv->value : NULL;
}

void *tz_table_rm(tz_table *t, const char *key)
{
	if (!t || !t->array || !key) return NULL;

	unsigned int index = t->h(key) % t->length;
	tz_dlist *l = &t->array[index];
	tz_dlist_node *n = l->head;
	struct key_value *kv;

	while (n) {
		kv = n->data;
		if (!kv) {
			n = n->next;
			continue;
		}
		if (!strncmp(kv->key, key, TZ_TABLE_MAX_KEY_LENGTH)) {
			break;
		}
		n = n->next;
	}

	if (!n) return NULL;

	// save their data
	void *rv = kv->value;
	
	// free our data (copied key + struct key_value)
	free(kv->key);
	free(kv);

	// manage list pointers
	tz_dlist_rm(l, n);
	t->size--;

	return rv;
}

/*\
 * Binary Tree
\*/

tz_btree *tz_btree_init(
	void *data,
	int (*compare)(const void *key1, const void *key2),
	void (*destroy)(void *data))
{
	tz_btree *t = malloc(sizeof(tz_btree));
	if (!t) die("tz_btree_init(): malloc():");
	memset(t, 0, sizeof(tz_btree));

	t->size = 1;
	t->data = data;
	t->compare = compare;
	t->destroy = destroy;
	return t;
}

static void increment_size(tz_btree *t)
{
	t->size++;
	if (t->parent) increment_size(t->parent);
}

static void decrement_size(tz_btree *t)
{
	t->size--;
	if (t->parent) decrement_size(t->parent);
}

void tz_btree_free(tz_btree *t, int destroy)
{
	if (!t) return;
	if (destroy && t->destroy) t->destroy(t->data);
	if (t->left) tz_btree_free(t->left, destroy);
	if (t->right) tz_btree_free(t->right, destroy);
	if (t->parent) {
		decrement_size(t->parent);
		tz_btree **parent = NULL;
		parent = t->parent->left == t
			? &t->parent->left
			: &t->parent->right;
		if (parent) *parent = NULL;
	}
	free(t);
}

tz_btree *tz_btree_ins_left(tz_btree *t, void *data)
{
	if (!t || t->left) return NULL;
	t->left = tz_btree_init(data, t->compare, t->destroy);
	t->left->parent = t;
	increment_size(t);
	return t->left;
}

tz_btree *tz_btree_ins_right(tz_btree *t, void *data)
{
	if (!t || t->right) return NULL;
	t->right = tz_btree_init(data, t->compare, t->destroy);
	t->right->parent = t;
	increment_size(t);
	return t->right;
}

tz_dlist *tz_btree_traverse_preorder(tz_btree *t, tz_dlist *l)
{
	if (!t) return l;
	if (!l) l = tz_dlist_init(NULL);

	tz_dlist_ins_next(l, l->tail, t->data);
	if (t->left) tz_btree_traverse_preorder(t->left, l);
	if (t->right) tz_btree_traverse_preorder(t->right, l);

	return l;
}

tz_dlist *tz_btree_traverse_inorder(tz_btree *t, tz_dlist *l)
{
	if (!t) return l;
	if (!l) l = tz_dlist_init(NULL);

	if (t->left) tz_btree_traverse_inorder(t->left, l);
	tz_dlist_ins_next(l, l->tail, t->data);
	if (t->right) tz_btree_traverse_inorder(t->right, l);

	return l;
}

tz_dlist *tz_btree_traverse_postorder(tz_btree *t, tz_dlist *l)
{
	if (!t) return l;
	if (!l) l = tz_dlist_init(NULL);

	if (t->left) tz_btree_traverse_postorder(t->left, l);
	if (t->right) tz_btree_traverse_postorder(t->right, l);
	tz_dlist_ins_next(l, l->tail, t->data);

	return l;
}

/*
// this method might not be what you think
// there is no balancing going on in this method
// if you push A B C D E F G you'll get
//		F
//	       / \
//	      D   G
//	     / \
//	    B   E
//	   / \
//	  A   C
*/
tz_btree *tz_btree_push(tz_btree *t, void *data)
{
	if (!t) return NULL;
	if (!t->left) {
		if (!t->data) {
			t->data = data;
			increment_size(t);
			return t;
		}
		tz_btree_ins_left(t, t->data);
		t->data = data;
		return t;
	}
	if (!t->right) {
		tz_btree_ins_right(t, data);
		return t;
	}

	tz_btree *new_root = tz_btree_init(data, t->compare, t->destroy);
	new_root->size = t->size + 1;
	t->parent = new_root;
	return new_root;
}
