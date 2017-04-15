/*
    heap.h

    Provided by Rhyscitlema
    @ http://rhyscitlema.com

    USE AT YOUR OWN RISK!
*/
#ifndef _HEAP_H
#define _HEAP_H

#include <stdbool.h>

typedef struct _Heap
{   // NOTE: do NOT access struct variables directly.
    int size;
    int max_size;
    bool indexed;
    const void* arg;
    int (*node_compare) (const void* arg, const void* a, const void* b);
} Heap;

Heap* heap_build (
    Heap* heap,         // If first time then heap==NULL.
    int max_size,       // If any other argument is 0
    bool indexed,       // or NULL then it is ignored,
    const void* arg,    // this is useful for editing.
    int (*heap_node_compare) (const void* arg, const void* a, const void* b));

int heap_size (Heap* heap);

void heap_free (Heap* heap);

void heap_push (Heap* heap, void* node);

bool heap_pop (Heap* heap, void* *node_ptr);

void heap_remove (Heap* heap, void* node);

void heap_update (Heap* heap, void* node);

void heap_append (Heap* heap, void* node);

void heap_heapify (Heap* heap);

bool heap_find (const Heap* heap, void* value);

void heap_print (const Heap* heap, int indent,
    const char* (*heap_node_print) (const void* arg, const void* a));

#endif



/*

1) Indexed heap:
If heap is indexed then void* node must actually be an int* node,
which points to a free int memory to be used only by heap functions.
This is only necessary if heap_update or heap_remove is to be used.

2) Append then heapify:
heap_append is to be used only if heap_heapify will be called later.
They are useful so to set a heap of n elements in O(n) time. The
alternative is to use heap_push, which would take O(n log n) time.

*/
