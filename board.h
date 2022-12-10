#ifndef __BOARD_H__
#define __BOARD_H__

#include <functional>
#include "piece.h"

#define BOARD_X  8
#define BOARD_Y  8
#define BOARD_HEIGHT 3

typedef unsigned long long int t_board; // ブロックの有無
typedef unsigned long long int t_color; // ブロックの色

class Board {
protected:
	bool turn; // 手番 (false : 先手、true : 後手)
	unsigned short pieces_in_hand0, pieces_in_hand1; // 持ち駒数 (それぞれ先手・後手、下位から 4 ビットずつ L・O・S・T)
	t_board board0, board1, board2; // ブロックの有無 (それぞれ 1 段目・2 段目・3 段目)
	t_color color; // 真上から見たときの色

	t_board fliph1(t_board b); // 左右反転 (1 変数)
	t_board flipv1(t_board b); // 上下反転 (1 変数)
	t_board flipxy1(t_board b); // XY 軸反転 (1 変数)

public:
	Board(); // コンストラクタ
	Board(const Board &board); // コピーコンストラクタ
	~Board(); // デストラクタ

	Board *clone(); // 自身のコピーインスタンスを作成
	void copyTo(Board *b); // コピー
	bool equals(Board *b) const; // 一致判定
	void output(); // コンソール出力
	void output(int tflag); // コンソール出力 (正規化前に戻してから出力)
	bool put(Piece p); // 駒を盤面に配置
	bool put(Piece p, int c); // 駒を盤面に配置 (色指定)
	bool put(int y, int x, int type, int dir, int c); // 駒を盤面に配置 (種類と座標指定)
	bool put(char *s, int c); // 駒を盤面に配置 (棋譜形式)
	void changeTurn(); // 手番を交代
	void fliph(); // 左右反転
	void flipv(); // 上下反転
	void flipxy(); // XY 軸反転
	int normalize(); // 正規化
	static int calcTurnFlag(int current, int tflag); // 回転フラグを計算
	int enumNext(bool (*callback)(Board*, Board*, Piece, int, bool, int, void*), void *args = nullptr); // 合法手を列挙
	bool judge(bool turn); // 勝敗判定
	int judgeStalemate(); // ステイルメイト判定

	// ハッシュテーブル用
	bool operator ==(const Board &b) const { return this -> equals((Board*)&b); } // == 演算子
	friend std::hash<Board>;

	void test();
};

namespace std {
	template<>
	class hash<Board> {
		public:
		size_t operator() (const Board &b) const {
			// 簡易的なハッシュ値
			return (b.board0 ^ b.board0 >> 32)
				^ (b.board1 ^ b.board1 >> 32) << 1
				^ (b.board2 ^ b.board2 >> 32) << 2
				^ (b.color ^ b.color >> 32) << 3;
		}
	};
}

#endif
