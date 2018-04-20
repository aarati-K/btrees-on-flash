#include <stdio.h>
#include <iostream>
#include "btree.h"


int main() {
    std::cout << "in main" << std::endl;
    BTree tree_to_test;

    tree_to_test.initializeEmptyTree(0, 3, 3, 0.8); 
    return 0;

}
