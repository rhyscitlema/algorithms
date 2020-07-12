/*
	graph.c

	Provided by Rhyscitlema
	@ http://rhyscitlema.com

	USE AT YOUR OWN RISK!
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "graph.h"
#include "heap.h"



AdjaP edge_list_to_adja_list (const_EdgeP edge)
{
	if(!edge) return NULL;
	int V = edge[0].u;
	int E = edge[0].v;
	bool directed = (bool)edge[0].c;

	Adja *next, *prev;
	Adja dnex, dpre;
	int i, u, v;

	u = 1+V;     // for adjacency list pointers.
	v = 1+V+E*2; // for the data at list pointers.
	if(directed) { u+=V; v+=V; }

	next = (AdjaP)malloc(u*sizeof(Adja) + v*sizeof(Adge));

	dnex = (Adja)(next+u);
	prev = next; if(directed) prev += V;
	dpre = dnex; if(directed) dpre += V+E;

	*(int*)next = V;
	dnex->v = E;
	dnex->c = directed;

	// get ready to count adjacent vertices
	for(i=1; i<=V; i++)
	{
		dnex[i].v=0;
		dpre[i].v=0;
	}
	for(i=1; i<=E; i++)
	{
		u = edge[i].u;
		v = edge[i].v;
		dnex[u].v++;    // count adjacent vertices
		dpre[v].v++;
	}
	u=1; v=1;
	for(i=1; i<=V; i++)
	{
		next[i] = dnex+u;   // assign pointers of
		prev[i] = dpre+v;   // adjacency list
		u += 1+dnex[i].v;
		v += 1+dpre[i].v;
	}
	for(i=1; i<=V; i++)
	{
		next[i][0].v=0;
		prev[i][0].v=0;
	}
	for(i=1; i<=E; i++)
	{
		cost c;
		int j;
		u = edge[i].u;
		v = edge[i].v;
		c = edge[i].c;

		j = ++next[u][0].v;
		next[u][j].v = v;
		next[u][j].c = c;

		j = ++prev[v][0].v;
		prev[v][j].v = u;
		prev[v][j].c = c;
	}
	return next;
}



EdgeP adja_list_to_edge_list (const_AdjaP adja)
{
	if(!adja) return NULL;
	int V = AdjaV(adja);
	int E = AdjaE(adja);
	bool directed = AdjaD(adja);

	EdgeP edge = (EdgeP)malloc((1+E)*sizeof(Edge));
	edge[0].u = V;
	edge[0].v = E;
	edge[0].c = directed;

	int i, u, m;
	for(E=0, u=1; u<=V; u++)
	{
		Adja U = AdjaU(adja,u);
		m = U[0].v;
		for(i=1; i<=m; i++)
		{
			int  v = U[i].v;
			cost c = U[i].c;
			if(!directed && u>v) continue;
			E++;
			edge[E].u = u;
			edge[E].v = v;
			edge[E].c = c;
		}
	}
	return edge;
}



const char* skipComment1 (const char* str)
{
	while(true)
	{
		if(!str || *str != '#') break;
		str++;
		if(*str != '{') // if a single line comment
		{
			// skip till '\n' is found, excluding it
			while(*str && *str != '\n') str++;
			break;
		}
		// else skip till "}#" is found, including it
		char a, b;
		int level=1;
		str++;
		b = *str;
		while(true)
		{
			str++;
			if(!*str) break;
			a = b;
			b = *str;
			if(a=='#' && b=='{') level++;
			if(a=='}' && b=='#')
			{
				if(--level==0)
				{ str++; break; }
			}
		}
	}
	return str;
}

static inline bool isSpace (char c)
{ return (c==' ' || c=='\t' || c=='\r' || c=='\n' || c=='\160'); }

const char* nextWord (const char* str)
{
	while(!isSpace(*str) && *str) str = skipComment1(++str);
	while( isSpace(*str)        ) str = skipComment1(++str);
	return str;
}



EdgeP load_edge_list (const char* str)
{
	if(!str) return NULL;
	int i, V, E;
	bool directed;
	bool weighted;

	str = skipComment1(str);
	while(isSpace(*str))
		str = skipComment1(++str);

	if(!*str || sscanf(str, "%d", &V)!=1) return NULL; str=nextWord(str);
	if(!*str || sscanf(str, "%d", &E)!=1) return NULL; str=nextWord(str);
	if(!*str || sscanf(str, "%d", &i)!=1) return NULL; str=nextWord(str); directed=i;
	if(!*str || sscanf(str, "%d", &i)!=1) return NULL; str=nextWord(str); weighted=i;
	if(!*str || sscanf(str, "%d", &i)!=1) return NULL; str=nextWord(str);
	if(V<0 || E<0) return NULL;

	EdgeP edge = (EdgeP)malloc((1+E)*sizeof(Edge));
	edge[0].u = V;  // V = number of vertexes
	edge[0].v = E;  // E = number of edges
	edge[0].c = directed;

	if(i==0) // if an edge_list
	{
		for(i=1; i<=E; i++)
		{
			sscanf(str, "%d", &edge[i].u); str=nextWord(str); // get source vertex u
			sscanf(str, "%d", &edge[i].v); str=nextWord(str); // get sink vertex v

			if(weighted){
				sscanf(str, "%ld", &edge[i].c); // get edge cost c
				str=nextWord(str);
			} else edge[i].c = 1; // else mark as unweighted
		}
	}
	else if(i==1) // else an adja_list of source-to-sink vertexes
	{
		int t, u, m;
		for(E=0, t=1; t<=V; t++)
		{
			sscanf(str, "%d", &u); str=nextWord(str); // get source vertex u
			sscanf(str, "%d", &m); str=nextWord(str); // get number of adjacent
			for(i=1; i<=m; i++)
			{
				cost c=1;
				int v;
				sscanf(str, "%d", &v); // get sink vertex v
				str=nextWord(str);

				if(weighted){
					sscanf(str, "%ld", &edge[i].c); // get edge cost c
					str=nextWord(str);
				} else edge[i].c = 1; // else mark as unweighted

				if(!directed && u>v) continue; // avoid redundant edges
				E++;
				edge[E].u = u;  // record loaded edge
				edge[E].v = v;
				edge[E].c = c;
			}
		}
	}
	else assert(false && "Graph neither edge list nor adja list");
	return edge;
}

AdjaP load_adja_list (const char* str)
{
	EdgeP edge = load_edge_list (str);
	AdjaP adja = edge_list_to_adja_list (edge);
	free(edge);
	return adja;
}


#ifndef HEADER
#define graph_header "\n%d %d %d %d %d\n\n"
#else
#define graph_header
	"#{\n"
	"    This is a block comment: #{...}#\n"
	"    A single-line comment has form: #...\n"
	"    Vertex numbering starts from 1 not 0.\n"
	"    Note the content structure as described.\n"
	"}#\n"
	"\n"
	"%-5d # number of vertexes\n"
	"%-5d # number of edges\n"
	"\n"
	"%d     # 1 if graph is directed, 0 otherwise\n"
	"%d     # 1 if graph is weighted, 0 otherwise\n"
	"\n"
	"%d     # 0 if graph is given as an edge list, or,\n"
	"      # 1 if graph is given as an adjacency list\n"
	"      #      of source-to-sink vertexes.\n"
	"\n";
#endif


char* print_edge_list (char* out, const_EdgeP edge)
{
	if(!out) return out;
	if(!edge) { *out=0; return out; }

	int V = edge[0].u;
	int E = edge[0].v;
	bool directed = (bool)edge[0].c;
	bool weighted = 0;
	int i;

	// check if any edge cost is not 1
	for(i=1; i<=E; i++) if(edge[i].c!=1) { weighted=1; break; }

	out += sprintf(out, graph_header, V, E, directed, weighted, 0);

	for(i=1; i<=E; i++)
	{
		out += sprintf(out, "%d %d", edge[i].u, edge[i].v);
		if(weighted) out += sprintf(out, " %ld", edge[i].c);
		*out++ = '\n';
	}
	*out=0;
	return out;
}


char* print_adja_list (char* out, const_AdjaP adja)
{
	if(!out) return out;
	if(!adja) { *out=0; return out; }
	int V = AdjaV(adja);
	int E = AdjaE(adja);
	bool directed = AdjaD(adja);
	bool weighted = 0;
	int i, u, m;

	// check if any edge cost is not 1
	for(u=1; u<=V; u++)
	{
		Adja U = AdjaU(adja,u);
		m = U[0].v;
		for(i=1; i<=m; i++) if(U[i].c!=1) break;
		if(i<=m) { weighted=1; break; }
	}
	out += sprintf(out, graph_header, V, E, directed, weighted, 1);

	// print adjacency list of source vertexes
	for(u=1; u<=V; u++)
	{
		Adja U = AdjaU(adja,u);
		m = U[0].v;
		out += sprintf(out, "%d  %d ", u, m);
		for(i=1; i<=m; i++)
		{
			out += sprintf(out, " %d", U[i].v);
			if(weighted) out += sprintf(out, " %ld ", U[i].c);
		}
		out += sprintf(out, "\n");
	}

	if(directed){ adja+=V; // if graph is directed
	*out++ = '\n';
	// print adjacency list of sink vertexes
	for(u=1; u<=V; u++)
	{
		Adja U = AdjaU(adja,u);
		m = U[0].v;
		out += sprintf(out, "%d  %d ", u, m);
		for(i=1; i<=m; i++)
		{
			out += sprintf(out, " %d", U[i].v);
			if(weighted) out += sprintf(out, " %ld ", U[i].c);
		}
		out += sprintf(out, "\n");
	}}
	return out;
}



EdgeP clone_edge_list (const_EdgeP edge)
{
	int E = edge[0].v;
	long size = (1+E)*sizeof(Edge);
	EdgeP out = (EdgeP)malloc(size);
	memcpy(out, edge, size);
	return out;
}

//AdjaP clone_adja_list (const_AdjaP adja);



void renumber_edge_list_vertexes (EdgeP edge)
{
	int u, v, i, j=0;
	int V = edge[0].u;
	int E = edge[0].v;
	int* temp = (int*)malloc((1+V)*sizeof(int));
	memset(temp, 0, (1+V)*sizeof(int));
	for(i=1; i<=E; i++) { u = edge[i].u; if(!temp[u]) temp[u] = ++j; edge[i].u = temp[u]; }
	for(i=1; i<=E; i++) { v = edge[i].v; if(!temp[v]) temp[v] = ++j; edge[i].v = temp[v]; }
	free(temp);
}


/*********************************************************************************/


