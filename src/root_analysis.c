#ifndef BUILD_root_analysis
#define BUILD_root_analysis
#include "firebird.h"
#include "control.h"
typeRootMoveList ROOT_MOVE_LIST[512];
#include "root_analysis.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyAnalysis( typePOS *POSITION, int ALPHA, int BETA, int depth )
    {
    int CNT, origALPHA, best_value, cnt, move_is_check, new_depth, v, TEMPvalu;
    typeRootMoveList *p, *q;
    typeDYNAMIC *POS0 = POSITION->DYN;
    uint32 move;
    int EXTEND;
    int to;

    if( MULTI_PV != 1 )
        return MyMultiPV(POSITION, depth);

    if( BETA > VALUE_MATE )
        BETA = VALUE_MATE;

    if( ALPHA < -VALUE_MATE )
        ALPHA = -VALUE_MATE;

    CNT = 0;

    for ( p = ROOT_MOVE_LIST; p->move; p++ )
        {
        CNT++;
        p->move &= 0x7fff;
        }
    origALPHA = ALPHA;
    p = ROOT_MOVE_LIST;
    v = best_value = -VALUE_INFINITY;
    cnt = 0;

    while( (move = p->move) )
        {
        MAKE(POSITION, move);
        EVAL(move);
        move_is_check = (MOVE_IS_CHECK != 0);
        EXTEND = 0;
        to = TO(move);

        if( POS1->cp || move_is_check || PassedPawnPush(to, FOURTH_RANK(to)) )
            EXTEND = 1;
        new_depth = depth - 2 + EXTEND;

        if( best_value == -VALUE_INFINITY || depth <= 2 )
            v = -OppPV(POSITION, -BETA, -ALPHA, new_depth, move_is_check);
        else
            {
            if( LOW_DEPTH_CONDITION_PV )
                {
                if( move_is_check )
                    v = -OppLowDepthCheck(POSITION, -ALPHA, new_depth);
                else
                    v = -OppLowDepth(POSITION, -ALPHA, new_depth);
                }
            else
                {
                if( new_depth >= 16 && ANALYSING )
                    {
                    int an = new_depth - 12;
                    v = VALUE_INFINITY;

                    while( an <= new_depth && v > ALPHA )
                        {
                        v = -OppPV(POSITION, -ALPHA - 1, -ALPHA, an, move_is_check);
                        an += 4;
                        }

                    if( an > new_depth )
                        goto DEC;
                    }

                if( move_is_check )
                    v = -OppCutCheck(POSITION, -ALPHA, new_depth);
                else
                    v = -OppCut(POSITION, -ALPHA, new_depth);
                }

            if( v > ALPHA )
                v = -OppPV(POSITION, -ALPHA - 1, -ALPHA, new_depth, move_is_check);
            DEC:
            if( v > ALPHA )
                v = -OppPV(POSITION, -BETA, -ALPHA, new_depth, move_is_check);

            if( v <= ALPHA )
                v = ALPHA;
            }
        UNDO(POSITION, move);
        CHECK_HALT();

        if( v <= ALPHA )
            TEMPvalu = origALPHA;
        else
            TEMPvalu = v;
        p->move |= (TEMPvalu + 0x8000) << 16;

        if( v > best_value )
            {
            best_value = v;

            if( best_value == -VALUE_INFINITY || v > ALPHA )
                {
                HashLower(POSITION->DYN->HASH, move, depth, v);
                ROOT_BEST_MOVE = move;
                ROOT_SCORE = v;
                ROOT_DEPTH = depth;

				if (depth > 15)
					{
					if( v > ALPHA && v < BETA)
						Information(POSITION, GetClock() - START_CLOCK, origALPHA, v, BETA);

					else if( v < BETA )
						Information(POSITION, GetClock() - START_CLOCK, origALPHA, ALPHA, BETA);

					else if( v > ALPHA )
						Information(POSITION, GetClock() - START_CLOCK, origALPHA, BETA, BETA);
					}
                }
            }

        if( v > ALPHA )
            ALPHA = v;
        cnt++;

        if( v < BETA )
            {
            p++;
            continue;
            }
        break;
        }

    for ( p = ROOT_MOVE_LIST + (CNT - 1); p >= ROOT_MOVE_LIST; p-- )
        {
        move = p->move;

        for ( q = p + 1; q < ROOT_MOVE_LIST + CNT; q++ )
            {
            if( (move & 0xffff0000) < (q->move & 0xffff0000) )
                (q - 1)->move = q->move;
            else
                break;
            }
        q--;
        q->move = move;
        }
    ROOT_DEPTH = depth;

    if( !DO_SEARCH_MOVES )
        {
        if( best_value <= origALPHA )
            HashUpper(POSITION->DYN->HASH, depth, origALPHA);

        else if( best_value < BETA )
            HashExact(POSITION, ROOT_BEST_MOVE, depth, best_value, FLAG_EXACT);
        }
	if (depth > 15)
		Information(POSITION, GetClock() - START_CLOCK, origALPHA, best_value, BETA);
    return best_value;
    }
