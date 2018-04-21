/***************************************************************
* Header file for the storage layout.
***************************************************************/

#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#define DEBUG(x) do { std::cerr << x; } while (0)

#define KEY_FOUND 0
#define KEY_NOT_FOUND -1

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

typedef long Key;
typedef char* Page;

/*
 * The nodes of the B-tree are organised such that we have one file
 * corresponding to nodes of one level.
 *
 * Each "Record" of the B-tree index contains a key, an offset to
 * the record in the "data file" corresponding to the key,
 * and the page offset into the next level's file.
 */
typedef struct Record {
	Key key;
	// Offset into the data file with the record of the key.
	// Ideally one sould have this in a real index,
	// but this isn't neccessary for our experiment.
	// long recordOffset;

	// page offset of the child node in the next level's file.
	int offset;
} Record;

/*
 * The summary of the level file. We have one file for each level.
 * Each level file holds the nodes in that level. Within each level file,
 * each node has contiguous pages. However, the nodes within the file
 * need not be in any particular order. The maximum size of the file
 * at a particular level is ((fanOut)^level)*(nodeSize)
 *
 * The LevelSummary construct is present in the first page of the level file.
 * The pages starting from 1 correspond to nodes. We want nodes to
 * start at page boundaries.
 */
typedef struct LevelSummary {
	int level;
	int numNodes;

	struct NodeRecord {
		int nodeId;
		// Page offset within the level file
		int pageOffset;
	};
	// List of node records
	struct NodeRecord** nodeRecords;
} LevelSummary;

// Not used in this implementation
typedef struct PageSummary {
	// The node the page belongs to.
	int nodeId;
	// A node consists of multiple pages.
	// This is the page offset witin the node;
	int nodeOffset;
	// The page offset into the level file for this level
	int offset;
	// Is the page dirty
	bool isDirty;
	// Lowest key in this page
	Key lowKey;
	// Highest key in this page
	Key highKey;
	// Number of records in this page
	int numRecords;
	// The list of records in this page
	Record** records;
} PageSummary;

typedef struct NodeSummary {
	// The node id
	int nodeId;
	// Is the node dirty
	bool isDirty;
	// Lowest key in the node
	Key lowKey;
	// Highest key in the node
	Key highKey;
	// Number of records in the node
	int numRecords;
	// The list of records in the node
	Record** records;
} NodeSummary;

#define PAGE_SIZE 2048
#define RECORD_SIZE sizeof(Record)
#define INVALID_OFFSET -1
#define NODE_SUMMARY(nodePtr) ((NodeSummary*)nodePtr)
#define NODE_ID_FROM_SUMMARY(nodePtr) (NODE_SUMMARY(nodePtr)->nodeId)
#define NODE_NUM_RECORDS_FROM_SUMMARY(nodePtr) (NODE_SUMMARY(nodePtr)->numRecords)
#define NODE_RECORDS_FROM_SUMMARY(nodePtr) (NODE_SUMMARY(nodePtr)->records)
#define PAGE_SUMMARY_SIZE sizeof(PageSummary)
#define PAGE_SUMMARY(page) ((PageSummary*) page)
#define NODE_ID(nodePtr) (PAGE_SUMMARY(nodePtr)->nodeId)
#define PAGE_NUM_RECORDS(page) (PAGE_SUMMARY(page)->numRecords)
#define PAGE_RECORDS(page) ((Record**)(page + PAGE_SUMMARY_SIZE))
#define PAGE_MAX_RECORDS(page) ((PAGE_SIZE - PAGE_SUMMARY_SIZE)/RECORD_SIZE)

#endif // __LAYOUT_H__

/* vim: set filetype=cpp: */
