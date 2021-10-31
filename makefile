.PHONY: all
all: piece.o board.o bridget.exe



piece.o: piece.h piece.cpp
	g++ -c -o piece.o piece.cpp

board.o: board.h board.cpp piece.o
	g++ -c -o board.o board.cpp

bridget.exe: bridget.cpp piece.o board.o
	g++ -o bridget.exe bridget.cpp piece.o board.o



.PHONY: clean
clean:
	rm -f piece.o board.o bridget.exe
