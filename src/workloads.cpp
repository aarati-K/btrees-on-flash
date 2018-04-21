#include "btree.h"
#include "workloads.h"
#include "layout.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>


// Populates the tree with X number of values
/* void populateTree(BTree tree, int num_values) */
/* { */
/* 	for (int i = 0; i < num_values; i++) { */
/* 		tree.insert(i); */
/* 	} */
/* } */

// Generates a file that contains a random workload consisting of searches, insertions, and deletions
// in proportion to the given arguments
void generateWorkload(std::string filename, int num_ops, double searches, double insertions, double deletions)
{
	std::ofstream outfile(filename);
	for (int i = 0; i < num_ops; i++) {
		double rand_op = double(rand()) / double(RAND_MAX);
		Key rand_key = (long) round(double(rand()) / double(RAND_MAX) * num_ops);
		if (rand_op < searches) {
			/* std::cout << "search " << rand_key << std::endl; */
			outfile << "search " << rand_key << std::endl;
		} else if (rand_op < (searches + insertions)) {
			/* std::cout << "insert " << rand_key << std::endl; */
			outfile << "insert " << rand_key << std::endl;
		} else if (rand_op < (searches + insertions + deletions)) {
			/* std::cout << "delete " << rand_key << std::endl; */
			outfile << "delete " << rand_key << std::endl;
		}
	}
	outfile.close();
}

/* void performWorkload(std::string filename) */
void performWorkload(BTree tree, std::string filename)
{
	std::ifstream infile(filename);
	std::string operation;
	Key key;
	while (infile >> operation >> key) {
		/* std::cout << operation << " " << key << "\n"; */
		if (operation == "search") {
			/* std::cout << operation << " " << key << "\n"; */
			tree.searchKey(key);
		} else if (operation == "insert") {
			/* std::cout << operation << " " << key << "\n"; */
			tree.insertKey(key);
		} else if (operation == "delete") {
			/* std::cout << operation << " " << key << "\n"; */
			tree.deleteKey(key);
		}
	}
	infile.close();
}

// vim:sw=4:ts=4
