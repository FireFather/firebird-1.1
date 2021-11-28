
#define WINDOWS_X64

#ifdef WINDOWS_X64
#define VERSION "1.1 x64"
#include "win64bits.h"
#else
#define VERSION "1.1 w32"
#include "win32bits.h"
#endif

#define NAME "FireBird"

#define BENCHMARK

#define true 1
#define false 0

#define MaxCPUs 16
#define MaxThreads 32
#define RPperCPU 8
#define MaxSP 16
#define MaxSplit 8

#define INLINE _inline
#define atoll _atoi64

#define MutexType CRITICAL_SECTION
#define CondType HANDLE
#define Lock(x) EnterCriticalSection (x)
#define UnLock(x) LeaveCriticalSection (x)
#define WaitForLock(x, y) WaitForSingleObject (x, INFINITE)
#define SignalForLock(x, y) SetEvent (x)
#define InitLock(x) InitializeCriticalSection(x)
#define InitCond(x, y) (x) = CreateEvent (0, false, false, 0)
#define PThreadCreate(N, b, thr, d) CreateThread (NULL, 0, thr, (LPVOID) (d), 0, (LPDWORD)N);
DWORD PThread[MaxCPUs];
#define SMPThread(A) DWORD WINAPI smp_thread (LPVOID A)

MutexType SMP[1];
MutexType PThreadCondMutex[MaxCPUs];
MutexType WakeupLock[MaxCPUs];
CondType WaitEvent[MaxCPUs];
CondType Wakeup[MaxCPUs];
CondType ThreadIsWaiting[MaxCPUs];
CondType ThreadIsRunning[MaxCPUs];

#define FlagLower 1
#define FlagUpper 2
#define FlagCut 4
#define FlagAll 8
#define FlagExact 16

#define IsCut(tr) ((tr->flags) & FlagCut)
#define IsAll(tr) ((tr->flags) & FlagAll)
#define IsExact(tr) ((tr)->flags & FlagExact)
#define HashUpperBound(tr) (tr->ValueLower)
#define HashLowerBound(tr) (tr->ValueUpper)

#define PVHashMask 0xfffc

#define NodeTypePV 1
#define NodeTypeAll 2
#define NodeTypeCut 3
#define MaxPly 1024

#define wBitboardK Position->bitboard[wEnumK]
#define wBitboardQ Position->bitboard[wEnumQ]
#define wBitboardR Position->bitboard[wEnumR]
#define wBitboardBL Position->bitboard[wEnumBL]
#define wBitboardBD Position->bitboard[wEnumBD]
#define wBitboardB (wBitboardBL|wBitboardBD)
#define wBitboardN Position->bitboard[wEnumN]
#define wBitboardP Position->bitboard[wEnumP]
#define wBitboardOcc Position->bitboard[wEnumOcc]
#define bBitboardK Position->bitboard[bEnumK]
#define bBitboardQ Position->bitboard[bEnumQ]
#define bBitboardR Position->bitboard[bEnumR]
#define bBitboardBL Position->bitboard[bEnumBL]
#define bBitboardBD Position->bitboard[bEnumBD]
#define bBitboardB (bBitboardBL|bBitboardBD)
#define bBitboardN Position->bitboard[bEnumN]
#define bBitboardP Position->bitboard[bEnumP]
#define bBitboardOcc Position->bitboard[bEnumOcc]

#define ShiftLeft45 LineShift[Direction_h1a8]
#define ShiftRight45 LineShift[Direction_a1h8]
#define ShiftAttack LineShift[Direction_horz]
#define ShiftLeft90 LineShift[Direction_vert]
#define AttLeft45 LineMask[Direction_h1a8]
#define AttRight45 LineMask[Direction_a1h8]
#define AttNormal LineMask[Direction_horz]
#define AttLeft90 LineMask[Direction_vert]

#define FlagEP 030000
#define FlagOO 010000
#define FlagMask 070000
#define FlagPromQ 070000
#define FlagPromR 060000
#define FlagPromB 050000
#define FlagPromN 040000

#define Direction_h1a8 0
#define Direction_a1h8 1
#define Direction_horz 2
#define Direction_vert 3
#define BadDirection 37
#define ValueMate 30000
#define ValueInfinity 32750
#define MoveNone 0

#define RANK1 0x00000000000000ff
#define RANK2 0x000000000000ff00
#define RANK3 0x0000000000ff0000
#define RANK4 0x00000000ff000000
#define RANK5 0x000000ff00000000
#define RANK6 0x0000ff0000000000
#define RANK7 0x00ff000000000000
#define RANK8 0xff00000000000000

#define FileA 0x0101010101010101
#define FILEb 0x0202020202020202
#define FILEc 0x0404040404040404
#define FILEd 0x0808080808080808
#define FILEe 0x1010101010101010
#define FILEf 0x2020202020202020
#define FILEg 0x4040404040404040
#define FileH 0x8080808080808080

#define Ranks78 0xffff000000000000
#define Ranks678 0xffffff0000000000
#define Ranks12 0x000000000000ffff
#define Ranks123 0x00000000000ffffff
#define wOutpost 0x00007e7e7e000000
#define bOutpost 0x0000007e7e7e0000

#define TempoValue 5
#define TempoValue2 5
#define PrunePawn 160
#define PruneMinor 500
#define PruneRook 800
#define PruneCheck 10
#define LazyValue 150
#define LazyValue2 300

#define BishopTrapValue Score(40, 40)
#define BishopTrapGuardValue Score(40, 40)

#define xrayB0 Score(0, 0)
#define xrayBmP Score(3, 5)
#define xrayBmN Score(3, 5)
#define xrayBmK Score(3, 5)
#define xrayBmB Score(0, 0)
#define xrayBmR Score(3, 5)
#define xrayBmQ Score(0, 0)

