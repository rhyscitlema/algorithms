/*
	graph.h

	Provided by Rhyscitlema
	@ http://rhyscitlema.com

	USE AT YOUR OWN RISK!
*/
#ifndef _GRAPH_H
#define _GRAPH_H

#include <stdbool.h>
#include <stddef.h>


typedef long cost; // must match with load_edge_list()
typedef struct { int u, v; cost c; } Edge, *EdgeP;
typedef struct { int    v; cost c; } Adge, *Adja, **AdjaP;
typedef const Edge* const_EdgeP;
typedef const Adja* const_AdjaP;

#define AdjaU(adja,u) (adja[u]) // get list of vertexes adjacent to u
#define AdjaV(adja) (*(int*)(adja))         // get number of vertexes
#define AdjaE(adja) (AdjaU(adja,1)-1)->v    // get number of edges
#define AdjaD(adja) (AdjaU(adja,1)-1)->c    // get graph-is-directed
#define AdjaPrev(adja) (adja + (AdjaD(adja) ? AdjaV(adja) : 0))


// The given graph is converted to a string which
// is put inside the given 'out' assumed to be
// large enough. The end of string is returned.
char* print_edge_list (char* out, const_EdgeP edgeList);
char* print_adja_list (char* out, const_AdjaP adjaList);

EdgeP load_edge_list (const char* str);
AdjaP load_adja_list (const char* str);

EdgeP clone_edge_list (const_EdgeP edgeList);
AdjaP clone_adja_list (const_AdjaP adjaList);

AdjaP edge_list_to_adja_list (const_EdgeP edgeList);
EdgeP adja_list_to_edge_list (const_AdjaP adjaList);

//void free_edge_list (EdgeP edgeList) { free(edgeList); }
//void free_adja_list (AdjaP adjaList) { free(adjaList); }


// code below shows how to access an adja_list
static inline Adja getEdgeInAdjaList (const_AdjaP adjaList, int u, int v)
{
	Adja U = AdjaU(adjaList,u);
	int i, m = U[0].v;      // get number of sink adjacent vertexes to u.
	for(i=1; i<=m; i++)     // for every sink adjacent vertex.
		if(v == U[i].v)     // check if it is the given vertex v.
			return U+i;     // return pointer to adjacent edge.
	return NULL;
}

// this does nothing but show how DFS traversal is coded
void DFS_traversal (const_AdjaP adjaList, int rootVertex);

// renumber source and sink vertexes separately
void renumber_edge_list_vertexes (EdgeP edgeList);


// reduce given graph to an MST
void minimum_spanning_tree (EdgeP edgeList);

// The result is the edges of the constructed tree,
// with result[i].c = total cost from given source.
EdgeP single_source_shortest_path (const_AdjaP adjaList, int source);

// return an array of single_source_shortest_paths,
// for every vertex starting from vertex 1 at index 1.
EdgeP* all_pairs_shortest_path (const_AdjaP adjaList);

// return the result graph as an unweighted undirected EdgeP
EdgeP maximum_matching_unweighted (const_AdjaP adjaList, bool skipPart1);


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
