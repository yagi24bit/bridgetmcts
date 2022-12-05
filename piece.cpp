#include <string.h>
#include "piece.h"

#define PIECE_TYPE_EMPTY 0xFF
#define PIECE_EMPTY {PIECE_TYPE_EMPTY, 0, 0, 0}
#define DEFAULT_PIECES_X 2
#define DEFAULT_PIECES_Y 2

Piece Piece::EMPTY;
Piece Piece::ALL_PIECES[ALL_PIECE_PATTERNS];
int Piece::ALL_PIECES_INDEX[PIECE_PUT_TYPES][4][PIECE_Y][PIECE_X];
int Piece::ALL_PIECES_TYPE[ALL_PIECE_PATTERNS];
int Piece::ALL_PIECES_DIR[ALL_PIECE_PATTERNS];
int Piece::ALL_PIECES_Y[ALL_PIECE_PATTERNS];
int Piece::ALL_PIECES_X[ALL_PIECE_PATTERNS];

// NOTE: 中心は (x, y) = (2, 2)、E→S→W→N の順
const Piece DEFAULT_PIECES[PIECE_PUT_TYPES][4] = {
	{{0, 0x0C04040000, 0, 0}, {0, 0x01070000, 0, 0}, {0, 0x040406, 0, 0}, {0, 0x1C1000, 0, 0}}, // L
	{{0, 0x04040C, 0, 0}, {0, 0x101C0000, 0, 0}, {0, 0x0604040000, 0, 0}, {0, 0x070100, 0, 0}}, // J
	{{0, 0x1C0000, 0x040000, 0}, {0, 0x0404040000, 0x040000, 0}, {0, 0x070000, 0x040000, 0}, {0, 0x040404, 0x040000, 0}}, // Pelican
	{{0, 0x040000, 0x1c0000, 0}, {0, 0x040000, 0x0404040000, 0}, {0, 0x040000, 0x070000, 0}, {0, 0x040000, 0x040404, 0}}, // Handle
	{{0, 0x0C0000, 0x040000, 0x040000}, {0, 0x04040000, 0x040000, 0x040000}, {0, 0x060000, 0x040000, 0x040000}, {0, 0x040400, 0x040000, 0x040000}}, // Boot
	{{0, 0x040000, 0x040000, 0x0C0000}, {0, 0x040000, 0x040000, 0x04040000}, {0, 0x040000, 0x040000, 0x060000}, {0, 0x040000, 0x040000, 0x040400}}, // Crane
	{{1, 0x0C0C0000, 0, 0}, PIECE_EMPTY, PIECE_EMPTY, PIECE_EMPTY}, // O
	{{1, 0x0C0000, 0x0C0000, 0}, {1, 0x04040000, 0x04040000, 0}, PIECE_EMPTY, PIECE_EMPTY},  // Wall
	{{2, 0x0C1800, 0, 0}, {2, 0x080C040000, 0, 0}, PIECE_EMPTY, PIECE_EMPTY}, // S
	{{2, 0x180C0000, 0, 0}, {2, 0x0206040000, 0, 0}, PIECE_EMPTY, PIECE_EMPTY}, // Z
	{{2, 0x060000, 0x0C0000, 0}, {2, 0x040400, 0x04040000, 0}, {2, 0x0C0000, 0x060000, 0}, {2, 0x04040000, 0x040400, 0}}, // Duck
	{{2, 0x040000, 0x0C0000, 0x080000}, {2, 0x040000, 0x04040000, 0x04000000}, {2, 0x040000, 0x060000, 0x020000}, {2, 0x040000, 0x040400, 0x0400}}, // Axe
	{{3, 0x040C0400}, {3, 0x040E0000, 0, 0}, {3, 0x04060400, 0, 0}, {3, 0x0E0400, 0, 0}}, // T
	{{3, 0x0E0000, 0x040000, 0}, {3, 0x04040400, 0x040000, 0}, PIECE_EMPTY, PIECE_EMPTY}, // Vessel
	{{3, 0x040000, 0x0E0000, 0}, {3, 0x040000, 0x04040400, 0}, PIECE_EMPTY, PIECE_EMPTY}, // Umbrella
	{{3, 0x040000, 0x0C0000, 0x040000}, {3, 0x040000, 0x04040000, 0x040000}, {3, 0x040000, 0x060000, 0x040000}, {3, 0x040000, 0x040400, 0x040000}}, // Guide Post
};

