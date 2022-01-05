#include <stdio.h>
#include "treenode.h"

unsigned int TreeNode::count;
std::unordered_map<Board, TreeNode*> TreeNode::map;

TreeNode::TreeNode(Board *b, int d) {
	indexNumber = __atomic_fetch_add(&count, 1, __ATOMIC_ACQ_REL);
	board = b;
	depth = d;
	totalCount = 0;
	winCount = 0;

	isExpanded = false;
	nextCount = 0;
}

TreeNode::~TreeNode() {
	delete board;
}

bool TreeNode::staticEnumNextCallback(Board *b, Piece p, int pindex, int tflag, void *args) {
	TreeNode *_this = (TreeNode*)args;
	return _this -> enumNextCallback(b, p, pindex, tflag, args);
}

bool TreeNode::enumNextCallback(Board *b, Piece p, int pindex, int tflag, void *args) {
	// 勝敗判定
	bool judge = board -> judge((depth - 1) & 1);

	if(judge) {
		// 勝利局面
		for(int i = 0; i < nextCount; i++) { delete nextNode[i]; } // これまでのノードを全て削除
		nextNode[0] = new TreeNode(b, depth + 1);
		nextPiece[0] = p;
		nextPieceIndex[0] = pindex;
		turnFlag[0] = tflag;
		nextCount = 1;
		return false; // 勝ち局面の場合は探索を中断
	} else {
		// 勝利局面以外
		if(map.count(*b) > 0) {
			nextNode[nextCount] = map[*b]; // 既に見つかっているノードのポインタを格納
		} else {
			TreeNode *newtree = new TreeNode(b, depth + 1);
			nextNode[nextCount] = newtree; // 新しいノードを作成してポインタを格納
			map[*b] = newtree;
		}
		nextPiece[nextCount] = p;
		nextPieceIndex[nextCount] = pindex;
		turnFlag[nextCount] = tflag;
		nextCount++;
		return true;
	}
}

void TreeNode::expand() {
	if(isExpanded) { return; }
	isExpanded = true;

	int count = board -> enumNext(depth & 1, &staticEnumNextCallback, (void*)this);

	// 駒が置けない場合はパス (ノードを 1 つだけ作ってぶら下げる)
	if(count == 0) {
		nextNode[0] = new TreeNode(board, depth + 1);
		nextPiece[0] = {0, 0, 0};
		nextPieceIndex[0] = -1;
		turnFlag[0] = 0;
		nextCount = 1;
	}

	// TODO: 先手・後手ともに 0 件の場合はステイルメイト判定
}

void TreeNode::test() {
	/*
	// expand のテスト
	printf("count = %d\n", count);

	expand();
	printf("nextCount = %d\n", nextCount);
	printf("count = %d\n", count);

	nextNode[0] -> expand();
	printf("nextCount = %d\n", nextNode[0] -> nextCount);
	printf("count = %d\n", count);
	*/

	// 重複局面検知のテスト
	expand();
	// 12JS(21LE) → 33O → 31TE
	nextNode[7] -> expand();
	nextNode[7] -> nextNode[680] -> expand();
	nextNode[7] -> nextNode[680] -> nextNode[837] -> board -> output();
	// 13TS(31TE) → 33O → 21LE
	nextNode[148] -> expand();
	nextNode[148] -> nextNode[680] -> expand();
	nextNode[148] -> nextNode[680] -> nextNode[4] -> board -> output();
	printf("%s\n", (nextNode[7] -> nextNode[680] -> nextNode[837] == nextNode[148] -> nextNode[680] -> nextNode[4] ? "OK" : "NG"));
}
