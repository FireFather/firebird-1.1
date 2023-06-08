#include "firebird.h"
#include "control.h"
#include "null_move.h"
#include <string.h>

#define LEGAL (POSITION->wtm ? !BLACK_IN_CHECK : !WHITE_IN_CHECK)
#define IN_CHECK (POSITION->wtm ? WHITE_IN_CHECK: BLACK_IN_CHECK)

void OutputBestMove( typePOS *POSITION )
    {
    int i, k;
    typePVHash *trans;
    int PONDER_MOVE = MOVE_NONE;

    if( !ROOT_BEST_MOVE )
        {
        SEND("bestmove NULL\n");
        return;
        }
    Make(POSITION, ROOT_BEST_MOVE);
    EVAL(0);

    k = POSITION->DYN->HASH & PVHashMask;

    for ( i = 0; i < 4; i++ )
        {
        trans = PVHashTable + (k + i);

        if( trans->hash == POSITION->DYN->HASH )
            {
            PONDER_MOVE = trans->move;
            break;
            }
        }
		
    if( (POSITION->wtm ? !WhiteOK(POSITION, PONDER_MOVE) : !BlackOK(POSITION, PONDER_MOVE)) )
        PONDER_MOVE = MOVE_NONE;
    else
        {
        Make(POSITION, PONDER_MOVE);
        EVAL(0);

        if( !POSITION->wtm ? (wBitboardK &POSITION->DYN->bAtt) : (bBitboardK &POSITION->DYN->wAtt) )
            PONDER_MOVE = MOVE_NONE;
        Undo(POSITION, PONDER_MOVE);
        }

    Undo(POSITION, ROOT_BEST_MOVE);
    SEND("bestmove %s ponder %s\n", Notate(ROOT_BEST_MOVE, STRING1), Notate(PONDER_MOVE, STRING2));
    }
static char *MODIFIER( int ALPHA, int Value, int BETA, char *s )
    {
    s[0] = 0;

    if( Value <= ALPHA )
        strcpy(s, " upperbound");

    else if( Value >= BETA )
        strcpy(s, " lowerbound");

    else
        strcpy(s, "");
    return s;
    }
static char *cp_mate( int Value, char *s )
    {
    if( Value > VALUE_MATE - MAXIMUM_PLY )
        sprintf(s, "mate %d", (VALUE_MATE + 1 - Value) / 2);

    else if( Value < -VALUE_MATE + MAXIMUM_PLY )
        sprintf(s, "mate %d", (-VALUE_MATE - Value) / 2);

    else
        sprintf(s, "cp %d", Value);
    return s;
    }
void Information( typePOS *POSITION, sint64 x, int ALPHA, int Value, int BETA )
    {
    uint64 t, nps, NODES = 0;
    int cpu, rp;
    int sd, k, move;

    char pv[256 * 6], *q;
    typePVHash *trans;
    typeHash *trans2;
    uint64 HashStack[256];
    int i;
    int mpv;
    int cnt = 0;
    boolean B;
    uint64 TBHITS = 0;
    DECLARE();

    for ( cpu = 0; cpu < NUM_THREADS; cpu++ )
        for ( rp = 0; rp < RP_PER_CPU; rp++ )
            NODES += ROOT_POSITION[cpu][rp].nodes;

    for ( cpu = 0; cpu < NUM_THREADS; cpu++ )
        for ( rp = 0; rp < RP_PER_CPU; rp++ )
            TBHITS += ROOT_POSITION[cpu][rp].tbhits;

    sd = 0;
    memset(HashStack, 0, 256 * sizeof(uint64));
    t = x / 1000;

    if( t == 0 )
        nps = 0;
    else
        nps = NODES / t;

    if( MULTI_PV == 1 )
        MPV[0].move = ROOT_BEST_MOVE;

    if( MULTI_PV == 1 )
        MPV[0].Value = Value;

    for ( mpv = 0; mpv < MULTI_PV; mpv++ )
        {
        move = MPV[mpv].move;

        if( move == MOVE_NONE )
            break;

        q = pv;
        cnt = 0;
        HashStack[cnt++] = POSITION->DYN->HASH;
        Notate(move, STRING1);
        strcpy(q, STRING1);
        q += strlen(STRING1);
        strcpy(q, " ");
        q++;

        while( move )
            {
            Make(POSITION, move);
            EVAL(0);
            B = false;

            for ( i = 0; i < cnt; i++ )
                if( HashStack[i] == POSITION->DYN->HASH )
                    B = true;

            if( B )
                break;
            HashStack[cnt++] = POSITION->DYN->HASH;
            move = 0;
            k = POSITION->DYN->HASH & PVHashMask;

            for ( i = 0; i < 4; i++ )
                {
                trans = PVHashTable + (k + i);

                if( trans->hash == POSITION->DYN->HASH )
                    {
                    move = trans->move;
                    break;
                    }
                }

            if( !move )
                {
                k = POSITION->DYN->HASH & HashMask;

                for ( i = 0; i < 4; i++ )
                    {
                    trans2 = HashTable + (k + i);

                    if( trans2->hash == POSITION->DYN->HASH )
                        {
                        move = trans2->move;
                        break;
                        }
                    }
                }

            if( !move || (POSITION->wtm ? !WhiteOK(POSITION, move) : !BlackOK(POSITION, move)) )
                break;

            if( cnt > 250 )
                break;
            Notate(move, STRING1);
            strcpy(q, STRING1);
            q += strlen(STRING1);
            strcpy(q, " ");
            q++;
            }
        q--;
        *q = 0;

        while( POSITION->DYN != (POSITION->DYN_ROOT + 1) )
            {
            if( !POSITION->DYN->move )
                UndoNull(POSITION);
            else
                Undo(POSITION, POSITION->DYN->move);
            }

        SEND("info multipv %d time " TYPE_64_BIT " nodes " TYPE_64_BIT
            " nps " TYPE_64_BIT, mpv + 1, t, NODES, nps * 1000);
        if( TBHITS )
            SEND( " tbhits " TYPE_64_BIT, TBHITS );
        SEND(" score %s%s depth %d pv %s", cp_mate(MPV[mpv].Value, STRING2),
            MODIFIER(ALPHA, MPV[mpv].Value, BETA, STRING3), ROOT_DEPTH / 2, pv);
        if( init_flag && x > 400000 && EXTRA_INFO )
            {
            SEND(" cpuload %u",
                (unsigned)MIN(((double)(ProcessClock() - CPU_TIME) / (double)(x * num_CPUs) * 1000), 1000));
			init_flag = false;
            }
        SEND("\n");
        }
    }