static int compare_edges (const void* a, const void* b)
{
	cost r = ((const_EdgeP)a)->c - ((const_EdgeP)b)->c;
	return (r<0) ? -1 : (r>0) ? +1 : 0;
}

void minimum_spanning_tree (EdgeP edge)
{
	int i, j;
	int u, v, k;
	int S[100]; // stack
	int V = edge[0].u;
	int E = edge[0].v;
	bool directed = (bool)edge[0].c;
	if(directed) return;

	qsort(edge+1, E, sizeof(Edge), compare_edges);

	int* parent = (int*)malloc((1+V)*sizeof(int));
	for(i=1; i<=V; i++) parent[i] = i;

	for(j=0, i=1; i<=E; i++)
	{
		u = edge[i].u;
		v = edge[i].v;

		for(k=0; u != parent[u]; u = parent[u]) S[k++]=u;
		if(k--) { while(k--) parent[S[k]] = u; }
		for(k=0; v != parent[v]; v = parent[v]) S[k++]=v;
		if(k--) { while(k--) parent[S[k]] = v; }

		if(u==v) continue; // if in same group
		if(u<v) { k=u; u=v; v=k; } // swap(u,v)
		parent[u] = v; // unify groups
		edge[++j] = edge[i];
	}
	edge[0].v = j;
	free(parent);
}


