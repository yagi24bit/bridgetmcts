.PHONY: all
all: piece.o board.o treenode.o xorshift.o bridget.exe



piece.o: piece.h piece.cpp
	g++ -c -o piece.o piece.cpp

board.o: board.h board.cpp piece.o
	g++ -std=c++11 -c -o board.o board.cpp

treenode.o: treenode.h treenode.cpp board.o
	g++ -std=c++11 -c -o treenode.o treenode.cpp

xorshift.o: xorshift.h xorshift.cpp
	g++ -c -o xorshift.o xorshift.cpp

bridget.exe: bridget.cpp piece.o board.o treenode.o
	g++ -std=c++11 -o bridget.exe bridget.cpp piece.o board.o treenode.o xorshift.o



.PHONY: clean
clean:
	rm -f piece.o board.o treenode.o bridget.exe
