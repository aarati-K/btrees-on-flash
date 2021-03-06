#include <stdio.h>
#include <iostream>
#include "btree.h"
#include "workloads.h"


int main()
{
    std::cout << "main()" << std::endl;

    BTree test_tree;
    test_tree.initializeEmptyTree(0, 4, 3, 0.8);
    test_tree.insertKey(100);
    test_tree.insertKey(1000);
    test_tree.insertKey(10000);
    test_tree.insertKey(100000);
    test_tree.insertKey(200);

    // Workloads
    // filename, num_ops, searches, insertions, deletions
    /* generateWorkload(std::string("search_heavy"), 100, 0.80, 0.10, 0.10); */
    /* performWorkload(std::string("search_heavy")); */
    /* generateWorkload(std::string("insert_heavy"), 100, 0.20, 0.60, 0.20); */
    /* performWorkload(std::string("insert_heavy")); */
    /* generateWorkload(std::string("delete_heavy"), 100, 0.20, 0.20, 0.60); */
    /* performWorkload(std::string("delete_heavy")); */

    return 0;
}

// vim:sw=4:ts=4
