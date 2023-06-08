#include "firebird.h"

#define QUEEN_ENDING 1
#define ROOK_ENDING 2
#define OPPOSITE_BISHOP_ENDING 3
#define BISHOP_ENDING 4
#define KNIGHT_ENDING 5
#define BISHOP_KNIGHT_ENDING 6
#define PAWN_ENDING 7
#define WHITE_MINOR_ONLY 8
#define BLACK_MINOR_ONLY 16
#define BISHOP_KNIGHT_MATE 32

static int InitFlags( int wP, int wN, int wB, int wBL, int wBD, int wR, int wQ, int bP, int bN, int bB, int bB1,
    int bB2, int bR, int bQ )
    {
    uint8 FLAGS = ((wN || wB || wQ || wR) << 1) | ((bN || bB || bQ || bR) << 0);

    if( !wQ && !wR && wB + wN == 1 && wP <= 4 && bP <= 4 )
        FLAGS &= 1;

    if( !bQ && !bR && bB + bN == 1 && wP <= 4 && bP <= 4 )
        FLAGS &= 2;

    if( wQ == 1 && bQ == 1 && !wR && !bR && !wB && !bB && !wN && !bN )
        FLAGS |= QUEEN_ENDING << 2;

    if( wR == 1 && bR == 1 && !wQ && !bQ && !wB && !bB && !wN && !bN )
        FLAGS |= ROOK_ENDING << 2;

    if( wB == 1 && bB == 1 && !wQ && !bQ && !wR && !bR && !wN && !bN )
        {
        if( (wBL == 1 && bB2 == 1) || (wBD == 1 && bB1 == 1) )
            FLAGS |= BISHOP_ENDING << 2;
        else
            FLAGS |= OPPOSITE_BISHOP_ENDING << 2;
        FLAGS |= (WHITE_MINOR_ONLY | BLACK_MINOR_ONLY) << 2;
        }

    if( wN == 1 && bN == 1 && !wQ && !bQ && !wR && !bR && !wB && !bB )
        FLAGS |= KNIGHT_ENDING << 2;

    if( wN == 1 && bB == 1 && !wQ && !bQ && !wR && !bR && !wB && !bN )
        FLAGS |= BISHOP_KNIGHT_ENDING << 2;

    if( wB == 1 && bN == 1 && !wQ && !bQ && !wR && !bR && !bB && !wN )
        FLAGS |= BISHOP_KNIGHT_ENDING << 2;

    if( wB == 1 && !wQ && !wR && !wN )
        FLAGS |= WHITE_MINOR_ONLY << 2;

    if( bB == 1 && !bQ && !bR && !bN )
        FLAGS |= BLACK_MINOR_ONLY << 2;

    if( wN == 1 && !wQ && !wR && !wB )
        FLAGS |= WHITE_MINOR_ONLY << 2;

    if( bN == 1 && !bQ && !bR && !bB )
        FLAGS |= BLACK_MINOR_ONLY << 2;

    if( !wN && !wB && !wR && !wQ && !bN && !bB && !bQ && !bQ && wP + bP == 1 )
        FLAGS |= PAWN_ENDING << 2;

    if( wN == 1 && wB == 1 && !wR && !wQ && !wP && !bQ && !bR && !bB && !bN && !bP )
        FLAGS |= BISHOP_KNIGHT_MATE << 2;

    if( bN == 1 && bB == 1 && !bR && !bQ && !bP && !wQ && !wR && !wB && !wN && !wP )
        FLAGS |= BISHOP_KNIGHT_MATE << 2;
    return FLAGS;
    }