#define xrayBoP Score(2, 5)
#define xrayBoN Score(2, 5)
#define xrayBoK Score(0, 0)
#define xrayBoB Score(0, 0)
#define xrayBoR Score(15, 25)
#define xrayBoQ Score(10, 20)

#define xrayR0 Score(0, 0)
#define xrayRmP Score(0, 0)
#define xrayRmN Score(3, 5)
#define xrayRmK Score(3, 5)
#define xrayRmB Score(3, 5)
#define xrayRmR Score(0, 0)
#define xrayRmQ Score(0, 0)

#define xrayRoP Score(2, 5)
#define xrayRoN Score(2, 5)
#define xrayRoK Score(0, 0)
#define xrayRoB Score(2, 5)
#define xrayRoR Score(0, 0)
#define xrayRoQ Score(10, 20)

#define xQ0d Score(0, 0)
#define xQmPd Score(1, 2)
#define xQmNd Score(2, 4)
#define xQmKd Score(2, 4)
#define xQmBd Score(0, 0)
#define xQmRd Score(2, 4)
#define xQmQd Score(0, 0)

#define xQoPd Score(0, 0)
#define xQoNd Score(2, 5)
#define xQoKd Score(0, 0)
#define xQoBd Score(0, 0)
#define xQoRd Score(2, 5)
#define xQoQd Score(0, 0)

#define xQ0hv Score(0, 0)
#define xQmPhv Score(0, 0)
#define xQmNhv Score(2, 4)
#define xQmKhv Score(2, 4)
#define xQmBhv Score(2, 4)
#define xQmRhv Score(0, 0)
#define xQmQhv Score(0, 0)

#define xQoPhv Score(0, 0)
#define xQoNhv Score(2, 5)
#define xQoKhv Score(0, 0)
#define xQoBhv Score(2, 5)
#define xQoRhv Score(0, 0)
#define xQoQhv Score(0, 0)

#define QguardK Score(5, 2)
#define RguardK Score(3, 1)
#define BguardK Score(2, 1)
#define NguardK Score(4, 2)

#define DoubQueen7th Score(10, 15)
#define DoubRook7thKingPawn Score(10, 20)
#define MultipleAtt Score(15, 25)
#define Queen7th Score(5, 25)
#define KingAttUnguardedPawn Score(0, 5)

#define PattQ Score(8, 12)
#define RattQ Score(5, 5)
#define NattRQ Score(7, 10)
#define bAttRQ Score(7, 10)
#define PattR Score(7, 10)
#define PattN Score(5, 7)
#define PattB Score(5, 7)
#define bAttN Score(5, 5)
#define NattB Score(5, 5)
#define Qatt Score(4, 4)
#define RattBN Score(4, 5)
#define RattP Score(2, 3)
#define NattP Score(3, 4)
#define bAttP Score(3, 4)

#define RookHalfOpen Score(3, 6)

#define RookOpenFile Score(20, 10)
#define RookOpenFixedMinor Score(10, 0)
#define RookOpenMinor Score(15, 5)

#define RookHalfOpenPawn Score(5, 5)
#define RookHalfOpenKing Score(15, 0)
#define RookKing8th Score(5, 10)
#define Rook7thKingPawn Score(10, 30)
#define Rook6thKingPawn Score(5, 15)

#define OutpostBishop Score(1, 2)
#define OutpostBishopGuarded Score(3, 4)
#define OutpostRook Score(1, 2)
#define OutpostRookGuarded Score(3, 4)
#define OutpostKnight Score(2, 3)
#define OutpostKnightPawn Score(2, 3)
#define OutpostKnightAttacks Score(5, 5)
#define OutpostKnight5th Score(2, 2)
#define OutpostKnightONde Score(3, 3)

#define HitP Score(1, 0)
#define HitQ Score(1, 40)
#define HitR Score(1, 25)
#define HitN Score(1, 15)
#define HitB Score(1, 15)
#define HitK Score(0, 0)
#define KingSafetyDivider 8

#define Rook7thEnd Score(100, 100)
#define Rook6thEnd Score(25, 25)
#define Queen7thEnd Score(10, 10)
#define PawnAntiMobility Score(3, 10)

#define EvalHashSize ( 0x8000 )
#define EvalHashMask ( EvalHashSize - 1 )
uint64 EvalHash[EvalHashSize];

#define MaxAge 256
#define MaxDepth 256

#define PtoQ ( 0xd8 << 24 )
#define PtoN ( 0xc2 << 24 )
#define FlagCheck 0x8000

#define VOID_STAR_TYPE DWORD
#define Tweak (0x74d3c012a8bf965e)

#define NullReduction 8

#define QueenEnding 1
#define RookEnding 2
#define OppositeBishopEnding 3
#define BishopEnding 4
#define KnightEnding 5
#define BishopKnightEnding 6
#define PawnEnding 7
#define WhiteMinorOnly 8
#define BlackMinorOnly 16
#define BishopKnightMate 32

#define PosBishopKnightMate (Position->Current->flags & 128)

#define PosWhiteMinorOnly (Position->Current->flags & 32)
#define PosBlackMinorOnly (Position->Current->flags & 64)

#define WhiteHasPiece (Position->Current->flags & 2)
#define BlackHasPiece (Position->Current->flags & 1)

#define QueenEnd ((Position->Current->flags & 28) == 4)
#define RookEnd ((Position->Current->flags & 28) == 8)

#define WhiteMinorOnlyShift (8 << 2)
#define BlackMinorOnlyShift (16 << 2)

#define ANSplitDepth 12
#define CNSplitDepth 14
#define PVSplitDepth 12

#define NullMoveVerification true
#define VerificationReduction 7
