#define SCORE(x,y) (((x) << 16) + (y))

#define WhiteKingPawnDistance(pawn, king)                             \
  MAX ( ( ( king > pawn) ? 3 : 6) * ABS (RANK (pawn) - RANK (king) ), \
        6 * ABS (FILE (pawn) - FILE (king) ) )
#define BlackKingPawnDistance(pawn, king)                             \
  MAX ( ( ( king < pawn) ? 3 : 6) * ABS (RANK (pawn) - RANK (king) ), \
        6 * ABS ( FILE (pawn) - FILE (king) ) )

static uint8 OpposingPawnsMult[9] =
    {
    6, 5, 4, 3, 2, 1, 0, 0, 0
    };
static uint8 PawnCountMult[9] =
    {
    6, 5, 4, 3, 2, 1, 0, 0, 0
    };
static const uint8 BlockedPawnValue[64] =
    {
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 2, 2, 2, 2, 1, 1,
	1, 2, 3, 3, 3, 3, 2, 1,
	1, 2, 3, 5, 5, 3, 2, 1,
	1, 2, 3, 5, 5, 3, 2, 1,
	1, 2, 3, 3, 3, 3, 2, 1,
	1, 1, 2, 2, 2, 2, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0
    };
static const sint32 MyKingPawnDistance[8] =
    {
    0, 0, 0, 1, 2, 3, 5, 0
    };
static const sint32 OppKingPawnDistance[8] =
    {
    0, 0, 0, 2, 4, 6, 10, 0
    };
static const sint32 PassedPawnValue[8] =
    {
    SCORE(0, 0), SCORE(0, 0), SCORE(0, 0), SCORE(10, 10), SCORE(20, 25), SCORE(40, 50), SCORE(60, 75), SCORE(0, 0)
    };
static const sint32 OutsidePassedPawnValue[8] =
    {
    SCORE(0, 0), SCORE(0, 0), SCORE(0, 0), SCORE(0, 0), SCORE(2, 5), SCORE(5, 10), SCORE(10, 20), SCORE(0, 0)
    };
static const sint32 ProtectedPassedPawnValue[8] =
    {
    SCORE(0, 0), SCORE(0, 0), SCORE(0, 0), SCORE(0, 0), SCORE(5, 10), SCORE(10, 15), SCORE(15, 25), SCORE(0, 0)
    };
static const sint32 ConnectedPassedPawnValue[8] =
    {
    SCORE(0, 0), SCORE(0, 0), SCORE(0, 0), SCORE(0, 0), SCORE(5, 10), SCORE(10, 15), SCORE(20, 30), SCORE(0, 0)
    };
static const sint32 CandidatePawnValue[8] =
    {
    SCORE(0, 0), SCORE(0, 0), SCORE(0, 0), SCORE(5, 5), SCORE(10, 12), SCORE(20, 25), SCORE(0, 0), SCORE(0, 0)
    };
#define Islands SCORE(0, 3)
#define Hole SCORE(1, 2)
#define DoubledClosed SCORE(2, 4)
#define DoubledOpen SCORE(4, 8)
#define DoubledClosedIsolated SCORE(2, 4)
#define DoubledOpenIsolated SCORE(6, 10)
#define IsolatedClosed SCORE(5, 8)
#define IsolatedOpen SCORE(15, 20)
#define BackwardClosed SCORE(5, 5)
#define BackwardOpen SCORE(10, 15)

#define KingAttPawn SCORE(0, 5)
#define KingOO SCORE(5, 0)
#define KingOOO SCORE(5, 0)
