#include "board.h"
#include "treenode.h"

int main(int argc, char *argv[]) {
	Board *b = new Board();
	TreeNode *t1 = new TreeNode();
	t1 -> test();
	delete t1;
}
