/*
	list.c

	Provided by Rhyscitlema
	@ http://rhyscitlema.com

	USE AT YOUR OWN RISK!
*/
#include "list.h"
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#ifdef LIB_STD
// used only for debugging
#include <_malloc.h>
#else
#include <malloc.h>
#define _malloc(size, type) malloc(size)
#define _free(ptr, type) free(ptr)
#endif


static inline ITEM* nodeToItem (ListNode* node) { return node ? (ITEM*)(node+1) : NULL; }

static inline ListNode* itemToNode (ITEM* item) { return item ? ((ListNode*)item)-1 : NULL; }


ITEM* list_head (List* list) { return list ? nodeToItem(list->head) : NULL; }

ITEM* list_tail (List* list) { return list ? nodeToItem(list->tail) : NULL; }

ITEM* list_next (ITEM* node) { return node ? nodeToItem(itemToNode(node)->next) : NULL; }

ITEM* list_prev (ITEM* node) { return node ? nodeToItem(itemToNode(node)->prev) : NULL; }


ITEM* list_new (const ITEM* item, unsigned int itemsize)
{
	assert(itemsize>0);
	unsigned int size = sizeof(ListNode) + itemsize;
	ListNode* node = (ListNode*)_malloc(size, "ListNode");
	memset(node, 0, size);
	node->itemsize = itemsize;
	ITEM* item2 = nodeToItem(node);
	if(item && item2) memcpy(item2, item, itemsize);
	return item2;
}

static void node_remove (ListNode* node)
{
	unsigned int size = sizeof(ListNode) + node->itemsize;
	memset(node, 0, size);
	_free(node, "ListNode");
}

void list_free (List* list)
{
	assert(list!=NULL);
	ListNode *next, *node = list->head;
	while(node)
	{
		next = node->next;
		node_remove(node);
		node = next;
	}
	list_clear(list);
}


void  list_merge (List* first, List* second)
{
	if(!first || !second) return;
	if(!second->head) return;
	if(!first->tail) *first = *second;
	else{
		first->tail->next = second->head;
		second->head->prev = first->tail;
		first->tail = second->tail;
	}
	list_clear(second);
}

ITEM* list_head_pop (List* list)
{
	assert(list!=NULL);
	ListNode* node = list->head;
	if(!node) return NULL;
	list->size--;
	list->head = node->next;
	if(list->head==NULL) list->tail = NULL;
	else list->head->prev = NULL;
	node->prev = node->next = NULL;
	return nodeToItem(node);
}

ITEM* list_tail_pop (List* list)
{
	assert(list!=NULL);
	ListNode* node = list->tail;
	if(!node) return NULL;
	list->size--;
	list->tail = node->prev;
	if(list->tail==NULL) list->head = NULL;
	else list->tail->next = NULL;
	node->prev = node->next = NULL;
	return nodeToItem(node);
}

ITEM* list_node_pop (List* list, ITEM* item)
{
	assert(list!=NULL);
	ListNode* node = itemToNode(item);
	if(!node) return NULL;
	list->size--;
	if(node==list->head) list->head = node->next;
	if(node==list->tail) list->tail = node->prev;
	if(node->next) node->next->prev = node->prev;
	if(node->prev) node->prev->next = node->next;
	node->prev = node->next = NULL;
	return nodeToItem(node);
}


void  list_head_push (List* list, ITEM* item)
{
	assert(list!=NULL);
	if(!list || !item) return;
	ListNode* node = itemToNode(item);
	node->prev = NULL;
	node->next = list->head;
	if(list->head) node->next->prev = node;
	else list->tail = node;
	list->head = node;
	list->size++;
}

void  list_tail_push (List* list, ITEM* item)
{
	assert(list!=NULL);
	if(!list || !item) return;
	ListNode* node = itemToNode(item);
	node->next = NULL;
	node->prev = list->tail;
	if(list->tail) node->prev->next = node;
	else list->head = node;
	list->tail = node;
	list->size++;
}

void  list_delete (List* list, ITEM* item)
{
	if(item==NULL) return;
	if(list) list_node_pop(list, item);
	node_remove(itemToNode(item));
}


ITEM* list_find (List* list, const void* arg, int compare(const ITEM* item1, const ITEM* item2, const void* arg), const ITEM* item1)
{
	assert(list!=NULL);
	ListNode* node = list->head;
	for( ; node!=NULL; node = node->next)
	{
		ITEM* item2 = nodeToItem(node);
		if(0==compare(item1, item2, arg)) return item2;
	}
	return NULL;
}


void  list_sort (List* list, const void* arg, int compare(const ITEM* item1, const ITEM* item2, const void* arg))
{
	assert(list!=NULL);
	int i, size;
	ListNode *end, *node;
	ListNode *head, *tail;
	ListNode *start, *stop;
	ListNode *first, *second;
	ListNode *root = list->head;

	for(size = 1; ; size *= 2)
	{
		stop  = NULL;
		start = NULL;
		first = root;
		while(first != NULL)
		{
			// get first list: first-to-second
			for(i=0, second = first;
				i<size && second != NULL;
				i++, second = second->next);

			// get second list: second-to-stop
			for(i=0, stop = second;
				i<size && stop != NULL;
				i++, stop = stop->next);

			// merge two sorted lists: first-to-second and second-to-stop.
			head = tail = NULL;
			end = second;
			while(first != end || second != stop)
			{
				if(second == stop
				|| (first != end
				&& compare(nodeToItem(first), nodeToItem(second), arg)<=0))
				{
					node = first;
					first = first->next;
				}
				else {
					node = second;
					second = second->next;
				}
				if(!tail) head = node;
				else tail->next = node;
				tail = node;
			}

			// fix the list links for head->prev and tail->next.
			if(!start) root = head;
			else start->next = head;
			tail->next = stop;

			// prepare for the next pair of sub-lists
			if(start==NULL && stop==NULL) break; // if no next pair
			start = tail;
			first = stop;
		}
		if(start==NULL && stop==NULL) break; // if entire list is sorted
	}
	// final fix of the list links
	root->prev = NULL;
	list->head = root;
	while(true)
	{
		end = root->next;
		if(end==NULL) { list->tail = root; break; }
		end->prev = root;
		root = end;
	}
}

