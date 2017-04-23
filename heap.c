/*
    heap.c

    Provided by Rhyscitlema
    @ http://rhyscitlema.com

    USE AT YOUR OWN RISK!
*/
#include "heap.h"
#include <malloc.h>
#include <string.h>
#include <stdio.h>

#define heap_data ((void**)(heap+1))



Heap* heap_build (
    Heap* heap,         // if first time then heap==NULL.
    int max_size,       // if any other argument is 0
    bool indexed,       // or NULL then it is ignored,
    const void* arg,    // this is useful for editing.
    int (*heap_node_compare) (const void* arg, const void* a, const void* b))
{
    if(heap==NULL || (max_size!=0 && max_size != heap->max_size))
    {
        if(max_size==0)
        {   if(heap==NULL) return NULL;
            else max_size = heap->max_size;
        }
        Heap* h = (Heap*) malloc (sizeof(Heap) + (max_size+1)*sizeof(void*));
        if(heap)
        {   memcpy(h, heap, sizeof(Heap) + (heap->size+1)*sizeof(void*));
            free(heap);
        } else memset(h, 0, sizeof(Heap));
        heap = h;
    }
    if(max_size) heap->max_size = max_size;
    if(indexed) heap->indexed = indexed;
    if(arg) heap->arg = arg;
    if(heap_node_compare) heap->node_compare = heap_node_compare;
    return heap;
}

void heap_free (Heap* heap)
{ free(heap); }

int heap_size (Heap* heap)
{ return heap->size; }



#define INDEX_UPDATE if(heap->indexed) *(int*)data[i] = i;

static bool update_downward (Heap* heap, void* node, int i)
{
    void** data = heap_data;
    while(i>1)
    {
        if(heap->node_compare(heap->arg, node, data[i/2]) < 0)
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
    void** data = heap_data;
    int size = heap->size;
    while(1)
    {
        if(i*2 > size) break;

        if(i*2 == size
        || heap->node_compare(heap->arg, data[i*2], data[i*2+1])<0)
        {
            if(heap->node_compare(heap->arg, data[i*2], node)<=0)
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
            if(heap->node_compare(heap->arg, data[i*2+1], node)<=0)
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
    int i = ++heap->size;
    heap_data[i] = node;
    if(heap->indexed) *(int*)node = i;
    update_downward(heap, node, i);
}


bool heap_pop (Heap* heap, void* *node_ptr)
{
    if(heap->size <= 0) return 0;
    void* root = heap_data[1];
    if(heap->indexed) *(int*)root = -1;
    *node_ptr = root;
    int size = heap->size--;
    void* node = heap_data[size];
    update_upward(heap, node, 1);
    return 1;
}


void heap_remove (Heap* heap, void* node)
{
    if(!heap->indexed) return;
    int i = *(int*)node;
    if(i<0) return;
    *(int*)node = -1;
    int size = heap->size--;
    node = heap_data[size];
    update_upward(heap, node, i);
}


void heap_update (Heap* heap, void* node)
{
    if(!heap->indexed) return;
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



bool heap_find (const Heap* heap, void* value)
{
    int i;
    for(i=1; i <= heap->size; i++)
        if(0==heap->node_compare(heap->arg, heap_data[i], value))
            return 1;
    return 0;
}



void do_heap_print (const Heap* heap, int indent, int space, int l,
    const char* (*node_print) (const void* arg, const void* a))
{
    int i;
    if(l > heap->size) return;
    do_heap_print (heap, indent, space+indent, l*2+1, node_print); // right subtree
    for(i=0; i<space; i++) putchar(' ');                           // print indentation
    printf("%s\r\n", node_print(heap->arg, heap_data[l]));         // print heap node
    do_heap_print (heap, indent, space+indent, l*2+0, node_print); // left subtree
}

void heap_print (const Heap* heap, int indent,
    const char* (*node_print) (const void* arg, const void* a))
{ do_heap_print (heap, indent, indent, 1, node_print); }