/*********************************************************************************/


#define convert(a) ((const_EdgeP)((const int*)a-1))

static int sssp_heap_node_compare (const ITEM* a, const ITEM* b, const void* arg)
{
	cost r = convert(a)->c - convert(b)->c;
	return (r<0) ? -1 : (r>0) ? +1 : 0;
}

EdgeP single_source_shortest_path (const_AdjaP adja, int s)
{
	int V = AdjaV(adja);
	int i, u, m;
	bool positive=true;

	if(s<1 || s>V) { printf("Error: 1 <= s=%d <= V=%d.\n", s, V); return NULL; }
	EdgeP path = (EdgeP)malloc((1+V)*sizeof(Edge));
	memset(path, 0, (1+V)*sizeof(Edge));
	//for(i=1; i<=V; i++) path[i].u = 0;

	/* check if any edge cost is negative */
	for(u=1; u<=V; u++)
	{
		Adja U = AdjaU(adja,u);
		m = U[0].v;
		for(i=1; i<=m; i++) if(U[i].c<0) break;
		if(i<=m) { positive=false; break; }
	}

	/* using Dijkstra's algorithm O(E log V)-time */
	if(positive)
	{
		Heap _heap = {
			.data = NULL,
			.size = 0,
			.capacity = V,
			.indexed = true,
			.arg = NULL,
			.compare = sssp_heap_node_compare
		};
		Heap *heap = &_heap;
		heap->data = (ITEM**)malloc(V*sizeof(ITEM*));

		heap_push(heap, &path[s].v);
		path[s].u = s;
		path[s].c = 0;
		while(1)
		{
			void* hnode = heap_pop(heap);
			if(!hnode) break; // if heap is empty then quit

			u = convert(hnode) - path; // get vertex value

			Adja U = AdjaU(adja,u);
			m = U[0].v;
			for(i=1; i<=m; i++)
			{
				int  v = U[i].v;
				cost c = U[i].c + path[u].c;

				if(path[v].u==0     // if a new vertex or
				|| path[v].c > c)   // if a better path
				{
					path[v].c = c;
					if(path[v].u==0)
					     heap_push  (heap, &path[v].v);
					else heap_update(heap, &path[v].v);
					path[v].u = u;
				}
			}
		}
		free(heap->data);
	}

	/* using Bellman-Ford algorithm O(V^2)-time */
	else
	{
		int* check = (int*)malloc(3*V*sizeof(int));
		int* a1 = check+V;
		int* a2 = a1+V;
		int l1=0, l2=0, count;

		for(i=0; i<V; i++) check[i]=0;
		path[s].u = s;
		path[s].c = 0;
		a1[l1++] = s;
		for(count=1; count<=V; count++)
		{
			while(l1--)
			{
				u = a1[l1];
				Adja U = AdjaU(adja,u);
				m = U[0].v;
				for(i=1; i<=m; i++)
				{
					int  v = U[i].v;
					cost c = U[i].c + path[u].c;

					if(path[v].u==0     // if a new vertex or
					|| path[v].c > c)   // if a better path
					{
						path[v].u = u;
						path[v].c = c;
						if(check[v-1]!=count)
						{
							check[v-1]=count;
							a2[l2++] = v;
						}
					}
				}
			}
			if(l2==0) break;
			// swap a1 and a2
			int* a0;
			a0=a1;
			a1=a2;
			a2=a0;
			l1=l2;
			l2=0;
		}
		free(check);
		if(count>V)
		{
			printf("Negative cycle present.\n");
			free(path);
			return NULL;
		}
	}

	int E;
	for(E=0, i=1; i<=V; i++)
		if(path[i].u != 0 && i!=s)
		{
			path[i].v = i;
			path[++E] = path[i];
		}
	path[0].u = V;
	path[0].v = E;
	path[0].c = AdjaD(adja);
	return path;
}


