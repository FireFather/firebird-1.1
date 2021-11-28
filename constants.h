
static const uint64 RankArray[8] =
	{
	0x00000000000000ff, 0x000000000000ff00,
	0x0000000000ff0000, 0x00000000ff000000,
	0x000000ff00000000, 0x0000ff0000000000,
	0x00ff000000000000, 0xff00000000000000
	};

static const uint64 FileArray[8] =
	{
	0x0101010101010101, 0x0202020202020202,
	0x0404040404040404, 0x0808080808080808,
	0x1010101010101010, 0x2020202020202020,
	0x4040404040404040, 0x8080808080808080
	};

static const uint64 Ranks2to6NOTa = 0x0000fefefefefe00;
static const uint64 Ranks2to6NOTab = 0x0000fcfcfcfcfc00;
static const uint64 Ranks2to6 = 0x0000ffffffffff00;
static const uint64 rRanks2to6NOTh = 0x00007f7f7f7f7f00;
static const uint64 Ranks2to6NOTgh = 0x00003f3f3f3f3f00;
static const uint64 Ranks3to7NOTa = 0x00fefefefefe0000;
static const uint64 Ranks3to7NOTab = 0x00fcfcfcfcfc0000;
static const uint64 Ranks3to7 = 0x00ffffffffffff0000;
static const uint64 Ranks3to7NOTgh = 0x003f3f3f3f3f0000;
static const uint64 Ranks3to7NOTh = 0x007f7f7f7f7f0000;
static const uint64 White = 0x55aa55aa55aa55aa;
static const uint64 Black = 0xaa55aa55aa55aa55;
static const uint64 Rank2NOTa = 0x000000000000fe00;
static const uint64 Rank2NOTh = 0x0000000000007f00;
static const uint64 Rank7NOTa = 0x00fe000000000000;
static const uint64 Rank7NOTh = 0x007f000000000000;
static const uint64 NOTa = 0xfefefefefefefefe;
static const uint64 NOTh = 0x7f7f7f7f7f7f7f7f;

static const uint64 F1G1 = 0x0000000000000060;
static const uint64 C1D1 = 0x000000000000000c;
static const uint64 B1C1D1 = 0x000000000000000e;
static const uint64 F8G8 = 0x6000000000000000;
static const uint64 C8D8 = 0x0c00000000000000;
static const uint64 B8C8D8 = 0x0e00000000000000;

static const uint8 Left90[64] =
	{
    7, 15, 23, 31, 39, 47, 55, 63,
    6, 14, 22, 30, 38, 46, 54, 62,
    5, 13, 21, 29, 37, 45, 53, 61,
    4, 12, 20, 28, 36, 44, 52, 60,
    3, 11, 19, 27, 35, 43, 51, 59,
    2, 10, 18, 26, 34, 42, 50, 58,
    1, 9, 17, 25, 33, 41, 49, 57,
    0, 8, 16, 24, 32, 40, 48, 56
	};

static const uint8 Left45[64] =
	{
    0, 2, 5, 9, 14, 20, 27, 35,
    1, 4, 8, 13, 19, 26, 34, 42,
    3, 7, 12, 18, 25, 33, 41, 48,
    6, 11, 17, 24, 32, 40, 47, 53,
    10, 16, 23, 31, 39, 46, 52, 57,
    15, 22, 30, 38, 45, 51, 56, 60,
    21, 29, 37, 44, 50, 55, 59, 62,
    28, 36, 43, 49, 54, 58, 61, 63
	};

static const uint8 Right45[64] =
	{
    28, 21, 15, 10, 6, 3, 1, 0,
    36, 29, 22, 16, 11, 7, 4, 2,
    43, 37, 30, 23, 17, 12, 8, 5,
    49, 44, 38, 31, 24, 18, 13, 9,
    54, 50, 45, 39, 32, 25, 19, 14,
    58, 55, 51, 46, 40, 33, 26, 20,
    61, 59, 56, 52, 47, 41, 34, 27,
    63, 62, 60, 57, 53, 48, 42, 35
	};

