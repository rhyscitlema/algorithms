/*
    graph.c

    Provided by Rhyscitlema
    @ http://rhyscitlema.com

    USE AT YOUR OWN RISK!
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "graph.h"
#include "heap.h"



Adja* edge_list_to_adja_list (const Edge* edge)
{
    if(!edge) return NULL;
    int V = edge[0].u;
    int E = edge[0].v;
    bool directed = (bool)edge[0].c;

    Adja *next, *prev;
    Edge *dnex, *dpre;
    int i, u, v;

    u = 1+V;     // for adjacency list pointers.
    v = 1+V+E*2; // for the data at list pointers.
    if(directed) { u+=V; v+=V; }
    i = u;

    u *= sizeof(Adja);
    v *= sizeof(Edge);
    next = (Adja*)malloc(u+v);
    dnex = (Edge*)(next+i);

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
        next[u][j].u = u;
        next[u][j].v = v;
        next[u][j].c = c;

        j = ++prev[v][0].v;
        next[v][j].u = v;
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
        while(i!=-1 && *str!=0)
        { if(*str=='\n') i--; str++; }
    }
    else return NULL;
    if(*str==0) return NULL;

    while(isSpace(*str)) str++;
    sscanf(str, "%d", &V); nextWord(str);
    sscanf(str, "%d", &E); nextWord(str);
    sscanf(str, "%d", &i); nextWord(str); directed=i;
    sscanf(str, "%d", &i); nextWord(str); weighted=i;
    sscanf(str, "%d", &i); nextWord(str);

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
    if(!edge) { *out=0; return out; }
    int V = edge[0].u;
    int E = edge[0].v;
    bool directed = (bool)edge[0].c;
    bool weighted = 0;
    int i;
    for(i=1; i<=E; i++) if(edge[i].c!=1) { weighted=1; break; }

    if(!out) return out;
    if(!edge) { *out=0; return out; }
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

    if(!out) return out;
    if(!adja) { *out=0; return out; }
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



void renumber_graph_vertexes (Edge* edge)
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

static int sssp_heap_node_compare (const void* arg, const void* a, const void* b)
{
    cost r = convert(a)->c - convert(b)->c;
    return (r<0) ? -1 : (r>0) ? +1 : 0;
}

Edge* single_source_shortest_path (const Adja* adja, int s)
{
    int V = AdjaV(adja);
    int i, u, m, positive=1;

    if(s<1 || s>V) { printf("Error: 1 <= s=%d <= V=%d.\n", s, V); return NULL; }
    Edge* path = (Edge*)malloc((1+V)*sizeof(Edge));
    for(i=1; i<=V; i++) path[i].u = 0;

    /* check if any edge cost is negative */
    for(u=1; u<=V; u++)
    {
        m = adja[u][0].v;
        for(i=1; i<=m; i++) if(adja[u][i].c<0) break;
        if(i<=m) { positive=0; break; }
    }

    /* using Dijkstra's algorithm O(ElogV) */
    if(positive)
    {
        Heap* heap = heap_build(NULL, V, 1, path, sssp_heap_node_compare);
        heap_push(heap, &path[s].v);
        path[s].u = s;
        path[s].c = 0;
        while(1)
        {
            //heap_print(heap, 4);
            void* ptr;
            if(!heap_pop(heap, &ptr)) break;
            u = convert(ptr) - path; // get vertex value

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
                    if(path[v].u==0)
                         heap_push(heap, &path[v].v);
                    else heap_update(heap, &path[v].v);
                    path[v].u = u;
                    path[v].c = c;
                }
            }
        }
        heap_free(heap);
    }

    /* using Bellman-Ford algorithm O(V^2) */
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

    bool directed = AdjaD(adja);
    int E;
    for(E=0, i=1; i<=V; i++)
        if(path[i].u != 0 && i!=s)
        {  path[i].v = i;
           path[++E] = path[i];
        }
    path[0].u = V;
    path[0].v = E;
    path[0].c = directed;
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
    int i;  // free memory needed by heap.h
    int c;  // count of free adjacent vertexes
    // i must be declared first
} mmug;

/* compare count of free adjacent vertexes */
static int mmug_heap_node_compare(const void* arg, const void* a, const void* b)
{ return ( ((const mmug*)a)->c - ((const mmug*)b)->c ); }


bool matching (int u, int V, int next[], const Adja* adja)
{
    int claim[10000] = {0};
    int queue[10000];
    int beg, end=0;
    queue[end++] = u;

    for(beg=0; beg!=end; ++beg)
    {
        u = queue[beg];
        int i, m = adja[u][0].v;
        for(i=1; i<=m; i++)
        {
            int v = adja[u][i].v;
            if(!next[v]) // if free vertex
            {
                while(1)
                {
                    int t;
                    t = next[u];
                    next[u] = v;
                    next[v] = u;
                    if(t==0) break;
                    v = t;
                    u = claim[v];
                }
                return 1;
            }
            if(!claim[v])               // if not claimed
            {   claim[v] = u;           // mark u as claiming v
                queue[end++] = next[v]; // push who monopolises v
            }
        }
    }
    return 0;
}


