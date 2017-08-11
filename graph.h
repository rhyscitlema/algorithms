/*
    graph.h

    Provided by Rhyscitlema
    @ http://rhyscitlema.com

    USE AT YOUR OWN RISK!
*/
#ifndef _GRAPH_H
#define _GRAPH_H

#include <stdbool.h>


typedef long cost;
typedef struct { int u, v; cost c; } Edge;
typedef struct { int    v; cost c; } *Adja;
#define EdgeList Edge*
#define AdjaList Adja*

#define AdjaGet(adjaList,u) (adjaList[u])
#define AdjaV(adjaList) (*(int*)(adjaList))         // get number of vertexes
#define AdjaE(adjaList) (AdjaGet(adjaList,1)-1)->v  // get number of edges
#define AdjaD(adjaList) (AdjaGet(adjaList,1)-1)->c  // get graph-is-directed
#define AdjaPrev(adjaList) (adjaList + (AdjaD(adjaList) ? AdjaV(adjaList) : 0))


// The given graph is converted to a string
// which is put inside the given 'out' and then
// returned. 'out' is assumed to be large enough.
char* print_edge_list (char* out, const EdgeList edgeList);
char* print_adja_list (char* out, const AdjaList adjaList);

EdgeList load_edge_list (const char* str);
AdjaList load_adja_list (const char* str);

EdgeList clone_edge_list (const EdgeList edgeList);
AdjaList clone_adja_list (const AdjaList adjaList);

AdjaList edge_list_to_adja_list (const EdgeList edgeList);
EdgeList adja_list_to_edge_list (const AdjaList adjaList);

//void free_edge_list (EdgeList edgeList) { free(edgeList); }
//void free_adja_list (AdjaList adjaList) { free(adjaList); }


// code below shows how to access an adja_list
static inline Adja getEdgeOfAdjaList (const AdjaList adjaList, int u, int v)
{
    int i, m = adjaList[u][0].v;    // get number of sink adjacent vertexes to u
    for(i=1; i<=m; i++)             // for every sink adjacent vertex
        if(v == adjaList[u][i].v)   // check if it is the given vertex v
            return &adjaList[u][i]; // return pointer to adjacent edge
    return NULL;
}

// this does nothing but show how DFS traversal is coded
void DFS_traversal (const AdjaList adjaList, int rootVertex);

// renumber source and sink vertexes separately
void renumber_edge_list_vertexes (EdgeList edgeList);


// reduce given graph to an MST
void minimum_spanning_tree (EdgeList edgeList);

// The result is the edges of the constructed tree,
// with result[i].c = total cost from given source.
EdgeList single_source_shortest_path (const AdjaList adjaList, int source);

// return an array of single_source_shortest_paths,
// for every vertex starting from vertex 1 at index 1.
EdgeList* all_pairs_shortest_path (const AdjaList adjaList);

// return the result graph as an unweighted undirected EdgeList
EdgeList maximum_matching_unweighted (const AdjaList adjaList, bool skipPart1);


// Alternative to the bsearch() from <stdlib.h>.
// Extra argument to compare() may be passed
// by using a pointer location just before key.
int binary_search (
    int (*compare)(const void* key, const void* array, int index),
    const void* key, const void* array, int low, int high);

#endif



/*
_______________

 Documentation
_______________

Unless otherwise specified, any returned pointer result
points to allocated memory which should be freed using
the "free(pointer)" function from <malloc.h>.

It is important to know how graph data is represented:
    - as characters in a string
    - as data bytes in memory

----------------------------------------
1) Graph data as characters in a string
----------------------------------------

Below is an example directed unweighted graph:
    """""
     23
    The first non-space content must be an integer.
    It denotes the number of following lines to
    skip before the start of the graph data.
    Here this number is 23.

    The graph data starts with 5 values:
    1) number of vertexes
    2) number of edges
    3) 1 if graph is directed, 0 otherwise
    4) 1 if graph is weighted, 0 otherwise

    5) graph-type identifier:
    5.a) 0: graph is given as a list of edges.
    5.b) 1: graph is given as an adjacency list
            of source vertexes.

    Unweighted defaults to weights of 1.
    Given weights must be of ECost type.
    load_edge_list() expects int type.

    Any content after the graph data is ignored.
    Vertex numbering starts from 1 not 0.

    9 5 1 0 0

    1 2
    3 4
    1 5
    8 9
    1 7
    """""

Below is an example undirected weighted graph:
    """""
    0
    9 5 0 1 0

    1 2 100
    3 4 200
    1 5 300
    8 9 400
    1 7 500
    """""

Below is the equivalent directed unweighted
graph using adjacency list:
    """""
    0
    9 5 1 0 1

    1  3  2 5 7
    2  0
    3  1  4
    4  0
    5  0
    6  0
    7  0
    8  1  9
    9  0
    """""

Below is the equivalent undirected weighted
graph using adjacency list:
    """""
    0
    9 5 0 1 1

    1  3  2 100  5 300  7 500
    2  1  1 100
    3  1  4 200
    4  1  3 200
    5  1  1 300
    6  0
    7  1  1 500
    8  1  9 400
    9  1  8 400
    """""


----------------------------------------
2) Graph data as data bytes in memory
----------------------------------------

... to be updated ...


*/
