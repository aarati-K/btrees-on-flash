#include <assert.h>
#include "btree.h"
#include "layout.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
using namespace std;

int BTree::create_new_file() {
   int fd = open((store_dir + to_string(numLevels)).c_str(), O_RDWR | O_CREAT); 
   this->fd_q.push_front(fd);
   return fd;
}

void Node::initialize(int level, int fd, int offset, int size) {
	this->level = level;
	this->fd = fd;
	this->offset = offset;
	this->size = size;
	if (!this->memoryAllocated) {
		this->node_content = (char*)malloc(sizeof(char) * this->size * PAGE_SIZE);
		this->memoryAllocated = true;
	}
}

void Node::load() {
	if (!this->memoryAllocated) {
		this->node_content = (char*)malloc(sizeof(char) * this->size * PAGE_SIZE);
		this->memoryAllocated = true;
	}
	lseek(this->fd, this->offset * PAGE_SIZE, SEEK_SET);
	read(fd, this->node_content, this->size * PAGE_SIZE);
	this->summary = NODE_SUMMARY(this->node_content);
	this->records = NODE_RECORDS(this->node_content);
}

void Node::flush() {
	if (!this->summary->isDirty) {
		return;
	}
	// Set this to false before writing
	this->summary->isDirty = false;
	lseek(this->fd, this->offset * PAGE_SIZE, SEEK_SET);
	write(fd, this->node_content, this->size * PAGE_SIZE);
}

void Node::insert_record(int pos, Key key, int offset) {
	int i;
	Record tempRecord, prevRecord;

	// Initially set the previous record to temp record for copying
	prevRecord.key = key;
	prevRecord.offset = offset;

	Record** nodeRecords = this->records;
	int numRecords = this->summary->numRecords;
	assert(pos <= numRecords);
	// The array one right starting at pos
	for (i=pos; i <= numRecords; i++) {
		tempRecord.key = nodeRecords[i]->key;
		tempRecord.offset = nodeRecords[i]->offset;
		nodeRecords[i]->key = prevRecord.key;
		nodeRecords[i]->offset = prevRecord.offset;
		prevRecord.key = tempRecord.key;
		prevRecord.offset = tempRecord.offset;
	}

	// Update the node summary
	if (pos == 0) {
		// the new key is the lowest
		this->summary->lowKey = key;
	} else if (pos == this->summary->numRecords) {
		// the new key is the highest
		this->summary->highKey = key;
	}
	this->summary->numRecords = this->summary->numRecords + 1;
	this->summary->isDirty = true;
}

Record* Node::getRecord(int pos) {
	assert(pos < this->summary->numRecords);
	return (this->records[pos]);
}

int
BTree::initializeEmptyTree(int treeId, int fanOut, int nodeSize, float fillFactor=0.5)
{
	DEBUG("initializeEmptyTree()");

	assert(fanOut > 1 && nodeSize > 0 && fillFactor >= 0.5);
	this->fanOut = fanOut;
	this->nodeSize = nodeSize;
	this->fillFactor = fillFactor;

	store_dir =  "/users/kanwu/data/";
	//cout << "Creating tree id: " << treeId << " root file location: " << rootFileName << endl;

    this->fds = (int *) malloc(sizeof(int));
    this->numLevels = 0; // Just the root

	// Initialize node buffer
	this->nodeBuffer = (char *) malloc(sizeof(char) * PAGE_SIZE * this->nodeSize);
	return 0;
}

int
BTree::searchKey(Key key)
{
	DEBUG("searchKey()");

	int i, recordPosition;
	Record* record;
	Node node;
	// Offset into the next level's node file
	// Initial offset is 0, the level 0 node file has only the
	// the root node, so NodeFileSummary struct is not necessary.
	int offset = 1;
	for (i = 0; i < this->numLevels; i++) {
		node.initialize(i, this->fds[i], offset, this->nodeSize);
		node.load();
		cout << "Node id: " << node.summary->nodeId <<  ", Tree level: " << i << endl;
		recordPosition = this->findPositionInNode(key, &node);
		record = node.getRecord(recordPosition);
		if (record->key == key) {
			cout << "Key " << key << " found" << endl;
			return KEY_FOUND;
		}
		offset = record->offset;
	}
	
	return KEY_NOT_FOUND;
}

int
BTree::findPositionInNode(Key key, Node* node)
{
	DEBUG("findPositionInNode()");

	int i;
	Record* curRecord;

	int numRecords = node->summary->numRecords;
	Record** nodeRecords = node->records;
	
	for (i=0; i<numRecords; i++) {
		curRecord = nodeRecords[i];
		if (curRecord->key >= key) {
			return i;
		}
	}
}