static int InitTokens( int wP, int wN, int wB, int wBL, int wBD, int wR, int wQ, int bP, int bN, int bB, int bB1,
    int bB2, int bR, int bQ )
    {
    int token = 0x80;

    if( wN == 0 && bN == 0 && wB == 0 && bB == 0 && wR == 0 && bR == 0 && wQ == 1 && bQ == 1 )
        token = 0x70 + MAX(wP, bP);

    if( wN == 0 && bN == 0 && wB == 0 && bB == 0 && wQ == 0 && bQ == 0 && wR == 1 && bR == 1 )
        token = 0x60 + 2 * MAX(wP, bP);

    if( wN == 0 && bN == 0 && wR == 0 && bR == 0 && wQ == 0 && bQ == 0 && wB == 1 && bB == 1 )
        {
        if( (wBL == 1 && wBD == 0 && bB1 == 0 && bB2 == 1) || (wBL == 0 && wBD == 1 && bB1 == 1 && bB2 == 0) )
            token = 0x30 + 4 * MAX(wP, bP);
        else
            token = 0x78 + 2 * MAX(wP, bP);
        }

    if( wN == 1 && bN == 1 && wR == 0 && bR == 0 && wQ == 0 && bQ == 0 && wB == 0 && bB == 0 )
        token = 0x80 + MAX(wP, bP);

    if( wN == 0 && bN == 0 && wR == 0 && bR == 0 && wQ == 0 && bQ == 0 && wB == 0 && bB == 0 )
        token = 0xc0 - 8 * MAX(wP, bP);

    if( wN == 0 && bN == 0 && wB == 1 && bB == 1 && wR == 1 && bR == 1 && wQ == 0 && bQ == 0 )
        {
        if( (wBL == 1 && wBD == 0 && bB1 == 0 && bB2 == 1) || (wBL == 0 && wBD == 1 && bB1 == 1 && bB2 == 0) )
            token = 0x70 + MAX(wP, bP);
        }
    return token;
    }
