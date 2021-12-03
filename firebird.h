#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib,"winmm.lib")

#pragma warning(disable : 4244)
// conversion from 'int' to 'short', possible loss of data
#pragma warning(disable : 4018)
// signed/unsigned mismatch
#pragma warning(disable : 4996)
// 'sscanf': This function or variable may be unsafe
#pragma warning(disable : 4334)
// '<<' : result of 32-bit shift implicitly converted to 64 bits
#pragma warning(disable : 4098)
// 'void' function returning a value
#pragma warning(disable : 4761)
// integral size mismatch in argument; conversion supplied
#pragma warning(disable : 4311)
// pointer truncation from 'void *' to 'DWORD'
#pragma warning(disable : 4090)
// different 'const' qualifiers
#pragma warning(disable : 4101)
// unreferenced local variable

#define WINDOWS
//#undef WINDOWS
#define WINDOWS_X64
//#undef WINDOWS_X64

#define NAME "FireBird"

#define true 1
#define false 0

#define MAX_CPUS 16
#define MAX_THREADS 16
#define RP_PER_CPU 8
#define MAX_SP 16

#define HAS_POPCNT

#ifdef WINDOWS
#include <windows.h>
#define NOME_WINDOWS

typedef __int8 sint8;
typedef __int16 sint16;
typedef __int32 sint32;
typedef __int64 sint64;
typedef unsigned __int8 boolean;
typedef unsigned __int8 uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

#define INLINE _inline
#define atoll _atoi64
#define TYPE_64_BIT "%I64d"
#define MEMALIGN(a, b, c) a = _aligned_malloc (c, b)
#define ALIGNED_FREE(x) _aligned_free (x)
#define __builtin_prefetch(x, y, z) _mm_prefetch((char*)x, z);

#ifdef WINDOWS_X64
#define VERSION "1.1 x64"
#include "win64bits.h"
#else
#define VERSION "1.1 w32"
#include "win32bits.h"

#endif
#define MUTEX_TYPE CRITICAL_SECTION
#define COND_TYPE HANDLE
#define LOCK(x) EnterCriticalSection (x)
#define UNLOCK(x) LeaveCriticalSection (x)
#define WAIT_CON_LOCK(x, y) WaitForSingleObject (x, INFINITE)
#define SIGNAL_CON_LOCK(x, y) SetEvent (x)
#define LOCK_INIT(x) InitializeCriticalSection(x)
#define COND_INIT(x, y) (x) = CreateEvent (0, FALSE, FALSE, 0)
#define PTHREAD_CREATE(N, b, thr, d) CreateThread (NULL, 0, thr, (LPVOID) (d), 0, (LPDWORD)N);
DWORD PTHREAD[MAX_CPUS];
#define IVAN_THREAD(A) DWORD WINAPI ivan_thread (LPVOID A)
#else /* LINUX */
#define sint8 signed char
#define sint16 signed short int
#define sint32 int
#define sint64 long long int
#define uint8 unsigned char
#define uint16 unsigned short int
#define uint32 unsigned int
#define uint64 unsigned long long int
#define INLINE inline
#define TYPE_64_BIT "%lld"
#define MEMALIGN(a, b, c) posix_memalign ((void*) &(a), b, c)
#define ALIGNED_FREE(x) free (x)
#include "bits.h"
#include <pthread.h>
#define MUTEX_TYPE pthread_mutex_t
#define COND_TYPE pthread_cond_t
#define LOCK(x) pthread_mutex_lock (x)
#define UNLOCK(x) pthread_mutex_unlock (x)
#define WAIT(x, y) pthread_cond_wait (x, y)
#define SIGNAL(x) pthread_cond_signal (x)
#define WAIT_CON_LOCK(x, y) { LOCK (&(y)); WAIT (&(x), &(y)); UNLOCK (&(y)); }
#define SIGNAL_CON_LOCK(x, y) { LOCK (&(y)); SIGNAL (&(x)); UNLOCK (&(y)); }
#define LOCK_INIT(x) pthread_mutex_init ((x), NULL)
#define COND_INIT(x, y) { pthread_cond_init (&(x), NULL); pthread_mutex_init (&(y), NULL); }
#define PTHREAD_CREATE(N, b, thr, d) pthread_create (N, NULL, thr, (void*) (d))

pthread_t PTHREAD[MAX_CPUS];
#define IVAN_THREAD(A) void* ivan_thread (void* A)

#endif

