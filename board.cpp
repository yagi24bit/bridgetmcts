#include <stdio.h>
#include <string.h>
#include "board.h"

Piece Board::ALL_PIECES[ALL_PIECE_PATTERNS];
int Board::ALL_PIECES_INDEX[PIECE_TYPES][4][BOARD_Y][BOARD_X];

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
	memset(ALL_PIECES_INDEX, (unsigned char)0xFF, sizeof(int) * PIECE_TYPES * 4 * BOARD_Y * BOARD_X);

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
					ALL_PIECES_INDEX[t][d][y][x_temp] = index;
					ALL_PIECES[index] = p_temp;
					index++;

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

// 駒を盤面に配置 (種類と座標指定)
bool Board::put(int type, int dir, int y, int x, int c) {
	if(type < 0 || type >= PIECE_TYPES) { return false; }
	if(dir < 0 || dir >= 4) { return false; }
	if(x < 0 || x >= BOARD_X) { return false; }
	if(y < 0 || y >= BOARD_Y) { return false; }

	int i = ALL_PIECES_INDEX[type][dir][y][x];
	if(i == -1) { return false; }

	return put(ALL_PIECES[i], c);
}

// 駒を盤面に配置 (棋譜形式)
bool Board::put(char *s, int c) {
		if(s == NULL || strlen(s) < 4) { return false; }

	int type, dir; // 種類, 向き
	int y = s[0] - '1', x = s[1] - '1'; // X 座標, Y 座標

	switch(s[2]) {
		case 'L': case 'l': type =  0; break;
		case 'J': case 'j': type =  1; break;
		case 'P': case 'p': type =  2; break;
		case 'H': case 'h': type =  3; break;
		case 'B': case 'b': type =  4; break;
		case 'C': case 'c': type =  5; break;
		case 'O': case 'o': type =  6; break;
		case 'W': case 'w': type =  7; break;
		case 'S': case 's': type =  8; break;
		case 'Z': case 'z': type =  9; break;
		case 'D': case 'd': type = 10; break;
		case 'A': case 'a': type = 11; break;
		case 'T': case 't': type = 12; break;
		case 'V': case 'v': type = 13; break;
		case 'U': case 'u': type = 14; break;
		case 'G': case 'g': type = 15; break;
		default:            return false;
	}

	switch(s[3]) {
		case 'E': case 'e': dir = 0; break;
		case 'S': case 's': dir = 1; break;
		case 'W': case 'w': dir = 2; break;
		case 'N': case 'n': dir = 3; break;
		default:            return false;
	}

	put(type, dir, y, x, c);
}

// 左右反転 (1 変数)
t_board Board::fliph1(t_board b) {
	b = (b & 0xF0F0F0F0F0F0F0F0ull) >> 4 | (b & 0x0F0F0F0F0F0F0F0Full) << 4;
	b = (b & 0xCCCCCCCCCCCCCCCCull) >> 2 | (b & 0x3333333333333333ull) << 2;
	b = (b & 0xAAAAAAAAAAAAAAAAull) >> 1 | (b & 0x5555555555555555ull) << 1;
	return b;
}

// 左右反転
void Board::fliph() {
	board0 = fliph1(board0);
	board1 = fliph1(board1);
	board2 = fliph1(board2);
	color  = fliph1(color);
}

// 上下反転 (1 変数)
t_board Board::flipv1(t_board b) {
	#ifdef _MSC_VER
	return _byteswap_uint64(b);
	#else
	return __builtin_bswap64(b);
	#endif
}

// 上下反転
void Board::flipv() {
	board0 = flipv1(board0);
	board1 = flipv1(board1);
	board2 = flipv1(board2);
	color  = flipv1(color);
}

// XY 軸反転 (1 変数)
t_board Board::flipxy1(t_board b) {
	t_board t;
	t  = (b ^ (b << 28)) & 0x0F0F0F0F00000000ull;
	b ^=  t ^ (t >> 28);
	t  = (b ^ (b << 14)) & 0x3333000033330000ull;
	b ^=  t ^ (t >> 14);
	t  = (b ^ (b <<  7)) & 0x5500550055005500ull;
	b ^=  t ^ (t >>  7);
	return b;
}

// XY 軸反転
void Board::flipxy() {
	board0 = flipxy1(board0);
	board1 = flipxy1(board1);
	board2 = flipxy1(board2);
	color  = flipxy1(color);
}

// 正規化
int Board::normalize() {
	t_board board0_min = 0xFFFFFFFFFFFFFFFFull, board1_min = 0xFFFFFFFFFFFFFFFFull, board2_min = 0xFFFFFFFFFFFFFFFFull, color_min = 0xFFFFFFFFFFFFFFFFull;
	int ret = 0;

	for(int z = 0; z <= 1; z++) {
		for(int y = 0; y <= 1; y++) {
			for(int x = 0; x <= 1; x++) {
				if(
					board2 < board2_min
					|| (board2 == board2_min && board1 < board1_min)
					|| (board2 == board2_min && board1 == board1_min && board0 < board0_min)
					|| (board2 == board2_min && board1 == board1_min && board0 == board0_min && color < color_min)
				) {
					board0_min = board0;
					board1_min = board1;
					board2_min = board2;
					color_min = color;
					ret = z << 2 | y << 1 | x;
				}

				fliph();
			}
			flipv();
		}

		if(z == 0) { flipxy(); }
	}

	board0 = board0_min;
	board1 = board1_min;
	board2 = board2_min;
	color = color_min;

	return ret;
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
	/*
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
	*/

	/*
	// 棋譜形式の置き方チェック
	const char* kifu[] = {"44WS", "45CW", "43VS", "42CE", "46BS", "55GE", "66CN", "65WS", "64CE", "63GN", "52CE", "51VS", "62GW", "71CN", "82VE", "35WE", "27WS", "73VE", "77VS", "12ZS", "13DW", "15DW", "84DW", "23DW"};
	for(int i = 0; i < sizeof(kifu) / sizeof(const char*); i++) {
		put((char*)kifu[i], i % 2);
		output();
	}
	*/

	// 正規化
	board0 = 0x0008181CFC3E6800;
	board1 = 0x000818187C3E0800;
	board2 = 0x00000818080E0000;
	color  = 0xAA55AA55AA55AA55;
	output();
	int r = normalize();
	output();
	printf("rotate = (%d, %d, %d)\n", r & 1, r >> 1 & 1, r >> 2 & 1);
}
