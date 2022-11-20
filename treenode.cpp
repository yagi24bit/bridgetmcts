#include <stdio.h>
#include <math.h>
#include "treenode.h"
#include "xorshift.h"

unsigned int TreeNode::count;
std::unordered_map<Board, TreeNode*> TreeNode::map;

TreeNode::TreeNode(Board *b) {
	indexNumber = __atomic_fetch_add(&count, 1, __ATOMIC_ACQ_REL);
	board = b;
	refCount = 1;
	totalCount = 0;
	winCount = 0;

	isExpanded = false;
	nextCount = 0;
}

TreeNode::~TreeNode() {
	deleteChildNodes();
	delete board;
}

bool TreeNode::staticEnumNextCallback(Board *b, Piece p, int pindex, bool judge, int tflag, void *args) {
	TreeNode *_this = (TreeNode*)args;
	return _this -> enumNextCallback(b, p, pindex, judge, tflag);
}

bool TreeNode::enumNextCallback(Board *b, Piece p, int pindex, bool judge, int tflag) {
	bool ret = true;

	if(judge) {
		deleteChildNodes(); // 勝利局面を見つけたら、それ以外のノードを全て削除 (NOTE: nodeCount = 0 になる)
		ret = false;
	}

	if(map.count(*b) > 0) {
		nextNode[nextCount] = map[*b]; // 既に見つかっているノードのポインタを格納
		nextNode[nextCount] -> refCount++;
	} else {
		TreeNode *newtree = new TreeNode(b);
		nextNode[nextCount] = newtree; // 新しいノードを作成してポインタを格納
		map[*b] = newtree;
	}
	nextPiece[nextCount] = p;
	nextPieceIndex[nextCount] = pindex;
	turnFlag[nextCount] = tflag;
	selectCount[nextCount] = 0;
	nextCount++;

	return ret; // 勝利局面の場合は探索を中断、それ以外の場合は継続
}

void TreeNode::deleteChildNodes() {
	for(int i = 0; i < nextCount; i++) {
		if(--(nextNode[i] -> refCount) == 0) {
			map.erase(*nextNode[i] -> board);
			delete nextNode[i];
		}
	}

	nextCount = 0;
}

void TreeNode::expand() {
	if(isExpanded) { return; }
	isExpanded = true;

	int count = board -> enumNext(&staticEnumNextCallback, (void*)this);

	// 駒が置けない場合はパス (ノードを 1 つだけ作ってぶら下げる)
	if(count == 0) {
		Board *b = new Board(*board);
		b -> changeTurn();
		enumNextCallback(b, {0, 0, 0, 0}, -1, false, 0);
	}

	// TODO: 先手・後手ともに 0 件の場合はステイルメイト判定
}

int TreeNode::select() {
	int tempCount = 0;
	int tempIndex[NEXT_BOARDS];

	if(!isExpanded) { expand(); }
	totalCount++;

	if(nextCount == 1) {
		// 1 手しか無い場合は固定
		selectCount[0]++;
		return 0;
	} else if(totalCount < nextCount) {
		// 試していない局面がある場合、試していないノードを候補に加える
		for(int i = 0; i < nextCount; i++) {
			if(selectCount[i] == 0) { tempIndex[tempCount++] = i; }
		}
	} else {
		// スコアが最大の手を候補に加える
		double max = -INFINITY;
		double logn = log(totalCount);
		for(int i = 0; i < nextCount; i++) {
			TreeNode *next = nextNode[i];
			double score = (next -> totalCount == 0 ? 0.0 : (double)next -> winCount / next -> totalCount) + sqrt(2.0 * logn / selectCount[i]);
			if(max < score) {
				max = score;
				tempIndex[0] = i;
				tempCount = 1;
			} else if(max == score) {
				tempIndex[tempCount++] = i;
			}
		}
	}

	// 候補の中からランダムに 1 つを選択
	int idx = tempIndex[XorShift::xor128() % tempCount];
	selectCount[idx]++;
	return idx;
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

	/*
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
	*/

	// 勝利局面を見つけたときのテスト
	expand();
	// 11PE → 17O → 21LE → 14LS → 14PE
	nextNode[42] -> expand();
	nextNode[42] -> nextNode[712] -> expand();
	nextNode[42] -> nextNode[712] -> nextNode[9] -> expand();
	nextNode[42] -> nextNode[712] -> nextNode[9] -> nextNode[31] -> expand();
	// 11PE → 17O → 14PE → 14LS (→ 28JN)
	nextNode[42] -> nextNode[712] -> nextNode[287] -> expand();
	nextNode[42] -> nextNode[712] -> nextNode[287] -> nextNode[35] -> expand();

	// 11PE → 17O → 21LE → 14LS → 14PE の局面 (11PE → 17O → 14PE → 14LS → 21LE と同一) は delete されずに残る
	nextNode[42] -> nextNode[712] -> nextNode[9] -> nextNode[31] -> nextNode[209] -> board -> output();
	// 11PE → 17O → 21LE → 14LS の子ノードは 28JN の局面 1 つだけ
	printf("%s\n", (nextNode[42] -> nextNode[712] -> nextNode[287] -> nextNode[35] -> nextCount == 1 ? "OK" : "NG"));
	nextNode[42] -> nextNode[712] -> nextNode[287] -> nextNode[35] -> nextNode[0] -> board -> output();
}
