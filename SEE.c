#ifndef BUILD_SEE
#define BUILD_SEE
#include "firebird.h"

static int Value[16] =
    {
    0, 100, 300, 12345678, 310, 310, 500, 950,
    0, 100, 300, 12345678, 310, 310, 500, 950
    };

#include "SEE.c"
#include "white.h"
#else
#include "black.h"
#endif

boolean MySEE( typePos *Position, uint32 move )
    {
    int fr, to, PieceValue, CaptureValue, d, dir;
    uint64 bit, cbf, mask, TableIndex[4], gcm = 0, T;
    int SlideIndex[4], b, w;
    T = MyXray & OppOccupied;
    fr = From(move);
    to = To(move);

    while( T )
        {
        b = LSB(T);
        w = MyXrayTable[b];
        BitClear(b, T);

        if( fr != w && Line[to][b] != Line[b][OppKingSq] )
            gcm |= SqSet[b];
        }
    gcm = ~gcm;
    PieceValue = Value[Position->sq[fr]];
    CaptureValue = Value[Position->sq[to]];

    if( PieceValue - CaptureValue > PValue && OppAttackedPawns[to] & BitboardOppP & gcm )
        return false;
    bit = (BitboardMyN | (BitboardOppN &gcm)) & AttN[to];
    d = PieceValue - CaptureValue;

    if( d > NValue && BitboardOppN & bit )
        return false;
    SlideIndex[Direction_h1a8] = (Position->OccupiedL45 >> LineShift[Direction_h1a8][to]) & 077;
    SlideIndex[Direction_a1h8] = (Position->OccupiedR45 >> LineShift[Direction_a1h8][to]) & 077;
    mask = BitboardMyQ | BitboardMyB | ((BitboardOppQ | BitboardOppB) & gcm);
    TableIndex[Direction_h1a8] = TableIndex[Direction_a1h8] = mask;
    bit |=
        (LineMask[Direction_h1a8][to][SlideIndex[Direction_h1a8]]
            | LineMask[Direction_a1h8][to][SlideIndex[Direction_a1h8]]) & mask;

    if( d > BValue && (BitboardOppB &bit) )
        return false;
    SlideIndex[Direction_horz] = (Position->OccupiedBW >> LineShift[Direction_horz][to]) & 077;
    SlideIndex[Direction_vert] = (Position->OccupiedL90 >> LineShift[Direction_vert][to]) & 077;
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
    dir = Line[fr][to];

    if( dir != BadDirection )
        bit |= LineMask[dir][fr][SlideIndex[dir]] & TableIndex[dir] & cbf;
    CaptureValue -= PieceValue;

    do
        {
        cbf &= ~bit;
        mask = BitboardOppP & bit;

        if( mask )
            {
            bit ^= (~(mask - 1)) & mask;
            PieceValue = PValue;
            }
        else
            {
            mask = BitboardOppN & bit;

            if( mask )
                {
                bit ^= (~(mask - 1)) & mask;
                PieceValue = NValue;
                }
            else
                {
                mask = BitboardOppB & bit;

                if( mask )
                    {
                    PieceValue = BValue;
                    fr = LSB(mask);
                    dir = Line[fr][to];
                    mask = LineMask[dir][fr][SlideIndex[dir]] & cbf & TableIndex[Direction_a1h8];
                    bit = mask | (SqClear[fr]&bit);
                    }
                else
                    {
                    mask = BitboardOppR & bit;

                    if( mask )
                        {
                        PieceValue = RValue;
                        fr = LSB(mask);
                        dir = Line[fr][to];
                        mask = LineMask[dir][fr][SlideIndex[dir]] & cbf & TableIndex[Direction_horz];
                        bit = mask | (SqClear[fr]&bit);
                        }
                    else
                        {
                        mask = BitboardOppQ & bit;

                        if( mask )
                            {
                            PieceValue = QValue;
                            fr = LSB(mask);
                            dir = Line[fr][to];
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
            PieceValue = PValue;
            }
        else
            {
            mask = BitboardMyN & bit;

            if( mask )
                {
                bit ^= (~(mask - 1)) & mask;
                PieceValue = NValue;
                }
            else
                {
                mask = BitboardMyB & bit;

                if( mask )
                    {
                    PieceValue = BValue;
                    fr = LSB(mask);
                    dir = Line[fr][to];
                    mask = LineMask[dir][fr][SlideIndex[dir]] & cbf & TableIndex[Direction_a1h8];
                    bit = mask | (SqClear[fr]&bit);
                    }
                else
                    {
                    mask = BitboardMyR & bit;

                    if( mask )
                        {
                        PieceValue = RValue;
                        fr = LSB(mask);
                        dir = Line[fr][to];
                        mask = LineMask[dir][fr][SlideIndex[dir]] & cbf & TableIndex[Direction_horz];
                        bit = mask | (SqClear[fr]&bit);
                        }
                    else
                        {
                        mask = BitboardMyQ & bit;

                        if( mask )
                            {
                            PieceValue = QValue;
                            fr = LSB(mask);
                            dir = Line[fr][to];
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
