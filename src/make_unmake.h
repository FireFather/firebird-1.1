
#define Bitboard2(x, y) (1ULL << (x))|(1ULL << (y))
#define F1H1 Bitboard2 (F1, H1)
#define F1H1Left90 Bitboard2 (47, 63)
#define F1H1Left45 Bitboard2 (20, 35)
#define F1H1Right45 Bitboard2 (3, 0)
#define A1D1 Bitboard2 (A1, D1)
#define A1D1Left90 Bitboard2 (7, 31)
#define A1D1Left45 Bitboard2 (0, 9)
#define A1D1Right45 Bitboard2 (28, 10)
#define F8H8 Bitboard2 (F8, H8)
#define F8H8Left90 Bitboard2 (40, 56)
#define F8H8Left45 Bitboard2 (58, 63)
#define F8H8Right45 Bitboard2 (48, 35)
#define A8D8 Bitboard2 (A8, D8)
#define A8D8Left90 Bitboard2 (0, 24)
#define A8D8Left45 Bitboard2 (28, 49)
#define A8D8Right45 Bitboard2 (63, 57)

#define ClearOccupied(M, x)                                        \
  { POSITION->OccupiedBW &= M;                                     \
    POSITION->OccupiedL90 &= ClearL90[x];                          \
    POSITION->OccupiedL45 &= ClearL45[x];                          \
    POSITION->OccupiedR45 &= ClearR45[x]; }
#define SetOccupied(M, x)                                          \
  { POSITION->OccupiedBW |= M; POSITION->OccupiedL90 |= SetL90[x]; \
    POSITION->OccupiedL45 |= SetL45[x]; POSITION->OccupiedR45 |= SetR45[x]; }