/* Explanation of implemented algorithm at:
   http://rhyscitlema.com/algorithms/maximum-bipartite-matching
*/
Edge* maximum_matching_unweighted(const Adja* adja)
{
    if(!adja) return NULL;
    int V = AdjaV(adja);     // get number of vertexes

    mmug* count= (mmug*)malloc((1+V)*sizeof(mmug));
    int* next  = (int *)malloc((1+V)*sizeof(int ));
    memset(next, 0, (1+V)*sizeof(int));

    int i, m, u, v;
    //const Adja* graph;
    const Adja* prev = AdjaPrev(adja);
    // adja = adjacency list of source-to-sink vertexes
    // prev = adjacency list of sink-from-source vertexes

    // initialise the heap with all vertexes
    Heap* heap = heap_build(NULL, V, 1, count, mmug_heap_node_compare);
    for(u=1; u<=V; u++)
    {
        // get m = total number of adjacent vertexes
        m = adja[u][0].v;
        if(adja!=prev)
            m += prev[u][0].v;
        count[u].c = m;
        if(m!=0) heap_push(heap, &count[u].i);
    }

    /*void* ptr;
    while(heap_pop(heap, &ptr))     // while heap is not empty
    {
        // get u = vertex with minimum free adjacent vertexes
        u = (mmug*)ptr - count;
        if(count[u].c==0) continue; // if no free adjacent vertex
        count[u].c=0;               // mark u as removed from heap

        int t=0;
        graph = adja;
        while(1)
        {
            m = graph[u][0].v;      // get number of adjacent vertexes
            for(i=1; i<=m; i++)     // for every adjacent vertex
            {
                v = graph[u][i].v;  // get adjacent vertex
                if(count[v].c)      // if is inside heap
                {  count[v].c--;    // decrement count of free vertexes
                   heap_update(heap, &count[v].i); // update position in heap
                   if(t==0 || count[t].c > count[v].c) t=v; // get free vertex
                }
            }
            if(graph==prev) break;
            else graph = prev;      // switch from adja to prev list
        }
        v=t;

        next[u] = v;    // assign free vertex
        next[v] = u;
        count[v].c = 0; // remove v from heap
        heap_remove(heap, &count[v].i);

        // now tell all adjacent to v that v is monopolised.
        u=v; // set u=v so to use exact same code as before
        graph = adja;
        while(1)
        {
            m = graph[u][0].v;      // get number of adjacent vertexes
            for(i=1; i<=m; i++)     // for every adjacent vertex
            {
                v = graph[u][i].v;  // get adjacent vertex
                if(count[v].c)      // if is inside heap
                {  count[v].c--;    // decrement count of free vertexes
                   heap_update(heap, &count[v].i); // update position in heap
                }
            }
            if(graph==prev) break;
            else graph = prev;      // switch from adja to prev list
        }
    }*/

    for(i=1; i<=V; i++) matching(i, V, next, adja);

    int E;
    // get number of edges of result graph
    for(E=0, u=1; u<=V; u++) if(next[u]) E++;

    // allocate memory for result graph
    Edge *edge = (Edge*) malloc ((1+E)*sizeof(Edge));

    // build the edge-list of the result graph
    for(E=0, u=1; u<=V; u++)
    {
        v = next[u];
        if(v<u) continue;
        E++;
        edge[E].u = u;
        edge[E].v = v;
        edge[E].c = 1;
    }
    edge[0].u = V;
    edge[0].v = E;
    edge[0].c = adja!=prev;

    // free allocated memory
    free(next);
    free(count);
    heap_free(heap);
    return edge;
}


/*********************************************************************************/


void DFS_traversal (const Adja* adja, int root)
{
    int V = *(int*)adja;
    int i, u, v, p;
    cost c;

    Edge* stack   = (Edge*)malloc((0+V)*sizeof(Edge));
    bool* visited = (bool*)malloc((1+V)*sizeof(bool));
    memset(visited, 0, (1+V)*sizeof(bool));

    // initialise stack with root node
    v = root; // get first node
    p = 0;
    stack[p].u = v;
    stack[p].v = 1;
    stack[p].c = 1;
    visited[v] = 1;

    while(p!=-1)
    {
        u = stack[p].u;
        i = stack[p].v;
        while(1)
        {
            // if no more v nodes for u
            if(i > adja[u][0].v)
            {
                if(p==0) { p--; break; }
                i = stack[p].c;
                u = stack[p-1].u;
                v = stack[p].u;
                c = getAdjaEdge(adja,u,v)->c;

                // process node below
                v = u*c*0; // nothing useful
                // process node above

                p--;
                stack[p].c += i;
                break;
            }
            v = adja[u][i++].v;
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

