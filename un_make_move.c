#include "firebird.h"

static INLINE void UnMakeWhiteOO( typePos *Position, int to )
    {
    if( to == G1 )
        {
        wBitboardOcc ^= F1H1;
        wBitboardR ^= F1H1;
        Position->sq[F1] = 0;
        Position->sq[H1] = wEnumR;
        Position->OccupiedBW ^= F1H1;
        Position->OccupiedL90 ^= F1H1Left90;
        Position->OccupiedL45 ^= F1H1Left45;
        Position->OccupiedR45 ^= F1H1Right45;
        }
    else if( to == C1 )
        {
        wBitboardOcc ^= A1D1;
        wBitboardR ^= A1D1;
        Position->sq[D1] = 0;
        Position->sq[A1] = wEnumR;
        Position->OccupiedBW ^= A1D1;
        Position->OccupiedL90 ^= A1D1Left90;
        Position->OccupiedL45 ^= A1D1Left45;
        Position->OccupiedR45 ^= A1D1Right45;
        }
    }
static INLINE void UnMakeBlackOO( typePos *Position, int to )
    {
    if( to == G8 )
        {
        bBitboardOcc ^= F8H8;
        bBitboardR ^= F8H8;
        Position->sq[F8] = 0;
        Position->sq[H8] = bEnumR;
        Position->OccupiedBW ^= F8H8;
        Position->OccupiedL90 ^= F8H8Left90;
        Position->OccupiedL45 ^= F8H8Left45;
        Position->OccupiedR45 ^= F8H8Right45;
        }
    else if( to == C8 )
        {
        bBitboardOcc ^= A8D8;
        bBitboardR ^= A8D8;
        Position->sq[D8] = 0;
        Position->sq[A8] = bEnumR;
        Position->OccupiedBW ^= A8D8;
        Position->OccupiedL90 ^= A8D8Left90;
        Position->OccupiedL45 ^= A8D8Left45;
        Position->OccupiedR45 ^= A8D8Right45;
        }
    }
void UndoWhite( typePos *Position, uint32 move )
    {
    int fr, to, pi, cp, z;
    uint64 mask;
    fr = From(move);
    to = To(move);
    pi = Position->sq[to];
    Position->wtm ^= 1;
    Position->height--;

    if( MoveIsProm(move) )
        {
        Position->bitboard[pi] &= SqClear[to];
        pi = wEnumP;
        }
    Position->sq[fr] = pi;
    Position->sq[to] = Position->Current->cp;

    if( pi == wEnumK )
        Position->wKsq = fr;
    mask = SqSet[fr];
    wBitboardOcc |= mask;
    Position->bitboard[pi] |= mask;
    SetOccupied(mask, fr);
    mask = SqClear[to];
    wBitboardOcc &= mask;
    Position->bitboard[pi] &= mask;
    cp = Position->Current->cp;

    if( cp )
        {
        mask = ~mask;
        bBitboardOcc |= mask;
        Position->bitboard[cp] |= mask;
        }
    else
        {
        ClearOccupied(mask, to);

        if( MoveIsOO(move) )
            UnMakeWhiteOO(Position, to);
        else if( MoveIsEP(move) )
            {
            z = to ^ 8;
            Position->sq[z] = bEnumP;
            mask = SqSet[z];
            bBitboardOcc |= mask;
            bBitboardP |= mask;
            SetOccupied(mask, z);
            }
        }
    Position->Current--;
    Position->StackHeight--;
    }
void UndoBlack( typePos *Position, uint32 move )
    {
    int fr, to, pi, cp, z;
    uint64 mask;
    fr = From(move);
    to = To(move);
    pi = Position->sq[to];
    Position->wtm ^= 1;
    Position->height--;

    if( MoveIsProm(move) )
        {
        Position->bitboard[pi] &= SqClear[to];
        pi = bEnumP;
        }
    Position->sq[fr] = pi;
    Position->sq[to] = Position->Current->cp;

    if( pi == bEnumK )
        Position->bKsq = fr;
    mask = SqSet[fr];
    bBitboardOcc |= mask;
    Position->bitboard[pi] |= mask;
    SetOccupied(mask, fr);
    mask = SqClear[to];
    bBitboardOcc &= mask;
    Position->bitboard[pi] &= mask;
    cp = Position->Current->cp;

    if( cp )
        {
        mask = ~mask;
        wBitboardOcc |= mask;
        Position->bitboard[cp] |= mask;
        }
    else
        {
        ClearOccupied(mask, to);

        if( MoveIsOO(move) )
            UnMakeBlackOO(Position, to);
        else if( MoveIsEP(move) )
            {
            z = to ^ 8;
            Position->sq[z] = wEnumP;
            mask = SqSet[z];
            wBitboardOcc |= mask;
            wBitboardP |= mask;
            SetOccupied(mask, z);
            }
        }
    Position->Current--;
    Position->StackHeight--;
    }

void Undo( typePos *Position, uint32 move )
    {
    if( !Position->wtm )
        UndoWhite(Position, move);
    else
        UndoBlack(Position, move);
    }
