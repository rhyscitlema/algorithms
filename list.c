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


static inline void* nodeToKey (ListNode* node) { return node ? node+1 : NULL; }

static inline ListNode* keyToNode (void* key) { return key ? (ListNode*)key-1 : NULL; }


void* list_head (List* list) { return list ? nodeToKey(list->head) : NULL; }

void* list_tail (List* list) { return list ? nodeToKey(list->tail) : NULL; }

void* list_next (void* node) { return node ? nodeToKey(keyToNode(node)->next) : NULL; }

void* list_prev (void* node) { return node ? nodeToKey(keyToNode(node)->prev) : NULL; }


void* list_new (const void* key1, unsigned int keysize)
{
    assert(keysize>0);
    unsigned int size = sizeof(ListNode) + keysize;
    ListNode* node = (ListNode*)_malloc(size, "ListNode");
    memset(node, 0, size);
    node->keysize = keysize;
    void* key2 = nodeToKey(node);
    if(key1 && key2) memcpy(key2, key1, keysize);
    return key2;
}

static void node_remove (ListNode* node)
{
    unsigned int size = sizeof(ListNode) + node->keysize;
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
    else
    {   first->tail->next = second->head;
        second->head->prev = first->tail;
        first->tail = second->tail;
    }
    list_clear(second);
}

void* list_head_pop (List* list)
{
    assert(list!=NULL);
    ListNode* node = list->head;
    if(!node) return NULL;
    list->size--;
    list->head = node->next;
    if(list->head==NULL) list->tail = NULL;
    else list->head->prev = NULL;
    node->prev = node->next = NULL;
    return nodeToKey(node);
}

void* list_tail_pop (List* list)
{
    assert(list!=NULL);
    ListNode* node = list->tail;
    if(!node) return NULL;
    list->size--;
    list->tail = node->prev;
    if(list->tail==NULL) list->head = NULL;
    else list->tail->next = NULL;
    node->prev = node->next = NULL;
    return nodeToKey(node);
}

void* list_node_pop (List* list, void* key)
{
    assert(list!=NULL);
    ListNode* node = keyToNode(key);
    if(!node) return NULL;
    list->size--;
    if(node==list->head) list->head = node->next;
    if(node==list->tail) list->tail = node->prev;
    if(node->next) node->next->prev = node->prev;
    if(node->prev) node->prev->next = node->next;
    node->prev = node->next = NULL;
    return nodeToKey(node);
}


void  list_head_push (List* list, void* key)
{
    assert(list && key);
    ListNode* node = keyToNode(key);
    node->prev = NULL;
    node->next = list->head;
    if(list->head) node->next->prev = node;
    else list->tail = node;
    list->head = node;
    list->size++;
}

void  list_tail_push (List* list, void* key)
{
    assert(list && key);
    ListNode* node = keyToNode(key);
    node->next = NULL;
    node->prev = list->tail;
    if(list->tail) node->prev->next = node;
    else list->head = node;
    list->tail = node;
    list->size++;
}

void  list_delete (List* list, void* key)
{
    if(key==NULL) return;
    if(list) list_node_pop(list, key);
    node_remove(keyToNode(key));
}


void* list_find (List* list, const void* arg, int compare(const void* key1, const void* key2, const void* arg), const void* key1)
{
    assert(list!=NULL);
    ListNode* node = list->head;
    for( ; node!=NULL; node = node->next)
    {
        void* key2 = nodeToKey(node);
        if(0==compare(key1, key2, arg)) return key2;
    }
    return NULL;
}


void  list_sort (List* list, const void* arg, int compare(const void* key1, const void* key2, const void* arg))
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
                if(second == stop || (first != end && compare( nodeToKey(first), nodeToKey(second), arg)<=0))
                     { node = first; first = first->next; }
                else { node = second; second = second->next; }
                if(tail) tail->next = node;
                else head = node;
                tail = node;
            }

            // fix the list links for head->prev and tail->next.
            if(start==NULL) root = head;
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

