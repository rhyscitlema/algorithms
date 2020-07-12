/*
	tree.h

	Provided by Rhyscitlema
	@ http://rhyscitlema.com

	USE AT YOUR OWN RISK!
*/
#ifndef _TREE_H
#define _TREE_H


#ifndef ITEM
#define ITEM void // Tree Item
// note: any type should be fine.
#endif

typedef struct _TreeNode {
	struct _TreeNode *left, *right, *parent;
	unsigned int itemsize;
	unsigned char height;
} TreeNode;

typedef struct _Tree {  // Binary Search Tree
	TreeNode* root;     // root of tree
	long size;          // number of nodes

	unsigned int itemsize; // given in bytes
	const void* arg;
	int (*compare) (const ITEM* item1, const ITEM* item2, const void* arg);
	//void (*freenode) (ITEM* item, const void* arg);
} Tree;

static inline void tree_clear (Tree *tree)
{ if(tree) { tree->root=0; tree->size=0; } }


enum TREE_OPR {     // *** first read comment on tree_do() ***
	TREE_FIND,      // find item2 which is == given item1
	TREE_CEIL,      // find lowest item2 which is >= item1
	TREE_FLOOR,     // find highest item2 which is <= item1
	TREE_ADD,       // add if not found, return found item2 or NULL
	TREE_INS,       // add even if found, return found item2 or NULL
	TREE_PUT,       // do TREE_INS with given item obtained from tree_new()
	TREE_REM,       // find one and remove, if found return removed item2
	TREE_DEL,       // find one and delete, if found return deleted item2
};


/**
	If on first call to tree_do() then set *tree = {0}.

	'arg' is only passed to the compare() function.
	'item1' is the given item,
	'item2' is from existing node.

	Note: if the given item1 does not already exist then
	NULl is returned, else the existing item2 is returned.

	When any of itemsize, arg and compare is 0 or NULL, the
	corresponding value from the Tree-struct is used instead.
	Note: itemsize is ignored unless on TREE_ADD or TREE_INS.
*/
ITEM* tree_do (
	enum TREE_OPR OPR,
	Tree* tree,
	const ITEM* item1,
	unsigned int itemsize,
	const void* arg,
	int (*compare) (const ITEM* item1, const ITEM* item2, const void* arg));


void  tree_free  (Tree* tree);      // given tree, free all nodes
ITEM* tree_root  (Tree* tree);      // given tree, get root node
ITEM* tree_first (Tree* tree);      // given tree, get left-most node
ITEM* tree_last  (Tree* tree);      // given tree, get right-most node

ITEM* tree_next (ITEM* item);       // given node, get its next
ITEM* tree_prev (ITEM* item);       // given node, get its prev
ITEM* tree_left (ITEM* item);       // given node, get its left
ITEM* tree_right (ITEM* item);      // given node, get its right
ITEM* tree_parent (ITEM* item);     // given node, get its parent

void tree_update (Tree* tree, ITEM* item); // given node, update it
void tree_remove (Tree* tree, ITEM* item); // given node, remove it
void tree_delete (Tree* tree, ITEM* item); // given node, delete it
// if tree = NULL then node obtained from tree_new() is deleted

// get a new node, use TREE_PUT later, if(item) then copy it.
ITEM* tree_new (const ITEM* item, unsigned int itemsize);

// check the tree data structure and return bool
int tree_valid (const Tree* tree);


#endif