MUTEX_TYPE SMP_IVAN[1];
MUTEX_TYPE PTHREAD_COND_MUTEX[MAX_CPUS];
MUTEX_TYPE WAKEUP_LOCK[MAX_CPUS];
COND_TYPE WAIT_EVENT[MAX_CPUS];
COND_TYPE WAKEUP[MAX_CPUS];
COND_TYPE THREAD_WAITS[MAX_CPUS];
COND_TYPE THREAD_RUNS[MAX_CPUS];
typedef struct
    {
    uint64 volatile PAWN_HASH;
    uint8 wPfile_count, bPfile_count, OpenFileCount;
    boolean locked;
    uint32 wKdanger, bKdanger;
    uint8 wPlight, wPdark, bPlight, bPdark, wPassedFiles, bPassedFiles, wDrawWeight, bDrawWeight;
    uint32 SCORE;
    } typePawnEval;
typePawnEval *PawnHash;

#define CHECK_HALT() { if (POSITION->stop) { return (0); } }

#define FLAG_LOWER 1
#define FLAG_UPPER 2
#define FLAG_CUT 4
#define FLAG_ALL 8
#define FLAG_EXACT 16

#define IsCUT(tr) ((tr->flags) & FLAG_CUT)
#define IsALL(tr) ((tr->flags) & FLAG_ALL)
#define IsExact(tr) ((tr)->flags & FLAG_EXACT)
#define HashUpperBound(tr) (tr->ValueLower)
#define HashLowerBound(tr) (tr->ValueUpper)

#define FILE(s) ((s) & 7)
#define RANK(s) ((s) >> 3)
#define FROM(s) (((s) >> 6) & 077)
#define TO(s) ((s) & 077)

int num_CPUs;
uint64 volatile NUM_THREADS;
uint64 NODE_CHECK;
typedef struct
    {
    uint64 h1, h2;
    } YUSUF128;
#define DECLARE()

#define UPDATE_AGE() trans->age = AGE;

#define EVAL(m) Eval (POSITION, -0x7fff0000, 0x7fff0000, m)
#define EvalLazy(B, A, p, m) Eval (POSITION, (B) - (p), (A) + (p), m)
#define WHITE_IN_CHECK (POSITION->DYN->bAtt & wBitboardK)
#define BLACK_IN_CHECK (POSITION->DYN->wAtt & bBitboardK)
#define POS1 (POS0 + 1)
#define MOVE_IS_CHECK_WHITE (POS1->wKcheck)
#define MOVE_IS_CHECK_BLACK (POS1->bKcheck)

#ifdef MULTI_POS_GAIN
sint16 MAX_POSITIONAL_GAIN[MAX_CPUS][0x10][010000];
#define MAX_POS_GAIN(pez, mos) MAX_POSITIONAL_GAIN[POSITION->cpu][pez][mos]
#else

sint16 MAX_POSITIONAL_GAIN[0x10][010000];
#define MAX_POS_GAIN(pez, mos) MAX_POSITIONAL_GAIN[pez][mos]

#endif

boolean BOARD_IS_OK, NEW_GAME;

#define CheckRepetition                                                             \
  CHECK_HALT();                                                                     \
  if (POSITION->DYN->reversible >= 100) return(0);                                  \
  for (i = 4; i <= POSITION->DYN->reversible && i <= POSITION->StackHeight; i += 2) \
    if (POSITION->STACK[POSITION->StackHeight - i] == POSITION->DYN->HASH) return(0);

typedef struct
    {
    uint32 hash;
    uint8 flags, age, DepthUpper, DepthLower;
    sint16 ValueUpper, ValueLower;
    uint16 move;
    uint8 rev, _2;
    } typeHash;

typeHash *HashTable;
uint64 HashMask, AGE;
typedef struct
    {
    uint64 hash;
    sint32 Value;
    uint16 move;
    uint8 depth, age;
    } typePVHash;
typePVHash PVHashTable[0x10000];
#define PVHashMask 0xfffc

int PawnHashSize;
typedef struct
    {
    uint32 move;
    } typeMoveList;
typedef struct
    {
    uint32 move;
    uint64 nodes;
    } typeRootMoveList;
typedef struct
    {
    uint64 HASH, PAWN_HASH;
    uint32 material, STATIC, _7;
    uint8 oo, reversible, ep, cp;
    uint64 wAtt, bAtt, wXray, bXray;
    sint32 Value, PositionalValue;
    uint16 _5, _6, killer1, killer2, move;
    uint8 _0, _3, exact, lazy, SAVED_FLAGS, flags;
    uint64 wKcheck, bKcheck, _1, _2, _8;
    } typeDYNAMIC;
typedef struct
    {
    int phase, mask, bc;
    uint32 trans_move, move, exclude;
    uint64 TARGET;
    typeMoveList LIST[512];
    uint32 BAD_CAPS[64];
    } typeNEXT;

#define HEIGHT(x) ((x)->height)