static int WhiteWeight( int wP, int wN, int wB, int wBL, int wBD, int wR, int wQ, int bP, int bN, int bB, int bB1,
    int bB2, int bR, int bQ )
    {
    int wMINOR, bMINOR, wPHASE, bPHASE, wWEIGHT, wVALUE, bVALUE;
    wMINOR = wB + wN;
    bMINOR = bB + bN;
    wPHASE = wMINOR + 2 * wR + 4 * wQ;
    bPHASE = bMINOR + 2 * bR + 4 * bQ;
    wVALUE = 3 * (wB + wN) + 5 * wR + 9 * wQ;
    bVALUE = 3 * (bB + bN) + 5 * bR + 9 * bQ;
    wWEIGHT = 10;

    if( !wP )
        {
        if( wPHASE == 1 )
            wWEIGHT = 0;

        if( wPHASE == 2 )
            {
            if( bPHASE == 0 )
                {
                if( wN == 2 )
                    {
                    if( bP >= 1 )
                        wWEIGHT = 3;
                    else
                        wWEIGHT = 0;
                    }
                }

            if( bPHASE == 1 )
                {
                wWEIGHT = 1;

                if( wB == 2 && bN == 1 )
                    wWEIGHT = 8;

                if( wR == 1 && bN == 1 )
                    wWEIGHT = 2;
                }

            if( bPHASE == 2 )
                wWEIGHT = 1;
            }

        if( wPHASE == 3 && wR == 1 )
            {
            if( bPHASE == 2 && bR == 1 )
                {
                if( wN == 1 )
                    wWEIGHT = 1;

                if( wB == 1 )
                    wWEIGHT = 1;
                }

            if( bPHASE == 2 && bR == 0 )
                {
                wWEIGHT = 2;

                if( wB == 1 && bN == 2 )
                    wWEIGHT = 6;

                if( bN == 1 && ((wBL == 1 && bB1 == 1) || (wBD == 1 && bB2 == 1)) )
                    wWEIGHT = 2;

                if( bN == 1 && ((wBD == 1 && bB1 == 1) || (wBL == 1 && bB2 == 1)) )
                    wWEIGHT = 7;
                }

            if( bPHASE == 3 )
                wWEIGHT = 2;
            }

        if( wPHASE == 3 && wR == 0 )
            {
            if( bPHASE == 2 && bR == 1 )
                {
                if( wN == 2 )
                    wWEIGHT = 2;

                if( wB == 2 )
                    wWEIGHT = 7;
                }

            if( bPHASE == 2 && bR == 0 )
                {
                wWEIGHT = 2;

                if( wB == 2 && bN == 2 )
                    wWEIGHT = 4;
                }

            if( bPHASE == 3 )
                wWEIGHT = 2;
            }

        if( wPHASE == 4 && wQ )
            {
            if( bPHASE == 2 && bN == 2 )
                wWEIGHT = 2;

            if( bPHASE == 2 && bN == 1 )
                wWEIGHT = 8;

            if( bPHASE == 2 && bN == 0 )
                wWEIGHT = 7;

            if( bPHASE == 3 )
                wWEIGHT = 1;

            if( bPHASE == 4 )
                wWEIGHT = 1;
            }

        if( wPHASE == 4 && wR == 2 )
            {
            if( bPHASE == 2 && bR == 0 )
                wWEIGHT = 7;

            if( bPHASE == 3 )
                wWEIGHT = 2;

            if( bPHASE == 4 )
                wWEIGHT = 1;
            }

        if( wPHASE == 4 && wR == 1 )
            {
            if( bPHASE == 3 && bR == 1 )
                wWEIGHT = 3;

            if( bPHASE == 3 && bR == 0 )
                wWEIGHT = 2;

            if( bPHASE == 4 )
                wWEIGHT = 2;
            }

        if( wPHASE == 4 && wR == 0 && wQ == 0 )
            {
            if( bPHASE == 3 && bR == 1 )
                wWEIGHT = 4;

            if( bPHASE == 3 && bR == 0 )
                wWEIGHT = 2;

            if( bPHASE == 4 && bQ )
                wWEIGHT = 8;

            if( bPHASE == 4 && bQ == 0 )
                wWEIGHT = 1;
            }

        if( wPHASE == 5 && wQ )
            {
            if( bPHASE == 4 )
                wWEIGHT = 2;

            if( bPHASE == 5 )
                wWEIGHT = 1;

            if( bPHASE == 4 && bR == 2 )
                {
                if( wN )
                    wWEIGHT = 3;

                if( wB )
                    wWEIGHT = 7;
                }

            if( bPHASE == 5 )
                wWEIGHT = 1;
            }

        if( wPHASE == 5 && wR == 1 )
            {
            if( bPHASE == 4 && bQ )
                wWEIGHT = 9;

            if( bPHASE == 4 && bR == 2 )
                wWEIGHT = 7;

            if( bPHASE == 4 && bR == 1 )
                wWEIGHT = 3;

            if( bPHASE == 4 && bQ == 0 && bR == 0 )
                wWEIGHT = 1;

            if( bPHASE == 5 )
                wWEIGHT = 2;
            }

        if( wPHASE == 5 && wR == 2 )
            {
            if( bPHASE == 4 && bQ && wB == 1 )
                wWEIGHT = 8;

            if( bPHASE == 4 && bQ && wN == 1 )
                wWEIGHT = 7;

            if( bPHASE == 4 && bR == 2 )
                wWEIGHT = 3;

            if( bPHASE == 4 && bR == 1 )
                wWEIGHT = 2;

            if( bPHASE == 4 && bQ == 0 && bR == 0 )
                wWEIGHT = 1;

            if( bPHASE == 5 )
                wWEIGHT = 1;
            }

        if( wPHASE == 6 && wQ && wR )
            {
            if( bPHASE == 4 && bQ == 0 && bR == 0 )
                wWEIGHT = 2;

            if( bPHASE == 5 && bQ )
                wWEIGHT = 1;

            if( bPHASE == 4 && bR == 1 )
                wWEIGHT = 6;

            if( bPHASE == 4 && bR == 2 )
                wWEIGHT = 3;

            if( bPHASE == 5 && bR )
                wWEIGHT = 1;

            if( bPHASE == 6 )
                wWEIGHT = 1;
            }

        if( wPHASE == 6 && wQ && wR == 0 )
            {
            if( bPHASE == 4 && bQ == 0 && bR == 0 )
                wWEIGHT = 5;

            if( bPHASE == 5 && bQ )
                wWEIGHT = 2;

            if( bPHASE == 5 && bR == 2 )
                wWEIGHT = 2;

            if( bPHASE == 5 && bR == 1 )
                wWEIGHT = 1;

            if( bPHASE == 6 )
                wWEIGHT = 1;
            }

        if( wPHASE == 6 && wQ == 0 && wR == 2 )
            {
            if( bPHASE == 5 && bQ )
                wWEIGHT = 7;

            if( bPHASE == 5 && bR == 1 )
                wWEIGHT = 1;

            if( bPHASE == 5 && bR == 2 )
                wWEIGHT = 2;

            if( bPHASE == 6 )
                wWEIGHT = 1;
            }

        if( wPHASE == 6 && wQ == 0 && wR == 1 )
            {
            if( bPHASE == 5 && bQ )
                wWEIGHT = 9;

            if( bPHASE == 5 && bR == 2 )
                wWEIGHT = 3;

            if( bPHASE == 5 && bR == 1 )
                wWEIGHT = 2;

            if( bPHASE == 6 )
                wWEIGHT = 1;

            if( bPHASE == 6 && bQ )
                wWEIGHT = 2;

            if( bPHASE == 6 && bQ && bR )
                wWEIGHT = 4;
            }

        if( wPHASE >= 7 )
            {
            if( wVALUE > bVALUE + 4 )
                wWEIGHT = 9;

            if( wVALUE == bVALUE + 4 )
                wWEIGHT = 7;

            if( wVALUE == bVALUE + 3 )
                wWEIGHT = 4;

            if( wVALUE == bVALUE + 2 )
                wWEIGHT = 2;

            if( wVALUE < bVALUE + 2 )
                wWEIGHT = 1;
            }
        }

    if( wP == 1 )
        {
        if( bPHASE == 1 )
            {
            if( wPHASE == 1 )
                wWEIGHT = 3;

            if( wPHASE == 2 && wN == 2 )
                {
                if( bP == 0 )
                    wWEIGHT = 3;
                else
                    wWEIGHT = 5;
                }

            if( wPHASE == 2 && wR == 1 )
                wWEIGHT = 7;
            }

        if( bPHASE == 2 && bR == 1 && wPHASE == 2 && wR == 1 )
            wWEIGHT = 8;

        if( bPHASE == 2 && bR == 0 && wPHASE == 2 )
            wWEIGHT = 4;

        if( bPHASE >= 3 && bMINOR > 0 && wPHASE == bPHASE )
            wWEIGHT = 3;

        if( bPHASE >= 3 && bMINOR == 0 && wPHASE == bPHASE )
            wWEIGHT = 5;

        if( bPHASE == 4 && bQ == 1 && wPHASE == bPHASE )
            wWEIGHT = 7;
        }
    return wWEIGHT;
    }
