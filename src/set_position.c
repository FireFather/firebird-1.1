#include "firebird.h"

char *EmitFen( typePOS *POSITION, char *ARR )
    {
    int r, f, e = 0;
    int n = 0;
    char PIECE_CHAR[32] = "0PNKBBRQ0pnkbbrq";

    for ( r = R8; r >= R1; r-- )
        {
        for ( f = FA; f <= FH; f++ )
            {
            if( !POSITION->sq[8 * r + f] )
                e++;
            else
                {
                if( e )
                    ARR[n++] = '0' + e;
                e = 0;
                ARR[n++] = PIECE_CHAR[POSITION->sq[8 * r + f]];
                }
            }

        if( e )
            ARR[n++] = '0' + e;
        e = 0;

        if( r != R1 )
            ARR[n++] = '/';
        }
    ARR[n++] = ' ';
    ARR[n++] = (POSITION->wtm) ? 'w' : 'b';
    ARR[n++] = ' ';

    if( WhiteOO )
        ARR[n++] = 'K';

    if( WhiteOOO )
        ARR[n++] = 'Q';

    if( BlackOO )
        ARR[n++] = 'k';

    if( BlackOOO )
        ARR[n++] = 'q';

    if( !POSITION->DYN->oo )
        ARR[n++] = '-';
    ARR[n++] = ' ';

    if( !POSITION->DYN->ep )
        ARR[n++] = '-';
    else
        {
        ARR[n++] = (POSITION->DYN->ep & 7) + 'a';
        ARR[n++] = (POSITION->DYN->ep >> 3) + '1';
        }
    ARR[n++] = ' ';

    if( POSITION->DYN->reversible >= 100 )
        ARR[n++] = (POSITION->DYN->reversible / 100) + '0';

    if( POSITION->DYN->reversible >= 10 )
        ARR[n++] = ((POSITION->DYN->reversible / 10) % 10) + '0';
    ARR[n++] = (POSITION->DYN->reversible % 10) + '0';
    ARR[n++] = ' ';
    ARR[n++] = '0';
    ARR[n++] = 0;
    return ARR;
    }
static void ParseFen( typePOS *POSITION, char *I )
    {
    int trans = 7, co = 0, c = 0, i, p;

    for ( i = A1; i <= H8; i++ )
        POSITION->sq[i] = 0;

    while( 1 )
        {
        if( trans < 0 || co > 8 )
            FEN_ERROR("FEN %s col: %d tra: %d\n", I, co, trans);
        p = I[c++];

        if( p == 0 )
            return;

        if( co == 8 && p != '/' )
            FEN_ERROR("FEN %s col: %d tra: %d pez: %d\n", I, co, trans, p);

        switch( p )
            {
            case '/':
                trans--;
                co = 0;
                break;

            case 'p':
                POSITION->sq[co + 8 * trans] = bEnumP;
                co++;
                break;

            case 'b':
                if( SqSet[co + 8 * trans] & DARK )
                    POSITION->sq[co + 8 * trans] = bEnumBD;
                else
                    POSITION->sq[co + 8 * trans] = bEnumBL;
                co++;
                break;

            case 'n':
                POSITION->sq[co + 8 * trans] = bEnumN;
                co++;
                break;

            case 'r':
                POSITION->sq[co + 8 * trans] = bEnumR;
                co++;
                break;

            case 'q':
                POSITION->sq[co + 8 * trans] = bEnumQ;
                co++;
                break;

            case 'k':
                POSITION->sq[co + 8 * trans] = bEnumK;
                co++;
                break;

            case 'P':
                POSITION->sq[co + 8 * trans] = wEnumP;
                co++;
                break;

            case 'B':
                if( SqSet[co + 8 * trans] & DARK )
                    POSITION->sq[co + 8 * trans] = wEnumBD;
                else
                    POSITION->sq[co + 8 * trans] = wEnumBL;
                co++;
                break;

            case 'N':
                POSITION->sq[co + 8 * trans] = wEnumN;
                co++;
                break;

            case 'R':
                POSITION->sq[co + 8 * trans] = wEnumR;
                co++;
                break;

            case 'Q':
                POSITION->sq[co + 8 * trans] = wEnumQ;
                co++;
                break;

            case 'K':
                POSITION->sq[co + 8 * trans] = wEnumK;
                co++;
                break;

            case '1':
                co += 1;
                break;

            case '2':
                co += 2;
                break;

            case '3':
                co += 3;
                break;

            case '4':
                co += 4;
                break;

            case '5':
                co += 5;
                break;

            case '6':
                co += 6;
                break;

            case '7':
                co += 7;
                break;

            case '8':
                co += 8;
                break;

            default:
                FEN_ERROR("FEN %s col:%d tra:%d pez:%d\n", I, co, trans, p);
            }

        if( (trans == 0) && (co >= 8) )
            break;
        }
    }

#include <string.h>

