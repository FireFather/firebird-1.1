#include "firebird.h"
#include <string.h>
#include "control.h"

#define INFINITY 0x7ffffffffffffff
#define STRTOK(p) p = strtok (NULL, " ")

static sint64 LAST_MESSAGE;
static sint64 ABSOLUTE_TIME, INCREMENT;
static sint64 BATTLE_TIME, EASY_TIME, ORDINARY_TIME;
static sint64 total_wtime, total_btime, TOTAL_TIME;
static int TOTAL_MTG, MTG_PREV;
static int DEPTH;
static boolean NEW_PONDERHIT;
jmp_buf J;
void ponderhit()
    {
    if( !DO_PONDER )
        return;
    PONDER_HIT = true;
    DO_PONDER = false;
    NEW_PONDERHIT = true;

    if( MPH == 3 )
        HaltSearch(5);

    else if( MPH == 2 && EASY_MOVE )
        HaltSearch(5);
    }
void HaltSearch( int tr )
    {
    STOP = true;

    if( JUMP_IS_SET )
        longjmp(J, 1);
    }
void Info( sint64 x )
    {
    uint64 t, nps, NODES = 0;
    int cpu, rp;
    clock_t u;
    uint64 TBHITS = 0;

    for ( cpu = 0; cpu < NUM_THREADS; cpu++ )
        for ( rp = 0; rp < RP_PER_CPU; rp++ )
            NODES += ROOT_POSITION[cpu][rp].nodes;

    for ( cpu = 0; cpu < NUM_THREADS; cpu++ )
        for ( rp = 0; rp < RP_PER_CPU; rp++ )
            TBHITS += ROOT_POSITION[cpu][rp].tbhits;
    u = clock();
    t = x / 1000;

    if( t == 0 )
        nps = 0;
    else
        nps = NODES / t;
    u = ProcessClock() - CPU_TIME;

    SEND("info time " TYPE_64_BIT " nodes " TYPE_64_BIT " nps " TYPE_64_BIT, t, NODES, nps * 1000);
    if( TBHITS )
        SEND( " tbhits " TYPE_64_BIT, TBHITS );
    SEND(" cpuload %d\n", (int) MIN(((double) u / (double) ((x - LAST_MESSAGE) * num_CPUs) * 1000.0), 1000));
    LAST_MESSAGE = x;
    CPU_TIME += u;
    }
void CheckDone( typePOS *POSITION, int d )
    {
    sint64 x;

    if( !JUMP_IS_SET )
        return;

    x = GetClock() - START_CLOCK;

    if( d == DEPTH )
        HaltSearch(1);

    if( EXTRA_INFO )
        {
        if( x - LAST_MESSAGE > 1000000 )
            Info(x);
        }

    if( DO_PONDER )
        goto END;

    if( DO_INFINITE )
        goto END;

    if( d >= 1 && d < 8 )
        goto END;

    if( x > ABSOLUTE_TIME )
        HaltSearch(1);

    if( d == 0 && !NEW_PONDERHIT )
        goto END;
    NEW_PONDERHIT = false;

    if( !BAD_MOVE && x >= BATTLE_TIME )
        HaltSearch(2);

    if( EASY_MOVE && x >= EASY_TIME )
        HaltSearch(3);

    if( !BATTLE_MOVE && x >= ORDINARY_TIME && !BAD_MOVE )
        HaltSearch(4);
    END:
    while( TryInput() )
        Input(POSITION);
    }
static uint32 FullMove( typePOS *POSITION, uint32 x )
    {
    int pi, to = TO(x), fr = FROM(x);

    if( !x )
        return x;

    pi = POSITION->sq[fr];

    if( pi == wEnumK || pi == bEnumK )
        {
        if( to - fr == 2 || fr - to == 2 )
            x |= FlagOO;
        }

    if( TO(x) != 0 && TO(x) == POSITION->DYN->ep && (pi == wEnumP || pi == bEnumP) )
        x |= FlagEP;
    return x;
    }
