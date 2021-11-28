#include "firebird.h"

static INLINE void MakeWhiteOO (typePos* Position, int to)
{
    if( to == G1 )
        {
        wBitboardOcc ^= F1H1;
        wBitboardR ^= F1H1;
        Position->OccupiedBW ^= F1H1;
        Position->OccupiedL90 ^= F1H1Left90;
        Position->OccupiedL45 ^= F1H1Left45;
        Position->OccupiedR45 ^= F1H1Right45;
        Position->Current->PST += PieceSquareValue[wEnumR][F1] - PieceSquareValue[wEnumR][H1];
        Position->Current->Hash ^= Hash[wEnumR][F1] ^ Hash[wEnumR][H1];
        Position->sq[H1] = 0;
        Position->sq[F1] = wEnumR;
        }
    else if( to == C1 )
        {
        wBitboardOcc ^= A1D1;
        wBitboardR ^= A1D1;
        Position->OccupiedBW ^= A1D1;
        Position->OccupiedL90 ^= A1D1Left90;
        Position->OccupiedL45 ^= A1D1Left45;
        Position->OccupiedR45 ^= A1D1Right45;
        Position->Current->PST += PieceSquareValue[wEnumR][D1] - PieceSquareValue[wEnumR][A1];
        Position->Current->Hash ^= Hash[wEnumR][A1] ^ Hash[wEnumR][D1];
        Position->sq[A1] = 0;
        Position->sq[D1] = wEnumR;
        }
    }
static INLINE void MakeBlackOO( typePos *Position, int to )
    {
    if( to == G8 )
        {
        bBitboardOcc ^= F8H8;
        bBitboardR ^= F8H8;
        Position->OccupiedBW ^= F8H8;
        Position->OccupiedL90 ^= F8H8Left90;
        Position->OccupiedL45 ^= F8H8Left45;
        Position->OccupiedR45 ^= F8H8Right45;
        Position->Current->PST += PieceSquareValue[bEnumR][F8] - PieceSquareValue[bEnumR][H8];
        Position->Current->Hash ^= Hash[bEnumR][F8] ^ Hash[bEnumR][H8];
        Position->sq[H8] = 0;
        Position->sq[F8] = bEnumR;
        }
    else if( to == C8 )
        {
        bBitboardOcc ^= A8D8;
        bBitboardR ^= A8D8;
        Position->OccupiedBW ^= A8D8;
        Position->OccupiedL90 ^= A8D8Left90;
        Position->OccupiedL45 ^= A8D8Left45;
        Position->OccupiedR45 ^= A8D8Right45;
        Position->Current->PST += PieceSquareValue[bEnumR][D8] - PieceSquareValue[bEnumR][A8];
        Position->Current->Hash ^= Hash[bEnumR][A8] ^ Hash[bEnumR][D8];
        Position->sq[A8] = 0;
        Position->sq[D8] = bEnumR;
        }
    }