static int BlackWeight( int wP, int wN, int wB, int wBL, int wBD, int wR, int wQ, int bP, int bN, int bB, int bB1,
    int bB2, int bR, int bQ )
    {
    int wMINOR, bMINOR, wPHASE, bPHASE, bWEIGHT, wVALUE, bVALUE;
    wMINOR = wB + wN;
    bMINOR = bB + bN;
    wPHASE = wMINOR + 2 * wR + 4 * wQ;
    bPHASE = bMINOR + 2 * bR + 4 * bQ;
    wVALUE = 3 * (wB + wN) + 5 * wR + 9 * wQ;
    bVALUE = 3 * (bB + bN) + 5 * bR + 9 * bQ;
    bWEIGHT = 10;

    if( !bP )
        {
        if( bPHASE == 1 )
            bWEIGHT = 0;

        if( bPHASE == 2 )
            {
            if( wPHASE == 0 )
                {
                if( bN == 2 )
                    {
                    if( bP >= 1 )
                        bWEIGHT = 3;
                    else
                        bWEIGHT = 0;
                    }
                }

            if( wPHASE == 1 )
                {
                bWEIGHT = 1;

                if( bB == 2 && wN == 1 )
                    bWEIGHT = 8;

                if( bR == 1 && wN == 1 )
                    bWEIGHT = 2;
                }

            if( wPHASE == 2 )
                bWEIGHT = 1;
            }

        if( bPHASE == 3 && bR == 1 )
            {
            if( wPHASE == 2 && wR == 1 )
                {
                if( bN == 1 )
                    bWEIGHT = 1;

                if( bB == 1 )
                    bWEIGHT = 1;
                }

            if( wPHASE == 2 && wR == 0 )
                {
                bWEIGHT = 2;

                if( bB == 1 && wN == 2 )
                    bWEIGHT = 6;

                if( wN == 1 && ((bB1 == 1 && wBL == 1) || (bB2 == 1 && wBD == 1)) )
                    bWEIGHT = 2;

                if( wN == 1 && ((bB2 == 1 && wBL == 1) || (bB1 == 1 && wBD == 1)) )
                    bWEIGHT = 7;
                }

            if( wPHASE == 3 )
                bWEIGHT = 2;
            }

        if( bPHASE == 3 && bR == 0 )
            {
            if( wPHASE == 2 && wR == 1 )
                {
                if( bN == 2 )
                    bWEIGHT = 2;

                if( bB == 2 )
                    bWEIGHT = 7;
                }

            if( wPHASE == 2 && wR == 0 )
                {
                bWEIGHT = 2;

                if( bB == 2 && wN == 2 )
                    bWEIGHT = 4;
                }

            if( wPHASE == 3 )
                bWEIGHT = 2;
            }

        if( bPHASE == 4 && bQ )
            {
            if( wPHASE == 2 && wN == 2 )
                bWEIGHT = 2;

            if( wPHASE == 2 && wN == 1 )
                bWEIGHT = 8;

            if( wPHASE == 2 && wN == 0 )
                bWEIGHT = 7;

            if( wPHASE == 3 )
                bWEIGHT = 1;

            if( wPHASE == 4 )
                bWEIGHT = 1;
            }

        if( bPHASE == 4 && bR == 2 )
            {
            if( wPHASE == 2 && wR == 0 )
                bWEIGHT = 7;

            if( wPHASE == 3 )
                bWEIGHT = 2;

            if( wPHASE == 4 )
                bWEIGHT = 1;
            }

        if( bPHASE == 4 && bR == 1 )
            {
            if( wPHASE == 3 && wR == 1 )
                bWEIGHT = 3;

            if( wPHASE == 3 && wR == 0 )
                bWEIGHT = 2;

            if( wPHASE == 4 )
                bWEIGHT = 2;
            }

        if( bPHASE == 4 && bR == 0 && bQ == 0 )
            {
            if( wPHASE == 3 && wR == 1 )
                bWEIGHT = 4;

            if( wPHASE == 3 && wR == 0 )
                bWEIGHT = 2;

            if( wPHASE == 4 && wQ )
                bWEIGHT = 8;

            if( wPHASE == 4 && wQ == 0 )
                bWEIGHT = 1;
            }

        if( bPHASE == 5 && bQ )
            {
            if( wPHASE == 4 )
                bWEIGHT = 2;

            if( wPHASE == 5 )
                bWEIGHT = 1;

            if( wPHASE == 4 && wR == 2 )
                {
                if( bN )
                    bWEIGHT = 3;

                if( bB )
                    bWEIGHT = 7;
                }

            if( wPHASE == 5 )
                bWEIGHT = 1;
            }

        if( bPHASE == 5 && bR == 1 )
            {
            if( wPHASE == 4 && wQ )
                bWEIGHT = 9;

            if( wPHASE == 4 && wR == 2 )
                bWEIGHT = 7;

            if( wPHASE == 4 && wR == 1 )
                bWEIGHT = 3;

            if( wPHASE == 4 && wQ == 0 && wR == 0 )
                bWEIGHT = 1;

            if( wPHASE == 5 )
                bWEIGHT = 2;
            }

        if( bPHASE == 5 && bR == 2 )
            {
            if( wPHASE == 4 && wQ && bB == 1 )
                bWEIGHT = 8;

            if( wPHASE == 4 && wQ && bN == 1 )
                bWEIGHT = 7;

            if( wPHASE == 4 && wR == 2 )
                bWEIGHT = 3;

            if( wPHASE == 4 && wR == 1 )
                bWEIGHT = 2;

            if( wPHASE == 4 && wQ == 0 && wR == 0 )
                bWEIGHT = 1;

            if( wPHASE == 5 )
                bWEIGHT = 1;
            }

        if( bPHASE == 6 && bQ && bR )
            {
            if( wPHASE == 4 && wQ == 0 && wR == 0 )
                bWEIGHT = 2;

            if( wPHASE == 5 && wQ )
                bWEIGHT = 1;

            if( wPHASE == 4 && wR == 1 )
                bWEIGHT = 6;

            if( wPHASE == 4 && wR == 2 )
                bWEIGHT = 3;

            if( wPHASE == 5 && wR )
                bWEIGHT = 1;

            if( wPHASE == 6 )
                bWEIGHT = 1;
            }

        if( bPHASE == 6 && bQ && bR == 0 )
            {
            if( wPHASE == 4 && wQ == 0 && wR == 0 )
                bWEIGHT = 5;

            if( wPHASE == 5 && wQ )
                bWEIGHT = 2;

            if( wPHASE == 5 && wR == 2 )
                bWEIGHT = 2;

            if( wPHASE == 5 && wR == 1 )
                bWEIGHT = 1;

            if( wPHASE == 6 )
                bWEIGHT = 1;
            }

        if( bPHASE == 6 && bQ == 0 && bR == 2 )
            {
            if( wPHASE == 5 && wQ )
                bWEIGHT = 7;

            if( wPHASE == 5 && wR == 1 )
                bWEIGHT = 1;

            if( wPHASE == 5 && wR == 2 )
                bWEIGHT = 2;

            if( wPHASE == 6 )
                bWEIGHT = 1;
            }

        if( bPHASE == 6 && bQ == 0 && bR == 1 )
            {
            if( wPHASE == 5 && wQ )
                bWEIGHT = 9;

            if( wPHASE == 5 && wR == 2 )
                bWEIGHT = 3;

            if( wPHASE == 5 && wR == 1 )
                bWEIGHT = 2;

            if( wPHASE == 6 )
                bWEIGHT = 1;

            if( wPHASE == 6 && wQ )
                bWEIGHT = 2;

            if( wPHASE == 6 && wQ && wR )
                bWEIGHT = 4;
            }

        if( bPHASE >= 7 )
            {
            if( bVALUE > wVALUE + 4 )
                bWEIGHT = 9;

            if( bVALUE == wVALUE + 4 )
                bWEIGHT = 7;

            if( bVALUE == wVALUE + 3 )
                bWEIGHT = 4;

            if( bVALUE == wVALUE + 2 )
                bWEIGHT = 2;

            if( bVALUE < wVALUE + 2 )
                bWEIGHT = 1;
            }
        }

    if( bP == 1 )
        {
        if( wPHASE == 1 )
            {
            if( bPHASE == 1 )
                bWEIGHT = 3;

            if( bPHASE == 2 && bN == 2 )
                {
                if( wP == 0 )
                    bWEIGHT = 3;
                else
                    bWEIGHT = 5;
                }

            if( bPHASE == 2 && bR == 1 )
                bWEIGHT = 7;
            }

        if( wPHASE == 2 && wR == 1 && bPHASE == 2 && bR == 1 )
            bWEIGHT = 8;

        if( wPHASE == 2 && wR == 0 && bPHASE == 2 )
            bWEIGHT = 4;

        if( wPHASE >= 3 && wMINOR > 0 && bPHASE == wPHASE )
            bWEIGHT = 3;

        if( wPHASE >= 3 && wMINOR == 0 && bPHASE == wPHASE )
            bWEIGHT = 5;

        if( wPHASE == 4 && wQ == 1 && bPHASE == wPHASE )
            bWEIGHT = 7;
        }
    return bWEIGHT;
    }

