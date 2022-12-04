#ifndef __PIECE_H__
#define __PIECE_H__

#define ALL_PIECE_PATTERNS 2497
#define PIECE_TYPES 4
#define PIECE_PUT_TYPES 16
#define PIECE_X 8
#define PIECE_Y 8

typedef unsigned long long int t_piece; // ブロックの有無

struct Piece {
	unsigned char type; // ブロックの種類
	t_piece piece0, piece1, piece2; // ブロックの有無 (それぞれ 1 段目・2 段目・3 段目)

	static Piece EMPTY;

	static void init();
	bool isEmpty();
	static Piece get(int index);
	static Piece get(int y, int x, int type, int dir);
	static Piece get(char *s);
	static int getNumberOfPieces(int type);

protected:
	static Piece ALL_PIECES[ALL_PIECE_PATTERNS];
	static int ALL_PIECES_INDEX[PIECE_PUT_TYPES][4][PIECE_Y][PIECE_X];
};

extern const Piece DEFAULT_PIECES[PIECE_PUT_TYPES][4];
extern const int DEFAULT_PIECE_TURNS[PIECE_PUT_TYPES];

#endif
