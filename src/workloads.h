/***************************************************************
* Header file for workloads.
*
* From Tree Indexing on Solid State Drives paper:
* Workload Design. We used the workloads with search only or update only,
* as well as mixed ones with different operations. In particular,
* we use a workload of 80% searches, 10% insertions, 5% deletions
* and 5% updates to simulate a workload dominated by reads, denoted
* as W-Search. We define W-Insert as a workload consisting 20% searches,
* 50% insertions, 20% deletions and 10% updates to simulate a workload
* dominated by insertions. We define W-Delete as a workload consisting
* 20% searches, 20% insertions, 50% deletions and 10% updates to simulate
* a workload dominated by deletions. All experiments focus on evaluating
* long running indexes. At the beginning of all experiments, B+-tree is
* built by bulk loading with the load factor of 0.7. Then we insert entries
* to make 20% leaf nodes split. We build FD-tree and LSM-tree in a similar
* process. The index is first built by bulk loading and then we keep
* insert-ing entries into the index until each level of FD-tree or each tree
* component of LSM-tree is at least half full. For BFTL, after bulk loading,
* we keep inserting entries into the index until each node scatters over
* multiple pages.
***************************************************************/

#ifndef __WORKLOADS_H__
#define __WORKLOADS_H__

/* #include "btree.h" */
#include <string>

/* void populateTree(BTree tree, int num_values); */
void generateWorkload(std::string filename, int num_ops, double searches, double insertions, double deletions);
/* void performWorkload(BTree tree, std::string filename); */
void performWorkload(std::string filename);

#endif

/* vim: set filetype=cpp: */
