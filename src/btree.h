/***************************************************************
* Header file for the B-tree.
* This is a B-tree implementation with customizable 
* node size and fan out.
***************************************************************/

#ifndef __BTREE_H__
#define __BTREE_H__

#include "layout.h"

#define KEY_FOUND 0
#define KEY_NOT_FOUND -1

class BTree {
public:
	// Number of levels in the B-tree
	int numLevels;
	// The fan out of the B-tree node
	int fanOut;
	// The number of pages per node
	int nodeSize;
	// The initial fill factor of a node. Each page of
	// the node is filled up to this factor initially.
	float fillFactor;

	int initializeEmptyTree(uint treeId, int fanOut, int nodeSize, float fillFactor);
	void search(Key key);
	void insert(Key key);
	void delete(Key key);
private:
	// The file descriptors of the node files
	int* fds;
	// Node buffer, the node is loaded into the buffer
	char* nodeBuffer;
	// Find the next node while searching the tree
	int findNextNode(Key key, char* node);
};

#endif // __BTREE_H__
