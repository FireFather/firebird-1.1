#include "firebird.h"
#include "null_move.h"

void OutputBestMove( typePos *Position )
    {
    int i, k;
    typePVHash *rank;
    int PonderMove = MoveNone;

    if( !RootBestMove )
        {
        Send("bestmove NULL\n");
        return;
        }
    Make(Position, RootBestMove);
    EVAL(0);

    k = Position->Current->Hash & PVHashMask;

    for ( i = 0; i < 4; i++ )
        {
        rank = PVHashTable + (k + i);

        if( rank->hash == Position->Current->Hash )
            {
            PonderMove = rank->move;
            break;
            }
        }
		
    if( (Position->wtm ? !WhiteOK(Position, PonderMove) : !BlackOK(Position, PonderMove)) )
        PonderMove = MoveNone;
    else
        {
        Make(Position, PonderMove);
        EVAL(0);

        if( !Position->wtm ? (wBitboardK &Position->Current->bAtt) : (bBitboardK &Position->Current->wAtt) )
            PonderMove = MoveNone;
        Undo(Position, PonderMove);
        }

    Undo(Position, RootBestMove);
    Send("bestmove %s ponder %s\n", Notate(RootBestMove, String1), Notate(PonderMove, String2));
    }
static char *Modifier( int Alpha, int Value, int Beta, char *s )
    {
    s[0] = 0;

    if( Value <= Alpha )
        strcpy(s, " upperbound");

    else if( Value >= Beta )
        strcpy(s, " lowerbound");

    else
        strcpy(s, "");
    return s;
    }
static char *cp_mate( int Value, char *s )
    {
    if( Value > ValueMate - MaxPly )
        sprintf(s, "mate %d", (ValueMate + 1 - Value) / 2);

    else if( Value < -ValueMate + MaxPly )
        sprintf(s, "mate %d", (-ValueMate - Value) / 2);

    else
        sprintf(s, "cp %d", Value);
    return s;
    }
void Information( typePos *Position, sint64 x, int Alpha, int Value, int Beta )
    {
    uint64 t, nps, Nodes = 0;
    int cpu, rp;
    int sd, k, move;

    char pv[256 * 6], *q;
    typePVHash *rank;
    typeHash *trans2;
    uint64 HashStack[256];
    int i;
    int mpv;
    int cnt = 0;
    boolean B;
 
    for ( cpu = 0; cpu < NumThreads; cpu++ )
        for ( rp = 0; rp < RPperCPU; rp++ )
            Nodes += RootPosition[cpu][rp].nodes;

    sd = 0;
    memset(HashStack, 0, 256 * sizeof(uint64));
    t = x / 1000;

    if( t == 0 )
        nps = 0;
    else
        nps = Nodes / t;

    if( MultiPV == 1 )
        MPV[0].move = RootBestMove;

    if( MultiPV == 1 )
        MPV[0].Value = Value;

    for ( mpv = 0; mpv < MultiPV; mpv++ )
        {
        move = MPV[mpv].move;

        if( move == MoveNone )
            break;

        q = pv;
        cnt = 0;
        HashStack[cnt++] = Position->Current->Hash;
        Notate(move, String1);
        strcpy(q, String1);
        q += strlen(String1);
        strcpy(q, " ");
        q++;

        while( move )
            {
            Make(Position, move);
            EVAL(0);
            B = false;

            for ( i = 0; i < cnt; i++ )
                if( HashStack[i] == Position->Current->Hash )
                    B = true;

            if( B )
                break;
            HashStack[cnt++] = Position->Current->Hash;
            move = 0;
            k = Position->Current->Hash & PVHashMask;

            for ( i = 0; i < 4; i++ )
                {
                rank = PVHashTable + (k + i);

                if( rank->hash == Position->Current->Hash )
                    {
                    move = rank->move;
                    break;
                    }
                }

            if( !move )
                {
                k = Position->Current->Hash & HashMask;

                for ( i = 0; i < 4; i++ )
                    {
                    trans2 = HashTable + (k + i);

                    if( trans2->hash == Position->Current->Hash )
                        {
                        move = trans2->move;
                        break;
                        }
                    }
                }

            if( !move || (Position->wtm ? !WhiteOK(Position, move) : !BlackOK(Position, move)) )
                break;

            if( cnt > 250 )
                break;
            Notate(move, String1);
            strcpy(q, String1);
            q += strlen(String1);
            strcpy(q, " ");
            q++;
            }
        q--;
        *q = 0;

        while( Position->Current != (Position->Root + 1) )
            {
            if( !Position->Current->move )
                UndoNull(Position);
            else
                Undo(Position, Position->Current->move);
            }

        Send("info multipv %d time %I64u nodes %I64u nps %I64u ", mpv + 1, t, Nodes, nps * 1000);
        Send("score %s%s depth %d pv %s", cp_mate(MPV[mpv].Value, String2), Modifier(Alpha, MPV[mpv].Value, Beta, String3), RootDepth / 2, pv);
        Send("\n");
        }
    }

void Search( typePos *Position )
    {
    int z;
    typePosition *p, *q;
    typePos *Pos;
    IsNewGame = false;
    StartClock = GetClock();
    PonderHit = false;
    Position->StackHeight = -1;
    RootBestMove = RootDepth = RootScore = 0;

    for ( p = Position->Root; p <= Position->Current; p++ )
        Position->Stack[++(Position->StackHeight)] = p->Hash;
    NodeCheck = 0;
    RootPosition0->nodes = 0;

    if( Analysing )
        {
        boolean Repetition;

        for ( p = Position->Root; p < Position->Current; p++ )
            {
            Repetition = false;

            for ( q = p + 2; q < Position->Current; q += 2 )
                if( p->Hash == q->Hash )
                    {
                    Repetition = true;
                    break;
                    }

            if( !Repetition )
                Position->Stack[p - Position->Root] = 0;
            (p + 1)->move = 0;
            }
        }
    memcpy(Position->Root + 1, Position->Current, sizeof(typePosition));
    memset(Position->Root + 2, 0, 254 * sizeof(typePosition));
    memset(Position->Root, 0, sizeof(typePosition));
    Position->Current = Position->Root + 1;
    Position->height = 0;
    IncrementAge();
    RootPrevious = -ValueMate;
    EasyMove = false;
    JumpIsSet = true;
    Pos = Position;

    InitSMP();
    Pos = &RootPosition[0][0];

    z = setjmp(J);

    if( !z )
        {
        if( Pos->wtm )
            TopWhite(Pos);
        else
            TopBlack(Pos);
        }

    EndSMP();

    JumpIsSet = false;
    PreviousDepth = RootDepth;

    if( Pos == Position )
        {
        while( Pos->Current != (Pos->Root + 1) )
            {
            if( !Pos->Current->move )
                UndoNull(Pos);
            else
                Undo(Pos, Pos->Current->move);
            }
        }
    Information(Position, GetClock() - StartClock, -32767, RootScore, 32767);

    if( DoInfinite && !Stop )
        {
        while( !Stop )
            Input(Position);
        }

    if( DoPonder && !Stop && !PonderHit )
        {
        while( !Stop && !PonderHit )
            Input(Position);
        }

    OutputBestMove(Position);
    }