const int DEFAULT_PIECE_TURNS[PIECE_PUT_TYPES] = {
	4, 4, 4, 4, 4, 4, // L
	1, 2,             // O
	2, 2, 4, 4,       // S
	4, 2, 2, 4,       // T
};

void Piece::init() {
	EMPTY = PIECE_EMPTY;

	// 初期化
	memset(ALL_PIECES, 0, sizeof(Piece) * ALL_PIECE_PATTERNS);
	memset(ALL_PIECES_INDEX, (unsigned char)0xFF, sizeof(int) * PIECE_PUT_TYPES * 4 * PIECE_Y * PIECE_X);

	int index = 0;
	for(int t = 0; t < PIECE_PUT_TYPES; t++) {
		for(int d = 0; d < 4; d++) {
			Piece p = DEFAULT_PIECES[t][d];
			if(p.isEmpty()) { continue; }

			// まずできるだけ原点に近い方向にスライド
			int x = DEFAULT_PIECES_X;
			while((p.piece0 & 0x0101010101010101ull | p.piece1 & 0x0101010101010101ull | p.piece2 & 0x0101010101010101ull) == 0) {
				x--;
				p.piece0 >>= 1; p.piece1 >>= 1; p.piece2 >>= 1;
			}
			int y = DEFAULT_PIECES_Y;
			while((p.piece0 & 0x00000000000000FFull | p.piece1 & 0x00000000000000FFull | p.piece2 & 0x00000000000000FFull) == 0) {
				y--;
				p.piece0 >>= PIECE_X; p.piece1 >>= PIECE_X; p.piece2 >>= PIECE_X;
			}

			// 1 升ずつスライドしながら ALL_PIECES に代入
			while(true) {
				Piece p_temp = p;
				int x_temp = x;

				while(true) {
					ALL_PIECES_INDEX[t][d][y][x_temp] = index;
					ALL_PIECES[index] = p_temp;
					ALL_PIECES_TYPE[index] = t;
					ALL_PIECES_DIR[index] = d;
					ALL_PIECES_Y[index] = y;
					ALL_PIECES_X[index] = x_temp;
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
				p.piece0 <<= PIECE_X;
				p.piece1 <<= PIECE_X;
				p.piece2 <<= PIECE_X;
				y++;
			}
		}
	}
}

bool Piece::isEmpty() {
	return (type == PIECE_TYPE_EMPTY);
}

bool Piece::equals(Piece p) {
	return (
		   piece0 == p.piece0
		&& piece1 == p.piece1
		&& piece2 == p.piece2
	);
}

int Piece::getIndex() {
	if(isEmpty()) { return -1; }

	// NOTE: パフォーマンスは悪いが、呼び出される頻度が低いので気にしない
	for(int i = 0; i < ALL_PIECE_PATTERNS; i++) {
		if(equals(ALL_PIECES[i])) { return i; }
	}

	return -1;
}

Piece Piece::get(int index) {
	return ALL_PIECES[index];
}

Piece Piece::get(int y, int x, int type, int dir) {
	if(type < 0 || type >= PIECE_PUT_TYPES) { return EMPTY; }
	if(dir < 0 || dir >= 4) { return EMPTY; }
	if(x < 0 || x >= PIECE_X) { return EMPTY; }
	if(y < 0 || y >= PIECE_Y) { return EMPTY; }

	return get(ALL_PIECES_INDEX[type][dir][y][x]);
}

Piece Piece::get(char *s) {
	if(s == nullptr || strlen(s) < 4) { return EMPTY; }

	int y = s[0] - '1', x = s[1] - '1'; // Y 座標, X 座標
	int type, dir; // 種類, 向き

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
		default:            return EMPTY;
	}

	switch(s[3]) {
		case 'E': case 'e': dir = 0; break;
		case 'S': case 's': dir = 1; break;
		case 'W': case 'w': dir = 2; break;
		case 'N': case 'n': dir = 3; break;
		default:            return EMPTY;
	}

	return get(y, x, type, dir);
}

int Piece::getNumberOfPieces(int type) {
	if(type < 0 || type >= PIECE_TYPES) { return 0; }

	const int ALL_PIECES_COUNT[] = {1168, 161, 584, 584};
	return ALL_PIECES_COUNT[type];
}

