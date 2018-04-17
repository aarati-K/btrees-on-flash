#include <assert.h>
#include "btree.h"
#include "layout.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

int BTree::initializeEmptyTree(uint treeId, int fanOut, int nodeSize) {
	assert(fanOut > 1 && nodeSize > 0);
	this->fanOut = fanOut;
	this->nodeSize = nodeSize;

	string rootFileName("data/node-files/");
	rootFileName +=  to_string(treeId) + "/0.txt";
	cout << "Creating tree id: " << treeId << " root file location: " << rootFileName << endl;

	this->fds = malloc(sizeof(int));
	this->fds[0] = open(root_file_name, O_WRONLY | O_CREAT);
	if (this->fds[0] < 0) {
		cout << "ERROR: Could not open file for root node" << endl;
		exit(1);
	}
	this->numLevels = 1; // Just the root
	return 0;
}


void BTree::search(Key key) {
}

void BTree::insert(Key key) {

}

void BTree::delete(Key key) {

}
