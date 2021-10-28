#ifndef __BOARD_H__
#define __BOARD_H__

#define BOARD_X  8
#define BOARD_Y  8
#define BOARD_HEIGHT 3

typedef unsigned long long int t_board; // ブロックの有無
typedef unsigned long long int t_color; // ブロックの色

class Board {
protected:
	t_board board0, board1, board2; // ブロックの有無 (それぞれ 1 段目・2 段目・3 段目)
	t_color color; // 真上から見たときの色

public:
	Board();
	~Board();

	// TODO: clone();
	void output(); // 出力
	// TODO: 駒を置く
	// TODO: 合法手の列挙
	bool judge(int turn); // 勝敗判定
};

#endif
