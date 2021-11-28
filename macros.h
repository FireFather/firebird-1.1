
#define FILE(s) ((s) & 7)
#define RANK(s) ((s) >> 3)
#define From(s) (((s) >> 6) & 077)
#define To(s) ((s) & 077)

#define WhiteOO (Position->Current->oo & 0x1)
#define WhiteOOO (Position->Current->oo & 0x2)
#define BlackOO (Position->Current->oo & 0x4)
#define BlackOOO (Position->Current->oo & 0x8)

#define MemAlign(a, b, c) a = _aligned_malloc (c, b)
#define FreeAligned(x) _aligned_free (x)
#define __builtin_prefetch(x, y, z) _mm_prefetch((char*)x, z);

#define CheckHalt() { if (Position->stop) { return (0); } }
#define UpdateAge() rank->age = Age;

#define EVAL(m) Eval (Position, -0x7fff0000, 0x7fff0000, m)
#define EvalLazy(B, A, p, m) Eval (Position, (B) - (p), (A) + (p), m)

#define Pos1 (TempPosition + 1)
#define MoveIsCheckWhite (Pos1->wKcheck)
#define MoveIsCheckBlack (Pos1->bKcheck)
#define Height(x) ((x)->height)

#ifdef MultiPosGain
sint16 MaxPositionalGain[MaxCPUs][0x10][010000];
#define MaxPosGain(pez, mos) MaxPositionalGain[Position->cpu][pez][mos]
#else
sint16 MaxPositionalGain[0x10][010000];
#define MaxPosGain(pez, mos) MaxPositionalGain[pez][mos]
#endif

#define CheckRepetition                                                             \
  CheckHalt();                                                                      \
  if (Position->Current->reversible >= 100) return(0);                                  \
  for (i = 4; i <= Position->Current->reversible && i <= Position->StackHeight; i += 2) \
    if (Position->Stack[Position->StackHeight - i] == Position->Current->Hash) return(0);

#ifdef MultiHistory
uint16 HISTORY[MaxCPUs][0x10][0100];
#define HistoryValue(P, M) HISTORY[P->cpu][P->sq[From (M)]][To (M)]
#else
uint16 HISTORY[0x10][0100];
#define HistoryValue(P, M) HISTORY[P->sq[From (M)]][To (M)]
#endif

#define Att_h1a8(fr)  AttLeft45  [fr][(Position->OccupiedL45 >> ShiftLeft45[fr]) & 077]
#define Att_a1h8(fr)  AttRight45  [fr][(Position->OccupiedR45 >> ShiftRight45[fr]) & 077]
#define AttRank(fr)  AttNormal  [fr][(Position->OccupiedBW >> ShiftAttack[fr]) & 077]
#define AttFile(fr)  AttLeft90  [fr][(Position->OccupiedL90 >> ShiftLeft90[fr]) & 077]

#define MAX(x, y) (( (x) >= (y)) ? (x) : (y))
#define MIN(x, y) (( (x) <= (y)) ? (x) : (y))
#define ABS(x) (( (x) >= 0) ? (x) : -(x))
#define FileDistance(x, y) (ABS(FILE(x) - FILE(y)))
#define RankDistance(x, y) (ABS(RANK(x) - RANK(y)))

#define AttB(fr) (Att_a1h8(fr) | Att_h1a8(fr))
#define AttR(fr) (AttRank(fr) | AttFile(fr))
#define AttQ(fr) (AttR(fr) | AttB(fr))

#define MoveIsEP(x) (((x) & FlagMask) == FlagEP)
#define MoveIsProm(x) (((x) & FlagMask) >= FlagPromN)
#define MoveIsOO(x) (((x) & FlagMask) == FlagOO)
#define MoveHistory(x) (((x) & 060000) == 0)

#define EarlyGame ((Position->Current->material & 0xff) >= 18)

#ifdef MultiplePosGain
#define MaxPositional(x) \
  ((int) MaxPositionalGain[Position->cpu][Position->sq[From(x)]][x & 07777])
#else
#define MaxPositional(x) ((int) MaxPositionalGain[Position->sq[From(x)]][x & 07777])
#endif

#define IsRepetition(x) (VALUE > x && TempPosition->reversible >= 2 && ((To(move) << 6) | From(move)) == (TempPosition - 1)->move && Position->sq[To(move)] == 0)
#define IsInterpose(x) (x & (1 << 15))
#define EasySEE(x) (x & 0x300000)
#define PassedPawnPush(to, x) (Position->sq[to] == EnumMyP && x &&   (BitboardOppP & IsPassedPawn[to]) == 0)
#define PieceIsWhite(pi) (pi <= 7)
#define PieceIsBlack(pi) (pi >= 8)
#define QSearchCondition (new_depth <= 1)
#define LowDepthCondition (new_depth <= 7)
#define LowDepthConditionPV (new_depth <= 7)

#define PosIsExact(x) (x)

