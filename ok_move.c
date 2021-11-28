#ifndef BUILD_ok_move
#define BUILD_ok_move
#include "firebird.h"
#include "ok_move.c"
#include "white.h"
#else
#include "black.h"
#endif

boolean MyOK( typePos *Position, uint32 move )
    {
    int fr, to, pi, cp;
    to = To(move);
    fr = From(move);
    pi = Position->sq[fr];

    if( pi == 0 )
        return false;
    if( PieceIsOpp(pi) )
        return false;
    cp = Position->sq[to];

    if( cp && PieceIsMine(cp) )
        return false;

    if( cp == EnumOppK )
        return false;

    if( pi == EnumMyP )
        {
        if( EighthRank(to) && !MoveIsProm(move) )
            return false;

        if( MoveIsEP(move) && to == Position->Current->ep && (fr == BackLeft(to) || fr == BackRight(to)) )
            return true;

        if( fr == BackLeft(to) || fr == BackRight(to) )
            {
            if( SqSet[to] & OppOccupied )
                return true;
            return false;
            }

        if( fr == Backward(to) )
            {
            if( (SqSet[to]&Position->OccupiedBW) == 0 )
                return true;
            return false;
            }

        if( fr != Backward2(to) || RANK(fr) != NumberRank2 )
            return false;

        if( Position->OccupiedBW & SqSet[Forward(fr)] )
            return false;
        return true;
        }

    if( pi == EnumMyN )
        {
        if( AttN[fr] & SqSet[to] )
            return true;
        return false;
        }

    if( pi == EnumMyBL || pi == EnumMyBD )
        {
        if( AttB(fr) & SqSet[to] )
            return true;
        return false;
        }

    if( MoveIsOO(move) )
        {
        if( to == WhiteG1 )
            {
            if( !CastleOO || Position->OccupiedBW & WhiteF1G1 || OppAttacked & WhiteF1G1 )
                return false;
            return true;
            }

        if( to == WhiteC1 )
            {
            if( !CastleOOO || Position->OccupiedBW & WhiteB1C1D1 || OppAttacked & WhiteC1D1 )
                return false;
            return true;
            }
        }

    if( pi == EnumMyR )
        {
        if( AttR(fr) & SqSet[to] )
            return true;
        return false;
        }

    if( pi == EnumMyQ )
        {
        if( AttQ(fr) & SqSet[to] )
            return true;
        return false;
        }

    if( pi == EnumMyK )
        {
        if( AttK[fr] & SqSet[to] && (SqSet[to]&OppAttacked) == 0 )
            return true;
        return false;
        }
    return false;
    }
