#include <stdio.h>
#include <string.h>
#include "board.h"

// コンストラクタ
Board::Board() {
	// ボード初期化
	board0 = 0x0000000000000000ull;
	board1 = 0x0000000000000000ull;
	board2 = 0x0000000000000000ull;
	color  = 0x0000000000000000ull;
}

// デストラクタ
Board::~Board() {
}

// デバッグ出力
void Board::output() {
	const char *block[] = {" . ", "[1]", "[2]", "[3]", " . ", "<1>", "<2>", "<3>"};
	const char *number[] = {"   ", " 1 ", " 2 ", " 3 ", " 4 ", " 5 ", " 6 ", " 7 ", " 8 "};

	for(int x = 0; x <= BOARD_X; x++) { printf("%s", number[x]); }
	// 暫定 (ここから)
	printf(" ");
	for(int i = 0; i < 3; i++) {
		printf("   ");
		for(int x = 0; x < BOARD_X; x++) { printf(" %d", x + 1); }
	}
	// 暫定 (ここまで)
	printf("\n");

	for(int y = 0; y < BOARD_Y; y++) {
		printf(number[y + 1]);

		for(int x = 0; x < BOARD_X; x++) {
			int b = y * BOARD_X + x;
			int h = (int)((board0 >> b & 1) + (board1 >> b & 1) + (board2 >> b & 1)); // 高さ
			int c = (int)(color >> b & 1); // 色

			printf(block[c << 2 | h]);
		}

		// 暫定 (ここから)
		printf("   %d", y + 1);
		for(int x = 0; x < BOARD_X; x++) {
			int b = y * BOARD_X + x;
			printf(" %s", (board0 >> b & 1 ? "#" : "."));
		}
		printf("  %d", y + 1);
		for(int x = 0; x < BOARD_X; x++) {
			int b = y * BOARD_X + x;
			printf(" %s", (board1 >> b & 1 ? "#" : "."));
		}
		printf("  %d", y + 1);
		for(int x = 0; x < BOARD_X; x++) {
			int b = y * BOARD_X + x;
			printf(" %s", (board2 >> b & 1 ? "#" : "."));
		}
		// 暫定 (ここまで)
		printf("\n");
	}
}

// 勝利判定
bool Board::judge(int turn) {
	t_color board = board0 & (turn ? color : ~color); // プレーヤの駒が置かれている場所

	// 縦方向チェック (最下段に 1 つ以上ある場合のみ)
	t_color c = 0x0000000000000000ull;
	if(board & 0xff00000000000000ull) {
		t_color c_temp = board & 0x00000000000000ffull;

		while(c != c_temp) {
			c = c_temp;
			c_temp = board & (c | (c & 0xFEFEFEFEFEFEFEFEull) >> 1 | (c & 0x7F7F7F7F7F7F7F7F) << 1 | c >> BOARD_X | c << BOARD_X);
		}
		if(c & 0xff00000000000000ull) return true;
	}

	// 偶然横方向が繋がっていたらそこで探索終了
	if((c & 0x0101010101010101ull) && (c & 0x8080808080808080ull)) return true;

	// 横方向チェック (左端に 1 つ以上ある場合のみ)
	c = 0x0000000000000000ull;
	if(board & 0x8080808080808080ull) {
		t_color c_temp = board & 0x0101010101010101ull;

		while(c != c_temp) {
			c = c_temp;
			c_temp = board & (c | (c & 0xFEFEFEFEFEFEFEFEull) >> 1 | (c & 0x7F7F7F7F7F7F7F7F) << 1 | c >> BOARD_X | c << BOARD_X);
		}
		if(c & 0x8080808080808080ull) return true;
	}

	return false;
}