static const int Shift[64] =
	{
    1,
    2, 2,
    4, 4, 4,
    7, 7, 7, 7,
    11, 11, 11, 11, 11,
    16, 16, 16, 16, 16, 16,
    22, 22, 22, 22, 22, 22, 22,
    29, 29, 29, 29, 29, 29, 29, 29,
    37, 37, 37, 37, 37, 37, 37,
    44, 44, 44, 44, 44, 44,
    50, 50, 50, 50, 50,
    55, 55, 55, 55,
    59, 59, 59,
    62, 62,
    64
	};

static int Length[64], Where[64];

static const int Hop[8] =
    {
    6, 10, 15, 17, -6, -10, -15, -17
    };

static const uint32 MATERIAL_VALUE[16] =
    {
    0, I(0, 0x1440, 1), I(1, 0x240, 1), 0, I(1, 0x24, 1), I(1, 0x48, 1), I(3, 0x04, 1), I(6, 0x1, 1), 0,
	I(0, 0xb640, 1), I(1, 0x6c0, 1), 0, I(1, 0x90, 1), I(1, 0x120, 1), I(3, 0xc, 1), I(6, 0x2, 1)
    };

static const uint64 RookTrapped[64] =
	{
	  0, Bitboard2 (A1, A2), Bitboard2 (A1, A2) | Bitboard2 (B1, B2), 0,
	  0, Bitboard2 (H1, H2) | Bitboard2 (G1, G2), Bitboard2 (H1, H2), 0,
	  0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0,
	  0, 0, 0, 0, 0, 0, 0, 0,
	  0, Bitboard2 (A8, A7), Bitboard2 (A8, A7) | Bitboard2 (B8, B7), 0,
	  0, Bitboard2 (H8, H7) | Bitboard2 (G8, G7), Bitboard2 (H8, H7), 0
	};
static const uint8 BishopTrapSq[64] =
    {
	0x00,  C2,  0x00, 0x00, 0x00, 0x00,  F2,  0x00,
	B3,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  G3,
	B4,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  G4,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	B5,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  G5,
	B6,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  G6,
	0x00,  C7,  0x00, 0x00, 0x00, 0x00,  F7,  0x00
	};
static const uint8 GoodBishopTrapSq[64] =
    {
	0x00,  D1,  0x00, 0x00, 0x00, 0x00,  E1,  0x00,
	C2,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  F2,
	C3,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  F3,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	C6,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  F6,
	C7,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  F7,
	0x00,  D8,  0x00, 0x00, 0x00, 0x00,  E8,  0x00
    };
static const uint32 wBxray[16] =
    {
    xrayB0, xrayBmP, xrayBmN, xrayBmK, xrayBmB, xrayBmB, xrayBmR, xrayBmQ,
	xrayB0, xrayBoP, xrayBoN, xrayBoK, xrayBoB, xrayBoB, xrayBoR, xrayBoQ
    };
static const uint32 bBxray[16] =
    {
    xrayB0, xrayBoP, xrayBoN, xrayBoK, xrayBoB, xrayBoB, xrayBoR, xrayBoQ,
	xrayB0, xrayBmP, xrayBmN, xrayBmK, xrayBmB, xrayBmB, xrayBmR, xrayBmQ
    };
static const uint32 wRxray[16] =
    {
    xrayR0, xrayRmP, xrayRmN, xrayRmK, xrayRmB, xrayRmB, xrayRmR, xrayRmQ,
	xrayR0, xrayRoP, xrayRoN, xrayRoK, xrayRoB, xrayRoB, xrayRoR, xrayRoQ
    };
static const uint32 bRxray[16] =
    {
    xrayR0, xrayRoP, xrayRoN, xrayRoK, xrayRoB, xrayRoB, xrayRoR, xrayRoQ,
	xrayR0, xrayRmP, xrayRmN, xrayRmK, xrayRmB, xrayRmB, xrayRmR, xrayRmQ
    };