// 反転したときの駒の種類
const int FLIPPED_PIECE[PIECE_PUT_TYPES] = {1, 0, 2, 3, 4, 5, 6, 7, 9, 8, 10, 11, 12, 13, 14, 15}; // L と J が入れ替え、S と Z が入れ替え、それ以外はそのまま

// 左右反転
void fliph(int *y, int *x, int *type, int *dir) {
	const int FLIPPED_DIR[4] = {2, 1, 0, 3}; // 0 と 2 が入れ替え、1 と 3 はそのまま

	*x = PIECE_X - 1 - *x;
	*type = FLIPPED_PIECE[*type];
	*dir = FLIPPED_DIR[*dir] % DEFAULT_PIECE_TURNS[*type];

	if(*type == 6 || (*type == 7 && *dir == 0)) { (*x)--; } // O と WE は X 座標を調整
	if((*type == 8 || *type == 9) && *dir == 0) { (*x) -= 2; (*y) += (*type == 8 ? 1 : -1); } // SE と ZE は座標を調整
}

// 上下反転
void flipv(int *y, int *x, int *type, int *dir) {
	const int FLIPPED_DIR[4] = {0, 3, 2, 1}; // 1 と 3 が入れ替え、0 と 2 はそのまま

	*y = PIECE_Y - 1 - *y;
	*type = FLIPPED_PIECE[*type];
	*dir = FLIPPED_DIR[*dir] % DEFAULT_PIECE_TURNS[*type];

	if(*type == 6 || (*type == 7 && *dir == 1)) { (*y)--; } // O と WS は Y 座標を調整
	if((*type == 8 || *type == 9) && *dir == 1) { (*x) += (*type == 8 ? -1 : 1); (*y) -= 2; } // SS と ZS は座標を調整
}

// XY 軸反転
void flipxy(int *y, int *x, int *type, int *dir) {
	int temp = *y;
	*y = *x;
	*x = temp;
	*type = FLIPPED_PIECE[*type];
	*dir = (*dir ^ 1) % DEFAULT_PIECE_TURNS[*type]; // 0 と 1 が入れ替え、2 と 3 が入れ替え
}

// 回転フラグに従って回転する
Piece Piece::flip(int tflag) {
	int index = getIndex();
	int y = ALL_PIECES_Y[index];
	int x = ALL_PIECES_X[index];
	int type = ALL_PIECES_TYPE[index];
	int dir = ALL_PIECES_DIR[index];

	if(tflag & 4) { flipxy(&y, &x, &type, &dir); } // XY 軸反転
	if(tflag & 2) { flipv(&y, &x, &type, &dir); } // 上下反転
	if(tflag & 1) { fliph(&y, &x, &type, &dir); } // 左右反転

	return get(y, x, type, dir);
}

// 回転した状態から元に戻す
Piece Piece::flipInv(int tflag) {
	int index = getIndex();
	int y = ALL_PIECES_Y[index];
	int x = ALL_PIECES_X[index];
	int type = ALL_PIECES_TYPE[index];
	int dir = ALL_PIECES_DIR[index];

	if(tflag & 1) { fliph(&y, &x, &type, &dir); } // 左右反転
	if(tflag & 2) { flipv(&y, &x, &type, &dir); } // 上下反転
	if(tflag & 4) { flipxy(&y, &x, &type, &dir); } // XY 軸反転

	return get(y, x, type, dir);
}

// 棋譜用の文字列を取得
bool Piece::getString(char *str) {
	const char TYPES[16] = {'L', 'J', 'P', 'H', 'B', 'C', 'O', 'W', 'S', 'Z', 'D', 'A', 'T', 'V', 'U', 'G'};
	const char DIRECTIONS[4] = {'E', 'S', 'W', 'N'};

	if(str == nullptr) { return false; }
	int index = getIndex();
	if(index < 0) { return false; }

	int y = ALL_PIECES_Y[index];
	int x = ALL_PIECES_X[index];
	int type = ALL_PIECES_TYPE[index];
	int dir = ALL_PIECES_DIR[index];

	str[0] = y + '1';
	str[1] = x + '1';
	str[2] = TYPES[type];
	if(type == 6) {
		str[3] = '\0'; // O のみ、向きが無いため省略
	} else {
		str[3] = DIRECTIONS[dir];
		str[4] = '\0';
	}

	return true;
}



// 初期化処理を自動で実行
class PieceInitializer {
public:
	PieceInitializer() {
		Piece::init();
	}
} _pieceInitializer;