Node
BTree::btInsertInternal(Node & b, int key, int *median)
{
    DEBUG("btInsertInternal()");
    // return a node with the key if need to split this node b
    // else return 0

    /*
    // median store the key need to be insert into b(offset) node
    // return the offset of new splitted node
    int pos;
    int mid;
    //bTree b2;
    Node b2;

    // read this node
    b.load();      //TODO
    pos = findNextNode(key, b.node_content);    //search this level  TODO need to return the position should be

    if(pos == KEY_FOUND) { //already exists
    // nothing to do
    return 0;
    }

    if(b.level == this.numLevels) {       //if leaf level and still cannot find
    //TODO move keys
    // everybody above pos moves up one space
    b.insert_record(pos, key, NULL);

    memmove(b->keys[pos+1], &b->keys[pos], sizeof(*(b->keys)) * (b->numKeys - pos));

    // insert this key
    b->keys[pos] = key;
    b->numKeys++;
    } else {    // insert into child

    // insert in child
    // TODO construct the child node
    b2 = btInsertInternal( b->kids[pos] , key, &mid);

    // maybe insert a new key in b
    if(b2) {    // need to add key to this layer, when inserting to child
    // insert to here  TODO related Record
    // every key above pos moves up one space
    b.insert_record(pos, mid, b2->offset);
    memmove(&b->keys[pos+1], &b->keys[pos], sizeof(*(b->keys)) * (b->numKeys - pos));
    // new kid goes in pos + 1
    memmove(&b->kids[pos+2], &b->kids[pos+1], sizeof(*(b->keys)) * (b->numKeys - pos));

    b->keys[pos] = mid;
    b->kids[pos+1] = b2;
    b->numKeys++;
    }
    }

    // we waste a tiny bit of space by splitting now

    // instead of on next insert
    if(b->numKeys >= this->fanout) {    // if need split
    mid = b->numKeys/2;

     *median = b->keys[mid];

    // make a new node for keys > median
    // picture is:
    //
    //      3 5 7
    //      A B C D
    //
    // becomes
    //          (5)
    //      3        7
    //      A B      C D
    //
    //TODO new node, split
    b2 = malloc(sizeof(*b2));      //TODO need a new node

    b2->numKeys = b->numKeys - mid - 1;
    b2->isLeaf = b->isLeaf;

    memmove(b2->keys, &b->keys[mid+1], sizeof(*(b->keys)) * b2->numKeys);
    if(!b->isLeaf) {
	memmove(b2->kids, &b->kids[mid+1], sizeof(*(b->kids)) * (b2->numKeys + 1));
	}

	b->numKeys = mid;

	// TODO write out the nodes

	return b2;
} else {

	//TODO write back the node b, when do we need to write back

	return 0;
}
return 0;
*/
}

bool
BTree::searchKey(Key key, char * node_to_insert)
{
	DEBUG("searchKey()");

	// return the leaf node that should contain this key
	return false;
}

void
BTree::insertKey(Key key)
{
    DEBUG("insertKey()");
    //    bTree b1;   // new left child
    //    bTree b2;   // new right child
    
    Node root_node;
    if (this->numLevels == 0) {
        cout << "inserting root node: " << key << endl;
        root_node.level = 0;
        root_node.fd = create_new_file();
        root_node.offset = 1;
        //root_node.load();
	root_node.node_content = (char*)malloc(sizeof(char) * nodeSize * PAGE_SIZE);
        
        root_node.summary = NODE_SUMMARY(root_node.node_content);
        
        root_node.insert_record(0, key, INVALID_OFFSET);
        root_node.flush();
        return;
    }
    
    int median;
    root_node.level = 0;
    root_node.fd =this->fd_q[0];
    root_node.offset =1;

    Node b2 = btInsertInternal(root_node, key, &median);
    if(b2.valid) {   // TODO
	// basic issue here is that we are at the root
	// so if we split, we have to make a new root

        //updata_levels();
        Node new_root_node;
        new_root_node.level = 0;
        new_root_node.fd = create_new_file();
        new_root_node.offset = 1;
        new_root_node.load();
        new_root_node.insert_record(-1, -1, root_node.offset);
        new_root_node.insert_record(0, median, b2.offset);

/*
	// copy root to b1
	memmove(b1, b, sizeof(*b));

	// make root point to b1 and b2
	b->numKeys = 1;
	b->isLeaf = 0;
	b->keys[0] = median;
	b->kids[0] = b1;
	b->kids[1] = b2;
*/
    }
    return;
}

void
BTree::deleteKey(Key key)
{
	DEBUG("deleteKey()");

}







/* vim: set filetype=cpp: */