/*********************************************************************************/


EdgeP* all_pairs_shortest_path (const_AdjaP adja) // TODO: use relative pointers
{
	int V = AdjaV(adja);
	int i, j, k, fail=0;

	i =   (1+V)*sizeof(EdgeP);
	j = V*(1+V)*sizeof(Edge);
	EdgeP* path = (EdgeP*)malloc(i+j);
	memset(path, 0, i+j);

	EdgeP edge = (EdgeP)(path+1+V);
	for(i=1; i<=V; i++) { path[i] = edge; edge += 1+V; }
	*(int*)path = V;

	for(j=1; j<=V; j++) path[j][j].u = j;

	/* initialise matrix with edge costs */
	for(j=1; j<=V; j++)
	{
		Adja U = AdjaU(adja,j);
		k = U[0].v;
		for(i=1; i<=k; i++)
		{
			int  v = U[i].v;
			cost c = U[i].c;
			path[j][v].u = j;
			path[j][v].c = c;
		}
	}

	/* using Floyd-Warshall algorithm O(V^3) */
	for(k=1; k<=V; k++)
	for(i=1; i<=V; i++)
	for(j=1; j<=V; j++)
	{
		if( path[i][k].u!=0 && path[k][j].u!=0 && (      // if cost is not infinity,
		    path[i][j].u==0 ||                           // if cost is infinity, or
		    path[i][j].c > path[i][k].c + path[k][j].c)) // if path is better
		{
			path[i][j].c = path[i][k].c + path[k][j].c;
			path[i][j].u = path[k][j].u;
		}
		if(i==j && path[i][j].c<0) fail=1;
	}

	if(fail){
		printf("Negative cycle present.\n");
		free(path); return NULL;
	}

	int E = AdjaE(adja);
	bool directed = AdjaD(adja);
	for(j=1; j<=V; j++)
	{
		for(E=0, i=1; i<=V; i++)
			if(path[j][i].u != 0 && i!=j)
			{
				path[j][i].v = i;
				path[j][++E] = path[j][i];
			}
		path[j][0].u = V;
		path[j][0].v = E;
		path[j][0].c = directed;
	}
	return path;
}


