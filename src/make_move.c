#include "firebird.h"
#include "make_unmake.h"
#include "material_value.h"

typedef enum
    {
    ooK = 1,
    ooQ = 2,
    ook = 4,
    ooq = 8
    } CastlingTable;
typedef enum
    {
    KQkq = ooK | ooQ | ook | ooq,
    Qkq = ooQ | ook | ooq,
    Kkq = ooK | ook | ooq,
    kq = ook | ooq,
    KQk = ooK | ooQ | ook,
    KQ = ooK | ooQ,
    KQq = ooK | ooQ | ooq
    } KQkqTable;

static const uint64 CastleTable[64] =
    {
	Kkq, KQkq, KQkq, KQkq, kq, KQkq, KQkq, Qkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq, KQkq,
    KQk, KQkq, KQkq, KQkq, KQ, KQkq, KQkq, KQq
    };
const static uint64 WhiteEP[8] =
	{
	Bitboard2 (B4, B4), Bitboard2 (A4, C4),
    Bitboard2 (B4, D4), Bitboard2 (C4, E4),
    Bitboard2 (D4, F4), Bitboard2 (E4, G4),
    Bitboard2 (F4, H4), Bitboard2 (G4, G4)
	};
const static uint64 BlackEP[8] =
	{
	Bitboard2 (B5, B5), Bitboard2 (A5, C5),
    Bitboard2 (B5, D5), Bitboard2 (C5, E5),
    Bitboard2 (D5, F5), Bitboard2 (E5, G5),
    Bitboard2 (F5, H5), Bitboard2 (G5, G5)
	};

static INLINE void MakeWhiteOO (typePOS* POSITION, int to)
{
    if( to == G1 )
        {
        wBitboardOcc ^= F1H1;
        wBitboardR ^= F1H1;
        POSITION->OccupiedBW ^= F1H1;
        POSITION->OccupiedL90 ^= F1H1Left90;
        POSITION->OccupiedL45 ^= F1H1Left45;
        POSITION->OccupiedR45 ^= F1H1Right45;
        POSITION->DYN->STATIC += PieceSquareValue[wEnumR][F1] - PieceSquareValue[wEnumR][H1];
        POSITION->DYN->HASH ^= HASH[wEnumR][F1] ^ HASH[wEnumR][H1];
        POSITION->sq[H1] = 0;
        POSITION->sq[F1] = wEnumR;
        }
    else if( to == C1 )
        {
        wBitboardOcc ^= A1D1;
        wBitboardR ^= A1D1;
        POSITION->OccupiedBW ^= A1D1;
        POSITION->OccupiedL90 ^= A1D1Left90;
        POSITION->OccupiedL45 ^= A1D1Left45;
        POSITION->OccupiedR45 ^= A1D1Right45;
        POSITION->DYN->STATIC += PieceSquareValue[wEnumR][D1] - PieceSquareValue[wEnumR][A1];
        POSITION->DYN->HASH ^= HASH[wEnumR][A1] ^ HASH[wEnumR][D1];
        POSITION->sq[A1] = 0;
        POSITION->sq[D1] = wEnumR;
        }
    }
static INLINE void MakeBlackOO( typePOS *POSITION, int to )
    {
    if( to == G8 )
        {
        bBitboardOcc ^= F8H8;
        bBitboardR ^= F8H8;
        POSITION->OccupiedBW ^= F8H8;
        POSITION->OccupiedL90 ^= F8H8Left90;
        POSITION->OccupiedL45 ^= F8H8Left45;
        POSITION->OccupiedR45 ^= F8H8Right45;
        POSITION->DYN->STATIC += PieceSquareValue[bEnumR][F8] - PieceSquareValue[bEnumR][H8];
        POSITION->DYN->HASH ^= HASH[bEnumR][F8] ^ HASH[bEnumR][H8];
        POSITION->sq[H8] = 0;
        POSITION->sq[F8] = bEnumR;
        }
    else if( to == C8 )
        {
        bBitboardOcc ^= A8D8;
        bBitboardR ^= A8D8;
        POSITION->OccupiedBW ^= A8D8;
        POSITION->OccupiedL90 ^= A8D8Left90;
        POSITION->OccupiedL45 ^= A8D8Left45;
        POSITION->OccupiedR45 ^= A8D8Right45;
        POSITION->DYN->STATIC += PieceSquareValue[bEnumR][D8] - PieceSquareValue[bEnumR][A8];
        POSITION->DYN->HASH ^= HASH[bEnumR][A8] ^ HASH[bEnumR][D8];
        POSITION->sq[A8] = 0;
        POSITION->sq[D8] = bEnumR;
        }
    }

const static uint8 PromW[8] =
    {
    0, 0, 0, 0, wEnumN, wEnumBL, wEnumR, wEnumQ
    };
