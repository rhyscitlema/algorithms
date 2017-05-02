/*
    list.h

    Provided by Rhyscitlema
    @ http://rhyscitlema.com

    USE AT YOUR OWN RISK!
*/
#ifndef _LIST_H
#define _LIST_H


typedef struct _ListNode
{   struct _ListNode *prev, *next;
    int keysize;
} ListNode;

typedef struct _List    // double-linked list
{   ListNode* head;     // head of list
    ListNode* tail;     // tail of list
    long size;          // number of nodes
} List;

static inline void list_clear (List *list)
{ if(list) { list->head=0; list->tail=0; list->size=0; } }


void  list_free (List* list);   // given list, free all nodes
void* list_head (List* list);   // given list, get head node
void* list_tail (List* list);   // given list, get tail node
void* list_next (void* node);   // given node, get its next
void* list_prev (void* node);   // given node, get its prev
void* list_new (const void* key1, int keysize); // get a new node, use list_push later

void* list_head_pop  (List* list);              // given list, pop out the head node
void* list_tail_pop  (List* list);              // given list, pop out the tail node
void* list_node_pop  (List* list, void* node);  // given node, pop out of the list
void  list_head_push (List* list, void* node);  // given node, push it as new head
void  list_tail_push (List* list, void* node);  // given node, push it as new tail
void  list_delete    (List* list, void* node);  // given node, delete it

void* list_find (List* list, const void* arg, int compare(const void* key1, const void* key2, const void* arg), const void* key1);
void  list_sort (List* list, const void* arg, int compare(const void* key1, const void* key2, const void* arg));
void  list_merge(List* first, List* second);    // given two lists, merge them and clear the second

#endif
