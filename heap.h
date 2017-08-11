/*
    heap.h

    A minimum-heap implementation.

    Provided by Rhyscitlema
    @ http://rhyscitlema.com

    USE AT YOUR OWN RISK!
*/
#ifndef _HEAP_H
#define _HEAP_H

#include <stdbool.h>

/** Before calling any function, first initialise
    heap.size to = 0, as well as heap.data to =
    (void**) malloc (heap.maxSize * sizeof(void*));
    The functions only change heap.size.
*/
typedef struct _Heap
{   void** data;
    int size;
    int maxSize;
    bool indexed;
    const void* arg;
    int (*node_compare) (const void* a, const void* b, const void* arg);
} Heap;

void heap_push (Heap* heap, void* node);

bool heap_pop (Heap* heap, void* *node_ptr);

void heap_remove (Heap* heap, void* node);

void heap_update (Heap* heap, void* node);

void heap_append (Heap* heap, void* node);

void heap_heapify (Heap* heap);

bool heap_find (const Heap* heap, void* value);

void heap_print(const Heap* heap, int level, // root is at level 1
                void (*node_print) (const void* node, const void* arg, int level));

#endif



/*

** Indexed heap:
If heap is indexed then void* node must actually be an int* node,
which points to a free int* memory to be used only by heap functions.
This is only necessary if heap_update or heap_remove is to be used.

** Append then heapify:
heap_append is to be used only if heap_heapify will be called later.
They are useful so to set a heap of n elements in O(n) time. The
alternative is to use heap_push, which would take O(n log n) time.

*/