/*********************************************************************************/


typedef struct {
	int i;  // the free-memory needed by heap.h
	int c;  // count of free adjacent vertexes
	// i must be declared first so to have:
	// vertex = (mmug*)heap_node - mmug_array;
} mmug;

/* compare count of free adjacent vertexes */
static int mmug_heap_node_compare (const ITEM* a, const ITEM* b, const void* arg)
{ return ( ((const mmug*)a)->c - ((const mmug*)b)->c ); }


/* See http://rhyscitlema.com/algorithms/maximum-bipartite-matching */
EdgeP maximum_matching_unweighted (const_AdjaP adja, bool skipPart1)
{
	if(!adja) return NULL;
	int V = AdjaV(adja); // get V = number of vertexes
	int i, m, u, v;

	const_AdjaP graph;
	const_AdjaP prev = AdjaPrev(adja);
	// adja = adjacency list of source-to-sink vertexes
	// prev = adjacency list of sink-from-source vertexes

	m = (1+V)*(sizeof(int)+sizeof(mmug)+sizeof(void*));
	int* match = (int*)malloc(m);   // m = total memory needed
	mmug* count = (mmug*)(match+1+V);
	ITEM** hdata = (ITEM**)(count+1+V);

	if(match==NULL) return NULL;    // if failed to allocate memory
	for(u=1; u<=V; u++) match[u]=0; // else initialise main array


	//*****************************************************************
	//*** Part 1 of algorithm : is often enough to solve optimally! ***
	//*****************************************************************
	if(!skipPart1) {

	// initialise the heap with all vertexes
	Heap _heap = {
		.data = hdata,
		.size = 0,
		.capacity = V,
		.indexed = true,
		.arg = NULL,
		.compare = mmug_heap_node_compare
	};
	Heap *heap = &_heap;
	for(u=1; u<=V; u++)
	{
		// get m = total number of adjacent vertexes
		m = AdjaU(adja,u)[0].v;
		if(adja!=prev) // if a directed graph
			m += AdjaU(prev,u)[0].v;
		count[u].c = m;
		if(m) heap_push(heap, &count[u].i);
	}

	while(1)
	{
		void* hnode = heap_pop(heap);
		if(!hnode) break; // if heap is empty then quit

		// get u = vertex with minimum free adjacent vertexes
		u = (mmug*)hnode - count;
		if(count[u].c==0) continue; // if no free adjacent vertex then skip
		count[u].c=0;               // else mark u as removed from heap

		int t=0;
		graph = adja;               // start with 'adja' list
		while(true)
		{
			Adja U = AdjaU(graph,u);
			m = U[0].v;             // get number of adjacent vertexes
			for(i=1; i<=m; i++)     // for every adjacent vertex
			{
				v = U[i].v;         // get adjacent vertex
				if(count[v].c > 0)  // if is inside heap
				{
					count[v].c--;    // decrement count of free adjacent vertexes
					heap_update(heap, &count[v].i); // update position in heap

					// get free vertex of minimum count of free adjacent vertexes
					if(t==0 || count[t].c > count[v].c) t=v;
				}
			}
			if(graph==prev) break;
			else graph = prev;      // switch from 'adja' to 'prev' list
		}
		v=t;    // get v = new free vertex

		match[u] = v;   // match free vertex
		match[v] = u;
		count[v].c = 0; // remove v from heap
		heap_remove(heap, &count[v].i);

		// now tell all adjacent to v that v is monopolised.
		u=v; // set u=v so to use exact same code as before!!!

		graph = adja;               // start with 'adja' list
		while(true)
		{
			Adja U = AdjaU(graph,u);
			m = U[0].v;             // get number of adjacent vertexes
			for(i=1; i<=m; i++)     // for every adjacent vertex
			{
				v = U[i].v;         // get adjacent vertex
				if(count[v].c > 0)  // if is inside heap
				{
					count[v].c--;    // decrement count of free adjacent vertexes
					heap_update(heap, &count[v].i); // update position in heap
				}
			}
			if(graph==prev) break;
			else graph = prev;      // switch from 'adja' to 'prev' list
		}
	}
	}


	//****************************************************************
	//*** Part 2 of algorithm : may be faster when executed alone! ***
	//****************************************************************

	int E; // number of edges of the result graph
	while(true)
	{
		int beg=0, end=0;
		int* queue = match+(1+V)*1; // queue used by the breadth-first search.
		int* claim = match+(1+V)*2; // claim[v] is the claimer of the claimed v.
		int* vfree = match+(1+V)*3; // vfree[v] is the free vertex to start claiming

		for(u=1; u<=V; u++) // initialisations, push all unmatched vertexes to queue
		{
			if(match[u]) // if u is a matched vertex
			     { claim[u] = vfree[u] = 0; }
			else { claim[u] = vfree[u] = queue[end++] = u; }
		}
		E = (V-end)/2; // count of matched vertex pairs
		//printf("-------------------------------------- E=%d\n", E);

		for( ; beg!=end; beg++)     // do the queue-based backtracking-search
		{
			u = queue[beg];
			v = vfree[u];           // get v = free vertex which u is derived from
			assert(v>0);
			if(match[v]) continue;  // check whether this v is still unmatched

			graph = adja;           // start with 'adja' list
			while(true)
			{
				Adja U = AdjaU(graph,u);
				m = U[0].v;  // get number of adjacent vertexes
				for(i=1; i<=m; i++) // for every adjacent vertex
				{
					v = U[i].v;  // get adjacent vertex
					int t = vfree[v];   // get t = free vertex which v is derived from

					// check whether a new re-matching has been uncovered
					if(t && !match[t] && t != vfree[u])
					{
						claim[v] = u;       // ensure v is never claimed again
						int tv = match[v];  // prepare for 2nd re-matching
						while(true)
						{
							claim[u] = u;   // ensure u is never claimed again
							t = match[u];   // prepare for next iteration
							match[u] = v;
							match[v] = u;   // uncover the augmenting path
							if(t==0)
							{
								if(tv==0) break; // if no further re-matching
								t=tv; tv=0;      // prevent any further re-matching
							}
							v = t;          // get v = the next claimed vertex
							u = claim[t];   // get u = the claimer to this v
						}
						E=-1; // notify that a re-matching process was performed
						break;
					}
					if(!claim[v])   // if not yet claimed
					{
						claim[v] = u;       // Mark u as claiming v
						t = match[v];       // Get who monopolises v
						vfree[t] = vfree[u];
						queue[end++] = t;   // Push it to queue
					}
				}
				if(i<=m) break; // if re-matching was performed then break

				if(graph==prev) break;
				else graph = prev;  // switch from 'adja' to 'prev' list
			}
		}
		if(E!=-1) break; // no re-matching performed means solution is optimal
	}

	// allocate memory for the result graph
	EdgeP edge = (EdgeP) malloc ((1+E)*sizeof(Edge));

	// build the edge-list of the result graph
	for(E=0, u=1; u<=V; u++)
	{
		v = match[u];
		if(v<u) continue; // avoid repetitions
		++E;
		edge[E].u = u;
		edge[E].v = v;
		edge[E].c = 1; // set weight to = 1
	}
	edge[0].u = V;
	edge[0].v = E;
	edge[0].c = 0; // mark as undirected

	// free allocated memory
	free(match);
	return edge;
}


