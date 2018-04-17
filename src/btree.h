/***************************************************************
* Header file for the B-tree.
* This is a B-tree implementation with customizable 
* node size and fan out.
***************************************************************/

#ifndef __BTREE_H__
#define __BTREE_H__

#include "layout.h"

class BTree {
public:
	// Number of levels
	int numLevels;
	// The file descriptors of the node files
	int* fds;
	// The fan out of the B-tree node
	int fanOut;
	// The number of pages per node
	int nodeSize

	int search(Key key);
	void insert(Key key);
	void delete(Key key);
private:
};

#endif // __BTREE_H__
