#include "firebird.h"
#include "make_unmake.h"

static INLINE void UnMakeWhiteOO( typePOS *POSITION, int to )
    {
    if( to == G1 )
        {
        wBitboardOcc ^= F1H1;
        wBitboardR ^= F1H1;
        POSITION->sq[F1] = 0;
        POSITION->sq[H1] = wEnumR;
        POSITION->OccupiedBW ^= F1H1;
        POSITION->OccupiedL90 ^= F1H1Left90;
        POSITION->OccupiedL45 ^= F1H1Left45;
        POSITION->OccupiedR45 ^= F1H1Right45;
        }
    else if( to == C1 )
        {
        wBitboardOcc ^= A1D1;
        wBitboardR ^= A1D1;
        POSITION->sq[D1] = 0;
        POSITION->sq[A1] = wEnumR;
        POSITION->OccupiedBW ^= A1D1;
        POSITION->OccupiedL90 ^= A1D1Left90;
        POSITION->OccupiedL45 ^= A1D1Left45;
        POSITION->OccupiedR45 ^= A1D1Right45;
        }
    }
static INLINE void UnMakeBlackOO( typePOS *POSITION, int to )
    {
    if( to == G8 )
        {
        bBitboardOcc ^= F8H8;
        bBitboardR ^= F8H8;
        POSITION->sq[F8] = 0;
        POSITION->sq[H8] = bEnumR;
        POSITION->OccupiedBW ^= F8H8;
        POSITION->OccupiedL90 ^= F8H8Left90;
        POSITION->OccupiedL45 ^= F8H8Left45;
        POSITION->OccupiedR45 ^= F8H8Right45;
        }
    else if( to == C8 )
        {
        bBitboardOcc ^= A8D8;
        bBitboardR ^= A8D8;
        POSITION->sq[D8] = 0;
        POSITION->sq[A8] = bEnumR;
        POSITION->OccupiedBW ^= A8D8;
        POSITION->OccupiedL90 ^= A8D8Left90;
        POSITION->OccupiedL45 ^= A8D8Left45;
        POSITION->OccupiedR45 ^= A8D8Right45;
        }
    }
void UndoWhite( typePOS *POSITION, uint32 move )
    {
    int fr, to, pi, cp, z;
    uint64 mask;
    fr = FROM(move);
    to = TO(move);
    pi = POSITION->sq[to];
    POSITION->wtm ^= 1;
    POSITION->height--;

    if( MoveIsProm(move) )
        {
        POSITION->bitboard[pi] &= SqClear[to];
        pi = wEnumP;
        }
    POSITION->sq[fr] = pi;
    POSITION->sq[to] = POSITION->DYN->cp;

    if( pi == wEnumK )
        POSITION->wKsq = fr;
    mask = SqSet[fr];
    wBitboardOcc |= mask;
    POSITION->bitboard[pi] |= mask;
    SetOccupied(mask, fr);
    mask = SqClear[to];
    wBitboardOcc &= mask;
    POSITION->bitboard[pi] &= mask;
    cp = POSITION->DYN->cp;

    if( cp )
        {
        mask = ~mask;
        bBitboardOcc |= mask;
        POSITION->bitboard[cp] |= mask;
        }
    else
        {
        ClearOccupied(mask, to);

        if( MoveIsOO(move) )
            UnMakeWhiteOO(POSITION, to);
        else if( MoveIsEP(move) )
            {
            z = to ^ 8;
            POSITION->sq[z] = bEnumP;
            mask = SqSet[z];
            bBitboardOcc |= mask;
            bBitboardP |= mask;
            SetOccupied(mask, z);
            }
        }
    POSITION->DYN--;
    POSITION->StackHeight--;
    }
void UndoBlack( typePOS *POSITION, uint32 move )
    {
    int fr, to, pi, cp, z;
    uint64 mask;
    fr = FROM(move);
    to = TO(move);
    pi = POSITION->sq[to];
    POSITION->wtm ^= 1;
    POSITION->height--;

    if( MoveIsProm(move) )
        {
        POSITION->bitboard[pi] &= SqClear[to];
        pi = bEnumP;
        }
    POSITION->sq[fr] = pi;
    POSITION->sq[to] = POSITION->DYN->cp;

    if( pi == bEnumK )
        POSITION->bKsq = fr;
    mask = SqSet[fr];
    bBitboardOcc |= mask;
    POSITION->bitboard[pi] |= mask;
    SetOccupied(mask, fr);
    mask = SqClear[to];
    bBitboardOcc &= mask;
    POSITION->bitboard[pi] &= mask;
    cp = POSITION->DYN->cp;

    if( cp )
        {
        mask = ~mask;
        wBitboardOcc |= mask;
        POSITION->bitboard[cp] |= mask;
        }
    else
        {
        ClearOccupied(mask, to);

        if( MoveIsOO(move) )
            UnMakeBlackOO(POSITION, to);
        else if( MoveIsEP(move) )
            {
            z = to ^ 8;
            POSITION->sq[z] = wEnumP;
            mask = SqSet[z];
            wBitboardOcc |= mask;
            wBitboardP |= mask;
            SetOccupied(mask, z);
            }
        }
    POSITION->DYN--;
    POSITION->StackHeight--;
    }

void Undo( typePOS *POSITION, uint32 move )
    {
    if( !POSITION->wtm )
        UndoWhite(POSITION, move);
    else
        UndoBlack(POSITION, move);
    }