char *ReadFEN( typePOS *POSITION, char *I )
    {
    char i[1024];
    boolean ok;
    int ep;
    sscanf(I, "%s", i);
    ParseFen(POSITION, i);
    memset(POSITION->DYN_ROOT, 0, 256 * sizeof(typeDYNAMIC));
    POSITION->DYN = POSITION->DYN_ROOT;
    I += strlen(i) + 1;
    sscanf(I, "%s", i);

    if( i[0] == 'w' )
        POSITION->wtm = true;

    else if( i[0] == 'b' )
        POSITION->wtm = false;

    else
        FEN_ERROR("FEN wb %s\n", i);
    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    POSITION->DYN->oo = 16;

    if( !strcmp(i, "KQkq") )
        POSITION->DYN->oo = 15;

    if( !strcmp(i, "Qkq") )
        POSITION->DYN->oo = 14;

    if( !strcmp(i, "Kkq") )
        POSITION->DYN->oo = 13;

    if( !strcmp(i, "kq") )
        POSITION->DYN->oo = 12;

    if( !strcmp(i, "KQq") )
        POSITION->DYN->oo = 11;

    if( !strcmp(i, "Qq") )
        POSITION->DYN->oo = 10;

    if( !strcmp(i, "Kq") )
        POSITION->DYN->oo = 9;

    if( !strcmp(i, "q") )
        POSITION->DYN->oo = 8;

    if( !strcmp(i, "KQk") )
        POSITION->DYN->oo = 7;

    if( !strcmp(i, "Qk") )
        POSITION->DYN->oo = 6;

    if( !strcmp(i, "Kk") )
        POSITION->DYN->oo = 5;

    if( !strcmp(i, "k") )
        POSITION->DYN->oo = 4;

    if( !strcmp(i, "KQ") )
        POSITION->DYN->oo = 3;

    if( !strcmp(i, "Q") )
        POSITION->DYN->oo = 2;

    if( !strcmp(i, "K") )
        POSITION->DYN->oo = 1;

    if( !strcmp(i, "-") )
        POSITION->DYN->oo = 0;

    if( POSITION->DYN->oo == 16 )
        FEN_ERROR("FEN oo %s\n", i);
    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    POSITION->DYN->ep = 0;

    if( !strcmp(i, "-") )
        ep = 0;
    else
        {
        ep = (i[0] - 'a') + 8 * (i[1] - '1');

        if( ep > H8 )
            FEN_ERROR("FEN ep %s\n", i);
        ok = 0;
        }

    if( ep )
        {
        if( POSITION->wtm )
            {
            if( FILE(ep) != FA && (POSITION->sq[ep - 9] == wEnumP) )
                ok = true;

            if( FILE(ep) != FH && (POSITION->sq[ep - 7] == wEnumP) )
                ok = true;
            }
        else
            {
            if( FILE(ep) != FA && (POSITION->sq[ep + 7] == bEnumP) )
                ok = true;

            if( FILE(ep) != FH && (POSITION->sq[ep + 9] == bEnumP) )
                ok = true;
            }

        if( ok )
            POSITION->DYN->ep = ep;
        }
    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    POSITION->DYN->reversible = (uint8)atoi(i);
    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    I += strlen(i) + 1;
    InitBitboards(POSITION);
    return I;
    }
static uint32 FullMove( typePOS *POSITION, uint32 x )
    {
    int pi, to = TO(x), fr = FROM(x);

    if( !x )
        return x;

    pi = POSITION->sq[fr];

    if( pi == wEnumK || pi == bEnumK )
        {
        if( to - fr == 2 || fr - to == 2 )
            x |= FlagOO;
        }

    if( TO(x) != 0 && TO(x) == POSITION->DYN->ep && (pi == wEnumP || pi == bEnumP) )
        x |= FlagEP;
    return x;
    }

#define IN_CHECK  ( POSITION->wtm ?               \
            ( POSITION->DYN->bAtt & wBitboardK) : \
            ( POSITION->DYN->wAtt & bBitboardK ) )

static void ReadMoves( typePOS *POSITION, char *I )
    {
    typeMoveList LIST[512], *list;
    char T[512];
    int i;
    uint32 full;

    while( I[0] )
        {
        Mobility(POSITION);

        if( IN_CHECK )
            {
            list = EvasionMoves(POSITION, LIST, 0xffffffffffffffff);
            list++;
            }
        else
            {
            list = CaptureMoves(POSITION, LIST, POSITION->OccupiedBW);
            list = OrdinaryMoves(POSITION, list);
            }
        full = FullMove(POSITION, (I[2] - 'a') + ((I[3] - '1') << 3) + ((I[0] - 'a') << 6) + ((I[1] - '1') << 9));
        sscanf(I, "%s", T);

        if( strlen(T) == 5 )
            {
            if( I[4] == 'b' )
                full |= FlagPromB;

            if( I[4] == 'n' )
                full |= FlagPromN;

            if( I[4] == 'r' )
                full |= FlagPromR;

            if( I[4] == 'q' )
                full |= FlagPromQ;
            }

        for ( i = 0; i < list - LIST; i++ )
            {
            if( full == (LIST[i].move & 0x7fff) )
                {
                Make(POSITION, full);
                break;
                }
            }

        if( i == list - LIST )
            {
            ERROR_END("moves? %s\n", T);
            }
        I += strlen(T) + 1;

        while( I[0] == ' ' )
            I++;
        }
    }

static const char ORIGINAL_POSITION[80] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void InitPosition( typePOS *POSITION, char *I )
    {
    char i[1024];
    NODE_CHECK = 0;
    sscanf(I, "%s", i);

    if( !strcmp(i, "startpos") )
        {
        ReadFEN(POSITION, ORIGINAL_POSITION);
        I += strlen("startpos") + 1;
        }

    if( !strcmp(i, "fen") )
        {
        I += strlen("fen") + 1;
        I = ReadFEN(POSITION, I);
        }

    if( I[0] )
        {
        sscanf(I, "%s", i);

        if( !strcmp(i, "moves") )
            {
            I += strlen("moves") + 1;
            ReadMoves(POSITION, I);
            }
        }
    POSITION->height = 0;

    if( NEW_GAME )
        ResetPositionalGain();
    }
