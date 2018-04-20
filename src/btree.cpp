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

int BTree::initializeEmptyTree(int treeId, int fanOut, int nodeSize, float fillFactor) {
        cout << "=== to initialize" << endl;
	assert(fanOut > 1 && nodeSize > 0 && fillFactor >= 0.5);
	this->fanOut = fanOut;
	this->nodeSize = nodeSize;
	this->fillFactor = fillFactor;

	store_dir =  "/users/kanwu/data/";
	//cout << "Creating tree id: " << treeId << " root file location: " << rootFileName << endl;

	this->fds = (int *) malloc(sizeof(int));
	this->fds[0] = open((store_dir + "0.txt").c_str() , O_RDWR | O_CREAT);
	if (this->fds[0] < 0) {
		cout << "ERROR: Could not create file for root node" << endl;
		exit(1);
	}
	this->numLevels = 1; // Just the root

	// Initialize node buffer
	this->nodeBuffer = (char *) malloc(sizeof(char) * PAGE_SIZE * this->nodeSize);
	return 0;
}


void BTree::search(Key key) {
        int i;
	// Offset into the next level's node file
	// Initial offset is 0, the level 0 node file has only the
	// the root node, so NodeFileSummary struct is not necessary.
	int offset = 0;
	for (i = 0; i < this->numLevels; i++) {
		int fd = this->fds[i];
		lseek(fd, offset * PAGE_SIZE, SEEK_SET);
		read(fd, this->nodeBuffer, this->nodeSize*PAGE_SIZE);
		cout << "Node id: " << NODE_ID(nodeBuffer) <<  ", Tree level: " << i << endl;
		offset = this->findNextNode(key, nodeBuffer);
		if (offset == KEY_FOUND) {
			cout << "Key " << key << " found" << endl;
			break;
		}
	}
	if (offset != KEY_FOUND) {
		assert(offset == KEY_NOT_FOUND);
		cout << "Key " << key << " not found" << endl;
	}
}

// The offset returned is KEY_FOUND = 0, if the key is found in the node
// Otherwise the offset into the next level's node file is returned
// If this is a leaf node, the offset returned is KEY_NOT_FOUND = -1
int BTree::findNextNode(Key key, char* node) {
	int i, j;
	char* page;
	PageSummary* pageSummary;
	Record** records;
	Record *curRecord, *prevRecord;
	// Iterate over all pages
	for (i = 0; i < this->nodeSize; i++) {
		page = node + i * PAGE_SIZE;
		pageSummary = PAGE_SUMMARY(page);
		if (key > pageSummary->highKey || key < pageSummary->lowKey) {
			continue;
		}
		// Key in this page, if at all
		cout << "Input key " << key << " in range of page " << i << endl;
		records = PAGE_RECORDS(page);
		for (j = 0; j < pageSummary->numRecords; j++) {
			curRecord = records[j];
			if(curRecord->key == key){
				return KEY_FOUND;
			} else if (curRecord->key > key) {
				assert(j > 0);
				prevRecord = records[j-1];
				return prevRecord->offset;
			}
		}
		// Has to be a leaf node
		return KEY_NOT_FOUND;
	}
        return KEY_FOUND;
}

Node
BTree::btInsertInternal(Node & b, int key, int *median)
{
// median store the key need to be insert into b(offset) node
// return the offset of new splitted node
/*
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
*/

    // we waste a tiny bit of space by splitting now

    // instead of on next insert
/*
    if(b->numKeys >= MAX_KEYS) {    // if need split
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
*/
    /* return 0; */
}



bool BTree::searchkey(Key key, char * node_to_insert) {
    // return the leaf node that should contain this key
    return false;
}

void BTree::insert(Key key) {
/*    // find the key
    char * node_to_insert = NULL;
    bool need_insert = searchkey(key, node_to_insert);    // pointer to the nodebuffer of leafnode that should hold that key   + need to store kind of offset to each level of node during looking up
    if (!need_insert) {
        cout << "Key already exist\n" << endl;
        return;
    }

    Key key_to_insert = key;
    while (key_to_insert != NULL && node_to_inserti!=NULL) {
        // insert to this node
        insert_to_node(key_to_insert, node_to_insert);

        // judge whether need split
        key_to_insert = NULL;
        node_to_insert = NULL;
        if (node_to_insert->numrecords == this->fanout) {
            // split the node

            key_to_insert = "";
            node_to_insert = "";

        }

    }

    // special: if need to split root?



*/

//    bTree b1;   /* new left child */
//    bTree b2;   /* new right child */
    int median;
    Node root_node;
    root_node.level = 0;
    root_node.offset = 0;
    Node b2 = btInsertInternal(root_node, key, &median);
/*
    if(b2) {
        // basic issue here is that we are at the root
        // so if we split, we have to make a new root

        b1 = malloc(sizeof(*b1));
        assert(b1);

        // copy root to b1
        memmove(b1, b, sizeof(*b));

        // make root point to b1 and b2
        b->numKeys = 1;
        b->isLeaf = 0;
        b->keys[0] = median;
        b->kids[0] = b1;
        b->kids[1] = b2;
    }
*/

    return;
}

void BTree::deleteKey(Key key) {

}