#include <string.h>

void Search( typePOS *POSITION )
    {
    int z;
    typeDYNAMIC *p, *q;
    typePOS *POS;
    init_flag = true;
    NEW_GAME = false;
    timeBeginPeriod(1);
    START_CLOCK = GetClock();
    CPU_TIME = ProcessClock();
    PONDER_HIT = false;
    POSITION->StackHeight = -1;
    ROOT_BEST_MOVE = ROOT_DEPTH = ROOT_SCORE = 0;

    for ( p = POSITION->DYN_ROOT; p <= POSITION->DYN; p++ )
        POSITION->STACK[++(POSITION->StackHeight)] = p->HASH;
    NODE_CHECK = 0;
    ROOT_POSITION0->nodes = 0;

    if( ANALYSING )
        {
        boolean REPETITION;

        for ( p = POSITION->DYN_ROOT; p < POSITION->DYN; p++ )
            {
            REPETITION = false;

            for ( q = p + 2; q < POSITION->DYN; q += 2 )
                if( p->HASH == q->HASH )
                    {
                    REPETITION = true;
                    break;
                    }

            if( !REPETITION )
                POSITION->STACK[p - POSITION->DYN_ROOT] = 0;
            (p + 1)->move = 0;
            }
        }
    memcpy(POSITION->DYN_ROOT + 1, POSITION->DYN, sizeof(typeDYNAMIC));
    memset(POSITION->DYN_ROOT + 2, 0, 254 * sizeof(typeDYNAMIC));
    memset(POSITION->DYN_ROOT, 0, sizeof(typeDYNAMIC));
    POSITION->DYN = POSITION->DYN_ROOT + 1;
    POSITION->height = 0;
    IncrementAge();
    ROOT_PREVIOUS = -VALUE_MATE;
    EASY_MOVE = false;
    JUMP_IS_SET = true;
    POS = POSITION;
	ROOT_POSITION0->tbhits = 0;

    ivan_init_smp();
    POS = &ROOT_POSITION[0][0];

    z = setjmp(J);

    if( !z )
        {
        if( POS->wtm )
            TopWhite(POS);
        else
            TopBlack(POS);
        }

    ivan_end_smp();

    JUMP_IS_SET = false;
    PREVIOUS_DEPTH = ROOT_DEPTH;

    if( POS == POSITION )
        {
        while( POS->DYN != (POS->DYN_ROOT + 1) )
            {
            if( !POS->DYN->move )
                UndoNull(POS);
            else
                Undo(POS, POS->DYN->move);
            }
        }
    Information(POSITION, GetClock() - START_CLOCK, -32767, ROOT_SCORE, 32767);

    if( DO_INFINITE && !STOP )
        {
        while( !STOP )
            Input(POSITION);
        }

    if( DO_PONDER && !STOP && !PONDER_HIT )
        {
        while( !STOP && !PONDER_HIT )
            Input(POSITION);
        }

    OutputBestMove(POSITION);
    timeEndPeriod(1);
    }
