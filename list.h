/*
	list.h

	Provided by Rhyscitlema
	@ http://rhyscitlema.com

	USE AT YOUR OWN RISK!
*/
#ifndef _LIST_H
#define _LIST_H


#ifndef ITEM
#define ITEM void // List Item
// note: any type should be fine.
#endif

typedef struct _ListNode {
	struct _ListNode *prev, *next;
	unsigned int itemsize;
} ListNode;

typedef struct _List {  // double-linked list
	ListNode* head;     // head of list
	ListNode* tail;     // tail of list
	long size;          // number of nodes
} List;

static inline void list_clear (List *list)
{ if(list) { list->head=0; list->tail=0; list->size=0; } }


void  list_free (List* list);   // given list, free all nodes
ITEM* list_head (List* list);   // given list, get head node
ITEM* list_tail (List* list);   // given list, get tail node
ITEM* list_next (ITEM* node);   // given node, get its next
ITEM* list_prev (ITEM* node);   // given node, get its prev

// get a new node, use list_push later, if(item) then copy it.
ITEM* list_new (const ITEM* item, unsigned int itemsize);

ITEM* list_head_pop  (List* list);              // given list, pop out the head node
ITEM* list_tail_pop  (List* list);              // given list, pop out the tail node
ITEM* list_node_pop  (List* list, ITEM* node);  // given node, pop out of the list
void  list_head_push (List* list, ITEM* node);  // given node, push it as new head
void  list_tail_push (List* list, ITEM* node);  // given node, push it as new tail
void  list_delete    (List* list, ITEM* node);  // given node, delete it. list can be NULL

ITEM* list_find (List* list, const void* arg, int compare(const ITEM* item1, const ITEM* item2, const void* arg), const ITEM* item1);
void  list_sort (List* list, const void* arg, int compare(const ITEM* item1, const ITEM* item2, const void* arg));
void  list_merge(List* first, List* second);    // given two lists, merge them and clear the second

#endif
