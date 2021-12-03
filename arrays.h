
extern const uint8 Left90[64];
extern const uint8 Left45[64];
extern const uint8 Right45[64];

uint64 AttN[64], AttK[64], AttPw[64], AttPb[64], LineMask[4][64][64];
int LineShift[4][64];

static const uint64 RankArray[8] =
{ 0x00000000000000ff, 0x000000000000ff00,
  0x0000000000ff0000, 0x00000000ff000000,
  0x000000ff00000000, 0x0000ff0000000000,
  0x00ff000000000000, 0xff00000000000000
};

#define RANK1 0x00000000000000ff
#define RANK2 0x000000000000ff00
#define RANK3 0x0000000000ff0000
#define RANK4 0x00000000ff000000
#define RANK5 0x000000ff00000000
#define RANK6 0x0000ff0000000000
#define RANK7 0x00ff000000000000
#define RANK8 0xff00000000000000

static const uint64 FileArray[8] =
{ 0x0101010101010101, 0x0202020202020202,
  0x0404040404040404, 0x0808080808080808,
  0x1010101010101010, 0x2020202020202020,
  0x4040404040404040, 0x8080808080808080
};

#define FILEa 0x0101010101010101
#define FILEb 0x0202020202020202
#define FILEc 0x0404040404040404
#define FILEd 0x0808080808080808
#define FILEe 0x1010101010101010
#define FILEf 0x2020202020202020
#define FILEg 0x4040404040404040
#define FILEh 0x8080808080808080

uint64 SqSet[64], SqClear[64], NON_DIAG[64], NON_ORTHO[64];
uint64 ORTHO[64], DIAG[64], ORTHO_DIAG[64];

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
static const uint64 LIGHT = 0x55aa55aa55aa55aa;
static const uint64 DARK = 0xaa55aa55aa55aa55;
static const uint64 Rank2NOTa = 0x000000000000fe00;
static const uint64 Rank2NOTh = 0x0000000000007f00;
static const uint64 Rank7NOTa = 0x00fe000000000000;
static const uint64 Rank7NOTh = 0x007f000000000000;
static const uint64 NOTa = 0xfefefefefefefefe;
static const uint64 NOTh = 0x7f7f7f7f7f7f7f7f;

uint64 OpenFileW[64], OpenFileB[64], PassedPawnW[64], PassedPawnB[64];
uint64 ProtectedPawnW[64], ProtectedPawnB[64];
uint64 IsolatedPawnB[64], IsolatedPawnW[64], ConnectedPawns[64];
uint64 InFrontW[8], NotInFrontW[8], InFrontB[8], NotInFrontB[8], IsolatedFiles[8];
uint64 FilesLeft[8], FilesRight[8];

static const uint64 F1G1 = 0x0000000000000060;
static const uint64 C1D1 = 0x000000000000000c;
static const uint64 B1C1D1 = 0x000000000000000e;
static const uint64 F8G8 = 0x6000000000000000;
static const uint64 C8D8 = 0x0c00000000000000;
static const uint64 B8C8D8 = 0x0e00000000000000;

uint64 DOUBLED[64];

uint64 LEFT2[64], RIGHT2[64], LEFT1[64], RIGHT1[64], ADJACENT[64], LONG_DIAG[64];

uint64 QuadrantWKwtm[64], QuadrantBKwtm[64];
uint64 QuadrantWKbtm[64], QuadrantBKbtm[64];
uint64 ShepherdWK[64], ShepherdBK[64];

uint64 INTERPOSE[64][64], EVADE[64][64];
uint64 NORTHWEST[64], SOUTHWEST[64], NORTHEAST[64], SOUTHEAST[64];
uint64 ClearL90[64], ClearL45[64], ClearR45[64], SetL90[64], SetL45[64], SetR45[64];
sint8 LINE[64][64];
uint64 ZobristCastling[16], HASH[16][64], ZobristEP[8], ZobristWTM;
