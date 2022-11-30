#ifndef __treenode_h__
#define __treenode_h__

#include "board.h"
#include <unordered_map>

#define NEXT_BOARDS ALL_PIECE_PATTERNS // TODO: 減らせるかどうか検討

enum Result {
	RESULT_UNKNOWN = 0,
	RESULT_WIN,
	RESULT_DRAW,
	RESULT_LOSE
};

class TreeNode {
protected:
	static unsigned int count;
	static std::unordered_map<Board, TreeNode*> map; // 重複チェック用のハッシュマップ

	unsigned int indexNumber; // 通し番号
	Board *board; // 盤面
	int refCount; // 親ノードの数
	bool isExpanded; // 子ノードを展開済みかどうか
	int nextCount; // 合法手の数
	TreeNode *nextNode[NEXT_BOARDS]; // 次のノード
	Piece nextPiece[NEXT_BOARDS]; // 駒の置き方
	int nextPieceIndex[NEXT_BOARDS]; // 駒の置き方の通し番号
	int turnFlag[NEXT_BOARDS]; // 回転フラグ

	Result result; // 勝敗 (手番側)
	int steps; // 勝ち局面の場合は最短手数、負け局面の場合は最長手数

	int totalCount; // 到達回数
	int totalCountOrig; // 到達回数 (「待った」の回数も含む)
	int winCount; // 勝利回数 (手番側)
	int selectCount[NEXT_BOARDS]; // 子ノードを選択した回数

	static bool staticEnumNextCallback(Board *b, Piece p, int pindex, bool judge, int tflag, void *args);
	bool enumNextCallback(Board *b, Piece p, int pindex, bool judge, int tflag);
	void deleteChildNodes();

public:
	TreeNode(Board *b);
	~TreeNode();

	void expand();
	int select();
	int selectWhenWin(int offset = 0);
	int selectWhenLose();
	void rollout();

	void test();
};

#endif