void MakeWhite( typePOS *POSITION, uint32 move )
    {
    int fr, to, pi, fl, cp, z;
    uint64 mask;
    POSITION->nodes++;

    if( POSITION->cpu == 0 )
        {
        NODE_CHECK++;

        if( (NODE_CHECK & 4095) == 0 )
            CheckDone(POSITION, 0);
        }
    memcpy(POSITION->DYN + 1, POSITION->DYN, 32);
    fr = FROM(move);
    to = TO(move);
    pi = POSITION->sq[fr];
    POSITION->DYN++;
    POSITION->DYN->reversible++;
    POSITION->DYN->move = move;
    fl = CastleTable[fr] & CastleTable[to] & POSITION->DYN->oo;
    POSITION->DYN->HASH ^= ZobristCastling[POSITION->DYN->oo ^ fl];
    POSITION->DYN->PAWN_HASH ^= ZobristCastling[POSITION->DYN->oo ^ fl];
    POSITION->DYN->oo = fl;

    if( POSITION->DYN->ep )
        {
        POSITION->DYN->HASH ^= ZobristEP[POSITION->DYN->ep & 7];
        POSITION->DYN->ep = 0;
        }
    POSITION->sq[fr] = 0;
    mask = SqClear[fr];
    wBitboardOcc &= mask;
    POSITION->bitboard[pi] &= mask;
    ClearOccupied(mask, fr);
    POSITION->DYN->STATIC += PieceSquareValue[pi][to] - PieceSquareValue[pi][fr];
    mask = HASH[pi][fr] ^ HASH[pi][to];
    cp = POSITION->sq[to];
    POSITION->DYN->cp = cp;
    POSITION->DYN->HASH ^= mask;

    if( pi == wEnumP )
        POSITION->DYN->PAWN_HASH ^= mask;
    POSITION->wtm ^= 1;
    POSITION->height++;
    POSITION->DYN->HASH ^= ZobristWTM;

    if( pi == wEnumK )
        {
        POSITION->DYN->PAWN_HASH ^= mask;
        POSITION->wKsq = to;
        }

    if( cp )
        {
        mask = SqClear[to];
        bBitboardOcc &= mask;
        POSITION->bitboard[cp] &= mask;
        POSITION->DYN->material -= MATERIAL_VALUE[cp];
        POSITION->DYN->STATIC -= PieceSquareValue[cp][to];

        if( cp == bEnumP )
            POSITION->DYN->PAWN_HASH ^= HASH[cp][to];
        POSITION->DYN->HASH ^= HASH[cp][to];
        POSITION->DYN->reversible = 0;
        }
    else
        {
        mask = SqSet[to];
        SetOccupied(mask, to);

        if( MoveIsOO(move) )
            {
            POSITION->DYN->reversible = 0;
            MakeWhiteOO(POSITION, to);
            }
        }
    POSITION->sq[to] = pi;
    wBitboardOcc |= SqSet[to];
    POSITION->bitboard[pi] |= SqSet[to];

    if( pi == wEnumP )
        {
        POSITION->DYN->reversible = 0;

        if( MoveIsEP(move) )
            {
            z = to ^ 8;
            mask = SqClear[z];
            bBitboardOcc &= mask;
            bBitboardP &= mask;
            ClearOccupied(mask, z);
            POSITION->DYN->material -= MATERIAL_VALUE[bEnumP];
            POSITION->DYN->STATIC -= PieceSquareValue[bEnumP][z];
            POSITION->DYN->HASH ^= HASH[bEnumP][z];
            POSITION->DYN->PAWN_HASH ^= HASH[bEnumP][z];
            POSITION->sq[z] = 0;
            }
        else if( MoveIsProm(move) )
            {
            pi = PromW[(move &FLAG_MASK) >> 12];

            if( pi == wEnumBL && SqSet[to] & DARK )
                pi = wEnumBD;
            POSITION->sq[to] = pi;

            if( POSITION->bitboard[pi] )
                POSITION->DYN->material |= 0x80000000;
            wBitboardP &= SqClear[to];
            POSITION->bitboard[pi] |= SqSet[to];
            POSITION->DYN->material += MATERIAL_VALUE[pi] - MATERIAL_VALUE[wEnumP];
            POSITION->DYN->STATIC += PieceSquareValue[pi][to] - PieceSquareValue[wEnumP][to];
            POSITION->DYN->HASH ^= HASH[pi][to] ^ HASH[wEnumP][to];
            POSITION->DYN->PAWN_HASH ^= HASH[wEnumP][to];
            }
        else if( (to ^ fr) == 16 )
            {
            if( WhiteEP[to & 7] & bBitboardP )
                {
                z = (fr + to) >> 1;
                POSITION->DYN->ep = z;
                POSITION->DYN->HASH ^= ZobristEP[z & 7];
                }
            }
        }
    POSITION->STACK[++(POSITION->StackHeight)] = POSITION->DYN->HASH;
    }

const static uint8 PromB[8] =
    {
    0, 0, 0, 0, bEnumN, bEnumBL, bEnumR, bEnumQ
    };
