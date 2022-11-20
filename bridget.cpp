#include "board.h"
#include "treenode.h"

int main(int argc, char *argv[]) {
	Board::initializeAllPieces();
	Board *b = new Board();
	TreeNode *t1 = new TreeNode(b);
	t1 -> test();
	delete t1;
}
