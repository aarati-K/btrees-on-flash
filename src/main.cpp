#include "workloads.h"

int main()
{
	// filename, num_ops, searches, insertions, deletions
	generateWorkload(std::string("search_heavy"), 100, 0.80, 0.10, 0.10);
	performWorkload(std::string("search_heavy"));
	/* generateWorkload(std::string("insert_heavy"), 100, 0.20, 0.60, 0.20); */
	/* performWorkload(std::string("insert_heavy")); */
	/* generateWorkload(std::string("delete_heavy"), 100, 0.20, 0.20, 0.60); */
	/* performWorkload(std::string("delete_heavy")); */

	return 0;
}
