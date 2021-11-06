#ifndef __BOARD_H__
#define __BOARD_H__

#include "piece.h"

#define BOARD_X  8
#define BOARD_Y  8
#define BOARD_HEIGHT 3

#define ALL_PIECE_PATTERNS 2497

typedef unsigned long long int t_board; // ブロックの有無
typedef unsigned long long int t_color; // ブロックの色

class Board {
protected:
	t_board board0, board1, board2; // ブロックの有無 (それぞれ 1 段目・2 段目・3 段目)
	t_color color; // 真上から見たときの色

	static Piece ALL_PIECES[ALL_PIECE_PATTERNS];

public:
	Board();
	~Board();

	static void initializeAllPieces();

	// TODO: clone();
	void output(); // 出力
	bool put(Piece p, int c); // 駒を盤面に配置
	// TODO: 合法手の列挙
	bool judge(int turn); // 勝敗判定

	void test();
};

#endif
