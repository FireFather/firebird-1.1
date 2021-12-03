#ifndef BUILD_ok_move
#define BUILD_ok_move
#include "firebird.h"
#include "ok_move.c"
#include "white.h"
#else
#include "black.h"
#endif

boolean MyOK( typePOS *POSITION, uint32 move )
    {
    int fr, to, pi, cp;
    uint64 mask;
    to = TO(move);
    mask = SqSet[to];
    fr = FROM(move);
    pi = POSITION->sq[fr];

    if( pi == 0 )
        return false;

    if( PIECE_IS_OPP(pi) )
        return false;
    cp = POSITION->sq[to];

    if( cp && PIECE_IS_MINE(cp) )
        return false;

    if( cp == EnumOppK )
        return false;

    if( pi == EnumMyP )
        {
        if( EIGHTH_RANK(to) && !MoveIsProm(move) )
            return false;

        if( MoveIsEP(move) && to == POSITION->DYN->ep && (fr == BACK_LEFT(to) || fr == BACK_RIGHT(to)) )
            return true;

        if( fr == BACK_LEFT(to) || fr == BACK_RIGHT(to) )
            {
            if( SqSet[to] & OppOccupied )
                return true;
            return false;
            }

        if( fr == BACKWARD(to) )
            {
            if( (SqSet[to]&POSITION->OccupiedBW) == 0 )
                return true;
            return false;
            }

        if( fr != BACKWARD2(to) || RANK(fr) != NUMBER_RANK2 )
            return false;

        if( POSITION->OccupiedBW & SqSet[FORWARD(fr)] )
            return false;
        return true;
        }

    if( pi == EnumMyN )
        {
        if( AttN[fr] & mask )
            return true;
        return false;
        }

    if( pi == EnumMyBL || pi == EnumMyBD )
        {
        if( AttB(fr) & mask )
            return true;
        return false;
        }

    if( MoveIsOO(move) )
        {
        if( to == WHITE_G1 )
            {
            if( !CastleOO || POSITION->OccupiedBW & WHITE_F1G1 || OppAttacked & WHITE_F1G1 )
                return false;
            return true;
            }

        if( to == WHITE_C1 )
            {
            if( !CastleOOO || POSITION->OccupiedBW & WHITE_B1C1D1 || OppAttacked & WHITE_C1D1 )
                return false;
            return true;
            }
        }

    if( pi == EnumMyR )
        {
        if( AttR(fr) & mask )
            return true;
        return false;
        }

    if( pi == EnumMyQ )
        {
        if( AttQ(fr) & mask )
            return true;
        return false;
        }

    if( pi == EnumMyK )
        {
        if( AttK[fr] & mask && (SqSet[to]&OppAttacked) == 0 )
            return true;
        return false;
        }
    return false;
    }
