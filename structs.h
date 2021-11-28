
typedef struct
    {
    uint64 volatile PawnHash;
    uint8 wPfile_count, bPfile_count, OpenFileCount;
    boolean locked;
    uint32 wKdanger, bKdanger;
    uint8 wPlight, wPdark, bPlight, bPdark, wPassedFiles, bPassedFiles, wDrawWeight, bDrawWeight;
    uint32 score;
    } typePawnEval;
typePawnEval *PawnHash;

typedef struct
    {
    uint32 hash;
    uint8 flags, age, DepthUpper, DepthLower;
    sint16 ValueUpper, ValueLower;
    uint16 move;
    uint8 rev, _2;
    } typeHash;

typeHash *HashTable;

typedef struct
    {
    uint64 hash;
    sint32 Value;
    uint16 move;
    uint8 depth, age;
    } typePVHash;
typePVHash PVHashTable[0x10000];

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
    uint64 Hash, PawnHash;
    uint32 material, PST, _7;
    uint8 oo, reversible, ep, cp;
    uint64 wAtt, bAtt, wXray, bXray;
    sint32 Value, PositionalValue;
    uint16 _5, _6, killer1, killer2, move;
    uint8 _0, _3, exact, lazy, SavedFlags, flags;
    uint64 wKcheck, bKcheck, _1, _2, _8;
    } typePosition;

typedef struct
    {
    int phase, mask, bc;
    uint32 trans_move, move, exclude;
    uint64 TARGET;
    typeMoveList List[512];
    uint32 BadCaps[64];
    } typeNext;

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
    typeNext *MovePick;
    boolean tot;
    boolean active;
    MutexType splock[1];
    } SplitPoint;
SplitPoint RootSP[MaxSP];

struct TP
    {
    uint8 sq[64];
    uint64 bitboard[16];
    uint64 OccupiedBW, OccupiedL90, OccupiedL45, OccupiedR45;
    uint8 XrayW[64], XrayB[64];
    uint8 wtm, wKsq, bKsq, height;
    typePosition *Current, *Root;
    uint64 Stack[1024];
    uint64 StackHeight, cpu, nodes;
    boolean stop, used;
    MutexType padlock[1];
    int child_count;
    struct TP *parent, *children[MaxCPUs];
    SplitPoint *SplitPoint;
    };

typedef struct TP typePos;
typePos RootPosition[MaxCPUs][RPperCPU];
typePos RootPosition0[1];
typePos NullParent[1];

typedef struct
    {
    sint16 Value;
    uint8 token;
    uint8 flags;
    } typeMATERIAL;
typeMATERIAL Material[419904];

typedef struct
    {
    uint32 move;
    sint32 Value;
    } typeMPV;
typeMPV MPV[256];

typedef struct
    {
    int cpu;
    } t_args;
t_args ARGS[MaxCPUs];