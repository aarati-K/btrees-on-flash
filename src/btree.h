/***************************************************************
* Header file for the B-tree.
* This is a B-tree implementation with customizable
* node size and fan out.
***************************************************************/

#ifndef __BTREE_H__
#define __BTREE_H__

#include <stdbool.h>
#include "layout.h"
#include <deque>
#include <iostream>

class Node
{
public:
    ~Node() {
        /*if (node_content != NULL)
            free(node_content);
        */ 
    }
	// Initialize the node
	void initialize(int level, int fd, int offset, int size);
	// load content to node_content
	void load();
	// write out the node content
	void flush();
	// insert a record to position (in memory)
	void insert_record(int pos, Key key, int offset);
	// Get the record at the position
	Record* getRecord(int pos);
        

        // print
        void dump() {
            // print node summary
            std::cout << "=== dumping a node: " << std::endl;
            std::cout << "number of records: " << this->summary->numRecords << std::endl;
            // print all valid records
            for (int i = 0; i < this->summary->numRecords; i++)
                std::cout << this->records[i].key << std::endl;
        }

        bool valid = false;   // whether it is useful or empty
        int level = -1;
        int fd = -1;
        int offset = -1;	// page offset within the file
	int size = -1;	// in number of pages
	char* node_content = NULL;
	NodeSummary* summary = NULL;
	Record* records = NULL;

private:
	bool memoryAllocated = false;
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

	int searchKey(Key key);
	void insertKey(Key key);
	void deleteKey(Key key);

	// Write and load map of levels to filenames
	void writeMap();
	void loadMap();

private:
	// The file descriptors of the level files
	int* fds;
	// ?
	std::deque<int> fd_q;
	// Node buffer, the node is loaded into the buffer
	char* nodeBuffer;
	// Find the next node while searching the tree
        int create_new_file();
	int findPositionInNode(Key key, Node* node);
	bool searchKey(Key key, char* node);
	Node btInsertInternal(Node & b, int key, int *median);
};

#endif // __BTREE_H__

// vim:sw=4:ts=4