void MakeWhite( typePos *Position, uint32 move )
    {
    int fr, to, pi, fl, cp, z;
    uint64 mask;
    Position->nodes++;

    if( Position->cpu == 0 )
        {
        NodeCheck++;

        if( (NodeCheck & 4095) == 0 )
            CheckDone(Position, 0);
        }
    memcpy(Position->Current + 1, Position->Current, 32);
    fr = From(move);
    to = To(move);
    pi = Position->sq[fr];
    Position->Current++;
    Position->Current->reversible++;
    Position->Current->move = move;
    fl = CastleTable[fr] & CastleTable[to] & Position->Current->oo;
    Position->Current->Hash ^= ZobristCastling[Position->Current->oo ^ fl];
    Position->Current->PawnHash ^= ZobristCastling[Position->Current->oo ^ fl];
    Position->Current->oo = fl;

    if( Position->Current->ep )
        {
        Position->Current->Hash ^= ZobristEP[Position->Current->ep & 7];
        Position->Current->ep = 0;
        }
    Position->sq[fr] = 0;
    mask = SqClear[fr];
    wBitboardOcc &= mask;
    Position->bitboard[pi] &= mask;
    ClearOccupied(mask, fr);
    Position->Current->PST += PieceSquareValue[pi][to] - PieceSquareValue[pi][fr];
    mask = Hash[pi][fr] ^ Hash[pi][to];
    cp = Position->sq[to];
    Position->Current->cp = cp;
    Position->Current->Hash ^= mask;

    if( pi == wEnumP )
        Position->Current->PawnHash ^= mask;
    Position->wtm ^= 1;
    Position->height++;
    Position->Current->Hash ^= ZobristWTM;

    if( pi == wEnumK )
        {
        Position->Current->PawnHash ^= mask;
        Position->wKsq = to;
        }

    if( cp )
        {
        mask = SqClear[to];
        bBitboardOcc &= mask;
        Position->bitboard[cp] &= mask;
        Position->Current->material -= MATERIAL_VALUE[cp];
        Position->Current->PST -= PieceSquareValue[cp][to];

        if( cp == bEnumP )
            Position->Current->PawnHash ^= Hash[cp][to];
        Position->Current->Hash ^= Hash[cp][to];
        Position->Current->reversible = 0;
        }
    else
        {
        mask = SqSet[to];
        SetOccupied(mask, to);

        if( MoveIsOO(move) )
            {
            Position->Current->reversible = 0;
            MakeWhiteOO(Position, to);
            }
        }
    Position->sq[to] = pi;
    wBitboardOcc |= SqSet[to];
    Position->bitboard[pi] |= SqSet[to];

    if( pi == wEnumP )
        {
        Position->Current->reversible = 0;

        if( MoveIsEP(move) )
            {
            z = to ^ 8;
            mask = SqClear[z];
            bBitboardOcc &= mask;
            bBitboardP &= mask;
            ClearOccupied(mask, z);
            Position->Current->material -= MATERIAL_VALUE[bEnumP];
            Position->Current->PST -= PieceSquareValue[bEnumP][z];
            Position->Current->Hash ^= Hash[bEnumP][z];
            Position->Current->PawnHash ^= Hash[bEnumP][z];
            Position->sq[z] = 0;
            }
        else if( MoveIsProm(move) )
            {
            pi = PromW[(move &FlagMask) >> 12];

            if( pi == wEnumBL && SqSet[to] & Black )
                pi = wEnumBD;
            Position->sq[to] = pi;

            if( Position->bitboard[pi] )
                Position->Current->material |= 0x80000000;
            wBitboardP &= SqClear[to];
            Position->bitboard[pi] |= SqSet[to];
            Position->Current->material += MATERIAL_VALUE[pi] - MATERIAL_VALUE[wEnumP];
            Position->Current->PST += PieceSquareValue[pi][to] - PieceSquareValue[wEnumP][to];
            Position->Current->Hash ^= Hash[pi][to] ^ Hash[wEnumP][to];
            Position->Current->PawnHash ^= Hash[wEnumP][to];
            }
        else if( (to ^ fr) == 16 )
            {
            if( WhiteEP[to & 7] & bBitboardP )
                {
                z = (fr + to) >> 1;
                Position->Current->ep = z;
                Position->Current->Hash ^= ZobristEP[z & 7];
                }
            }
        }
    Position->Stack[++(Position->StackHeight)] = Position->Current->Hash;
    }

