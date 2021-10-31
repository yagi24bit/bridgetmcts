#include "board.h"

int main(int argc, char *argv[]) {
	Board *b = new Board();
	b -> test();

	delete b;
}
