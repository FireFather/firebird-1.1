#include "firebird.h"
#include <string.h>

char *Notate( uint32 move, char *M )
    {
    int fr, to, pr;
    char c[16] = "0123nbrq";
    fr = FROM(move);
    to = TO(move);

    if( move == MOVE_NONE )
        {
        M[0] = 'N';
        M[1] = 'U';
        M[2] = M[3] = 'L';
        M[4] = 0;
        return M;
        }
    sprintf(M, "%c%c%c%c", 'a' + (fr & 7), '1' + ((fr >> 3) & 7),
		'a' + (to & 7), '1' + ((to >> 3) & 7));

    if( MoveIsProm(move) )
        {
        pr = (move &FLAG_MASK) >> 12;
        sprintf(M + 4, "%c", c[pr]);
        }
    return M;
    }
static void BAD( char *x )
    {
    ERROR_END("position error: %s\n", x);
    }

#include "material_value.h"

#define TWEAK (0x74d3c012a8bf965e)

void InitBitboards( typePOS *POSITION )
    {
    int i, b, pi;
    uint64 O;
    BOARD_IS_OK = false;

    for ( i = 0; i < 16; i++ )
        POSITION->bitboard[i] = 0;
    POSITION->DYN->HASH = POSITION->DYN->PAWN_HASH = 0;
    POSITION->DYN->material = 0;
    POSITION->DYN->STATIC = 0;

    for ( i = A1; i <= H8; i++ )
        {
        if( (pi = POSITION->sq[i]) )
            {
            POSITION->DYN->STATIC += PieceSquareValue[pi][i];
            POSITION->DYN->HASH ^= HASH[pi][i];

            if( pi == wEnumP || pi == bEnumP )
                POSITION->DYN->PAWN_HASH ^= HASH[pi][i];
            POSITION->DYN->material += MATERIAL_VALUE[pi];
            BitSet(i, POSITION->bitboard[POSITION->sq[i]]);
            }
        }
    wBitboardOcc = wBitboardK | wBitboardQ | wBitboardR | wBitboardB | wBitboardN | wBitboardP;
    bBitboardOcc = bBitboardK | bBitboardQ | bBitboardR | bBitboardB | bBitboardN | bBitboardP;
    POSITION->OccupiedBW = wBitboardOcc | bBitboardOcc;
    POSITION->OccupiedL90 = POSITION->OccupiedL45 = POSITION->OccupiedR45 = 0;
    O = POSITION->OccupiedBW;

	if (POPCNT (wBitboardQ) > 1 || POPCNT (bBitboardQ) > 1
		|| POPCNT (wBitboardR) > 2 || POPCNT (bBitboardR) > 2
		|| POPCNT (wBitboardBL) > 1 || POPCNT (bBitboardBL) > 1
		|| POPCNT (wBitboardN) > 2 || POPCNT (bBitboardN) > 2
		|| POPCNT (wBitboardBD) > 1 || POPCNT (bBitboardBD) > 1)
    POSITION->DYN->material |= 0x80000000;

    if( POPCNT(wBitboardK) != 1 )
        BAD("white king = 1");

    if( POPCNT(bBitboardK) != 1 )
        BAD("black  king = 1");

    if( POPCNT(wBitboardQ) > 9 )
        BAD("white queen > 9");

    if( POPCNT(bBitboardQ) > 9 )
        BAD("black queen > 9");

    if( POPCNT(wBitboardR) > 10 )
        BAD("white rook > 10");

    if( POPCNT(bBitboardR) > 10 )
        BAD("black rook > 10");

    if( POPCNT(wBitboardBL) > 9 )
        BAD("white light bishop > 9");

    if( POPCNT(bBitboardBL) > 9 )
        BAD("black light bishop > 9");

    if( POPCNT(wBitboardBD) > 9 )
        BAD("white dark bishop > 9");

    if( POPCNT(bBitboardBD) > 9 )
        BAD("black dark bishop > 9");

    if( POPCNT(wBitboardBL | wBitboardBD) > 10 )
        BAD("white bishop > 10");

    if( POPCNT(bBitboardBL | bBitboardBD) > 10 )
        BAD("black bishop > 10");

    if( POPCNT(wBitboardN) > 10 )
        BAD("white knight > 10");

    if( POPCNT(bBitboardN) > 10 )
        BAD("black knight > 10");

    if( POPCNT(wBitboardP) > 8 )
        BAD("white pawn > 8");

    if( POPCNT(bBitboardP) > 8 )
        BAD("black pawn > 8");

    if( POPCNT(wBitboardOcc) > 16 )
        BAD("white piece > 16");

    if( POPCNT(bBitboardOcc) > 16 )
        BAD("black piece > 16");

    if( (wBitboardP | bBitboardP) & (RANK1 | RANK8) )
        BAD("pawn on rank one or eight");

    while( O )
        {
        b = LSB(O);
        BitClear(b, O);
        BitSet(Left90[b], POSITION->OccupiedL90);
        BitSet(Left45[b], POSITION->OccupiedL45);
        BitSet(Right45[b], POSITION->OccupiedR45);
        }
    POSITION->wKsq = LSB(wBitboardK);
    POSITION->bKsq = LSB(bBitboardK);

    if( (WhiteOO && (POSITION->wKsq != E1 || !(wBitboardR &SqSet[H1])))
        || (WhiteOOO && (POSITION->wKsq != E1 || !(wBitboardR &SqSet[A1])))
            || (BlackOO && (POSITION->bKsq != E8 || !(bBitboardR &SqSet[H8])))
            || (BlackOOO && (POSITION->bKsq != E8 || !(bBitboardR &SqSet[A8]))) )
        BAD("illegal castle move");
    POSITION->DYN->HASH ^= ZobristCastling[POSITION->DYN->oo];

    if( POSITION->DYN->ep )
        POSITION->DYN->HASH ^= ZobristEP[POSITION->DYN->ep & 7];
    POSITION->DYN->PAWN_HASH ^=
        ZobristCastling[POSITION->DYN->oo] ^ TWEAK ^ HASH[wEnumK][POSITION->wKsq] ^ HASH[bEnumK][POSITION->bKsq];

    if( POSITION->wtm )
        POSITION->DYN->HASH ^= ZobristWTM;
    EVAL(0);

    if( POSITION->wtm && POSITION->DYN->wAtt & bBitboardK )
        BAD("white captures king");

    if( !POSITION->wtm && POSITION->DYN->bAtt & wBitboardK )
        BAD("black captures king");
    BOARD_IS_OK = true;
    }

