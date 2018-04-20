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

class Node {
    void load() {
        // load content to node_content(malloc space for this)

    }

    void flush() {
        // write out the node content

    }

    void insert_record(int pos, Key key, int offset) {
        // insert a record to position (in memory)

    }
    // TODO
    /*~Node() {

    }*/ 

 public:
    int level;
    int fd;
    int offset;    //offset within the file
    char * node_content;
};

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
        std::string store_dir;

        //loadfrom();
	//flush();  ??
        int initializeEmptyTree(uint treeId, int fanOut, int nodeSize, float fillFactor);
	void search(Key key);
	void insert(Key key);
	void deletekey(Key key);
        
private:
	// The file descriptors of the node files
	int* fds;
	// Node buffer, the node is loaded into the buffer
	char* nodeBuffer;
	// Find the next node while searching the tree
	int findNextNode(Key key, char* node);
        bool searchkey(Key key, char* node);
        int btInsertInternal(Node & b, int key, int *median);
};

#endif // __BTREE_H__
