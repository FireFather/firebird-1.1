#include "firebird.h"
jmp_buf J;

static int MovesInTC;
static int OldMTG;
static sint64 TotalTimeWhite;
static sint64 TotalTimeBlack;
static int AbsoluteFactor = 25;
static int BattleFactor = 100;
static int DesiredMillis = 40;
static int EasyFactor = 15;
static int EasyFactorPonder = 33;
static int NormalFactor = 75;

void ponderhit()
    {
    if( !DoPonder )
        return;
    PonderHit = true;
    DoPonder = false;
    NewPonderHit = true;

	if( EasyMove )
		HaltSearch();
    }
void HaltSearch()
    {
    Stop = true;

    if( JumpIsSet )
        longjmp(J, 1);
    }
void Info( sint64 x )
    {
    uint64 time, nps, nodes = 0;
    int cpu, rp;

    for ( cpu = 0; cpu < NumThreads; cpu++ )
        for ( rp = 0; rp < RPperCPU; rp++ )
            nodes += RootPosition[cpu][rp].nodes;

    time = x / 1000;

    if( time == 0 )
        nps = 0;
    else
        nps = nodes / time * 1000;

    Send("info time %I64u nodes %I64u nps %I64u\n", time, nodes, nps);
    }
void CheckDone( typePos *Position, int d )
    {
    sint64 x;

    if( !JumpIsSet )
        return;

    x = GetClock() - StartClock;

    if( d == Depth )
        HaltSearch(1);

    if( ExtraInfo )
        {
        if( x - LastMessage > 1000000 )
            Info(x);
        }

    if( DoPonder )
        goto End;

    if( DoInfinite )
        goto End;

    if( d >= 1 && d < 8 )
        goto End;

    if( x > AbsoluteTime )
        HaltSearch();

    if( d == 0 && !NewPonderHit )
        goto End;
    NewPonderHit = false;

    if( !BadMove && x >= BattleTime )
        HaltSearch();

    if( EasyMove && x >= EasyTime )
        HaltSearch();

    if( !BattleMove && x >= OrdinaryTime && !BadMove )
        HaltSearch();
    End:
    while( TryInput() )
        Input(Position);
    }
static uint32 FullMove( typePos *Position, uint32 x )
    {
    int pi, to = To(x), fr = From(x);

    if( !x )
        return x;

    pi = Position->sq[fr];

    if( pi == wEnumK || pi == bEnumK )
        {
        if( to - fr == 2 || fr - to == 2 )
            x |= FlagOO;
        }

    if( To(x) != 0 && To(x) == Position->Current->ep && (pi == wEnumP || pi == bEnumP) )
        x |= FlagEP;
    return x;
    }
static uint32 numeric_move( typePos *Position, char *str )
    {
    int x;
    x = FullMove(Position, (str[2] - 'a') + ((str[3] - '1') << 3) + ((str[0] - 'a') << 6) + ((str[1] - '1') << 9));

    if( str[4] == 'b' )
        x |= FlagPromB;

    if( str[4] == 'n' )
        x |= FlagPromN;

    if( str[4] == 'r' )
        x |= FlagPromR;

    if( str[4] == 'q' )
        x |= FlagPromQ;
    return x;
    }

void TimeManager(sint64 Time, sint64 OppTime, sint64 Increment, int MTG)
	{
	if (MTG)
		{
		if (MTG > 25)
			MTG = 25;
		DesiredTime = Time / MTG + Increment;
		AbsoluteTime = (Time * MTG) / ((MTG << 2) - 3) - MIN(1000000, Time / 10);
		if (MTG == 1)
			AbsoluteTime -= MIN(1000000, AbsoluteTime / 10);
		if (AbsoluteTime < 1000)
			AbsoluteTime = 1000;
		}
	else
		{
		AbsoluteTime = (Time * AbsoluteFactor) / 100 - 10000;
		if (AbsoluteTime < 1000)
			AbsoluteTime = 1000;
		DesiredTime = (Time * DesiredMillis) / 1000 + Increment;
		}
	if (DesiredTime > AbsoluteTime)
		DesiredTime = AbsoluteTime;
	if (DesiredTime < 1000)
		DesiredTime = 1000;
	EasyTime = (DesiredTime * EasyFactor) / 100;
	if (DoPonder)
		EasyTime = (DesiredTime * EasyFactorPonder) / 100;
	BattleTime = (DesiredTime * BattleFactor) / 100;
	OrdinaryTime = (DesiredTime * NormalFactor) / 100;
	}

void InitSearch( typePos *Position, char *str )
    {
    char *p;
    sint64 wtime = Infinite, btime = Infinite, Time, OppTime;
    int winc = 0, binc = 0, MTG = 0;
    int sm = 0;
    Depth = 255;
    AbsoluteTime = DesiredTime = Infinite;
    Stop = false;
    DoInfinite = false;
    DoPonder = false;
    NewPonderHit = false;
    DoSearchMoves = false;
    LastMessage = 0;

    p = strtok(str, " ");

    for ( StrTok(p); p != NULL; StrTok(p) )
        {
        if( !strcmp(p, "depth") )
            {
            StrTok(p);
            Depth = MAX(1, atoi(p));
            }
        else if( !strcmp(p, "movetime") )
            {
            StrTok(p);
            AbsoluteTime = MAX(1, atoll(p)) * 1000 - 10000;
            }
        else if( !strcmp(p, "wtime") )
            {
            StrTok(p);
            wtime = atoll(p) * 1000;
            }
        else if( !strcmp(p, "winc") )
            {
            StrTok(p);
            winc = atoll(p) * 1000;
            }
        else if( !strcmp(p, "btime") )
            {
            StrTok(p);
            btime = atoll(p) * 1000;
            }
        else if( !strcmp(p, "binc") )
            {
            StrTok(p);
            binc = atoll(p) * 1000;
            }
        else if( !strcmp(p, "movestogo") )
            {
            StrTok(p);
            MTG = atoi(p);
            }

        else if( !strcmp(p, "infinite") )
            DoInfinite = true;

        else if( !strcmp(p, "ponder") )
            DoPonder = true;

        else if( !strcmp(p, "searchmoves") )
            DoSearchMoves = true;

        else if( DoSearchMoves )
            SearchMoves[sm++] = numeric_move(Position, p);

        else { };
        }

	BattleTime = Infinite;
	OrdinaryTime = Infinite;
	EasyTime = Infinite;

	if (IsNewGame || MTG > OldMTG)
		{
		MovesInTC = MTG;
		TotalTimeWhite = MAX(wtime - 500000, (95 * wtime) / 100);
		TotalTimeBlack = MAX(btime - 500000, (95 * btime) / 100);
		}

	Time = Position->wtm ? wtime : btime;
	OppTime = Position->wtm ? btime : wtime;

	if (Time < 0)
		Time = 0;
	if (Time == Infinite)
		goto End;

	Increment = Position->wtm ? winc : binc;

	TimeManager(Time, OppTime, Increment, MTG);

    End:
    if( AbsoluteTime == Infinite )
        Analysing = true;
    else
        Analysing = false;

    if( DoSearchMoves )
        SearchMoves[sm] = MoveNone;
    }