void MakeBlack( typePos *Position, uint32 move )
    {
    int fr, to, pi, fl, cp, z;
    uint64 mask;
    Position->nodes++;
    memcpy(Position->Current + 1, Position->Current, 32);
    fr = From(move);
    to = To(move);
    pi = Position->sq[fr];
    Position->Current++;
    Position->Current->reversible++;
    Position->Current->move = move;
    fl = CastleTable[fr] & CastleTable[to] & Position->Current->oo;
    Position->Current->Hash ^= ZobristCastling[Position->Current->oo ^ fl];
    Position->Current->PawnHash ^= ZobristCastling[Position->Current->oo ^ fl];
    Position->Current->oo = fl;

    if( Position->Current->ep )
        {
        Position->Current->Hash ^= ZobristEP[Position->Current->ep & 7];
        Position->Current->ep = 0;
        }
    Position->sq[fr] = 0;
    mask = SqClear[fr];
    bBitboardOcc &= mask;
    Position->bitboard[pi] &= mask;
    ClearOccupied(mask, fr);
    Position->Current->PST += PieceSquareValue[pi][to] - PieceSquareValue[pi][fr];
    mask = Hash[pi][fr] ^ Hash[pi][to];
    cp = Position->sq[to];
    Position->Current->cp = cp;
    Position->Current->Hash ^= mask;

    if( pi == bEnumP )
        Position->Current->PawnHash ^= mask;
    Position->wtm ^= 1;
    Position->height++;
    Position->Current->Hash ^= ZobristWTM;

    if( pi == bEnumK )
        {
        Position->Current->PawnHash ^= mask;
        Position->bKsq = to;
        }

    if( cp )
        {
        mask = SqClear[to];
        wBitboardOcc &= mask;
        Position->bitboard[cp] &= mask;
        Position->Current->material -= MATERIAL_VALUE[cp];
        Position->Current->PST -= PieceSquareValue[cp][to];

        if( cp == wEnumP )
            Position->Current->PawnHash ^= Hash[cp][to];
        Position->Current->Hash ^= Hash[cp][to];
        Position->Current->reversible = 0;
        }
    else
        {
        mask = SqSet[to];
        SetOccupied(mask, to);

        if( MoveIsOO(move) )
            {
            Position->Current->reversible = 0;
            MakeBlackOO(Position, to);
            }
        }
    Position->sq[to] = pi;
    bBitboardOcc |= SqSet[to];
    Position->bitboard[pi] |= SqSet[to];

    if( pi == bEnumP )
        {
        Position->Current->reversible = 0;

        if( MoveIsEP(move) )
            {
            z = to ^ 8;
            mask = SqClear[z];
            wBitboardOcc &= mask;
            wBitboardP &= mask;
            ClearOccupied(mask, z);
            Position->Current->material -= MATERIAL_VALUE[wEnumP];
            Position->Current->PST -= PieceSquareValue[wEnumP][z];
            Position->Current->Hash ^= Hash[wEnumP][z];
            Position->Current->PawnHash ^= Hash[wEnumP][z];
            Position->sq[z] = 0;
            }
        else if( MoveIsProm(move) )
            {
            pi = PromB[(move &FlagMask) >> 12];

            if( pi == bEnumBL && SqSet[to] & Black )
                pi = bEnumBD;
            Position->sq[to] = pi;

            if( Position->bitboard[pi] )
                Position->Current->material |= 0x80000000;
            Position->bitboard[bEnumP] &= SqClear[to];
            Position->bitboard[pi] |= SqSet[to];
            Position->Current->material += MATERIAL_VALUE[pi] - MATERIAL_VALUE[bEnumP];
            Position->Current->PST += PieceSquareValue[pi][to] - PieceSquareValue[bEnumP][to];
            Position->Current->Hash ^= Hash[pi][to] ^ Hash[bEnumP][to];
            Position->Current->PawnHash ^= Hash[bEnumP][to];
            }
        else if( (to ^ fr) == 16 )
            {
            if( BlackEP[to & 7] & wBitboardP )
                {
                z = (fr + to) >> 1;
                Position->Current->ep = z;
                Position->Current->Hash ^= ZobristEP[z & 7];
                }
            }
        }
    Position->Stack[++(Position->StackHeight)] = Position->Current->Hash;
    }

void Make( typePos *Position, uint32 move )
    {
    if( Position->wtm )
        {
        if( NodeCheck & 4095 )
            NodeCheck--;
        Position->nodes--;
        MakeWhite(Position, move);
        }
    else
        {
        Position->nodes--;
        MakeBlack(Position, move);
        }
    }