typedef struct
    {
    int alpha;
    int beta;
    int depth;
    int node_type;
    int value;
    uint32 move;
    uint32 good_move;
    uint32 childs;
    typeNEXT *MOVE_PICK;
    boolean tot;
    boolean active;
    MUTEX_TYPE splock[1];
    } SPLITPOINT;

SPLITPOINT ROOT_SP[MAX_SP];
volatile int SMP_FREE;
#define NODE_TYPE_PV 1
#define NODE_TYPE_ALL 2
#define NODE_TYPE_CUT 3
#include <setjmp.h>

struct TP
    {
    uint8 sq[64];
    uint64 bitboard[16];
    uint64 OccupiedBW, OccupiedL90, OccupiedL45, OccupiedR45;
    uint8 XRAYw[64], XRAYb[64];
    uint8 wtm, wKsq, bKsq, height;
    /* 64 + 128 + 32 + 128 + 4 = 356 */
    typeDYNAMIC *DYN, *DYN_ROOT;
    uint64 STACK[1024];
    uint64 StackHeight, cpu, nodes, tbhits;
    boolean stop, used;
    MUTEX_TYPE padlock[1];
    int child_count;
    struct TP *parent, *children[MAX_CPUS];
    SPLITPOINT *SplitPoint;
    };

typedef struct TP typePOS;

typePOS ROOT_POSITION[MAX_CPUS][RP_PER_CPU];
typePOS ROOT_POSITION0[1];
typePOS NULL_PARENT[1];

#define MAXIMUM_PLY 1024
#define WhiteOO (POSITION->DYN->oo & 0x1)
#define WhiteOOO (POSITION->DYN->oo & 0x2)
#define BlackOO (POSITION->DYN->oo & 0x4)
#define BlackOOO (POSITION->DYN->oo & 0x8)

typedef struct
    {
    sint16 Value;
    uint8 token;
    uint8 flags;
    } typeMATERIAL;
typeMATERIAL MATERIAL[419904];
typedef enum
    {
    wEnumOcc,
    wEnumP,
    wEnumN,
    wEnumK,
    wEnumBL,
    wEnumBD,
    wEnumR,
    wEnumQ,
    bEnumOcc,
    bEnumP,
    bEnumN,
    bEnumK,
    bEnumBL,
    bEnumBD,
    bEnumR,
    bEnumQ
    } EnumPieces;
typedef enum
    {
    TRANS,
    CAPTURE_GEN,
    CAPTURE_MOVES,
    KILLER1,
    KILLER2,
    ORDINARY_MOVES,
    BAD_CAPS,
    TRANS2,
    CAPTURE_PGEN2,
    CAPTURE_MOVES2,
    QUIET_CHECKS,
    EVADE_PHASE,
    TRANS3,
    CAPTURE_GEN3,
    CAPTURE_MOVES3,
    QUIET_CHECKS3,
    POSITIONAL_GAIN_PHASE,
    FASE_0
    } EnumPhases;
typedef enum
    {
    A1,
    B1,
    C1,
    D1,
    E1,
    F1,
    G1,
    H1,
    A2,
    B2,
    C2,
    D2,
    E2,
    F2,
    G2,
    H2,
    A3,
    B3,
    C3,
    D3,
    E3,
    F3,
    G3,
    H3,
    A4,
    B4,
    C4,
    D4,
    E4,
    F4,
    G4,
    H4,
    A5,
    B5,
    C5,
    D5,
    E5,
    F5,
    G5,
    H5,
    A6,
    B6,
    C6,
    D6,
    E6,
    F6,
    G6,
    H6,
    A7,
    B7,
    C7,
    D7,
    E7,
    F7,
    G7,
    H7,
    A8,
    B8,
    C8,
    D8,
    E8,
    F8,
    G8,
    H8
    } EnumSquares;
typedef enum
    {
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8
    } EnumRanks;
typedef enum
    {
    FA,
    FB,
    FC,
    FD,
    FE,
    FF,
    FG,
    FH
    } EnumFiles;

sint32 PieceSquareValue[0x10][0100];

#ifdef MULTI_HISTORY

uint16 HISTORY[MAX_CPUS][0x10][0100];
#define HISTORY_VALUE(P, M) HISTORY[P->cpu][P->sq[FROM (M)]][TO (M)]
#else

uint16 HISTORY[0x10][0100];
#define HISTORY_VALUE(P, M) HISTORY[P->sq[FROM (M)]][TO (M)]

#endif

char STRING1[64], STRING2[64], STRING3[64], STRING4[64];

