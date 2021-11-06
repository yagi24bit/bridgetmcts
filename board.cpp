#include <stdio.h>
#include <string.h>
#include "board.h"

Piece Board::ALL_PIECES[ALL_PIECE_PATTERNS];

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

void Board::initializeAllPieces() {
	// 初期化
	memset(ALL_PIECES, 0, sizeof(Piece) * ALL_PIECE_PATTERNS);

	int index = 0;
	for(int t = 0; t < PIECE_TYPES; t++) {
		for(int d = 0; d < 4; d++) {
			Piece p = DEFAULT_PIECES[t][d];
			if((p.piece0 | p.piece1 | p.piece2) == 0ull) { continue; }

			// まずできるだけ原点に近い方向にスライド
			int x = DEFAULT_PIECES_X;
			while((p.piece0 & 0x0101010101010101ull | p.piece1 & 0x0101010101010101ull | p.piece2 & 0x0101010101010101ull) == 0) {
				x--;
				p.piece0 >>= 1; p.piece1 >>= 1; p.piece2 >>= 1;
			}
			int y = DEFAULT_PIECES_Y;
			while((p.piece0 & 0x00000000000000FFull | p.piece1 & 0x00000000000000FFull | p.piece2 & 0x00000000000000FFull) == 0) {
				y--;
				p.piece0 >>= BOARD_X; p.piece1 >>= BOARD_X; p.piece2 >>= BOARD_X;
			}

			// 1 升ずつスライドしながら ALL_PIECES に代入
			while(true) {
				Piece p_temp = p;
				int x_temp = x;

				while(true) {
					ALL_PIECES[index++] = p_temp;

					if((p_temp.piece0 | p_temp.piece1 | p_temp.piece2) & 0x8080808080808080ull) { break; }
					// p_temp を x 方向にスライド
					p_temp.piece0 <<= 1;
					p_temp.piece1 <<= 1;
					p_temp.piece2 <<= 1;
					x_temp++;
				}

				if((p.piece0 | p.piece1 | p.piece2) & 0xFF00000000000000ull) { break; }
				// p を y 方向にスライド
				p.piece0 <<= BOARD_X;
				p.piece1 <<= BOARD_X;
				p.piece2 <<= BOARD_X;
				y++;
			}
		}
	}
}

// デバッグ出力
void Board::output() {
	const char *block[] = {" . ", "[1]", "[2]", "[3]", " . ", "<1>", "<2>", "<3>"};
	const char *number[] = {"   ", " 1 ", " 2 ", " 3 ", " 4 ", " 5 ", " 6 ", " 7 ", " 8 "};

	for(int x = 0; x <= BOARD_X; x++) { printf("%s", number[x]); }
	printf(" ");
	for(int i = 0; i < 3; i++) {
		printf("   ");
		for(int x = 0; x < BOARD_X; x++) { printf(" %d", x + 1); }
	}
	printf("\n");

	for(int y = 0; y < BOARD_Y; y++) {
		printf(number[y + 1]);

		for(int x = 0; x < BOARD_X; x++) {
			int b = y * BOARD_X + x;
			int h = (int)((board0 >> b & 1) + (board1 >> b & 1) + (board2 >> b & 1)); // 高さ
			int c = (int)(color >> b & 1); // 色

			printf(block[c << 2 | h]);
		}

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
		printf("\n");
	}
}

// 駒を盤面に配置
bool Board::put(Piece p, int c) {
	// 既存の駒と重なる置き方は禁止
	if(board0 & p.piece0 | board1 & p.piece1 | board2 & p.piece2) { return false; }
	// 駒の下に空白ができる置き方は禁止
	if(~(board0 | p.piece0) & p.piece1 | ~(board1 | p.piece1) & p.piece2) { return false; }

	board0 |= p.piece0;
	board1 |= p.piece1;
	board2 |= p.piece2;
	if(c == 0) {
		color &= ~(p.piece0 | p.piece1 | p.piece2);
	} else {
		color |= (p.piece0 | p.piece1 | p.piece2);
	}
	return true;
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

void Board::test() {
	/*
	// 初期状態で置ける駒を列挙
	for(int i = 0; i < PIECE_TYPES; i++) {
		for(int j = 0; j < DEFAULT_PIECE_TURNS[i]; j++) {
			board0 = 0; board1 = 0; board2 = 0;
			if(put(DEFAULT_PIECES[i][j], 0)) { output(); }
		}
	}
	*/

	/*
	// 空中に置けないチェック
	put(default_pieces[5][3], 0);
	output();
	Piece w = default_pieces[7][0];
	w.piece0 <<= 1; w.piece1 <<= 1; w.piece2 <<= 1;
	put(w, 1);
	output();
	put(default_pieces[5][3], 0);
	output();
	*/

	// 駒の全パターン列挙
	int count = 0;
	for(int i = 0; i < ALL_PIECE_PATTERNS; i++) {
		Piece p = ALL_PIECES[i];
		board0 = 0; board1 = 0; board2 = 0;
		if(put(p, 0)) {
			// output();
			count++;
		}
	}
	printf("count = %d\n", count);
}
