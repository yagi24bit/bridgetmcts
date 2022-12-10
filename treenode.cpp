#include <stdio.h>
#include <limits.h>
#include <math.h>
#include "treenode.h"
#include "xorshift.h"

#define MAX_DEPTH 64
unsigned int TreeNode::count;
std::unordered_map<Board, TreeNode*> TreeNode::map;

TreeNode::TreeNode() {
	indexNumber = __atomic_fetch_add(&count, 1, __ATOMIC_ACQ_REL);
	refCount = 1;

	result = RESULT_UNKNOWN;
	steps = INT_MAX;
	totalCount = 0;
	winCount = 0;

	isExpanded = false;
	nextCount = 0;
}

TreeNode::~TreeNode() {
}

bool TreeNode::staticEnumNextCallback(Board *orig, Board *b, Piece p, int pindex, bool judge, int tflag, void *args) {
	TreeNode *_this = (TreeNode*)args;
	return _this -> enumNextCallback(orig, b, p, pindex, judge, tflag);
}

bool TreeNode::enumNextCallback(Board *orig, Board *b, Piece p, int pindex, bool judge, int tflag) {
	bool ret = true;

	if(judge) {
		deleteChildNodes(orig); // 勝利局面を見つけたら、それ以外のノードを全て削除 (NOTE: nodeCount = 0 になる)
		ret = false;
		result = RESULT_WIN;
		steps = 1;
	}

	if(map.count(*b) > 0) {
		nextNode[nextCount] = map[*b]; // 既に見つかっているノードのポインタを格納
		nextNode[nextCount] -> refCount++;
	} else {
		TreeNode *newtree = new TreeNode();
		nextNode[nextCount] = newtree; // 新しいノードを作成してポインタを格納
		map[*b] = newtree;
		if(judge) { newtree -> result = RESULT_LOSE; newtree -> steps = 0; }
	}
	nextPiece[nextCount] = p;
	nextPieceIndex[nextCount] = pindex;
	turnFlag[nextCount] = tflag;
	selectCount[nextCount] = 0;
	nextCount++;

	return ret; // 勝利局面の場合は探索を中断、それ以外の場合は継続
}

void TreeNode::deleteChildNodes(Board *board) {
	for(int i = 0; i < nextCount; i++) {
		if(--(nextNode[i] -> refCount) == 0) {
			Board *b = new Board(*board);
			b -> put(nextPiece[i]);
			b -> normalize();
			nextNode[i] -> deleteChildNodes(b);
			delete nextNode[i];
			nextNode[i] = nullptr;
			map.erase(*b);
			delete b;
		}
	}

	nextCount = 0;
}

void TreeNode::expand(Board *board) {
	if(isExpanded) { return; }
	isExpanded = true;

	int count = board -> enumNext(&staticEnumNextCallback, (void*)this);

	// 駒が置けない場合はパス (ノードを 1 つだけ作ってぶら下げる)
	if(count == 0) {
		Board *b = new Board(*board);
		enumNextCallback(b, b, {0, 0, 0, 0}, -1, false, 0);
	}
}

int TreeNode::select(Board *board) {
	int tempCount = 0;
	int tempIndex[NEXT_BOARDS];

	if(!isExpanded) { expand(board); }
	totalCount++;
	totalCountOrig++;

	if(nextCount == 1) { selectCount[0]++; return 0; } // 1 手しか無い場合は固定
	switch(result) {
		case RESULT_WIN: return selectWhenWin(); // 勝ち局面の場合は別メソッドで処理
		case RESULT_LOSE: return selectWhenLose(); // 負け局面の場合は別メソッドで処理
	}

	if(totalCountOrig < nextCount) {
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
			// 負け局面 (次の手番の勝ち局面) は候補に加えない
			if(next -> result == RESULT_WIN) { continue; }
			// 勝ち局面 (次の手番の負け局面) を見つけたら、別メソッドで処理
			if(next -> result == RESULT_LOSE) { result = RESULT_WIN; return selectWhenWin(i); }

			double score = (next -> totalCount == 0 ? 0.0 : (double)(next -> totalCount - next -> winCount) / next -> totalCount) + sqrt(2.0 * logn / selectCount[i]);
			if(max < score) {
				max = score;
				tempIndex[0] = i;
				tempCount = 1;
			} else if(max == score) {
				tempIndex[tempCount++] = i;
			}
		}
	}

	// 負け局面 (次の手番の勝ち局面) しか無い場合、別メソッドで処理
	if(tempCount == 0) { result = RESULT_LOSE; return selectWhenLose(); }

	// 候補の中からランダムに 1 つを選択
	int idx = tempIndex[XorShift::xor128() % tempCount];
	selectCount[idx]++;
	return idx;
}

// 勝ち局面 (次の手番の負け局面) のうち、手数が最小の手を返す
// NOTE: 次の局面のうち 1 つ以上は result == RESULT_LOSE である前提
// NOTE: 同手数の勝ち方が複数ある場合、最初に見つかったものを採用 (ランダム選択はしない)
int TreeNode::selectWhenWin(int offset) {
	int idx = offset;
	int min = INT_MAX;
	for(int i = offset; i < nextCount; i++) {
		if(nextNode[i] -> result == RESULT_LOSE && min > nextNode[i] -> steps) {
			min = nextNode[i] -> steps;
			idx = i;
		}
	}

	steps = min + 1; // steps 更新
	selectCount[idx]++;
	return idx;
}

