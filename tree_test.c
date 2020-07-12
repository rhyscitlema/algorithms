/*
	tree_test.c

	To compile, execute:
		gcc -Wall -pedantic tree_test.c tree.c -o tree

	To run, execute: ./tree
*/
#include <stdio.h>
#include <string.h>
#include "tree.h"


static void tree_print_tree (ITEM* item, int indent)
{
	if(item==NULL) return;
	tree_print_tree(tree_right(item), indent+4);

	int i;
	for(i=0; i<indent; i++)
		putchar(' ');
	printf("%s\n", (char*)item);

	tree_print_tree(tree_left(item), indent+4);
}

static int tree_compare (const ITEM* item1, const ITEM* item2, const void* arg)
{ return strcmp( (const char*)item1, (const char*)item2 ); }


int main()
{
	Tree tree = {0};
	char str[1000];
	enum TREE_OPR opr=0;

	printf("--------------------------------------------\n");
	printf("| Enter: opr=<opr> <item> <item> ... quit  |\n");
	printf("| <opr>: find, ceil, floor, add, ins, del. |\n");
	printf("| <item>: a string with no space.          |\n");
	printf("| Example: opr=add pen paper book          |\n");
	printf("--------------------------------------------\n");
	printf(": ");

	while(1)
	{
		scanf("%s", str);
		if(0==memcmp(str, "opr=", 4))
		{
		     if(0==strcmp(str+4, "find" )) opr = TREE_FIND;
		else if(0==strcmp(str+4, "ceil" )) opr = TREE_CEIL;
		else if(0==strcmp(str+4, "floor")) opr = TREE_FLOOR;
		else if(0==strcmp(str+4, "add"  )) opr = TREE_ADD;
		else if(0==strcmp(str+4, "ins"  )) opr = TREE_INS;
		else if(0==strcmp(str+4, "del"  )) opr = TREE_DEL;
		else printf("Error: %s is not recognised\n", str);
		}
		else if(0==strcmp(str, "quit")) break;
		else
		{
			ITEM* item2 = tree_do (opr, &tree, str, strlen(str)+1, NULL, tree_compare);

			if(item2 && opr==TREE_DEL) item2 = "FOUND";

			printf("item1 = %s , item2 = %s\n", str, (char*)item2);

			if( opr==TREE_INS
			|| (opr==TREE_ADD && !item2)
			|| (opr==TREE_DEL &&  item2))
			{
				printf("-----------------------------\n");
				tree_print_tree(tree_root(&tree), 0);
				printf("----------------------------- size = %ld\n", tree.size);
			}
			if(!tree_valid(&tree))
				printf("Tree data structure is invalid.\n");
		}
	}
	tree_free(&tree);
}