#define wBitboardK POSITION->bitboard[wEnumK]
#define wBitboardQ POSITION->bitboard[wEnumQ]
#define wBitboardR POSITION->bitboard[wEnumR]
#define wBitboardBL POSITION->bitboard[wEnumBL]
#define wBitboardBD POSITION->bitboard[wEnumBD]
#define wBitboardB (wBitboardBL|wBitboardBD)
#define wBitboardN POSITION->bitboard[wEnumN]
#define wBitboardP POSITION->bitboard[wEnumP]
#define wBitboardOcc POSITION->bitboard[wEnumOcc]
#define bBitboardK POSITION->bitboard[bEnumK]
#define bBitboardQ POSITION->bitboard[bEnumQ]
#define bBitboardR POSITION->bitboard[bEnumR]
#define bBitboardBL POSITION->bitboard[bEnumBL]
#define bBitboardBD POSITION->bitboard[bEnumBD]
#define bBitboardB (bBitboardBL|bBitboardBD)
#define bBitboardN POSITION->bitboard[bEnumN]
#define bBitboardP POSITION->bitboard[bEnumP]
#define bBitboardOcc POSITION->bitboard[bEnumOcc]

#define ShiftLeft45 LineShift[Direction_h1a8]
#define ShiftRight45 LineShift[Direction_a1h8]
#define ShiftAttack LineShift[Direction_horz]
#define ShiftLeft90 LineShift[Direction_vert]
#define AttLeft45 LineMask[Direction_h1a8]
#define AttRight45 LineMask[Direction_a1h8]
#define AttNormal LineMask[Direction_horz]
#define AttLeft90 LineMask[Direction_vert]

#define Att_h1a8(fr)  AttLeft45  [fr][(POSITION->OccupiedL45 >> ShiftLeft45[fr]) & 077]
#define Att_a1h8(fr)  AttRight45  [fr][(POSITION->OccupiedR45 >> ShiftRight45[fr]) & 077]
#define AttRank(fr)  AttNormal  [fr][(POSITION->OccupiedBW >> ShiftAttack[fr]) & 077]
#define AttFile(fr)  AttLeft90  [fr][(POSITION->OccupiedL90 >> ShiftLeft90[fr]) & 077]

#define MAX(x, y) (( (x) >= (y)) ? (x) : (y))
#define MIN(x, y) (( (x) <= (y)) ? (x) : (y))
#define ABS(x) (( (x) >= 0) ? (x) : -(x))
#define FileDistance(x, y) (ABS(FILE(x) - FILE(y)))
#define RankDistance(x, y) (ABS(RANK(x) - RANK(y)))

#define AttB(fr) (Att_a1h8(fr) | Att_h1a8(fr))
#define AttR(fr) (AttRank(fr) | AttFile(fr))
#define AttQ(fr) (AttR(fr) | AttB(fr))

#define FlagEP 030000
#define FlagOO 010000
#define FLAG_MASK 070000
#define FlagPromQ 070000
#define FlagPromR 060000
#define FlagPromB 050000
#define FlagPromN 040000
#define MoveIsEP(x) (((x) & FLAG_MASK) == FlagEP)
#define MoveIsProm(x) (((x) & FLAG_MASK) >= FlagPromN)
#define MoveIsOO(x) (((x) & FLAG_MASK) == FlagOO)
#define MoveHistory(x) (((x) & 060000) == 0)

#define Direction_h1a8 0
#define Direction_a1h8 1
#define Direction_horz 2
#define Direction_vert 3
#define BAD_DIRECTION 37
#define VALUE_MATE 30000
#define VALUE_INFINITY 32750
#define MOVE_NONE 0

boolean ANALYSIS_MODE;
boolean DO_SEARCH_MOVES;
boolean DO_PONDER;
boolean DO_INFINITE;
boolean EXTRA_INFO;
boolean MULTI_POS_GAIN;
boolean MULTI_HISTORY;
boolean NMR_SCALING;
boolean NULL_MOVE_VERIFICATION;
boolean PONDER_HIT;
boolean STOP;
boolean UCI_PONDER;

int OPT_MAX_THREADS;
int MULTI_PV;
int MULTI_CENTI_PAWN_PV;
int MPH;
int RANDOM_COUNT;
int RANDOM_BITS;
int PValue;
int NValue;
int BValue;
int RValue;
int QValue;
int BPValue;
int AN_SPLIT_DEPTH;
int CN_SPLIT_DEPTH;
int PV_SPLIT_DEPTH;
int VERIFICATION_REDUCTION;

uint32 SEARCH_MOVES[256];
uint64 BUFFER_TIME;

typedef struct
    {
    uint32 move;
    sint32 Value;
    } typeMPV;
typeMPV MPV[256];

#include "arrays.h"
#include "functions.h"
#include "common.h"

#define IS_EXACT(x) (x)
