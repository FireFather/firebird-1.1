#ifndef BUILD_root_analysis
#define BUILD_root_analysis
#include "firebird.h"
typeRootMoveList RootMoveList[512];
#include "root_analysis.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyAnalysis( typePos *Position, int Alpha, int Beta, int depth )
    {
    int Cnt, origAlpha, best_value, cnt, move_is_check, new_depth, v, Tempvalue;
    typeRootMoveList *p, *q;
    typePosition *TempPosition = Position->Current;
    uint32 move;
    int Extend;
    int to;

    if( MultiPV != 1 )
        return MyMultiPV(Position, depth);

    if( Beta > ValueMate )
        Beta = ValueMate;

    if( Alpha < -ValueMate )
        Alpha = -ValueMate;

    Cnt = 0;

    for ( p = RootMoveList; p->move; p++ )
        {
        Cnt++;
        p->move &= 0x7fff;
        }
    origAlpha = Alpha;
    p = RootMoveList;
    v = best_value = -ValueInfinity;
    cnt = 0;

    while( (move = p->move) )
        {
        Make(Position, move);
        EVAL(move);
        move_is_check = (MoveIsCheck != 0);
        Extend = 0;
        to = To(move);

        if( Pos1->cp || move_is_check || PassedPawnPush(to, FourthRank(to)) )
            Extend = 1;
        new_depth = depth - 2 + Extend;

        if( best_value == -ValueInfinity || depth <= 2 )
            v = -OppPV(Position, -Beta, -Alpha, new_depth, move_is_check);
        else
            {
            if( LowDepthConditionPV )
                {
                if( move_is_check )
                    v = -OppLowDepthCheck(Position, -Alpha, new_depth);
                else
                    v = -OppLowDepth(Position, -Alpha, new_depth);
                }
            else
                {
                if( new_depth >= 16 && Analysing )
                    {
                    int an = new_depth - 12;
                    v = ValueInfinity;

                    while( an <= new_depth && v > Alpha )
                        {
                        v = -OppPV(Position, -Alpha - 1, -Alpha, an, move_is_check);
                        an += 4;
                        }

                    if( an > new_depth )
                        goto DEC;
                    }

                if( move_is_check )
                    v = -OppCutCheck(Position, -Alpha, new_depth);
                else
                    v = -OppCut(Position, -Alpha, new_depth);
                }

            if( v > Alpha )
                v = -OppPV(Position, -Alpha - 1, -Alpha, new_depth, move_is_check);
            DEC:
            if( v > Alpha )
                v = -OppPV(Position, -Beta, -Alpha, new_depth, move_is_check);

            if( v <= Alpha )
                v = Alpha;
            }
        Undo(Position, move);
        CheckHalt();

        if( v <= Alpha )
            Tempvalue = origAlpha;
        else
            Tempvalue = v;
        p->move |= (Tempvalue + 0x8000) << 16;

        if( v > best_value )
            {
            best_value = v;

            if( best_value == -ValueInfinity || v > Alpha )
                {
                HashLower(Position->Current->Hash, move, depth, v);
                RootBestMove = move;
                RootScore = v;
                RootDepth = depth;

				if (depth > 15)
					{
					if( v > Alpha && v < Beta)
						Information(Position, GetClock() - StartClock, origAlpha, v, Beta);

					else if( v < Beta )
						Information(Position, GetClock() - StartClock, origAlpha, Alpha, Beta);

					else if( v > Alpha )
						Information(Position, GetClock() - StartClock, origAlpha, Beta, Beta);
					}
                }
            }

        if( v > Alpha )
            Alpha = v;
        cnt++;

        if( v < Beta )
            {
            p++;
            continue;
            }
        break;
        }

    for ( p = RootMoveList + (Cnt - 1); p >= RootMoveList; p-- )
        {
        move = p->move;

        for ( q = p + 1; q < RootMoveList + Cnt; q++ )
            {
            if( (move & 0xffff0000) < (q->move & 0xffff0000) )
                (q - 1)->move = q->move;
            else
                break;
            }
        q--;
        q->move = move;
        }
    RootDepth = depth;

    if( !DoSearchMoves )
        {
        if( best_value <= origAlpha )
            HashUpper(Position->Current->Hash, depth, origAlpha);

        else if( best_value < Beta )
            HashExact(Position, RootBestMove, depth, best_value, FlagExact);
        }
	if (depth > 15)
		Information(Position, GetClock() - StartClock, origAlpha, best_value, Beta);
    return best_value;
    }
