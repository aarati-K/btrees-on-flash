#include "btree.h"
#include "layout.h"
#include <assert.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <sys/stat.h>

using namespace std;

// Create file which maps file descriptors to levels in B-Tree
void BTree::writeMap()
{
	DEBUG("writeMap()");
	std::ofstream mapFile(this->store_dir + std::string("map"));
	// Map each fd to level
	for (int i = 0; i < this->numLevels; i++) {
		int fd = this->fd_q[i];
		char filePath[128];
		if (fcntl(fd, F_GETPATH, filePath) != -1) {
			mapFile << i << " " << filePath << std::endl;
		}
	}
	mapFile.close();
}

// Populate fd_q with open file for each level from map file
void BTree::loadMap()
{
	DEBUG("loadMap()");
	std::ifstream mapFile(this->store_dir + std::string("map"));
	int level, filename;
	// Open each level file and map fd to level in fd_q
	while (mapFile >> level >> filename) {
		int fd = open((store_dir + filename.c_str(), O_RDWR);
		this->fd_q.push_back(fd);
	}
	mapFile.close();
}

// Create file for level of BTree
int BTree::create_new_file() {
   int fd = open((store_dir + to_string(numLevels)).c_str(), O_RDWR | O_CREAT);
   this->fd_q.push_front(fd);
   return fd;
}

void Node::initialize(int level, int fd, int offset, int size) {
	cout << "    initialize node with: level: "  <<  level << " fd: " << fd << " offset: " << offset << " size: " << size << endl;
        this->level = level;
	this->fd = fd;
	this->offset = offset;
	this->size = size;
	if (!this->memoryAllocated) {
		this->node_content = (char*)malloc(sizeof(char) * this->size * PAGE_SIZE);
		this->memoryAllocated = true;
                this->summary = NODE_SUMMARY(this->node_content);
	        this->records = NODE_RECORDS(this->node_content);
	        memset(this->summary, 0, sizeof(NodeSummary));
        }
}

void Node::load() {
        this->valid = true;
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
        DEBUG("    to flush a node");
        struct stat st;
        fstat(this->fd, &st);
        int size = st.st_size;

        cout << "    size of the file: " << size << " PAGE_SIZE: " << PAGE_SIZE << endl;
	
        cout << "    fd: " << this->fd << " offset: " << this->offset << " size: " << this->size << endl; 
        if (!this->summary->isDirty) {
		return;
	}
	// Set this to false before writing
	this->summary->isDirty = false;
	this->dump();
        lseek(this->fd, this->offset * PAGE_SIZE, SEEK_SET);
	write(fd, this->node_content, this->size * PAGE_SIZE);
        
        cout << "writing out: " << this->size* PAGE_SIZE << endl;
        fstat(this->fd, &st);
        size = st.st_size;

        cout << "    size of the file: " << size << " PAGE_SIZE: " << PAGE_SIZE << endl;
}

void Node::insert_record(int pos, Key key, int offset) {
        DEBUG("    inserting a record into node");
        int i;
	Record tempRecord, prevRecord;
        
        cout << "    insert " << key << " into overall: " << this->summary->numRecords << endl;
	// Initially set the previous record to temp record for copying
	prevRecord.key = key;
	prevRecord.offset = offset;

	int numRecords = this->summary->numRecords;
        Record * record_base = this->records; //(Record *)((char *)this->summary + sizeof(NodeSummary));
        /*//assert(pos <= numRecords);
	// The array one right starting at pos
	for (i=pos; i <= numRecords; i++) {
		tempRecord.key = nodeRecords[i]->key;
		tempRecord.offset = nodeRecords[i]->offset;
		nodeRecords[i]->key = prevRecord.key;
		nodeRecords[i]->offset = prevRecord.offset;
		prevRecord.key = tempRecord.key;
		prevRecord.offset = tempRecord.offset;
	}
        */
	assert(pos <= this->summary->numRecords);
        if (pos == this->summary->numRecords) {
            record_base[pos].key = key;
            record_base[pos].offset = offset;
        }

	for (i = pos; i < numRecords; i++) {
		tempRecord.key = record_base[i].key;
		tempRecord.offset = record_base[i].offset;
		record_base[i].key = prevRecord.key;
		record_base[i].offset = prevRecord.offset;
		prevRecord.key = tempRecord.key;
		prevRecord.offset = tempRecord.offset;
        }
        if (numRecords == 0) {
            record_base[0].key = key;
            record_base[0].offset = offset;
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
        if (pos>=this->summary->numRecords) {
           cout << "larger that largest " << endl; 
           return NULL;

        }
	assert(pos < this->summary->numRecords);
	return (&(this->records[pos]));
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
	Record * nodeRecords = node->records;
        //Record * nodeRecords = (Record *)((char *)node->summary + sizeof(NodeSummary));
	for (i=0; i<numRecords; i++) {
		//curRecord = nodeRecords[i];
		if (nodeRecords[i].key >= key) {
			return i;
		}
	}
}

Node
BTree::btInsertInternal(Node & b, int key, int *median)
{
    DEBUG("btInsertInternal()");

    Node b1;

    // return a valid node with the key if need to split this node b
    // else return none valid node

    // median store the key need to be insert into b(offset) node
    // return the offset of new splitted node
    int pos;
    int mid;
    Node b2;

    // read this node
    b.load();
    pos = findPositionInNode(key, &b);    //search this level  TODO need to return the position should be
    //cout << "found at " << pos << endl;
    Record * to_check_record = b.getRecord(pos);
    if(to_check_record != NULL && key == to_check_record->key) { //already exists
        // nothing to do
        cout << "Key already exists" << endl;
        return b1;
    }


    // truly need to insert
    if(b.level == this->numLevels - 1) {       //if leaf level and still cannot find
        b.dump();
        b.insert_record(pos, key, INVALID_OFFSET);
        b.dump();
    }
/*
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
*/
    // we waste a tiny bit of space by splitting now

    // instead of on next insert
    cout << "FANOUT: " << this->fanOut << endl;
    if(b.summary->numRecords >= this->fanOut) {    // if need split
        cout << "    !!!!!!!!!!!!!!!!!!!!!!!!!!!! to split a node" << endl;
        mid = b.summary->numRecords/2;
        *median = b.records[mid].key;

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
        struct stat st;
        fstat(b.fd, &st);
        int size = st.st_size;

        cout << "    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1 size of the file: " << size << " PAGE_SIZE: " << PAGE_SIZE << endl;
        b2.initialize(b.level, b.fd, size/PAGE_SIZE, b.size);
        
        b.dump();
        b2.dump();

        // move all records
        for (int i = mid+1; i < b.summary->numRecords; i++) {
            b2.insert_record(i-mid-1, b.records[i].key, b.records[i].offset);
        }

        b.summary->highKey = b.records[mid-1].key;
        b.summary->numRecords = mid;
        
        b.dump();
        b2.dump();

        b2.valid = true;
        b.flush();
        b2.flush();
        return b2;
    } 

    b.flush();
    return b1;
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
    DEBUG("===== insertKey()");
    //    bTree b1;   // new left child
    //    bTree b2;   // new right child
    
    Node root_node;
    if (this->numLevels == 0) {
        cout << "    inserting root node: " << key << endl;
        root_node.initialize(0, create_new_file(), 0, this->nodeSize);
        cout << "    to insert record" << endl;
        root_node.insert_record(0, key, INVALID_OFFSET);
        root_node.dump();
        root_node.flush();
        this->numLevels += 1;
    
        // check
        /*cout << "    ====== check here" << endl;
        Node check_node;
        check_node.initialize(0, this->fd_q[0], 0, this->nodeSize);
        check_node.load();
        check_node.dump(); */
        return;
    }
    
    int median;
    root_node.initialize(0, this->fd_q[0], 0, this->nodeSize);
    root_node.load();
    root_node.dump();

    Node b2 = btInsertInternal(root_node, key, &median);
    
    cout << "==================== get back from btInsertInternal" << endl;
    if(b2.valid) {   // TODO
	// basic issue here is that we are at the root
	// so if we split, we have to make a new root
        b2.dump();
        //updata_levels();
        Node new_root_node;
        cout << "median: " << median << " offset: " << b2.offset << endl;
        new_root_node.initialize(0, create_new_file(), 0, this->nodeSize);
        new_root_node.insert_record(0, median, b2.offset);
        cout << "here!!!!!!!!!!!!!!!!!!!!!" << endl; 
        new_root_node.dump();
        new_root_node.flush();
        this->numLevels += 1;
    }
    cout << "===== insert finished " << endl;
    return;
}

void
BTree::deleteKey(Key key)
{
	DEBUG("deleteKey()");

}







// vim:sw=4:ts=4
