#include "firebird.h"

void PawnHashReset()
    {
    memset(PawnHash, 0, PawnHashSize * sizeof(typePawnEval));
    }
int InitPawnHash( int mb )
    {
    if( mb > 1024 )
        mb = 1024;
    PawnHashSize = ((1 << MSB(mb)) << 20) / sizeof(typePawnEval);
    mb = (PawnHashSize * sizeof(typePawnEval)) >> 20;

    if( PawnHash )
        ALIGNED_FREE(PawnHash);
    MEMALIGN(PawnHash, 64, PawnHashSize * sizeof(typePawnEval));
    PawnHashReset();
    return (mb);
    }
void ResetPositionalGain()
    {
    int p, m;

#ifdef MULTI_POS_GAIN
    int cpu;
    for ( cpu = 0; cpu < MAX_CPUS; cpu++ )
#endif

	for ( p = 0; p < 0x10; p++ )
		for ( m = 0; m < 010000; m++ )

#ifdef MULTI_POS_GAIN
	MAX_POSITIONAL_GAIN[cpu][p][m] = 0;
#else
    MAX_POSITIONAL_GAIN[p][m] = 0;
#endif

    }
void ResetHistory()
    {
    int pi, sq;

#ifdef MULTI_HISTORY
    int cpu;
    for ( cpu = 0; cpu < MAX_CPUS; cpu++ )
#endif

	for ( pi = 0; pi < 0x10; pi++ )
		for ( sq = A1; sq <= H8; sq++ )

#ifdef MULTI_HISTORY
	HISTORY[cpu][pi][sq] = 0x800;
#else
    HISTORY[pi][sq] = 0x800;
#endif
    }
static void InitSys()
    {
    PawnHash = NULL;
    PawnHashSize = (1 << 16);

    memset((void *)ROOT_POSITION0, 0, sizeof(typePOS));
    ROOT_POSITION0->DYN_ROOT = malloc(MAXIMUM_PLY * sizeof(typeDYNAMIC));
    ROOT_POSITION0->DYN = ROOT_POSITION0->DYN_ROOT;
    ROOT_POSITION0->stop = false;

    ANALYSIS_MODE = false;
    BUFFER_TIME = 0;
    EXTRA_INFO = false;
    DO_INFINITE = false;
    DO_PONDER = false;
    DO_SEARCH_MOVES = false;

    NUM_THREADS = 1;
    MULTI_CENTI_PAWN_PV = 65535;
    MULTI_POS_GAIN = true;
    MULTI_HISTORY = true;
    NMR_SCALING = false;
    MPH = 2;
    MULTI_PV = 1;
    OPT_MAX_THREADS = 16;
    PONDER_HIT = false;
    RANDOM_BITS = 1;
    RANDOM_COUNT = 0;
    AN_SPLIT_DEPTH = 12;
    CN_SPLIT_DEPTH = 14;
    PV_SPLIT_DEPTH = 12;
    STOP = false;
    NULL_MOVE_VERIFICATION = true;
    VERIFICATION_REDUCTION = 7;
    PValue = 100;
    NValue = 325;
    BValue = 325;
    RValue = 500;
    QValue = 975;
    BPValue = 50;
    }

int main()
    {
    InitSys();
    rp_init();
    ShowBanner();
    GetSysInfo();
    InitPawnHash(32);
    InitHash(128);
    ResetHistory();
    InitCaptureValues();
    InitArrays();
    InitPawns();
    InitMaterialValue();
    InitStatic();
    NewGame(ROOT_POSITION0, true);
    while( 1 )
        Input(ROOT_POSITION0);
    return 0;
    }
