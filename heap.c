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


#define INDEX_UPDATE if(heap->indexed) *(int*)data[i] = i;

static bool update_downward (Heap* heap, void* node, int i)
{
	void** data = heap->data;
	while(i>1)
	{
		if(heap->node_compare(node, data[i/2], heap->arg) < 0)
		{
			data[i] = data[i/2];
			INDEX_UPDATE
			i = i/2;
		}
		else break;
	}
	if(data[i]==node) return 0; // if no update
	data[i] = node;
	INDEX_UPDATE
	return 1;
}

static bool update_upward (Heap* heap, void* node, int i)
{
	void** data = heap->data;
	int size = heap->size;
	while(1)
	{
		if(i*2 > size) break;

		if(i*2 == size
		|| heap->node_compare(data[i*2], data[i*2+1], heap->arg)<0)
		{
			if(heap->node_compare(data[i*2], node, heap->arg)<=0)
			{
				// shift left child
				data[i] = data[i*2];
				INDEX_UPDATE
				i = i*2;
				continue;
			}
		}
		else
		{
			if(heap->node_compare(data[i*2+1], node, heap->arg)<=0)
			{
				// shift right child
				data[i] = data[i*2+1];
				INDEX_UPDATE
				i = i*2+1;
				continue;
			}
		}
		break;
	}
	if(data[i]==node) return 0; // if no update
	data[i] = node;
	INDEX_UPDATE
	return 1;
}

void heap_push (Heap* heap, void* node)
{
	assert(heap!=NULL);
	if(!heap) return;
	int i = ++heap->size;
	heap->data[i] = node;
	if(heap->indexed) *(int*)node = i;
	update_downward(heap, node, i);
}

void* heap_pop (Heap* heap)
{
	assert(heap!=NULL);
	if(!heap) return NULL;
	if(heap->size <= 0) return NULL;
	void* root = heap->data[1];
	if(heap->indexed) *(int*)root = -1;
	int size = heap->size--;
	void* node = heap->data[size];
	update_upward(heap, node, 1);
	return root;
}

void heap_remove (Heap* heap, void* node)
{
	assert(heap && node && heap->indexed);
	if(!(heap && node && heap->indexed)) return;
	int i = *(int*)node;
	if(i<0) return;
	*(int*)node = -1;
	int size = heap->size--;
	node = heap->data[size];
	update_upward(heap, node, i);
}

void heap_update (Heap* heap, void* node)
{
	assert(heap && node && heap->indexed);
	if(!(heap && node && heap->indexed)) return;
	int i = *(int*)node;
	if(i<0) return;
	if(!update_downward(heap, node, i))
		update_upward(heap, node, i);
}


void heap_append (Heap* heap, void* node)
{
	heap_push(heap, node);
}

void heap_heapify (Heap* heap)
{}


void* heap_find (const Heap* heap, void* value)
{
	assert(heap!=NULL);
	if(!heap) return NULL;
	int i;
	for(i=1; i <= heap->size; i++)
		if(0==heap->node_compare(heap->data[i], value, heap->arg))
			return heap->data[i];
	return NULL;
}


void heap_print(const Heap* heap, int level, // root is at level 1, the minimum
				void (*node_print) (const void* node, const void* arg, int level))
{
	if(level < 1) level = 1;
	if(level > heap->size) return;
	heap_print (heap, level*2+1  , node_print);      // right subtree
	node_print (heap->data[level], heap->arg, level);// print heap node
	heap_print (heap, level*2+0  , node_print);      // left subtree
}

