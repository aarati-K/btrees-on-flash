/***************************************************************
* Header file for the B-tree.
* This is a B-tree implementation with customizable
* node size and fan out.
***************************************************************/

#ifndef __BTREE_H__
#define __BTREE_H__

#include <stdbool.h>
#include "layout.h"

class Node
{
public:
	// load content to node_content
    void load();
    // write out the node content
    void flush();
    // insert a record to position (in memory)
    void insert_record(int pos, Key key, int offset);
    // TODO
    /*~Node() {}*/
    int level;
    int fd;
    int offset;	// page offset within the file
    int size;	// in number of pages
    char* node_content;
    NodeSummary* nodeSummary;
};

class BTree
{
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
	std::string store_dir;

	int initializeEmptyTree(int treeId, int fanOut, int nodeSize, float fillFactor);

	void searchKey(Key key);
	void insertKey(Key key);
	void deleteKey(Key key);

    //loadfrom();
	//flush();  ??
private:
	// The file descriptors of the level files
	int* fds;
	// Node buffer, the node is loaded into the buffer
	char* nodeBuffer;
	// Find the next node while searching the tree
	int findNextNode(Key key, char* node);
    bool searchKey(Key key, char* node);
    Node btInsertInternal(Node & b, int key, int *median);
};

#endif // __BTREE_H__

/* vim: set filetype=cpp: */
