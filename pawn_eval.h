#define Islands Score(0, 3)
#define Hole Score(1, 2)
#define DoubledClosed Score(2, 4)
#define DoubledOpen Score(4, 8)
#define DoubledClosedIsolated Score(2, 4)
#define DoubledOpenIsolated Score(6, 10)
#define IsolatedClosed Score(5, 8)
#define IsolatedOpen Score(15, 20)
#define BackwardClosed Score(5, 5)
#define BackwardOpen Score(10, 15)

#define KingAttPawn Score(0, 5)
#define KingOO Score(5, 0)
#define KingOOO Score(5, 0)

#define WhiteKingPawnDistance(pawn, king)                             \
  MAX ( ( ( king > pawn) ? 3 : 6) * ABS (RANK (pawn) - RANK (king) ), \
        6 * ABS (FILE (pawn) - FILE (king) ) )
#define BlackKingPawnDistance(pawn, king)                             \
  MAX ( ( ( king < pawn) ? 3 : 6) * ABS (RANK (pawn) - RANK (king) ), \
        6 * ABS ( FILE (pawn) - FILE (king) ) )

typedef struct
    {
    uint64 Edge, Middle, Center;
    uint8 ShelterEdge[8], ShelterMiddle[8], ShelterCenter[8];
    uint8 StormEdge[8], StormMiddle[8], StormCenter[8], ShelterDiag[8], ZERO, VALU_ZERO;
    } typePawnPtr;

typePawnPtr PawnPtr[8];

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
    Score(0, 0), Score(0, 0), Score(0, 0), Score(10, 10), Score(20, 25), Score(40, 50), Score(60, 75), Score(0, 0)
    };
static const sint32 OutsidePassedPawnValue[8] =
    {
    Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(2, 5), Score(5, 10), Score(10, 20), Score(0, 0)
    };
static const sint32 ProtectedPassedPawnValue[8] =
    {
    Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(5, 10), Score(10, 15), Score(15, 25), Score(0, 0)
    };
static const sint32 ConnectedPassedPawnValue[8] =
    {
    Score(0, 0), Score(0, 0), Score(0, 0), Score(0, 0), Score(5, 10), Score(10, 15), Score(20, 30), Score(0, 0)
    };
static const sint32 CandidatePawnValue[8] =
    {
    Score(0, 0), Score(0, 0), Score(0, 0), Score(5, 5), Score(10, 12), Score(20, 25), Score(0, 0), Score(0, 0)
    };

const uint8 Shelter_aa[8] =
    {
    30, 0, 5, 15, 20, 25, 25, 25
    };
const uint8 Shelter_ab[8] =
    {
    55, 0, 15, 40, 50, 55, 55, 55
    };
const uint8 Shelter_ac[8] =
    {
    30, 0, 10, 20, 25, 30, 30, 30
    };
const uint8 Storm_aa[8] =
    {
    5, 0, 35, 15, 5, 0, 0, 0
    };
const uint8 Storm_ab[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 Storm_ac[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 Shelter_ba[8] =
    {
    30, 0, 5, 15, 20, 25, 25, 25
    };
const uint8 Shelter_bb[8] =
    {
    55, 0, 15, 40, 50, 55, 55, 55
    };
const uint8 Shelter_bc[8] =
    {
    30, 0, 10, 20, 25, 30, 30, 30
    };
const uint8 Storm_ba[8] =
    {
    5, 0, 35, 15, 5, 0, 0, 0
    };
const uint8 Storm_bb[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 Storm_bc[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 Shelter_cb[8] =
    {
    30, 0, 5, 15, 20, 25, 25, 25
    };
const uint8 Shelter_cc[8] =
    {
    55, 0, 15, 40, 50, 55, 55, 55
    };
const uint8 Shelter_cd[8] =
    {
    30, 0, 10, 20, 25, 30, 30, 30
    };
const uint8 Storm_cb[8] =
    {
    5, 0, 35, 15, 5, 0, 0, 0
    };
const uint8 Storm_cc[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 Storm_cd[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 Shelter_dc[8] =
    {
    30, 0, 5, 15, 20, 25, 25, 25
    };
const uint8 Shelter_dd[8] =
    {
    55, 0, 15, 40, 50, 55, 55, 55
    };
const uint8 Shelter_de[8] =
    {
    30, 0, 10, 20, 25, 30, 30, 30
    };
const uint8 Storm_dc[8] =
    {
    5, 0, 35, 15, 5, 0, 0, 0
    };
const uint8 Storm_dd[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 Storm_de[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 ShelterLongDiagA[8] =
    {
    10, 0, 2, 4, 6, 8, 10, 10
    };
const uint8 ShelterLongDiagB[8] =
    {
    8, 0, 2, 4, 6, 7, 8, 8
    };
const uint8 ShelterLongDiagC[8] =
    {
    6, 0, 2, 3, 4, 5, 6, 6
    };
const uint8 ShelterLongDiagD[8] =
    {
    4, 0, 1, 2, 3, 4, 4, 4
    };