// 負け局面 (次の手番の勝ち局面) のうち、手数が最大の手を返す
// NOTE: 次の局面が全て result == RESULT_WIN である前提
// NOTE: 同手数の負け方が複数ある場合、最初に見つかったものを採用 (ランダム選択はしない)
int TreeNode::selectWhenLose() {
	int idx = 0;
	int max = 0;
	for(int i = 1; i < nextCount; i++) {
		if(max < nextNode[i] -> steps) {
			max = nextNode[i] -> steps;
			idx = i;
		}
	}

	steps = max + 1; // steps 更新
	selectCount[idx]++;
	return idx;
}

void TreeNode::rollout(Board *board) {
	int index[MAX_DEPTH];
	int depth = 0;
	TreeNode *current = this;
	TreeNode *treenode[MAX_DEPTH];
	Board *boards[MAX_DEPTH];

	treenode[0] = current;
	boards[0] = new Board(*board);
	while(depth < MAX_DEPTH) {
		int idx = current -> select(boards[depth]);
		index[depth] = idx;
		Board *temp = new Board(*boards[depth]);
		temp -> put(current -> nextPiece[idx]);
		temp -> normalize();
		depth++;
		current = current -> nextNode[idx];
		treenode[depth] = current;
		boards[depth] = temp;

		// ステイルメイト判定
		// NOTE: 2 連続でパスすると同一局面に戻るため、current == treenode[depth] == treenode[depth - 2] になる
		if(depth >= 2 && current == treenode[depth - 2]) {
			int judge = boards[depth] -> judgeStalemate() * ((depth % 1) ? -1 : 1);
			current -> result = (judge == 1 ? RESULT_WIN : judge == -1 ? RESULT_LOSE : RESULT_DRAW);
			if(judge != 0) { current -> steps = 0; }

			current -> deleteChildNodes(boards[depth]); // NOTE: このままだとループしてしまうので、不要な子ノード ( treenode[depth - 1] ) を削除
			delete boards[depth];
			delete boards[depth - 1];
			depth -= 2;
			break;
		}

		// 勝敗判定
		if(current -> result == RESULT_LOSE) {
			if(current -> steps == 0 && depth >= 2 && treenode[depth - 2] -> nextCount > 1 && treenode[depth - 2] -> totalCountOrig <= treenode[depth - 2] -> nextCount) {
				// ランダム選択した直後に王手放置で負けた場合「待った」をする
				treenode[depth - 1] -> totalCount--;
				depth -= 2;
				current = treenode[depth];
				current -> totalCount--;
				continue;
			} else {
				current -> totalCount++;
				current -> totalCountOrig++;
				break; // 「待った」をしない場合は終局
			}
		}
	}

	// 勝利数と勝敗を上のノードに反映
	int winCountArr[2] = {0, 0};
	if(current -> result == RESULT_WIN) { winCountArr[depth & 1] = 1; } else if(current -> result == RESULT_LOSE) { winCountArr[(depth + 1) & 1] = 1; }
	for(int i = depth - 1; i > 0; i--) {
		TreeNode *cur = treenode[i]; // current node
		TreeNode *par = treenode[i - 1]; // parent node

		// 勝利数
		cur -> winCount += winCountArr[i & 1];

		// 勝敗・手数
		if(cur -> result == RESULT_LOSE) {
			par -> result = RESULT_WIN;
			if(par -> steps > cur -> steps + 1) { par -> steps = cur -> steps + 1; }
		} else if(cur -> result == RESULT_WIN && par -> nextCount == 1) {
			par -> result = RESULT_LOSE;
			if(par -> steps < cur -> steps + 1) { par -> steps = cur -> steps + 1; }
		}
	}
	treenode[0] -> winCount += winCountArr[0];

	// メモリ解放
	for(int i = 0; i <= depth; i++) { delete boards[i]; }
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

	/*
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
	*/

	// rollout のテスト
	Board *board = new Board();
	rollout(board);
	TreeNode *current = this;
	int depth = 0;
	int tflag = 0;
	while(1) {
		printf("depth : %d\n", depth);
		printf(
			"result : %s (steps : %d), win : (%d / %d(%d))\n",
			(current -> result == RESULT_WIN ? "WIN" : current -> result == RESULT_LOSE ? "LOSE" : current -> result == RESULT_DRAW ? "DRAW" : "UNKNOWN"),
			(current -> result == RESULT_WIN || current -> result == RESULT_LOSE ? current -> steps : -1),
			current -> winCount,
			current -> totalCount,
			current -> totalCountOrig
		);
		board -> output(tflag);

		bool found = false;
		for(int i = 0; i < current -> nextCount; i++) {
			if(current -> nextNode[i] -> totalCount > 0 || current -> nextNode[i] -> steps == 0) {
				found = true;
				board -> put(current -> nextPiece[i]);
				board -> normalize();
				char buf[5];
				if(current -> nextPiece[i].flipInv(tflag).getString(buf)) {
					printf("[%s] tflag : %d%d%d, ", buf, tflag >> 2 & 1, tflag >> 1 & 1, tflag & 1);
					printf("nextIndex : (%d / %d), pindex : %d\n\n", i, current -> nextCount, current -> nextPieceIndex[i]);
				}
				tflag = Board::calcTurnFlag(tflag, current -> turnFlag[i]);
				current = current -> nextNode[i];
				depth++;
				break;
			}
		}
		if(!found) { break; }
	}
}
