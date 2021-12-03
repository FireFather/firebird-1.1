#ifndef BUILD_SEE
#define BUILD_SEE
#include "firebird.h"

typedef enum
    {
    ValueP = 100,
    ValueN = 325,
    ValueB = 325,
    ValueR = 500,
    ValueQ = 975,
    ValueKing = 12345
    } EnumValues;
static const int Value[16] =
    {
    0, ValueP, ValueN, 12345678, ValueB, ValueB, ValueR, ValueQ,
	0, ValueP, ValueN, 12345678, ValueB, ValueB, ValueR, ValueQ
    };

#include "SEE.c"
#include "white.h"
#else
#include "black.h"
#endif

boolean MySEE( typePOS *POSITION, uint32 move )
    {
    int fr, to, PieceValue, CaptureValue, d, dir;
    uint64 bit, cbf, mask, TableIndex[4], gcm = 0, T;
    int SlideIndex[4], b, w;
    T = MyXRAY & OppOccupied;
    fr = FROM(move);
    to = TO(move);

    while( T )
        {
        b = LSB(T);
        w = MyXrayTable[b];
        BitClear(b, T);

        if( fr != w && LINE[to][b] != LINE[b][OppKingSq] )
            gcm |= SqSet[b];
        }
    gcm = ~gcm;
    PieceValue = Value[POSITION->sq[fr]];
    CaptureValue = Value[POSITION->sq[to]];

    if( PieceValue - CaptureValue > ValueP && OppAttackedPawns[to] & BitboardOppP & gcm )
        return false;
    bit = (BitboardMyN | (BitboardOppN &gcm)) & AttN[to];
    d = PieceValue - CaptureValue;

    if( d > ValueN && BitboardOppN & bit )
        return false;
    SlideIndex[Direction_h1a8] = (POSITION->OccupiedL45 >> LineShift[Direction_h1a8][to]) & 077;
    SlideIndex[Direction_a1h8] = (POSITION->OccupiedR45 >> LineShift[Direction_a1h8][to]) & 077;
    mask = BitboardMyQ | BitboardMyB | ((BitboardOppQ | BitboardOppB) & gcm);
    TableIndex[Direction_h1a8] = TableIndex[Direction_a1h8] = mask;
    bit |=
        (LineMask[Direction_h1a8][to][SlideIndex[Direction_h1a8]]
            | LineMask[Direction_a1h8][to][SlideIndex[Direction_a1h8]]) & mask;

    if( d > ValueB && (BitboardOppB &bit) )
        return false;
    SlideIndex[Direction_horz] = (POSITION->OccupiedBW >> LineShift[Direction_horz][to]) & 077;
    SlideIndex[Direction_vert] = (POSITION->OccupiedL90 >> LineShift[Direction_vert][to]) & 077;
    mask = BitboardMyQ | BitboardMyR | ((BitboardOppQ | BitboardOppR) & gcm);
    TableIndex[Direction_horz] = TableIndex[Direction_vert] = mask;
    bit |=
        (LineMask[Direction_horz][to][SlideIndex[Direction_horz]]
            | LineMask[Direction_vert][to][SlideIndex[Direction_vert]]) & mask;
    bit |= (BitboardMyK | BitboardOppK) & AttK[to];
    bit |= BitboardOppP & OppAttackedPawns[to] & gcm;
    bit |= BitboardMyP & MyAttackedPawns[to];
    cbf = ~(SqSet[fr] | SqSet[to]);
    bit &= cbf;
    dir = LINE[fr][to];

    if( dir != BAD_DIRECTION )
        bit |= LineMask[dir][fr][SlideIndex[dir]] & TableIndex[dir] & cbf;
    CaptureValue -= PieceValue;

    do
        {
        cbf &= ~bit;
        mask = BitboardOppP & bit;

        if( mask )
            {
            bit ^= (~(mask - 1)) & mask;
            PieceValue = ValueP;
            }
        else
            {
            mask = BitboardOppN & bit;

            if( mask )
                {
                bit ^= (~(mask - 1)) & mask;
                PieceValue = ValueN;
                }
            else
                {
                mask = BitboardOppB & bit;

                if( mask )
                    {
                    PieceValue = ValueB;
                    fr = LSB(mask);
                    dir = LINE[fr][to];
                    mask = LineMask[dir][fr][SlideIndex[dir]] & cbf & TableIndex[Direction_a1h8];
                    bit = mask | (SqClear[fr]&bit);
                    }
                else
                    {
                    mask = BitboardOppR & bit;

                    if( mask )
                        {
                        PieceValue = ValueR;
                        fr = LSB(mask);
                        dir = LINE[fr][to];
                        mask = LineMask[dir][fr][SlideIndex[dir]] & cbf & TableIndex[Direction_horz];
                        bit = mask | (SqClear[fr]&bit);
                        }
                    else
                        {
                        mask = BitboardOppQ & bit;

                        if( mask )
                            {
                            PieceValue = ValueQ;
                            fr = LSB(mask);
                            dir = LINE[fr][to];
                            mask = LineMask[dir][fr][SlideIndex[dir]] & cbf & TableIndex[dir];
                            bit = mask | (SqClear[fr]&bit);
                            }
                        else
                            {
                            if( !(BitboardOppK &bit) )
                                return true;
                            PieceValue = 12345;
                            }
                        }
                    }
                }
            }
        CaptureValue += PieceValue;

        if( CaptureValue < -60 )
            return false;
        mask = BitboardMyP & bit;

        if( mask )
            {
            bit ^= (~(mask - 1)) & mask;
            PieceValue = ValueP;
            }
        else
            {
            mask = BitboardMyN & bit;

            if( mask )
                {
                bit ^= (~(mask - 1)) & mask;
                PieceValue = ValueN;
                }
            else
                {
                mask = BitboardMyB & bit;

                if( mask )
                    {
                    PieceValue = ValueB;
                    fr = LSB(mask);
                    dir = LINE[fr][to];
                    mask = LineMask[dir][fr][SlideIndex[dir]] & cbf & TableIndex[Direction_a1h8];
                    bit = mask | (SqClear[fr]&bit);
                    }
                else
                    {
                    mask = BitboardMyR & bit;

                    if( mask )
                        {
                        PieceValue = ValueR;
                        fr = LSB(mask);
                        dir = LINE[fr][to];
                        mask = LineMask[dir][fr][SlideIndex[dir]] & cbf & TableIndex[Direction_horz];
                        bit = mask | (SqClear[fr]&bit);
                        }
                    else
                        {
                        mask = BitboardMyQ & bit;

                        if( mask )
                            {
                            PieceValue = ValueQ;
                            fr = LSB(mask);
                            dir = LINE[fr][to];
                            mask = LineMask[dir][fr][SlideIndex[dir]] & cbf & TableIndex[dir];
                            bit = mask | (SqClear[fr]&bit);
                            }
                        else
                            {
                            if( !(BitboardMyK &bit) )
                                return false;

                            if( CaptureValue > 6174 )
                                return true;
                            PieceValue = 23456;
                            }
                        }
                    }
                }
            }
        CaptureValue -= PieceValue;
        } while ( CaptureValue < -60 );
    return true;
    }