void MakeBlack( typePOS *POSITION, uint32 move )
    {
    int fr, to, pi, fl, cp, z;
    uint64 mask;
    POSITION->nodes++;
    memcpy(POSITION->DYN + 1, POSITION->DYN, 32);
    fr = FROM(move);
    to = TO(move);
    pi = POSITION->sq[fr];
    POSITION->DYN++;
    POSITION->DYN->reversible++;
    POSITION->DYN->move = move;
    fl = CastleTable[fr] & CastleTable[to] & POSITION->DYN->oo;
    POSITION->DYN->HASH ^= ZobristCastling[POSITION->DYN->oo ^ fl];
    POSITION->DYN->PAWN_HASH ^= ZobristCastling[POSITION->DYN->oo ^ fl];
    POSITION->DYN->oo = fl;

    if( POSITION->DYN->ep )
        {
        POSITION->DYN->HASH ^= ZobristEP[POSITION->DYN->ep & 7];
        POSITION->DYN->ep = 0;
        }
    POSITION->sq[fr] = 0;
    mask = SqClear[fr];
    bBitboardOcc &= mask;
    POSITION->bitboard[pi] &= mask;
    ClearOccupied(mask, fr);
    POSITION->DYN->STATIC += PieceSquareValue[pi][to] - PieceSquareValue[pi][fr];
    mask = HASH[pi][fr] ^ HASH[pi][to];
    cp = POSITION->sq[to];
    POSITION->DYN->cp = cp;
    POSITION->DYN->HASH ^= mask;

    if( pi == bEnumP )
        POSITION->DYN->PAWN_HASH ^= mask;
    POSITION->wtm ^= 1;
    POSITION->height++;
    POSITION->DYN->HASH ^= ZobristWTM;

    if( pi == bEnumK )
        {
        POSITION->DYN->PAWN_HASH ^= mask;
        POSITION->bKsq = to;
        }

    if( cp )
        {
        mask = SqClear[to];
        wBitboardOcc &= mask;
        POSITION->bitboard[cp] &= mask;
        POSITION->DYN->material -= MATERIAL_VALUE[cp];
        POSITION->DYN->STATIC -= PieceSquareValue[cp][to];

        if( cp == wEnumP )
            POSITION->DYN->PAWN_HASH ^= HASH[cp][to];
        POSITION->DYN->HASH ^= HASH[cp][to];
        POSITION->DYN->reversible = 0;
        }
    else
        {
        mask = SqSet[to];
        SetOccupied(mask, to);

        if( MoveIsOO(move) )
            {
            POSITION->DYN->reversible = 0;
            MakeBlackOO(POSITION, to);
            }
        }
    POSITION->sq[to] = pi;
    bBitboardOcc |= SqSet[to];
    POSITION->bitboard[pi] |= SqSet[to];

    if( pi == bEnumP )
        {
        POSITION->DYN->reversible = 0;

        if( MoveIsEP(move) )
            {
            z = to ^ 8;
            mask = SqClear[z];
            wBitboardOcc &= mask;
            wBitboardP &= mask;
            ClearOccupied(mask, z);
            POSITION->DYN->material -= MATERIAL_VALUE[wEnumP];
            POSITION->DYN->STATIC -= PieceSquareValue[wEnumP][z];
            POSITION->DYN->HASH ^= HASH[wEnumP][z];
            POSITION->DYN->PAWN_HASH ^= HASH[wEnumP][z];
            POSITION->sq[z] = 0;
            }
        else if( MoveIsProm(move) )
            {
            pi = PromB[(move &FLAG_MASK) >> 12];

            if( pi == bEnumBL && SqSet[to] & DARK )
                pi = bEnumBD;
            POSITION->sq[to] = pi;

            if( POSITION->bitboard[pi] )
                POSITION->DYN->material |= 0x80000000;
            POSITION->bitboard[bEnumP] &= SqClear[to];
            POSITION->bitboard[pi] |= SqSet[to];
            POSITION->DYN->material += MATERIAL_VALUE[pi] - MATERIAL_VALUE[bEnumP];
            POSITION->DYN->STATIC += PieceSquareValue[pi][to] - PieceSquareValue[bEnumP][to];
            POSITION->DYN->HASH ^= HASH[pi][to] ^ HASH[bEnumP][to];
            POSITION->DYN->PAWN_HASH ^= HASH[bEnumP][to];
            }
        else if( (to ^ fr) == 16 )
            {
            if( BlackEP[to & 7] & wBitboardP )
                {
                z = (fr + to) >> 1;
                POSITION->DYN->ep = z;
                POSITION->DYN->HASH ^= ZobristEP[z & 7];
                }
            }
        }
    POSITION->STACK[++(POSITION->StackHeight)] = POSITION->DYN->HASH;
    }

void Make( typePOS *POSITION, uint32 move )
    {
    if( POSITION->wtm )
        {
        if( NODE_CHECK & 4095 )
            NODE_CHECK--;
        POSITION->nodes--;
        MakeWhite(POSITION, move);
        }
    else
        {
        POSITION->nodes--;
        MakeBlack(POSITION, move);
        }
    }
