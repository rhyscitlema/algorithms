/*
	heap.c

	Provided by Rhyscitlema
	@ http://rhyscitlema.com

	USE AT YOUR OWN RISK!
*/

#include "heap.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>


#define HDATA(level) heap->data[(level)-1]

#define INDEX_UPDATE \
	if(heap->indexed) \
		*(int*)HDATA(level) = level; \


static bool update_downward (Heap* heap, ITEM* node, int level)
{
	while(true)
	{
		if(level < 2) break;
		int parent = level/2;

		if(heap->compare(node, HDATA(parent), heap->arg) < 0)
		{
			HDATA(level) = HDATA(parent);
			INDEX_UPDATE
			level = parent;
			continue;
		}
		break;
	}
	if(HDATA(level)==node) // if no update
		return false;
	HDATA(level) = node;
	INDEX_UPDATE
	return true;
}


static bool update_upward (Heap* heap, ITEM* node, int level)
{
	int size = heap->size;
	while(true)
	{
		int left  = level*2+0;
		int right = level*2+1;
		if(left > size) break;

		if(left == size
		|| heap->compare(HDATA(left), HDATA(right), heap->arg) < 0)
		{
			if(heap->compare(node, HDATA(left), heap->arg) >= 0)
			{
				// shift left child
				HDATA(level) = HDATA(left);
				INDEX_UPDATE
				level = left;
				continue;
			}
		}
		else
		{
			if(heap->compare(node, HDATA(right), heap->arg) >= 0)
			{
				// shift right child
				HDATA(level) = HDATA(right);
				INDEX_UPDATE
				level = right;
				continue;
			}
		}
		break;
	}
	if(HDATA(level)==node) // if no update
		return false;
	HDATA(level) = node;
	INDEX_UPDATE
	return true;
}


void heap_push (Heap* heap, ITEM* node)
{
	assert(heap!=NULL);
	if(!heap) return;

	int level = ++heap->size;
	HDATA(level) = node;
	if(heap->indexed) *(int*)node = level;

	update_downward(heap, node, level);
}


ITEM* heap_pop (Heap* heap)
{
	assert(heap!=NULL);
	if(!heap) return NULL;
	if(heap->size <= 0) return NULL;

	ITEM* root = HDATA(1);
	if(heap->indexed) *(int*)root = -1;

	int size = heap->size--;
	ITEM* node = HDATA(size);

	update_upward(heap, node, 1);
	return root;
}


ITEM* heap_peek (Heap* heap)
{
	assert(heap!=NULL);
	if(!heap) return NULL;
	if(heap->size <= 0) return NULL;
	ITEM* root = HDATA(1);
	return root;
}


void heap_remove (Heap* heap, ITEM* node)
{
	assert(heap && node && heap->indexed);
	if(!(heap && node && heap->indexed)) return;

	int level = *(int*)node;
	assert(0 < level && level <= heap->size);
	if(!(0 < level && level <= heap->size)) return;

	*(int*)node = -1; // set to invalid
	int size = heap->size--;

	node = HDATA(size);
	update_upward(heap, node, level);
}


void heap_update (Heap* heap, ITEM* node)
{
	assert(heap && node && heap->indexed);
	if(!(heap && node && heap->indexed)) return;

	int level = *(int*)node;
	assert(0 < level && level <= heap->size);
	if(!(0 < level && level <= heap->size)) return;

	if(!update_downward(heap, node, level))
		update_upward(heap, node, level);
}


void heap_append (Heap* heap, ITEM* node)
{
	heap_push(heap, node);
}


void heap_heapify (Heap* heap) // TODO: implement this
{}


ITEM* heap_find (const Heap* heap, ITEM* item)
{
	assert(heap!=NULL);
	if(!heap) return NULL;
	int level;
	for(level=1; level <= heap->size; level++)
	{
		ITEM* node = HDATA(level);
		if(0==heap->compare(node, item, heap->arg))
			return node;
	}
	return NULL;
}


void heap_print(
	const Heap* heap,
	int level, // root is at level 1, the minimum
	void (*node_print) (int level, const ITEM* node, const void* arg))
{
	if(level < 1) level = 1;
	if(level <= heap->size)
	{
		int left  = level*2+0;
		int right = level*2+1;
		heap_print (heap, right, node_print);
		node_print (level, HDATA(level), heap->arg);
		heap_print (heap, left, node_print);
	}
}