#ifdef WINDOWS
#include <time.h>
#include <conio.h>

boolean TryInput()
    {
    static int init = 0, is_pipe;
    static HANDLE stdin_h;
    DWORD val;

    if( !init )
        {
        init = 1;
        stdin_h = GetStdHandle(STD_INPUT_HANDLE);
        is_pipe = !GetConsoleMode(stdin_h, &val);

        if( !is_pipe )
            {
            SetConsoleMode(stdin_h, val & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
            FlushConsoleInputBuffer(stdin_h);
            }
        }

    if( is_pipe )
        {
        if( !PeekNamedPipe(stdin_h, NULL, 0, NULL, &val, NULL) )
            return 1;
        return val > 0;
        }
    else
        {
        return _kbhit();
        /* GetNumberOfConsoleInputEvents(stdin_h, &val);
        return val > 1; */
        }
    return 0;
    }

uint64 GetClock()
{
	return (timeGetTime() * (uint64)1000);
}
	
uint64 ProcessClock()
{
	FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;
	LARGE_INTEGER user_time, kernel_time;
	uint64 x;
	uint64 tt = 10; 
	
	GetProcessTimes(GetCurrentProcess(), &ftProcCreation, &ftProcExit, &ftProcKernel, &ftProcUser);

	user_time.LowPart = ftProcUser.dwLowDateTime;
	user_time.HighPart = ftProcUser.dwHighDateTime;
	kernel_time.LowPart = ftProcKernel.dwLowDateTime;
	kernel_time.HighPart = ftProcKernel.dwHighDateTime;  	
	x = (uint64) (user_time.QuadPart + kernel_time.QuadPart) / tt;
	return x;
}
#else /* !WINDOWS */
#include <unistd.h>

boolean TryInput()
    {
    int v;
    fd_set s[1];
    struct timeval tv[1];
    FD_ZERO(s);
    FD_SET(STDIN_FILENO, s);
    tv->tv_sec = 0;
    tv->tv_usec = 0;
    v = select(STDIN_FILENO + 1, s, NULL, NULL, tv);
    return (v > 0);
    }
#include <sys/time.h>

uint64 GetClock()
    {
    uint64 x;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    x = tv.tv_sec;
    x *= 1000000;
    x += tv.tv_usec;
    return x;
    }
uint64 ProcessClock()
    {
    return (uint64)clock();
    }
#endif /* ifdef WINDOWS */

#include <stdarg.h>

void SEND( char *fmt, ... )
    {
    va_list Value;
    va_start(Value, fmt);
    vfprintf(stdout, fmt, Value);
    va_end(Value);
    fflush(stdout);
    }

void ERROR_END( char *fmt, ... )
    {
    va_list Value;
    va_start(Value, fmt);
    va_end(Value);
    //  stdout = stderr; // WINDOWS
    fprintf(stdout, "*** ERRORE ***\n");
    vfprintf(stdout, fmt, Value);
    exit(1);
    }
void FEN_ERROR( char *fmt, ... )
    {
    va_list Value;
    va_start(Value, fmt);
    va_end(Value);
    //  stdout = stderr; // WINDOWS
    fprintf(stdout, "*** FEN ERR ***\n");
    vfprintf(stdout, fmt, Value);
    exit(1);
    }

int PREVIOUS_DEPTH, PREVIOUS_FAST;
void NewGame( typePOS *POSITION, boolean full )
    {
    int i;

    for ( i = A1; i <= H8; i++ )
        POSITION->sq[i] = 0;
    memset(POSITION->DYN_ROOT, 0, 256 * sizeof(typeDYNAMIC));
    POSITION->DYN = POSITION->DYN_ROOT + 1;
    POSITION->wtm = true;
    POSITION->height = 0;
    POSITION->DYN->oo = 0x0f;
    POSITION->DYN->ep = 0;
    POSITION->DYN->reversible = 0;

    for ( i = A2; i <= H2; i++ )
        POSITION->sq[i] = wEnumP;

    for ( i = A7; i <= H7; i++ )
        POSITION->sq[i] = bEnumP;
    POSITION->sq[D1] = wEnumQ;
    POSITION->sq[D8] = bEnumQ;
    POSITION->sq[E1] = wEnumK;
    POSITION->sq[E8] = bEnumK;
    POSITION->sq[A1] = POSITION->sq[H1] = wEnumR;
    POSITION->sq[A8] = POSITION->sq[H8] = bEnumR;
    POSITION->sq[B1] = POSITION->sq[G1] = wEnumN;
    POSITION->sq[B8] = POSITION->sq[G8] = bEnumN;
    POSITION->sq[C1] = wEnumBD;
    POSITION->sq[F1] = wEnumBL;
    POSITION->sq[C8] = bEnumBL;
    POSITION->sq[F8] = bEnumBD;
    PREVIOUS_DEPTH = 1000;
    PREVIOUS_FAST = false;
    NEW_GAME = true;
    POSITION->StackHeight = 0;
    InitBitboards(POSITION);

    if( !full )
        return;
    HashClear();
    EvalHashClear();
    ResetHistory();
    ResetPositionalGain();
    PawnHashReset();
    }
void ShowBanner()
    {
    char *startup_banner = "" NAME " " VERSION "\n"
        "by Kranium and Sentinel\n"
        "based on IppoLit\n"
        "Compiled by KLO\n"
        "" __DATE__ " " __TIME__ "\n\n";

    SEND(startup_banner);
    fflush(stdout);
    }

void GetSysInfo()
    {
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    num_CPUs = sysinfo.dwNumberOfProcessors;

    if( num_CPUs < 1 )
        num_CPUs = 1;

    if( num_CPUs > 1 )
        SEND("%d CPUs found\n", num_CPUs);
    else
        SEND("%d CPU found\n", num_CPUs);
	    
	NUM_THREADS = num_CPUs;

    if( NUM_THREADS > MAX_CPUS )
        NUM_THREADS = MAX_CPUS;

    if( NUM_THREADS > OPT_MAX_THREADS )
        NUM_THREADS = OPT_MAX_THREADS;


    if( NUM_THREADS > 1 )
        SEND("using %d threads\n\n", NUM_THREADS);
    else
        SEND("using %d thread\n\n", NUM_THREADS);
    }
