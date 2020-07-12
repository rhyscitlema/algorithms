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

#ifndef ITEM
#define ITEM void // Heap Item
// note: any type should be fine.
#endif


/**
	First initialise heap.size to = 0
	and initialise heap.data to =
		(ITEM**) malloc (heap.capacity * sizeof(ITEM*));

	The functions will only change these two.
	Set heap.compare to the callback function.
	heap.arg is only passed to heap.compare().

	heap.indexed is explained in the documentation.
*/
typedef struct _Heap {
	ITEM** data;
	int size;
	int capacity;
	bool indexed;
	const void* arg;
	int (*compare) (const ITEM* item1, const ITEM* item2, const void* arg);
} Heap;


void heap_push (Heap* heap, ITEM* node);

ITEM* heap_pop (Heap* heap); // return NULL if heap is empty

ITEM* heap_peek (Heap* heap); // return NULL if heap is empty

void heap_remove (Heap* heap, ITEM* node);

void heap_update (Heap* heap, ITEM* node);

void heap_append (Heap* heap, ITEM* node);

void heap_heapify (Heap* heap);


ITEM* heap_find (const Heap* heap, ITEM* item); // return NULL if not found

void heap_print(
	const Heap* heap,
	int level, // root is at level 1, the minimum
	void (*node_print) (int level, const ITEM* node, const void* arg));

#endif


/*

** heap->indexed = true:
This is only necessary if heap_update() or heap_remove() will be used.
If heap is indexed then every ITEM* node must actually be an int* node,
pointing to FREE int* memory, to only be used by these heap functions.

** Append then heapify:
heap_append() is to be used only if heap_heapify() will be called later.
They are useful so to set a heap of n elements in O(n) time. The
alternative is to use heap_push, which would take O(n log n) time.

*/
