#define wOutpost 0x00007e7e7e000000
#define bOutpost 0x0000007e7e7e0000

#define Bitboard2(x,y) (1ULL << (x))|(1ULL << (y))
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

#define SCORE(x,y) (( (x) << 16) + (y))

#define PawnAntiMobility SCORE(3, 10)
#define MobQ(Y) SCORE(2, 2) * POPCNT(Y)
#define MobB(Y, Z) SCORE (5, 5) * POPCNT( Y & Z)
#define MobR(Y) SCORE(2, 3) * POPCNT(Y)
#define MobN(Y, Z) SCORE (6, 8) * POPCNT(Y & Z)

#define xrayB0 SCORE(0, 0)
#define xrayBmP SCORE(3, 5)
#define xrayBmN SCORE(3, 5)
#define xrayBmK SCORE(3, 5)
#define xrayBmB SCORE(0, 0)
#define xrayBmR SCORE(3, 5)
#define xrayBmQ SCORE(0, 0)

#define xrayBoP SCORE(2, 5)
#define xrayBoN SCORE(2, 5)
#define xrayBoK SCORE(0, 0)
#define xrayBoB SCORE(0, 0)
#define xrayBoR SCORE(15, 25)
#define xrayBoQ SCORE(10, 20)

#define xrayR0 SCORE(0, 0)
#define xrayRmP SCORE(0, 0)
#define xrayRmN SCORE(3, 5)
#define xrayRmK SCORE(3, 5)
#define xrayRmB SCORE(3, 5)
#define xrayRmR SCORE(0, 0)
#define xrayRmQ SCORE(0, 0)

#define xrayRoP SCORE(2, 5)
#define xrayRoN SCORE(2, 5)
#define xrayRoK SCORE(0, 0)
#define xrayRoB SCORE(2, 5)
#define xrayRoR SCORE(0, 0)
#define xrayRoQ SCORE(10, 20)

static const uint32 wBxray[16] =
    {
    xrayB0, xrayBmP, xrayBmN, xrayBmK, xrayBmB, xrayBmB, xrayBmR, xrayBmQ, xrayB0, xrayBoP, xrayBoN, xrayBoK, xrayBoB,
        xrayBoB, xrayBoR, xrayBoQ
    };
static const uint32 bBxray[16] =
    {
    xrayB0, xrayBoP, xrayBoN, xrayBoK, xrayBoB, xrayBoB, xrayBoR, xrayBoQ, xrayB0, xrayBmP, xrayBmN, xrayBmK, xrayBmB,
        xrayBmB, xrayBmR, xrayBmQ
    };
static const uint32 wRxray[16] =
    {
    xrayR0, xrayRmP, xrayRmN, xrayRmK, xrayRmB, xrayRmB, xrayRmR, xrayRmQ, xrayR0, xrayRoP, xrayRoN, xrayRoK, xrayRoB,
        xrayRoB, xrayRoR, xrayRoQ
    };
static const uint32 bRxray[16] =
    {
    xrayR0, xrayRoP, xrayRoN, xrayRoK, xrayRoB, xrayRoB, xrayRoR, xrayRoQ, xrayR0, xrayRmP, xrayRmN, xrayRmK, xrayRmB,
        xrayRmB, xrayRmR, xrayRmQ
    };

#define xQ0d SCORE(0, 0)
#define xQmPd SCORE(1, 2)
#define xQmNd SCORE(2, 4)
#define xQmKd SCORE(2, 4)
#define xQmBd SCORE(0, 0)
#define xQmRd SCORE(2, 4)
#define xQmQd SCORE(0, 0)

#define xQoPd SCORE(0, 0)
#define xQoNd SCORE(2, 5)
#define xQoKd SCORE(0, 0)
#define xQoBd SCORE(0, 0)
#define xQoRd SCORE(2, 5)
#define xQoQd SCORE(0, 0)

#define xQ0hv SCORE(0, 0)
#define xQmPhv SCORE(0, 0)
#define xQmNhv SCORE(2, 4)
#define xQmKhv SCORE(2, 4)
#define xQmBhv SCORE(2, 4)
#define xQmRhv SCORE(0, 0)
#define xQmQhv SCORE(0, 0)

#define xQoPhv SCORE(0, 0)
#define xQoNhv SCORE(2, 5)
#define xQoKhv SCORE(0, 0)
#define xQoBhv SCORE(2, 5)
#define xQoRhv SCORE(0, 0)
#define xQoQhv SCORE(0, 0)

static const uint32 wQxrayD[16] =
    {
    xQ0d, xQmPd, xQmNd, xQmKd, xQmBd, xQmBd, xQmRd, xQmQd, xQ0d, xQoPd, xQoNd, xQoKd, xQoBd, xQoBd, xQoRd, xQoQd
    };
static const uint32 bQxrayD[16] =
    {
    xQ0d, xQoPd, xQoNd, xQoKd, xQoBd, xQoBd, xQoRd, xQoQd, xQ0d, xQmPd, xQmNd, xQmKd, xQmBd, xQmBd, xQmRd, xQmQd
    };
