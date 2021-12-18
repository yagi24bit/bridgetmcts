#include <stdio.h>
#include "treenode.h"

unsigned int TreeNode::count;

TreeNode::TreeNode(Board *b, int d) {
	indexNumber = __atomic_fetch_add(&count, 1, __ATOMIC_ACQ_REL);
	board = b;
	depth = d;
	totalCount = 0;
	winCount = 0;

	isExpanded = false;
	nextCount = 0;
}

TreeNode::~TreeNode() {
	delete board;
}

void TreeNode::test() {
	board -> output();
}
