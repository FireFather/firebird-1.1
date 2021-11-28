#ifndef BUILD_root_multipv
#define BUILD_root_multipv
#include "firebird.h"

typeRootMoveList RootMoveList[512];
void APPLY_SORT( int AgeDepthMix, typeMPV *mpv )
    {
    typeMPV *p;
    int s, x;

    for ( s = 0; s < AgeDepthMix && mpv[s].move; s++ )
        {
        if( s == 0 )
            continue;
        p = &mpv[s];

        while( p != mpv && p->Value > (p - 1)->Value )
            {
            x = p->Value;
            p->Value = (p - 1)->Value;
            (p - 1)->Value = x;
            x = p->move;
            p->move = (p - 1)->move;
            (p - 1)->move = x;
            p--;
            }
        }
    }

#include "root_multipv.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyMultiPV( typePos *Position, int depth )
    {
    int Cnt, cnt, best_value, move_is_check, new_depth, v;
    typeRootMoveList *p;
    typePosition *TempPosition = Position->Current;
    uint32 move;
    int Extend;
    int to;
    int i, j, x;
    int Beta = ValueMate;
    int Alpha = -ValueMate;
    int GoodMoves = 0;

    for ( i = 0; i < 256; i++ )
        MPV[i].move = MPV[i].Value = 0;

    if( ExtraInfo )
		{
		if( Analysing || (depth > 8 && depth <= 70 && DesiredTime > 500000) )
			{
			Send("info depth %d\n", depth / 2);
			}
		}
    Cnt = 0;

    for ( p = RootMoveList; p->move; p++ )
        {
        Cnt++;
        p->move &= 0x7fff;
        }
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

        if( ExtraInfo )
            {
            if( (Analysing && depth >= 24) || (depth >= 24 && (GetClock() - StartClock) > 3000000) )
				{
                Send("info currmove %s currmovenumber %d\n", Notate(move, String1), (p - RootMoveList) + 1);
				}
            }

        if( GoodMoves < MultiPV || depth <= 2 )
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

        if( v > Alpha )
            {
            if( v > best_value )
                HashLower(Position->Current->Hash, move, depth, v);
            MPV[GoodMoves].move = move;
            MPV[GoodMoves++].Value = v;
            APPLY_SORT(GoodMoves, MPV);

            if( GoodMoves >= MultiPV )
                Alpha = MPV[MultiPV - 1].Value;
            else
                Alpha = MAX(MPV[0].Value - 65535, -ValueMate);
            RootBestMove = MPV[0].move;
            best_value = RootScore = MPV[0].Value;
			if( (depth > 10 && depth <= 70 && DesiredTime > 500000) 
			    || (depth > 18 && depth <= 70 && DesiredTime > 50000) 
				|| (depth > 70 && DesiredTime > 10000000) || (Analysing && depth >= 10))
                Information(Position, GetClock() - StartClock, -ValueMate, MPV[0].Value, ValueMate);
            }
        p++;
        }

    for ( i = 0; i < GoodMoves; i++ )
        {
        for ( j = 0; j < Cnt; j++ )
            {
            if( RootMoveList[j].move == MPV[i].move )
                {
                x = RootMoveList[i].move;
                RootMoveList[i].move = RootMoveList[j].move;
                RootMoveList[j].move = x;
                break;
                }
            }
        }
    RootDepth = depth;

    if( !DoSearchMoves )
        {
        HashExact(Position, MPV[0].move, depth, MPV[0].Value, FlagExact);
        }
	if (Analysing)
        Information(Position, GetClock() - StartClock, -ValueMate, MPV[0].Value, ValueMate);
    return MPV[0].Value;
    }