static const uint32 wQxrayO[16] =
    {
    xQ0hv, xQmPhv, xQmNhv, xQmKhv, xQmBhv, xQmBhv, xQmRhv, xQmQhv, xQ0hv, xQoPhv, xQoNhv, xQoKhv, xQoBhv, xQoBhv,
        xQoRhv, xQoQhv
    };
static const uint32 bQxrayO[16] =
    {
    xQ0hv, xQoPhv, xQoNhv, xQoKhv, xQoBhv, xQoBhv, xQoRhv, xQoQhv, xQ0hv, xQmPhv, xQmNhv, xQmKhv, xQmBhv, xQmBhv,
        xQmRhv, xQmQhv
    };
static const uint8 BishopTrapSq[64] =
    {
    0x00, C2, 0x00, 0x00, 0x00, 0x00, F2, 0x00, B3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, G3, B4, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, G4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        B5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, G5, B6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, G6, 0x00, C7, 0x00, 0x00,
        0x00, 0x00, F7, 0x00
    };

#define BishopTrapValue SCORE(40, 40)
#define BishopTrapGuardValue SCORE(40, 40)

static const uint8 GoodBishopTrapSq[64] =
    {
    0x00, D1, 0x00, 0x00, 0x00, 0x00, E1, 0x00, C2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, F2, C3, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, F3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        C6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, F6, C7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, F7, 0x00, D8, 0x00, 0x00,
        0x00, 0x00, E8, 0x00
    };
static const uint32 PassedPawnMeClear[8] =
    {
    0, 0, 0, SCORE(0, 0), SCORE(0, 0), SCORE(3, 5), SCORE(5, 10), 0
    };
static const uint32 PassedPawnOppClear[8] =
    {
    0, 0, 0, SCORE(0, 0), SCORE(5, 10), SCORE(15, 30), SCORE(25, 50)
    };
static const uint32 PassedPawnCanMove[8] =
    {
    0, 0, 0, SCORE(1, 2), SCORE(2, 3), SCORE(3, 5), SCORE(5, 10), 0
    };
static const uint32 PassedPawnIsFree[8] =
    {
    0, 0, 0, SCORE(0, 0), SCORE(5, 10), SCORE(10, 20), SCORE(20, 40)
    };

#define QguardK SCORE(5, 2)
#define RguardK SCORE(3, 1)
#define BguardK SCORE(2, 1)
#define NguardK SCORE(4, 2)

#define DoubQueen7th SCORE(10, 15)
#define DoubRook7thKingPawn SCORE(10, 20)
#define MultipleAtt SCORE(15, 25)
#define Queen7th SCORE(5, 25)
#define KingAttUnguardedPawn SCORE(0, 5)

#define PattQ SCORE(8, 12)
#define RattQ SCORE(5, 5)
#define NattRQ SCORE(7, 10)
#define bAttRQ SCORE(7, 10)
#define PattR SCORE(7, 10)
#define PattN SCORE(5, 7)
#define PattB SCORE(5, 7)
#define bAttN SCORE(5, 5)
#define NattB SCORE(5, 5)
#define Qatt SCORE(4, 4)
#define RattBN SCORE(4, 5)
#define RattP SCORE(2, 3)
#define NattP SCORE(3, 4)
#define bAttP SCORE(3, 4)

#define RookHalfOpen SCORE(3, 6)

#define RookOpenFile SCORE(20, 10)
#define RookOpenFixedMinor SCORE(10, 0)
#define RookOpenMinor SCORE(15, 5)

#define RookHalfOpenPawn SCORE(5, 5)
#define RookHalfOpenKing SCORE(15, 0)
#define RookKing8th SCORE(5, 10)
#define Rook7thKingPawn SCORE(10, 30)
#define Rook6thKingPawn SCORE(5, 15)

#define OutpostBishop SCORE(1, 2)
#define OutpostBishopGuarded SCORE(3, 4)
#define OutpostRook SCORE(1, 2)
#define OutpostRookGuarded SCORE(3, 4)
#define OutpostKnight SCORE(2, 3)
#define OutpostKnightPawn SCORE(2, 3)
#define OutpostKnightAttacks SCORE(5, 5)
#define OutpostKnight5th SCORE(2, 2)
#define OutpostKnightONde SCORE(3, 3)

static const uint32 KingSafetyMult[16] =
    {
    0, 1, 4, 9, 16, 25, 36, 49, 50, 50, 50, 50, 50, 50, 50, 50
    };
#define Hit(x,y) ( (x) << 16) + (y)
#define HitP Hit(1, 0)
#define HitQ Hit(1, 40)
#define HitR Hit(1, 25)
#define HitN Hit(1, 15)
#define HitB Hit(1, 15)
#define HitK Hit(0, 0)
#define KingSafetyDivider 8

static const uint32 RankQueenEnd[8] =
    {
    0, 0, 0, SCORE(5, 5), SCORE(10, 10), SCORE(20, 20), SCORE(40, 40), 0
    };

#define Rook7thEnd SCORE(100, 100)
#define Rook6thEnd SCORE(25, 25)
#define Queen7thEnd SCORE(10, 10)

static const uint64 CrampFile[8] =
    {
    FILEb, 0, 0, 0, 0, 0, 0, FILEg
    };
