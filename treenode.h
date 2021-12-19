#ifndef __treenode_h__
#define __treenode_h__

#include "board.h"

#define NEXT_BOARDS ALL_PIECE_PATTERNS // TODO: 減らせるかどうか検討

class TreeNode {
protected:
	static unsigned int count;
	// TODO: static ハッシュテーブル

	unsigned int indexNumber; // 通し番号
	Board *board; // 盤面
	int depth; // ノードの深さ
	int totalCount; // 到達回数
	int winCount; // 勝利回数

	bool isExpanded; // 子ノードを展開済みかどうか
	int nextCount; // 合法手の数
	TreeNode *nextNode[NEXT_BOARDS]; // 次のノード
	Piece nextPiece[NEXT_BOARDS]; // 駒の置き方
	int nextPieceIndex[NEXT_BOARDS]; // 駒の置き方の通し番号
	int turnFlag[NEXT_BOARDS]; // 回転フラグ

	static bool staticEnumNextCallback(Board *b, Piece p, int pindex, int tflag, void *args);
	bool enumNextCallback(Board *b, Piece p, int pindex, int tflag, void *args);

public:
	TreeNode(Board *b, int d);
	~TreeNode();

	void expand();

	void test();
};

#endif