static uint32 numeric_move( typePOS *POSITION, char *str )
    {
    int x;
    x = FullMove(POSITION, (str[2] - 'a') + ((str[3] - '1') << 3) + ((str[0] - 'a') << 6) + ((str[1] - '1') << 9));

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

void InitSearch( typePOS *POSITION, char *str )
    {
    char *p;
    sint64 wtime = INFINITY, btime = INFINITY, TIME;
    int winc = 0, binc = 0, MTG = 0;
    int sm = 0;
    int PTF = 0;
    float time_factor = 1.0;
    float scale_factor = 1.0;
    DEPTH = 255;
    ABSOLUTE_TIME = DESIRED_TIME = INFINITY;
    STOP = false;
    DO_INFINITE = false;
    DO_PONDER = false;
    NEW_PONDERHIT = false;
    DO_SEARCH_MOVES = false;
    LAST_MESSAGE = 0;

    p = strtok(str, " ");

    for ( STRTOK(p); p != NULL; STRTOK(p) )
        {
        if( !strcmp(p, "depth") )
            {
            STRTOK(p);
            DEPTH = MAX(1, atoi(p));
            }
        else if( !strcmp(p, "movetime") )
            {
            STRTOK(p);
            ABSOLUTE_TIME = MAX(1, atoll(p)) * 1000 - 10000;
            }
        else if( !strcmp(p, "wtime") )
            {
            STRTOK(p);
            wtime = atoll(p) * 1000;
            }
        else if( !strcmp(p, "winc") )
            {
            STRTOK(p);
            winc = atoll(p) * 1000;
            }
        else if( !strcmp(p, "btime") )
            {
            STRTOK(p);
            btime = atoll(p) * 1000;
            }
        else if( !strcmp(p, "binc") )
            {
            STRTOK(p);
            binc = atoll(p) * 1000;
            }
        else if( !strcmp(p, "movestogo") )
            {
            STRTOK(p);
            MTG = atoi(p);
            }

        else if( !strcmp(p, "infinite") )
            DO_INFINITE = true;

        else if( !strcmp(p, "ponder") )
            DO_PONDER = true;

        else if( !strcmp(p, "searchmoves") )
            DO_SEARCH_MOVES = true;

        else if( DO_SEARCH_MOVES )
            SEARCH_MOVES[sm++] = numeric_move(POSITION, p);

        else { };
        }

    if( DO_PONDER )
        if( MPH == 3 )
            PTF = 8;

        else if( MPH == 2 )
            PTF = 6;

    BATTLE_TIME = INFINITY;
    ORDINARY_TIME = INFINITY;
    EASY_TIME = INFINITY;

    TIME = POSITION->wtm ? wtime : btime;

    if( TIME == INFINITY )
        goto END;

    INCREMENT = POSITION->wtm ? winc : binc;

    if( INCREMENT < 0 )
        INCREMENT = 0;

    if( INCREMENT < 500000 )
        BUFFER_TIME = 2000;
    else
        BUFFER_TIME = 500;
    TIME = MAX(TIME - BUFFER_TIME * 1000ULL, 9 * TIME / 10);

    if( TIME < 0 )
        TIME = 1;

    if( NEW_GAME || MTG > MTG_PREV )
        {
        TOTAL_MTG = MTG;
        total_wtime = MAX(wtime - BUFFER_TIME, 95 * wtime / 100);
        total_btime = MAX(btime - BUFFER_TIME, 95 * btime / 100);
        }
    TOTAL_TIME = POSITION->wtm ? total_wtime : total_btime;

    if( MTG )
        {
        if( TOTAL_TIME > TIME )
            time_factor = (float)(TIME * TOTAL_MTG) / (float)(TOTAL_TIME * MTG);
        MTG_PREV = MTG;

        if( time_factor < 1 )
            DESIRED_TIME = TIME / MTG + INCREMENT;
        else
            DESIRED_TIME = MIN(TIME * time_factor * time_factor / MTG + INCREMENT, TIME);

        if( time_factor < 1.2 && MTG > 2 )
            ABSOLUTE_TIME = MIN(TIME * time_factor * (MTG + 2) / (MTG * 3), 6 * DESIRED_TIME);
        else
            ABSOLUTE_TIME = TIME * (MTG + 1) / (MTG * 2);

        if( ABSOLUTE_TIME < 10000 )
            ABSOLUTE_TIME = 10000;
        }
    else
        {
        if( (TIME / 40) > INCREMENT )
            {
            DESIRED_TIME = TIME / (((48 - PTF) * TIME / 40 - INCREMENT * 18) / (TIME / 40)) + INCREMENT;
            ABSOLUTE_TIME = TIME / ((5 * TIME / 40 - INCREMENT * 3) / (TIME / 40));
            }
        else
            {
            DESIRED_TIME = (TIME / (30 - PTF / 2)) + INCREMENT;
            ABSOLUTE_TIME = TIME / 2;
            }

        if( TOTAL_TIME > TIME )
            scale_factor = (TIME + INCREMENT * 20) / TOTAL_TIME;

        if( scale_factor < 1 / 3 )
            ABSOLUTE_TIME = ABSOLUTE_TIME * scale_factor * 3;

        else if( scale_factor < 1 / 9 )
            ABSOLUTE_TIME = ABSOLUTE_TIME / 3;

        if( TIME < 500000 )
            {
            DESIRED_TIME = 5000;
            ABSOLUTE_TIME = 10000;
            }
        else if( TIME < 1000000 && INCREMENT < 500000 )
            {
            DESIRED_TIME = TIME / 80;
            ABSOLUTE_TIME = TIME / 20;
            }
        else if( TIME < 2000000 && INCREMENT < 500000 )
            {
            ABSOLUTE_TIME = TIME / 10;
            }
        }

    if( DESIRED_TIME < 5000 )
        DESIRED_TIME = 5000;

    EASY_TIME = DESIRED_TIME / 4;
    BATTLE_TIME = DESIRED_TIME;
    ORDINARY_TIME = (3 * DESIRED_TIME) / 4;

    END:
    if( ABSOLUTE_TIME == INFINITY )
        ANALYSING = true;
    else
        ANALYSING = false;

    if( DO_SEARCH_MOVES )
        SEARCH_MOVES[sm] = MOVE_NONE;
    }