/*********************************************************************************/


void DFS_traversal (const_AdjaP adja, int rootVertex)
{
	int V = AdjaV(adja);
	int i, u, v, p;
	cost c;

	EdgeP stack   = (EdgeP)malloc((0+V)*sizeof(Edge));
	bool* visited = (bool*)malloc((1+V)*sizeof(bool));
	memset(visited, 0, (1+V)*sizeof(bool));

	// initialise stack with root vertex
	v = rootVertex;
	p = 0;
	stack[p].u = v;
	stack[p].v = 1;
	stack[p].c = 1;
	visited[v] = 1;

	while(p!=-1)
	{
		u = stack[p].u;
		i = stack[p].v;
		while(true)
		{
			Adja U = AdjaU(adja,u);
			// if no more v nodes for u
			if(i > U[0].v)
			{
				if(p==0) { p--; break; }
				i = stack[p].c;
				u = stack[p-1].u;
				v = stack[p].u;
				c = getEdgeInAdjaList(adja,u,v)->c;

				// process node below
				v = u*c*0; // nothing useful
				// process node above

				p--;
				stack[p].c += i;
				break;
			}
			v = U[i++].v;
			if(!visited[v])
			{
				// first record position of next v
				stack[p].v = i;
				p++;
				stack[p].u = v;
				stack[p].v = 1;
				stack[p].c = 1;
				visited[v] = 1;
				break;
			}
		}
	}
	free(visited);
	free(stack);
}


/*********************************************************************************/


int binary_search (
	int (*compare)(const void* item, const void* array, int index),
	const void* item, const void* array, int low, int high)
{
	while(low<=high)
	{
		int middle = (low+high)/2;
		int i = compare(item, array, middle);
		if(i<0) high = middle-1;
		else if(i>0) low = middle+1;
		else return middle;
	}
	return -1;
}


/*********************************************************************************/

