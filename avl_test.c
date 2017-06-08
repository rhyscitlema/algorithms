/*
    avl_test.c

    To compile, execute:
        gcc -Wall -pedantic avl_test.c avl.c -o avl

    To run, execute: ./avl
*/
#include <stdio.h>
#include <string.h>
#include "avl.h"


static void avl_print_tree (void* avl, int indent)
{
    if(avl==NULL) return;
    avl_print_tree(avl_right(avl), indent+4);
    int i; for(i=0; i<indent; i++) putchar(' ');
    printf("%s\n", (char*)avl);
    avl_print_tree(avl_left(avl), indent+4);
}

static int avl_compare (const void* key1, const void* key2, const void* arg)
{ return strcmp((const char*)key1, (const char*)key2); }


int main()
{
    AVLT tree = {0};
    char str[1000];
    enum AVL_OPR opr=0;

    printf("--------------------------------------------\n");
    printf("| Enter: opr=<opr> <key1> <key2> ... quit  |\n");
    printf("| <opr>: find, ceil, floor, add, ins, del. |\n");
    printf("| <key>: a string with no space.           |\n");
    printf("| Example: opr=add pen paper book          |\n");
    printf("--------------------------------------------\n");
    printf(": ");

    while(1)
    {
        scanf("%s", str);
        if(0==memcmp(str, "opr=", 4))
        {
             if(0==strcmp(str+4, "find" )) opr = AVL_FIND;
        else if(0==strcmp(str+4, "ceil" )) opr = AVL_CEIL;
        else if(0==strcmp(str+4, "floor")) opr = AVL_FLOOR;
        else if(0==strcmp(str+4, "add"  )) opr = AVL_ADD;
        else if(0==strcmp(str+4, "ins"  )) opr = AVL_INS;
        else if(0==strcmp(str+4, "del"  )) opr = AVL_DEL;
        else printf("Error: %s is not recognised\n", str);
        }
        else if(0==strcmp(str, "quit")) break;
        else
        {
            void* key2 = avl_do (opr, &tree, str, strlen(str)+1, NULL, avl_compare);

            if(key2==(void*)1) key2 = "FOUND";

            printf("key1 = %s , key2 = %s\n", str, (char*)key2);

            if((opr==AVL_ADD && !key2) || opr==AVL_INS || (opr==AVL_DEL && key2))
            {
                printf("-----------------------------\n");
                avl_print_tree(avl_root(&tree), 0);
                printf("----------------------------- size = %d\n", tree.size);
            }
        }
    }
    avl_free(&tree);
}