#define VALUE4(w,x,y,z) \
	((((uint64) z) << 48) + (((uint64) y) << 32) + \
	(((uint64) x) << 16) + (((uint64) w) << 0))
#define BishopPair VALUE4 (35, 40, 50, 55)
#define ValueP VALUE4 (80, 90, 110, 125)
#define ValueN VALUE4 (265, 280, 320, 355)
#define ValueR VALUE4 (405, 450, 550, 610)
#define ValueQ VALUE4 (800, 875, 1025, 1150)
#define ValueB VALUE4 (280, 295, 325, 360)
#define KnightPawnAdjust VALUE4 (0, 2, 4, 5)
#define RookPawnAdjust VALUE4 (5, 4, 2, 0)

static uint64 ComputeValue( int wP, int wN, int wB, int wBL, int wBD, int wR, int wQ, int bP, int bN, int bB, int bB1,
    int bB2, int bR, int bQ )
    {
    uint64 va = 0;
    int wMINOR = wN + wB;
    int bMINOR = bN + bB;
    va += (wB / 2 - bB / 2) * BishopPair;
    va += (wP - bP) * ValueP;
    va += (wN - bN) * ValueN;
    va += (wR - bR) * ValueR;
    va += (wQ - bQ) * ValueQ;
    va += (wB - bB) * ValueB;

    if( wR == 2 )
        va -= VALUE4(16, 20, 28, 32);

    if( bR == 2 )
        va += VALUE4(16, 20, 28, 32);

    if( wQ + wR >= 2 )
        va -= VALUE4(8, 10, 14, 16);

    if( bQ + bR >= 2 )
        va += VALUE4(8, 10, 14, 16);

    if( wMINOR > bMINOR )
        va += VALUE4(20, 15, 10, 5);

    if( bMINOR > wMINOR )
        va -= VALUE4(20, 15, 10, 5);
    va -= (wP - 5) * wR * RookPawnAdjust;
    va += (wP - 5) * wN * KnightPawnAdjust;
    va += (bP - 5) * bR * RookPawnAdjust;
    va -= (bP - 5) * bN * KnightPawnAdjust;
    return va;
    }

