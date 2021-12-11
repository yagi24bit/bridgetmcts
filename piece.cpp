#include "piece.h"

#define PIECE_EMPTY {0, 0, 0, 0}

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
