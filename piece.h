#ifndef __PIECE_H__
#define __PIECE_H__

typedef unsigned long long int t_piece; // ブロックの有無

typedef struct {
	unsigned char type; // ブロックの種類
	t_piece piece0, piece1, piece2; // ブロックの有無 (それぞれ 1 段目・2 段目・3 段目)
} Piece;

#define PIECE_TYPES 4
#define PIECE_PUT_TYPES 16

extern const Piece DEFAULT_PIECES[PIECE_PUT_TYPES][4];
extern const int DEFAULT_PIECE_TURNS[PIECE_PUT_TYPES];

#define DEFAULT_PIECES_X 2
#define DEFAULT_PIECES_Y 2

#endif
