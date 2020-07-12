/*
	tree.c

	Provided by Rhyscitlema
	@ http://rhyscitlema.com

	USE AT YOUR OWN RISK!
*/
#include "tree.h"
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#ifdef LIB_STD
// used only for debugging
#include <_malloc.h>
#else
#include <malloc.h>
#define _malloc(size, type) malloc(size)
#define _free(ptr, type) free(ptr)
#endif


static inline ITEM* getItem (TreeNode* node) { return node ? (ITEM*)(node+1) : NULL; }

static inline TreeNode* getNode (ITEM* item) { return item ? ((TreeNode*)item)-1 : NULL; }


ITEM* tree_left   (ITEM* item) { return item ? getItem(getNode(item)->left)   : NULL; }

ITEM* tree_right  (ITEM* item) { return item ? getItem(getNode(item)->right)  : NULL; }

ITEM* tree_parent (ITEM* item) { return item ? getItem(getNode(item)->parent) : NULL; }

ITEM* tree_root (Tree* tree) { return getItem(tree->root); }


ITEM* tree_first (Tree* tree)
{
	TreeNode* node = tree->root;
	if(node){
		while(node->left)
			node = node->left;
	} return getItem(node);
}

ITEM* tree_last (Tree* tree)
{
	TreeNode* node = tree->root;
	if(node){
		while(node->right)
			node = node->right;
	} return getItem(node);
}



ITEM* tree_next (ITEM* item)
{
	TreeNode* node = getNode(item);
	if(node)
	{
		if(node->right)
		{
			node = node->right;
			while(node->left)
				node = node->left;
		}
		else while(true)
		{
			TreeNode* t = node;
			node = node->parent;
			if(!node || node->left==t) break;
		}
	}
	return getItem(node);
}

ITEM* tree_prev (ITEM* item)
{
	TreeNode* node = getNode(item);
	if(node)
	{
		if(node->left)
		{
			node = node->left;
			while(node->right)
				node = node->right;
		}
		else while(true)
		{
			TreeNode* t = node;
			node = node->parent;
			if(!node || node->right==t) break;
		}
	}
	return getItem(node);
}



ITEM* tree_new (const ITEM* item, unsigned int itemsize)
{
	if(itemsize<=0) return NULL;
	unsigned int size = sizeof(TreeNode) + itemsize;

	TreeNode* node = (TreeNode*)_malloc(size, "TREE_NODE");
	memset(node, 0, size);
	node->itemsize = itemsize;

	ITEM* item2 = getItem(node);
	if(item && item2) memcpy(item2, item, itemsize);
	return item2;
}

static void tree_free_node (TreeNode* node)
{
	unsigned int size = sizeof(*node) + node->itemsize;
	memset(node, 0, size);
	_free(node, "TREE_NODE");
}

static void tree_free_do (TreeNode* node)
{
	if(node==NULL) return;
	tree_free_do(node->left);
	tree_free_do(node->right);
	tree_free_node(node);
}

void tree_free (Tree* tree)
{
	assert(tree!=NULL);
	tree_free_do(tree->root);
	tree_clear(tree);
}



static bool rebalance (Tree* tree, TreeNode* node)
{
	TreeNode *pnode, *tnode, *snode;
	int l = (node->left ) ? node->left->height  : 0;
	int r = (node->right) ? node->right->height : 0;
	node->height = 1 + (l>r?l:r);

	if(l-r > 1)     // if left is longer
	{
		tnode = node->left;
		snode = tnode->right;
		l = (tnode->left ) ? tnode->left->height  : 0;
		r = (tnode->right) ? tnode->right->height : 0;

		if(l>=r)    // do single rotation
		{
			node->left = snode; if(snode) snode->parent = node;
			tnode->right = node;
			tnode->parent = pnode = node->parent;
			if(pnode==NULL) tree->root = tnode;
			else { if(pnode->left==node) pnode->left = tnode; else pnode->right = tnode; }
			node->parent = tnode;
		}
		else        // do double rotation
		{
			snode->parent = pnode = node->parent;
			if(pnode==NULL) tree->root = snode;
			else { if(pnode->left==node) pnode->left = snode; else pnode->right = snode; }
			node->left   = pnode = snode->right; if(pnode) pnode->parent = node;
			tnode->right = pnode = snode->left;  if(pnode) pnode->parent = tnode;
			tnode->parent = snode;
			snode->left = tnode;
			snode->right = node;
			node->parent = snode;
			tnode->height -= 1;
			snode->height += 1;
		}
		node->height -= 2;
		return true;
	}
	if(r-l > 1)     // if right is longer
	{
		tnode = node->right;
		snode = tnode->left;
		l = (tnode->left ) ? tnode->left->height  : 0;
		r = (tnode->right) ? tnode->right->height : 0;

		if(l<=r)    // do single rotation
		{
			node->right = snode; if(snode) snode->parent = node;
			tnode->left = node;
			tnode->parent = pnode = node->parent;
			if(pnode==NULL) tree->root = tnode;
			else { if(pnode->left==node) pnode->left = tnode; else pnode->right = tnode; }
			node->parent = tnode;
		}
		else        // do double rotation
		{
			snode->parent = pnode = node->parent;
			if(pnode==NULL) tree->root = snode;
			else { if(pnode->left==node) pnode->left = snode; else pnode->right = snode; }
			node->right = pnode = snode->left;  if(pnode) pnode->parent = node;
			tnode->left = pnode = snode->right; if(pnode) pnode->parent = tnode;
			tnode->parent = snode;
			snode->right = tnode;
			snode->left = node;
			node->parent = snode;
			tnode->height -= 1;
			snode->height += 1;
		}
		node->height -= 2;
		return true;
	}
	return false;
}



