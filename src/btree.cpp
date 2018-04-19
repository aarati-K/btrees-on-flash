#include <assert.h>
#include "btree.h"
#include "layout.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int BTree::initializeEmptyTree(uint treeId, int fanOut, int nodeSize, float fillFactor) {
	assert(fanOut > 1 && nodeSize > 0 && fillFactor >= 0.5);
	this->fanOut = fanOut;
	this->nodeSize = nodeSize;
	this->fillFactor = fillFactor;

	string rootFileName("data/node-files/");
	rootFileName +=  to_string(treeId) + "/0.txt";
	cout << "Creating tree id: " << treeId << " root file location: " << rootFileName << endl;

	this->fds = malloc(sizeof(int));
	this->fds[0] = open(root_file_name, O_RDWR | O_CREAT);
	if (this->fds[0] < 0) {
		cout << "ERROR: Could not create file for root node" << endl;
		exit(1);
	}
	this->numLevels = 1; // Just the root

	// Initialize node buffer
	this->nodeBuffer = malloc(sizeof(char) * PAGE_SIZE * this->nodeSize);
	return 0;
}


void BTree::search(Key key) {
	int i;
	// Offset into the next level's node file
	// Initial offset is 0, the level 0 node file has only the
	// the root node, so NodeFileSummary struct is not necessary.
	int pageOffset = 0;
	for (i = 0; i < this->numLevels, i++) {
		int fd = this->fd[i];
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
	Record* curRecord, prevRecord;
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
}

void BTree::insert(Key key) {

}

void BTree::delete(Key key) {

}
