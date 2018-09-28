#include "../base/libList.h"


void onDelete(Node* n) {
	fprintf(stderr, "=== Deleting Node Node: #%d = %s ===\n", n->id, n->name);

}

short iterate(int index, Node* n, short* delete, void* param, va_list*) {
	fprintf(stderr, "--- Node: #%d = %s\n", n->id, n->name);
}

int main() {
	
	/* CREATE NEW LIST */
	ListManager* list = initListMgr();


	/* Add Empty Node */
	Node* n = addNode(
		list, 		// List 
		"Node-1"	// Node Name
	);

	/* Set Node Value */
	int value1 = 10;
	n->value = (void*) &value1;
	
	/* Set On Delete Fuction */
	n->del = onDelete;

	/* Add Node With Value */
	int value2 = 20;
	n = addNodeV(
		list,				// List
		"Node-2",			// Node Name
		(void*) testValue,	// Node Value
		0					// Node Value Is Alloc (Will Be Free On Delete)
	);

	/* Set On Delete Fuction */
	n->del = onDelete;


	/* Get Node By Id */
	n = getNode(list, 0);

	/* Get Node By Name */
	n = getNodeByName(list, "Node-1");


	/* Iterate List */
	fprintf(stderr, "==== Iterate List ====\n");
	listIterateFnc(
		list,		// List
		iterate,	// Function
		NULL,		// Start From Node
		NULL,		// Optionnal Param
		NULL		// Optionnals Params ...
	);


	/* Delete List */
	deleteList(list);

	return 0;
}