ITEM* tree_do (
	enum TREE_OPR OPR,
	Tree* tree,
	const ITEM* item1,
	unsigned int itemsize,
	const void* arg,
	int (*compare) (const ITEM* item1, const ITEM* item2, const void* arg))
{
	assert(tree && (!tree->root == !tree->size));
	if(!itemsize) itemsize = tree->itemsize;
	if(!arg) arg = tree->arg;
	if(!compare) compare = tree->compare;
	if(!compare || !item1) return NULL;

	TreeNode* node = tree->root;
	if(node==NULL)
	{
		if(OPR==TREE_ADD
		|| OPR==TREE_INS
		|| OPR==TREE_PUT)
		{
			ITEM* item;
			if(OPR==TREE_PUT)
				item = (ITEM*)(size_t)item1;
			else item = tree_new(item1, itemsize);

			if(item){
				node = getNode(item);
				node->height = 1;
				tree->root = node;
				tree->size++;
			}
		}
		return NULL;
	}

	ITEM* item2=NULL;
	int i=0;
	while(true)
	{
		item2 = getItem(node);
		i = compare(item1, item2, arg);

		if(i<0) // if item1 < item2
		{
			if(node->left==NULL) break;
			else node = node->left;
		}
		else if(i>0 // if item1 > item2,
		|| OPR==TREE_INS // or do insert
		|| OPR==TREE_PUT)
		{
			if(node->right==NULL) break;
			else node = node->right;
		}
		else break;
	}

	if((OPR==TREE_ADD && i!=0)
	|| OPR==TREE_INS
	|| OPR==TREE_PUT)
	{
		ITEM* item;
		if(OPR==TREE_PUT)
			item = (ITEM*)(size_t)item1;
		else item = tree_new (item1, itemsize);

		if(item){
			tree->size++;
			TreeNode* pnode = node;

			node = getNode(item);
			node->parent = pnode;
			node->height = 1;

			if(i<0) pnode->left  = node;
			else    pnode->right = node;

			do{ if(rebalance(tree, pnode)) break;
				pnode = pnode->parent;
			} while(pnode);
		}
	}
	else if(OPR==TREE_REM) tree_remove(tree, item2);
	else if(OPR==TREE_DEL) tree_delete(tree, item2);

	else if(OPR==TREE_CEIL ) return i>0 ? tree_next(item2) : item2;
	else if(OPR==TREE_FLOOR) return i<0 ? tree_prev(item2) : item2;

	return i==0 ? item2 : NULL;
}



static void tree_delete_do (Tree* tree, ITEM* item, bool keepdata)
{
	TreeNode *node, *pnode, *tnode, *snode;
	if(item==NULL) return;
	node = getNode(item);
	if(tree)
	{
		if(node->left)
		{
			tnode = node->left;
			while(tnode->right)
				tnode = tnode->right;
			snode = tnode->left;
		}
		else if(node->right)
		{
			tnode = node->right;
			while(tnode->left)
				tnode = tnode->left;
			snode = tnode->right;
		}
		else { tnode = node; snode = NULL; }

		pnode = tnode->parent;
		if(pnode==NULL) {
			if(tree->root == node)
				tree->root = NULL;
		}
		else
		{
			if(pnode->left == tnode)
				pnode->left = snode;
			else pnode->right = snode;
			if(snode) snode->parent = pnode;

			do{ rebalance(tree, pnode);
				pnode = pnode->parent;
			} while(pnode);

			if(node!=tnode)
			{
				tnode->left  = snode = node->left;  if(snode) snode->parent = tnode;
				tnode->right = snode = node->right; if(snode) snode->parent = tnode;
				tnode->parent= pnode = node->parent;
				if(pnode==NULL) tree->root = tnode;
				else {
					if(pnode->left==node)
						pnode->left = tnode;
					else pnode->right = tnode;
				}
				tnode->height = node->height;
			}
		}
		tree->size--;
	}
	if(!keepdata) tree_free_node(node);
	else memset(node, 0, sizeof(*node));
}

void tree_remove (Tree* tree, ITEM* item) { tree_delete_do (tree, item, true ); }
void tree_delete (Tree* tree, ITEM* item) { tree_delete_do (tree, item, false); }

void tree_update (Tree* tree, ITEM* item)
{
	tree_remove(tree, item);
	tree_do(TREE_PUT, tree, item, 0,0,0);
}



static long node_valid (const TreeNode* node)
{
	assert(node!=NULL);
	if(!node) return 0;
	long l=0, r=0;
	if(node->left)
	{
		if(node->left->parent != node) return 0;
		if(!(l = node_valid(node->left))) return 0;
	}
	if(node->right)
	{
		if(node->right->parent != node) return 0;
		if(!(r = node_valid(node->right))) return 0;
		if(node->left == node->right) return 0;
	}
	return l+r+1;
}

int tree_valid (const Tree* tree)
{
	if(!tree->size && !tree->root) return true;
	if(!tree->size || !tree->root) return false;
	long size = node_valid(tree->root);
	return size == tree->size;
}

