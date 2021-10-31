#ifndef __PIECE_H__
#define __PIECE_H__

typedef unsigned long long int t_piece; // ブロックの有無

typedef struct {
	t_piece piece0, piece1, piece2; // ブロックの有無 (それぞれ 1 段目・2 段目・3 段目)
} Piece;

#define PIECE_TYPES 16
extern const Piece DEFAULT_PIECES[][4];
extern const int DEFAULT_PIECE_TURNS[];

#endif