#define Infinite 0x7ffffffffffffff
#define StrTok(p) p = strtok (NULL, " ")

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
  { Position->OccupiedBW &= M;                                     \
    Position->OccupiedL90 &= ClearL90[x];                          \
    Position->OccupiedL45 &= ClearL45[x];                          \
    Position->OccupiedR45 &= ClearR45[x]; }
#define SetOccupied(M, x)                                          \
  { Position->OccupiedBW |= M; Position->OccupiedL90 |= SetL90[x]; \
    Position->OccupiedL45 |= SetL45[x]; Position->OccupiedR45 |= SetR45[x]; }

#define I(a,b,c) ( (a & 0xff) | (b << 8) | (0 << 27))

#define MobQ(Y) Score(2, 2) * POPCNT(Y)
#define MobB(Y, Z) Score (5, 5) * POPCNT( Y & Z)
#define MobR(Y) Score(2, 3) * POPCNT(Y)
#define MobN(Y, Z) Score (6, 8) * POPCNT(Y & Z)

#define Bitboard2(x,y) (1ULL << (x))|(1ULL << (y))

#define PrefetchPawnHash _mm_prefetch((char*)PawnPointer, 2);

#define HistoryGood(move, depth)                                             \
 { int sv = HistoryValue (Position , move);                                  \
   HistoryValue (Position, move) = sv + (( (0xff00 - sv) * depth) >> 8); \
   if (move != Position->Current->killer1)                                        \
     { Position->Current->killer2 = Position->Current->killer1;                       \
       Position->Current->killer1 = move; } }
#define HistoryBad(move, depth)                                              \
  { int sv = HistoryValue (Position, move);                                  \
    if (TempPosition->Value > VALUE - 50)                                             \
      HistoryValue (Position, move) = sv - ((sv * depth) >> 8); }
#define HistoryBad1(move, depth)                                             \
  { int sv = HistoryValue (Position, move);                                  \
    if (TempPosition->Value > Alpha - 50)                                             \
      HistoryValue (Position, move) = sv - ((sv * depth) >> 8); }

#define Value4(w,x,y,z) ((((uint64) z) << 48) + (((uint64) y) << 32) + (((uint64) x) << 16) + (((uint64) w) << 0))

#define Add(L, x, y) { (L++)->move = (x) | (y);}
#define AddTo(T, w)                        \
  { while (T)                              \
      { to = LSB(T); c = Position->sq[to]; \
    Add (List, (sq << 6) | to, w);  BitClear (to, T); } }


#ifdef MultipleHistory
#define MoveAdd(L, x, pi, to, check)                               \
  { (L++)->move = (x) | ( (SqSet[to] & (check) ) ? FlagCheck : 0) \
                      | ( HISTORY[Position->cpu][pi][to] << 16); }
#else
#define MoveAdd(L, x, pi, to, check)                               \
  { (L++)->move = (x) | ( (SqSet[to] & (check) ) ? FlagCheck : 0) \
                      | ( HISTORY[pi][to] << 16); }
#endif

#define MovesTo(T, pi, check)                                       \
  { while (T)                                                       \
      { to = LSB(T); MoveAdd (List, (sq << 6) | to, pi, to, check); \
    BitClear (to, T); } }

#define UnderPromWhite()                                            \
  { if ( (AttN[to] & bBitboardK) == 0)                              \
      MoveAdd ( List, FlagPromN | (sq << 6) | to, wEnumP, to, 0);   \
    MoveAdd (List, FlagPromR | (sq << 6) | to, wEnumP, to, 0);      \
    MoveAdd (List, FlagPromB | (sq << 6) | to, wEnumP, to, 0); }

#define UnderPromBlack()                                            \
  { if ( (AttN[to] & wBitboardK) == 0)                              \
      MoveAdd (List, FlagPromN | (sq << 6) | to, bEnumP, to, 0);    \
    MoveAdd (List, FlagPromR | (sq << 6) | to, bEnumP, to, 0);      \
    MoveAdd (List, FlagPromB | (sq << 6) | to, bEnumP, to, 0); }

#define OK(x) (((x & 0x7fff) != s1) && ((x & 0x7fff) != s2) && ((x & 0x7fff) != s3))

#define WhiteInCheck (Position->Current->bAtt & wBitboardK)
#define BlackInCheck (Position->Current->wAtt & bBitboardK)
#define PosInCheck  ( Position->wtm ? ( Position->Current->bAtt & wBitboardK) : ( Position->Current->wAtt & bBitboardK ))

#define DoLocked(x) { Lock (SMP); (x); UnLock (SMP); }

#define Score(x,y) (((x) << 16) + (y))

#define BitClear(b, B) B &= (B - 1)
#define BitSet(b, B) B |= ((uint64) 1) << (b)

#define MaxDist(i,j) (MAX (FileDistance (i, j), RankDistance (i, j)))
#define MinDist(i,j) (MIN (FileDistance (i, j), RankDistance (i, j)))