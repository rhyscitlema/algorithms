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



Adja* edge_list_to_adja_list (const Edge* edge)
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

    next = (Adja*)malloc(u*sizeof(Adja) + v*sizeof(Edge));

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



Edge* adja_list_to_edge_list (const Adja* adja)
{
    if(!adja) return NULL;
    int V = AdjaV(adja);
    int E = AdjaE(adja);
    bool directed = AdjaD(adja);

    Edge* edge = (Edge*)malloc((1+E)*sizeof(Edge));
    edge[0].u = V;
    edge[0].v = E;
    edge[0].c = directed;

    int i, u, m;
    for(E=0, u=1; u<=V; u++)
    {
        m = adja[u][0].v;
        for(i=1; i<=m; i++)
        {
            int  v = adja[u][i].v;
            cost c = adja[u][i].c;
            if(!directed && u>v) continue;
            E++;
            edge[E].u = u;
            edge[E].v = v;
            edge[E].c = c;
        }
    }
    return edge;
}



#define isSpace(c) (c==' ' || c=='\t' || c=='\r' || c=='\n')
#define nextWord(str) { while(!isSpace(*str) && *str) str++; \
                        while( isSpace(*str)) str++; }

Edge* load_edge_list (const char* str)
{
    if(!str) return NULL;
    int i, V, E;
    bool directed;
    bool weighted;

    while(isSpace(*str)) str++;
    if(0<=*str && *str<='9')
    {
        sscanf(str, "%d", &i);
        while(i>=0 && *str!=0)
        { if(*str=='\n') i--; str++; }
    }
    else return NULL;
    if(*str==0) return NULL;

    while(isSpace(*str)) str++;
    if(sscanf(str, "%d", &V)!=1) return NULL; nextWord(str);
    if(sscanf(str, "%d", &E)!=1) return NULL; nextWord(str);
    if(sscanf(str, "%d", &i)!=1) return NULL; nextWord(str); directed=i;
    if(sscanf(str, "%d", &i)!=1) return NULL; nextWord(str); weighted=i;
    if(sscanf(str, "%d", &i)!=1) return NULL; nextWord(str);

    Edge* edge = (Edge*)malloc((1+E)*sizeof(Edge));
    edge[0].u = V;
    edge[0].v = E;
    edge[0].c = directed;

    if(i==0) // if an edge_list
    {
        for(i=1; i<=E; i++)
        {
            sscanf(str, "%d %d", &edge[i].u, &edge[i].v);
            nextWord(str); nextWord(str);
            if(weighted) { sscanf(str, "%ld", &edge[i].c); nextWord(str); }
            else edge[i].c = 1;
        }
    }
    else // else an adja_list
    {
        int t, u, m;
        for(E=0, t=1; t<=V; t++)
        {
            sscanf(str, "%d %d", &u, &m);
            nextWord(str); nextWord(str);
            for(i=1; i<=m; i++)
            {
                cost c=1;
                int v;
                sscanf(str, "%d", &v);
                nextWord(str);
                if(weighted) { sscanf(str, "%ld", &c); nextWord(str); }

                if(!directed && u>v) continue;
                E++;
                edge[E].u = u;
                edge[E].v = v;
                edge[E].c = c;
            }
        }
    }
    return edge;
}

Adja* load_adja_list (const char* str)
{
    Edge* edge = load_edge_list (str);
    Adja* adja = edge_list_to_adja_list (edge);
    free(edge);
    return adja;
}



char* print_edge_list (char* out, const Edge* edge)
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
    char* str = out;
    str += sprintf(str, "0\n%d %d %d %d 0\n\n", V, E, directed, weighted);

    for(i=1; i<=E; i++)
    {
        str += sprintf(str, "%d %d", edge[i].u, edge[i].v);
        if(weighted) str += sprintf(str, " %ld", edge[i].c);
        *str++ = '\n';
    }
    *str=0;
    return out;
}



char* print_adja_list (char* out, const Adja* adja)
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
        m = adja[u][0].v;
        for(i=1; i<=m; i++) if(adja[u][i].c!=1) break;
        if(i<=m) { weighted=1; break; }
    }
    char* str = out;
    str += sprintf(str, "0\n%d %d %d %d 1\n\n", V, E, directed, weighted);

    // print adjacency list of source vertexes
    for(u=1; u<=V; u++)
    {
        m = adja[u][0].v;
        str += sprintf(str, "%d  %d ", u, m);
        for(i=1; i<=m; i++)
        {
            str += sprintf(str, " %d", adja[u][i].v);
            if(weighted) str += sprintf(str, " %ld ", adja[u][i].c);
        }
        str += sprintf(str, "\n");
    }

    if(directed){ adja+=V; // if graph is directed
    *str++ = '\n';
    // print adjacency list of sink vertexes
    for(u=1; u<=V; u++)
    {
        m = adja[u][0].v;
        str += sprintf(str, "%d  %d ", u, m);
        for(i=1; i<=m; i++)
        {
            str += sprintf(str, " %d", adja[u][i].v);
            if(weighted) str += sprintf(str, " %ld ", adja[u][i].c);
        }
        str += sprintf(str, "\n");
    }}
    return out;
}



