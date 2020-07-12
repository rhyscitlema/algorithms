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

/**
	First initialise heap.size to = 0
	and initialise heap.data to =
	(void**) malloc (heap.maxSize * sizeof(void*));

	The functions will only change these two.
	Set heap.node_compare to the callback function.
	heap.arg is only passed to heap.node_compare().

	heap.indexed is explained in the documentation.
*/
typedef struct _Heap {
	void** data;
	int size;
	int maxSize;
	bool indexed;
	const void* arg;
	int (*node_compare) (const void* a, const void* b, const void* arg);
} Heap;

void heap_push (Heap* heap, void* node);

void* heap_pop (Heap* heap); // return NULL if heap is empty

void heap_remove (Heap* heap, void* node);

void heap_update (Heap* heap, void* node);

void heap_append (Heap* heap, void* node);

void heap_heapify (Heap* heap);

void* heap_find (const Heap* heap, void* value); // return NULL if not found

void heap_print(
	const Heap* heap,
	int level, // root is at level 1, the minimum
	void (*node_print) (const void* node, const void* arg, int level));

#endif

/*

** Indexed heap:
This is only necessary if heap_update() or heap_remove() will be used.
If heap is indexed then every "void* node" must actually be an int* node
pointing to FREE int* memory to only be used by these heap functions.

** Append then heapify:
heap_append() is to be used only if heap_heapify() will be called later.
They are useful so to set a heap of n elements in O(n) time. The
alternative is to use heap_push, which would take O(n log n) time.

*/
