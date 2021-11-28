#include "firebird.h"

char *Notate( uint32 move, char *M )
    {
    int fr, to, pr;
    char c[16] = "0123nbrq";
    fr = From(move);
    to = To(move);

    if( move == MoveNone )
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
        pr = (move &FlagMask) >> 12;
        sprintf(M + 4, "%c", c[pr]);
        }
    return M;
    }

void InitBitboards( typePos *Position )
    {
    int i, b, pi;
    uint64 O;
    BoardIsOk = false;

    for ( i = 0; i < 16; i++ )
        Position->bitboard[i] = 0;
    Position->Current->Hash = Position->Current->PawnHash = 0;
    Position->Current->material = 0;
    Position->Current->PST = 0;

    for ( i = A1; i <= H8; i++ )
        {
        if( (pi = Position->sq[i]) )
            {
            Position->Current->PST += PieceSquareValue[pi][i];
            Position->Current->Hash ^= Hash[pi][i];

            if( pi == wEnumP || pi == bEnumP )
                Position->Current->PawnHash ^= Hash[pi][i];
            Position->Current->material += MATERIAL_VALUE[pi];
            BitSet(i, Position->bitboard[Position->sq[i]]);
            }
        }
    wBitboardOcc = wBitboardK | wBitboardQ | wBitboardR | wBitboardB | wBitboardN | wBitboardP;
    bBitboardOcc = bBitboardK | bBitboardQ | bBitboardR | bBitboardB | bBitboardN | bBitboardP;
    Position->OccupiedBW = wBitboardOcc | bBitboardOcc;
    Position->OccupiedL90 = Position->OccupiedL45 = Position->OccupiedR45 = 0;
    O = Position->OccupiedBW;

	if (POPCNT (wBitboardQ) > 1 || POPCNT (bBitboardQ) > 1
		|| POPCNT (wBitboardR) > 2 || POPCNT (bBitboardR) > 2
		|| POPCNT (wBitboardBL) > 1 || POPCNT (bBitboardBL) > 1
		|| POPCNT (wBitboardN) > 2 || POPCNT (bBitboardN) > 2
		|| POPCNT (wBitboardBD) > 1 || POPCNT (bBitboardBD) > 1)
    Position->Current->material |= 0x80000000;

    while( O )
        {
        b = LSB(O);
        BitClear(b, O);
        BitSet(Left90[b], Position->OccupiedL90);
        BitSet(Left45[b], Position->OccupiedL45);
        BitSet(Right45[b], Position->OccupiedR45);
        }
    Position->wKsq = LSB(wBitboardK);
    Position->bKsq = LSB(bBitboardK);

    Position->Current->Hash ^= ZobristCastling[Position->Current->oo];

    if( Position->Current->ep )
        Position->Current->Hash ^= ZobristEP[Position->Current->ep & 7];
    Position->Current->PawnHash ^=
        ZobristCastling[Position->Current->oo] ^ Tweak ^ Hash[wEnumK][Position->wKsq] ^ Hash[bEnumK][Position->bKsq];

    if( Position->wtm )
        Position->Current->Hash ^= ZobristWTM;

    EVAL(0);

    BoardIsOk = true;
    }

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
        return _kbhit();
    return 0;
    }

uint64 GetClock( void )
    {
    uint64 x;
	uint64 y = 1000; 
    x = GetTickCount() * y;
    return x;
    }

void Send( char *fmt, ... )
    {
    va_list Value;
    va_start(Value, fmt);
    vfprintf(stdout, fmt, Value);
    va_end(Value);
    fflush(stdout);
    }

void NewGame( typePos *Position, boolean full )
    {
    int i;

    for ( i = A1; i <= H8; i++ )
        Position->sq[i] = 0;
    memset(Position->Root, 0, 256 * sizeof(typePosition));
    Position->Current = Position->Root + 1;
    Position->wtm = true;
    Position->height = 0;
    Position->Current->oo = 0x0f;
    Position->Current->ep = 0;
    Position->Current->reversible = 0;

    for ( i = A2; i <= H2; i++ )
        Position->sq[i] = wEnumP;

    for ( i = A7; i <= H7; i++ )
        Position->sq[i] = bEnumP;
    Position->sq[D1] = wEnumQ;
    Position->sq[D8] = bEnumQ;
    Position->sq[E1] = wEnumK;
    Position->sq[E8] = bEnumK;
    Position->sq[A1] = Position->sq[H1] = wEnumR;
    Position->sq[A8] = Position->sq[H8] = bEnumR;
    Position->sq[B1] = Position->sq[G1] = wEnumN;
    Position->sq[B8] = Position->sq[G8] = bEnumN;
    Position->sq[C1] = wEnumBD;
    Position->sq[F1] = wEnumBL;
    Position->sq[C8] = bEnumBL;
    Position->sq[F8] = bEnumBD;
    PreviousDepth = 1000;
    PreviousFast = false;
    IsNewGame = true;
    Position->StackHeight = 0;
    InitBitboards(Position);

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

    Send(startup_banner);
    fflush(stdout);
    }

void GetSysInfo()
    {
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    NumCPUs = sysinfo.dwNumberOfProcessors;

    if( NumCPUs < 1 )
        NumCPUs = 1;

    if( NumCPUs > MaxCPUs )
        NumCPUs = MaxCPUs;

    if( NumCPUs > 1 )
        Send("info string %d CPUs found\n", NumCPUs);
    else
        Send("info string %d CPU found\n", NumCPUs);

 	UCIMaxThreads = NumCPUs;
    if( UCIMaxThreads > MaxThreads )
        UCIMaxThreads = MaxThreads;

	NumThreads = NumCPUs;
    if( NumThreads > MaxThreads )
        NumThreads = MaxThreads;

    if( NumThreads > UCIMaxThreads )
        NumThreads = UCIMaxThreads;

    if( NumThreads > 1 )
        Send("info string using %d threads\n\n", NumThreads);
    else
        Send("info string using %d thread\n\n", NumThreads);
    }