#define PHASE_MINOR (1)
#define PHASE_ROOK (3)
#define PHASE_QUEEN (6)

static void CalculateMaterialValue( int c )
    {
    int wQ, bQ, wR, bR, wBL, bB1, wBD, bB2, wN, bN, wP, bP, n, Value, wB, bB;
    int wt, wWEIGHT, bWEIGHT, phase, va1, va2, va3, va4;
    uint64 va;
    n = c;
    wQ = n % 2;
    n /= 2;
    bQ = n % 2;
    n /= 2;
    wR = n % 3;
    n /= 3;
    bR = n % 3;
    n /= 3;
    wBL = n % 2;
    n /= 2;
    wBD = n % 2;
    n /= 2;
    bB1 = n % 2;
    n /= 2;
    bB2 = n % 2;
    n /= 2;
    wN = n % 3;
    n /= 3;
    bN = n % 3;
    n /= 3;
    wP = n % 9;
    n /= 9;
    bP = n % 9;
    wB = wBL + wBD;
    bB = bB1 + bB2;
    va = ComputeValue(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bB1, bB2, bR, bQ);
    phase = PHASE_MINOR * (wN + wB + bN + bB) + PHASE_ROOK * (wR + bR) + PHASE_QUEEN * (wQ + bQ);
    va1 = va & 0xffff;
    va2 = ((va >> 16) & 0xffff) + (va1 > 0x8000);
    va1 = (sint16)va1;
    va3 = ((va >> 32) & 0xffff) + (va2 > 0x8000);
    va2 = (sint16)va2;
    va4 = ((va >> 48) & 0xffff) + (va3 > 0x8000);
    va3 = (sint16)va3;
    va4 = (sint16)va4;

    if( phase < 8 )
        {
        va4 *= 8 - phase;
        va3 *= phase;
        va = va3 + va4;
        Value = ((int)va) / 8;
        }
    else if( phase < 24 )
        {
        va3 *= 24 - phase;
        va2 *= phase - 8;
        va = va2 + va3;
        Value = ((int)va) / 16;
        }
    else
        {
        va2 *= 32 - phase;
        va1 *= phase - 24;
        va = va1 + va2;
        Value = ((int)va) / 8;
        }
    wWEIGHT = WhiteWeight(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bB1, bB2, bR, bQ);
    bWEIGHT = BlackWeight(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bB1, bB2, bR, bQ);

    if( Value > 0 )
        wt = wWEIGHT;
    else
        wt = bWEIGHT;
    Value *= wt;
    Value /= 10;
    MATERIAL[c].Value = Value;
    MATERIAL[c].token = InitTokens(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bB1, bB2, bR, bQ);
    MATERIAL[c].flags = InitFlags(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bB1, bB2, bR, bQ);
    }

void InitMaterialValue()
    {
    int c;

    for ( c = 0; c < 419904; c++ )
        CalculateMaterialValue(c);
    }