Edge* clone_edge_list (const Edge* edge)
{
    int E = edge[0].v;
    long size = (1+E)*sizeof(Edge);
    Edge* out = (Edge*)malloc(size);
    memcpy(out, edge, size);
    return out;
}

//Adja* clone_adja_list (const Adja* adja);



void renumber_edge_list_vertexes (Edge* edge)
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
    cost r = ((const Edge*)a)->c - ((const Edge*)b)->c;
    return (r<0) ? -1 : (r>0) ? +1 : 0;
}

void minimum_spanning_tree (Edge* edge)
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


#define convert(a) ((const Edge*)((const int*)a-1))

static int sssp_heap_node_compare (const void* a, const void* b, const void* arg)
{
    cost r = convert(a)->c - convert(b)->c;
    return (r<0) ? -1 : (r>0) ? +1 : 0;
}

Edge* single_source_shortest_path (const Adja* adja, int s)
{
    int V = AdjaV(adja);
    int i, u, m;
    bool positive=true;

    if(s<1 || s>V) { printf("Error: 1 <= s=%d <= V=%d.\n", s, V); return NULL; }
    Edge* path = (Edge*)malloc((1+V)*sizeof(Edge));
    memset(path, 0, (1+V)*sizeof(Edge));
    //for(i=1; i<=V; i++) path[i].u = 0;

    /* check if any edge cost is negative */
    for(u=1; u<=V; u++)
    {
        m = adja[u][0].v;
        for(i=1; i<=m; i++) if(adja[u][i].c<0) break;
        if(i<=m) { positive=false; break; }
    }

    /* using Dijkstra's algorithm O(E log V)-time */
    if(positive)
    {
        Heap _heap = { NULL, 0, V, true, NULL, sssp_heap_node_compare };
        Heap *heap = &_heap;
        heap->data = (void**)malloc(V*sizeof(void*));

        heap_push(heap, &path[s].v);
        path[s].u = s;
        path[s].c = 0;
        while(1)
        {
            void* ptr;
            if(!heap_pop(heap, &ptr)) break;
            u = convert(ptr) - path; // get vertex value

            m = adja[u][0].v;
            for(i=1; i<=m; i++)
            {
                int  v = adja[u][i].v;
                cost c = adja[u][i].c + path[u].c; 

                if(path[v].u==0     // if a new vertex or
                || path[v].c > c)   // if a better path
                {
                    path[v].c = c;
                    if(path[v].u==0)
                         heap_push(heap, &path[v].v);
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
                m = adja[u][0].v;
                for(i=1; i<=m; i++)
                {
                    cost c;
                    int v;
                    v = adja[u][i].v;
                    c = adja[u][i].c + path[u].c; 

                    if(path[v].u==0     // if a new vertex or
                    || path[v].c > c)   // if a better path
                    {
                        path[v].u = u;
                        path[v].c = c;
                        if(check[v-1]!=count)
                        {   check[v-1]=count;
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
        {   printf("Negative cycle present.\n");
            free(path);
            return NULL;
        }
    }

    int E;
    for(E=0, i=1; i<=V; i++)
        if(path[i].u != 0 && i!=s)
        {  path[i].v = i;
           path[++E] = path[i];
        }
    path[0].u = V;
    path[0].v = E;
    path[0].c = AdjaD(adja);
    return path;
}


/*********************************************************************************/


Edge** all_pairs_shortest_path (const Adja* adja)
{
    int V = AdjaV(adja);
    int i, j, k, fail=0;

    i =   (1+V)*sizeof(Edge*);
    j = V*(1+V)*sizeof(Edge);
    Edge** path = (Edge**)malloc(i+j);
    memset(path, 0, i+j);

    Edge* edge = (Edge*)(path+1+V);
    for(i=1; i<=V; i++) { path[i] = edge; edge += 1+V; }
    *(int*)path = V;

    for(j=1; j<=V; j++) path[j][j].u = j;

    /* initialise matrix with edge costs */
    for(j=1; j<=V; j++)
    {
        k = adja[j][0].v;
        for(i=1; i<=k; i++)
        {
            int  v = adja[j][i].v;
            cost c = adja[j][i].c;
            path[j][v].u = j;
            path[j][v].c = c;
        }
    }

    /* using Floyd-Warshall algorithm O(V^3) */
    for(k=1; k<=V; k++)
    for(i=1; i<=V; i++)
    for(j=1; j<=V; j++)
    {
        if( path[i][k].u!=0 && path[k][j].u!=0 &&        // if cost is not infinity,
           (path[i][j].u==0 ||                           // if cost is infinity, or
            path[i][j].c > path[i][k].c + path[k][j].c)) // if path is better
        {
            path[i][j].c = path[i][k].c + path[k][j].c;
            path[i][j].u = path[k][j].u;
        }
        if(i==j && path[i][j].c<0) fail=1;
    }
    if(fail)
    {   printf("Negative cycle present.\n");
        free(path); return NULL;
    }

    int E = AdjaE(adja);
    bool directed = AdjaD(adja);
    for(j=1; j<=V; j++)
    {
        for(E=0, i=1; i<=V; i++)
            if(path[j][i].u != 0 && i!=j)
            {  path[j][i].v = i;
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
static int mmug_heap_node_compare (const void* a, const void* b, const void* arg)
{ return ( ((const mmug*)a)->c - ((const mmug*)b)->c ); }


/* Explanation of implemented algorithm at:
   http://rhyscitlema.com/algorithms/maximum-bipartite-matching
*/
Edge* maximum_matching_unweighted (const Adja* adja, bool skipPart1)
{
    if(!adja) return NULL;
    int V = AdjaV(adja); // get V = number of vertexes
    int i, m, u, v;

    const Adja* graph;
    const Adja* prev = AdjaPrev(adja);
    // adja = adjacency list of source-to-sink vertexes
    // prev = adjacency list of sink-from-source vertexes

    m = (1+V)*(sizeof(int)+sizeof(mmug)+sizeof(void*));
    int* match = (int*)malloc(m);   // m = total memory needed
    mmug* count = (mmug*)(match+1+V);
    void** hdata = (void**)(count+1+V);

    if(match==NULL) return NULL;    // if failed to allocate memory
    for(u=1; u<=V; u++) match[u]=0; // else initialise main array


    //*****************************************************************
    //*** Part 1 of algorithm : is often enough to solve optimally! ***
    //*****************************************************************
    if(!skipPart1) {

    // initialise the heap with all vertexes
    Heap _heap = { hdata, 0, V, true, NULL, mmug_heap_node_compare };
    Heap *heap = &_heap;
    for(u=1; u<=V; u++)
    {
        // get m = total number of adjacent vertexes
        m = adja[u][0].v;
        if(adja!=prev) // if a directed graph
            m += prev[u][0].v;
        count[u].c = m;
        if(m) heap_push(heap, &count[u].i);
    }

    void* heapNode;
    while(heap_pop(heap, &heapNode)) // while heap is not empty
    {
        // get u = vertex with minimum free adjacent vertexes
        u = (mmug*)heapNode - count;
        if(count[u].c==0) continue; // if no free adjacent vertex then skip
        count[u].c=0;               // else mark u as removed from heap

        int t=0;
        graph = adja;               // start with 'adja' list
        while(true)
        {
            m = graph[u][0].v;      // get number of adjacent vertexes
            for(i=1; i<=m; i++)     // for every adjacent vertex
            {
                v = graph[u][i].v;  // get adjacent vertex
                if(count[v].c>0)    // if is inside heap
                {  count[v].c--;    // decrement count of free adjacent vertexes
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
            m = graph[u][0].v;      // get number of adjacent vertexes
            for(i=1; i<=m; i++)     // for every adjacent vertex
            {
                v = graph[u][i].v;  // get adjacent vertex
                if(count[v].c>0)    // if is inside heap
                {  count[v].c--;    // decrement count of free adjacent vertexes
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
                m = graph[u][0].v;  // get number of adjacent vertexes
                for(i=1; i<=m; i++) // for every adjacent vertex
                {
                    v = graph[u][i].v;  // get adjacent vertex
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
                            {   if(tv==0) break; // if no further re-matching
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
    Edge *edge = (Edge*) malloc ((1+E)*sizeof(Edge));

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


void DFS_traversal (const Adja* adjaList, int rootVertex)
{
    int V = AdjaV(adjaList);
    int i, u, v, p;
    cost c;

    Edge* stack   = (Edge*)malloc((0+V)*sizeof(Edge));
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
            // if no more v nodes for u
            if(i > adjaList[u][0].v)
            {
                if(p==0) { p--; break; }
                i = stack[p].c;
                u = stack[p-1].u;
                v = stack[p].u;
                c = getEdgeOfAdjaList(adjaList,u,v)->c;

                // process node below
                v = u*c*0; // nothing useful
                // process node above

                p--;
                stack[p].c += i;
                break;
            }
            v = adjaList[u][i++].v;
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
    int (*compare)(const void* key, const void* array, int index),
    const void* key, const void* array, int low, int high)
{
    while(low<=high)
    {
        int middle = (low+high)/2;
        int i = compare(key, array, middle);
        if(i<0) high = middle-1;
        else if(i>0) low = middle+1;
        else return middle;
    }
    return -1;
}


/*********************************************************************************/