static const uint32 wQxrayD[16] =
    {
    xQ0d, xQmPd, xQmNd, xQmKd, xQmBd, xQmBd, xQmRd, xQmQd,
	xQ0d, xQoPd, xQoNd, xQoKd, xQoBd, xQoBd, xQoRd, xQoQd
    };
static const uint32 bQxrayD[16] =
    {
    xQ0d, xQoPd, xQoNd, xQoKd, xQoBd, xQoBd, xQoRd, xQoQd,
	xQ0d, xQmPd, xQmNd, xQmKd, xQmBd, xQmBd, xQmRd, xQmQd
    };
static const uint32 wQxrayO[16] =
    {
    xQ0hv, xQmPhv, xQmNhv, xQmKhv, xQmBhv, xQmBhv, xQmRhv, xQmQhv,
	xQ0hv, xQoPhv, xQoNhv, xQoKhv, xQoBhv, xQoBhv, xQoRhv, xQoQhv
    };
static const uint32 bQxrayO[16] =
    {
    xQ0hv, xQoPhv, xQoNhv, xQoKhv, xQoBhv, xQoBhv, xQoRhv, xQoQhv,
	xQ0hv, xQmPhv, xQmNhv, xQmKhv, xQmBhv, xQmBhv, xQmRhv, xQmQhv
    };
static const uint32 PassedPawnMeClear[8] =
    {
    0, 0, 0, Score(0, 0), Score(0, 0), Score(3, 5), Score(5, 10), 0
    };
static const uint32 PassedPawnOppClear[8] =
    {
    0, 0, 0, Score(0, 0), Score(5, 10), Score(15, 30), Score(25, 50)
    };
static const uint32 PassedPawnCanMove[8] =
    {
    0, 0, 0, Score(1, 2), Score(2, 3), Score(3, 5), Score(5, 10), 0
    };
static const uint32 PassedPawnIsFree[8] =
    {
    0, 0, 0, Score(0, 0), Score(5, 10), Score(10, 20), Score(20, 40)
    };
static const uint32 KingSafetyMult[16] =
    {
    0, 1, 4, 9, 16, 25, 36, 49, 50, 50, 50, 50, 50, 50, 50, 50
    };
static const uint32 RankQueenEnd[8] =
    {
    0, 0, 0, Score(5, 5), Score(10, 10), Score(20, 20), Score(40, 40), 0
    };
static const uint64 CrampFile[8] =
    {
    FILEb, 0, 0, 0, 0, 0, 0, FILEg
    };
static const uint64 CastleTable[64] =
    {
	Kkq, KQkq, KQkq, KQkq, kq, KQkq, KQkq, Qkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQk, KQkq, KQkq, KQkq, KQ, KQkq, KQkq, KQq
    };
const static uint64 WhiteEP[8] =
	{
	Bitboard2 (B4, B4), Bitboard2 (A4, C4),
    Bitboard2 (B4, D4), Bitboard2 (C4, E4),
    Bitboard2 (D4, F4), Bitboard2 (E4, G4),
    Bitboard2 (F4, H4), Bitboard2 (G4, G4)
	};
const static uint64 BlackEP[8] =
	{
	Bitboard2 (B5, B5), Bitboard2 (A5, C5),
    Bitboard2 (B5, D5), Bitboard2 (C5, E5),
    Bitboard2 (D5, F5), Bitboard2 (E5, G5),
    Bitboard2 (F5, H5), Bitboard2 (G5, G5)
	};
const static uint8 PromW[8] =
    {
    0, 0, 0, 0, wEnumN, wEnumBL, wEnumR, wEnumQ
    };
const static uint8 PromB[8] =
    {
    0, 0, 0, 0, bEnumN, bEnumBL, bEnumR, bEnumQ
    };

static const char StartPosition[80] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";