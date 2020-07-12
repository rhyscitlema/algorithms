/*
    list_test.c

    To compile, execute:
        gcc -Wall -pedantic list_test.c list.c -o list

    To run, execute: ./list
*/
#include <stdio.h>
#include <string.h>
#include "list.h"


enum OPR
{   FIND,
    SORT,
    DEL,
    HEADPUSH,
    TAILPUSH,
    HEADPOP,
    TAILPOP
};

static void list_print (List* list)
{
    void* node = list_head(list);
    for( ; node!=NULL; node = list_next(node))
        printf(" %s", (char*)node);
    printf("\n----------------------------- size = %ld\n", list->size);
}

static int node_compare (const void* key1, const void* key2, const void* arg)
{ return strcmp((const char*)key1, (const char*)key2); }


int main()
{
    List list = {0};
    char str[1000];
    enum OPR opr=0;

    printf("--------------------------------------------\n");
    printf("| Enter: opr=<opr> <key> <key> ... quit    |\n");
    printf("| <opr>: find, sort, del, headpush,        |\n");
    printf("         tailpush, headpop, tailpop        |\n");
    printf("| <key>: a string with no space.           |\n");
    printf("| Example: opr=headpush pen paper book     |\n");
    printf("--------------------------------------------\n");
    printf(": ");

    while(1)
    {
        scanf("%s", str);
        if(0==memcmp(str, "opr=", 4))
        {
             if(0==strcmp(str+4, "find"    )) opr = FIND;
        else if(0==strcmp(str+4, "sort"    )) opr = SORT;
        else if(0==strcmp(str+4, "del"     )) opr = DEL;
        else if(0==strcmp(str+4, "headpush")) opr = HEADPUSH;
        else if(0==strcmp(str+4, "tailpush")) opr = TAILPUSH;
        else if(0==strcmp(str+4, "headpop" )) opr = HEADPOP;
        else if(0==strcmp(str+4, "tailpop" )) opr = TAILPOP;
        else printf("Error: %s is not recognised\n", str);
        }
        else if(0==strcmp(str, "quit")) break;
        else
        {
            int len = strlen(str)+1;
            void* key2;
            switch(opr)
            {
            case FIND: key2 = list_find(&list, NULL, node_compare, str); break;
            case SORT: list_sort(&list, NULL, node_compare); key2=str; break;
            case HEADPUSH: key2 = list_new(str, len); list_head_push(&list, key2); break;
            case TAILPUSH: key2 = list_new(str, len); list_tail_push(&list, key2); break;
            case HEADPOP: key2 = list_head_pop(&list); if(key2) list_delete(NULL, key2); break;
            case TAILPOP: key2 = list_tail_pop(&list); if(key2) list_delete(NULL, key2); break;
            case DEL: key2 = list_find(&list, NULL, node_compare, str); if(key2) list_delete(&list, key2); break;
            }
            if(opr!=FIND && key2!=NULL) list_print(&list);
            else printf("%s\n", (key2 ? "success" : "failure"));
        }
    }
    list_free(&list);
}

