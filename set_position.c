
#include "firebird.h"

static void ParseFen( typePos *Position, char *I )
    {
    int rank = 7, file = 0, c = 0, i, p;

    for ( i = A1; i <= H8; i++ )
        Position->sq[i] = 0;

    while( 1 )
        {
        p = I[c++];

        if( p == 0 )
            return;

        switch( p )
            {
            case '/':
                rank--;
                file = 0;
                break;

            case 'p':
                Position->sq[file + 8 * rank] = bEnumP;
                file++;
                break;

            case 'b':
                if( SqSet[file + 8 * rank] & Black )
                    Position->sq[file + 8 * rank] = bEnumBD;
                else
                    Position->sq[file + 8 * rank] = bEnumBL;
                file++;
                break;

            case 'n':
                Position->sq[file + 8 * rank] = bEnumN;
                file++;
                break;

            case 'r':
                Position->sq[file + 8 * rank] = bEnumR;
                file++;
                break;

            case 'q':
                Position->sq[file + 8 * rank] = bEnumQ;
                file++;
                break;

            case 'k':
                Position->sq[file + 8 * rank] = bEnumK;
                file++;
                break;

            case 'P':
                Position->sq[file + 8 * rank] = wEnumP;
                file++;
                break;

            case 'B':
                if( SqSet[file + 8 * rank] & Black )
                    Position->sq[file + 8 * rank] = wEnumBD;
                else
                    Position->sq[file + 8 * rank] = wEnumBL;
                file++;
                break;

            case 'N':
                Position->sq[file + 8 * rank] = wEnumN;
                file++;
                break;

            case 'R':
                Position->sq[file + 8 * rank] = wEnumR;
                file++;
                break;

            case 'Q':
                Position->sq[file + 8 * rank] = wEnumQ;
                file++;
                break;

            case 'K':
                Position->sq[file + 8 * rank] = wEnumK;
                file++;
                break;

            case '1':
                file += 1;
                break;

            case '2':
                file += 2;
                break;

            case '3':
                file += 3;
                break;

            case '4':
                file += 4;
                break;

            case '5':
                file += 5;
                break;

            case '6':
                file += 6;
                break;

            case '7':
                file += 7;
                break;

            case '8':
                file += 8;
                break;

            default:
				{}
            }

        if( (rank == 0) && (file >= 8) )
            break;
        }
    }

char *ReadFEN( typePos *Position, char *I )
    {
    char i[1024];
    boolean ok;
    int ep;
    sscanf(I, "%s", i);
    ParseFen(Position, i);
    memset(Position->Root, 0, 256 * sizeof(typePosition));
    Position->Current = Position->Root;
    I += strlen(i) + 1;
    sscanf(I, "%s", i);

    if( i[0] == 'w' )
        Position->wtm = true;

    else if( i[0] == 'b' )
        Position->wtm = false;

    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    Position->Current->oo = 16;

    if( !strcmp(i, "KQkq") )
        Position->Current->oo = 15;

    if( !strcmp(i, "Qkq") )
        Position->Current->oo = 14;

    if( !strcmp(i, "Kkq") )
        Position->Current->oo = 13;

    if( !strcmp(i, "kq") )
        Position->Current->oo = 12;

    if( !strcmp(i, "KQq") )
        Position->Current->oo = 11;

    if( !strcmp(i, "Qq") )
        Position->Current->oo = 10;

    if( !strcmp(i, "Kq") )
        Position->Current->oo = 9;

    if( !strcmp(i, "q") )
        Position->Current->oo = 8;

    if( !strcmp(i, "KQk") )
        Position->Current->oo = 7;

    if( !strcmp(i, "Qk") )
        Position->Current->oo = 6;

    if( !strcmp(i, "Kk") )
        Position->Current->oo = 5;

    if( !strcmp(i, "k") )
        Position->Current->oo = 4;

    if( !strcmp(i, "KQ") )
        Position->Current->oo = 3;

    if( !strcmp(i, "Q") )
        Position->Current->oo = 2;

    if( !strcmp(i, "K") )
        Position->Current->oo = 1;

    if( !strcmp(i, "-") )
        Position->Current->oo = 0;

    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    Position->Current->ep = 0;

    if( !strcmp(i, "-") )
        ep = 0;
    else
        {
        ep = (i[0] - 'a') + 8 * (i[1] - '1');

        ok = 0;
        }

    if( ep )
        {
        if( Position->wtm )
            {
            if( FILE(ep) != FA && (Position->sq[ep - 9] == wEnumP) )
                ok = true;

            if( FILE(ep) != FH && (Position->sq[ep - 7] == wEnumP) )
                ok = true;
            }
        else
            {
            if( FILE(ep) != FA && (Position->sq[ep + 7] == bEnumP) )
                ok = true;

            if( FILE(ep) != FH && (Position->sq[ep + 9] == bEnumP) )
                ok = true;
            }

        if( ok )
            Position->Current->ep = ep;
        }
    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    Position->Current->reversible = (uint8)atoi(i);
    I += strlen(i) + 1;
    sscanf(I, "%s", i);
    I += strlen(i) + 1;
    InitBitboards(Position);
    return I;
    }

static uint32 FullMove( typePos *Position, uint32 x )
    {
    int pi, to = To(x), fr = From(x);

    if( !x )
        return x;

    pi = Position->sq[fr];

    if( pi == wEnumK || pi == bEnumK )
        {
        if( to - fr == 2 || fr - to == 2 )
            x |= FlagOO;
        }

    if( To(x) != 0 && To(x) == Position->Current->ep && (pi == wEnumP || pi == bEnumP) )
        x |= FlagEP;
    return x;
    }

static void ReadMoves( typePos *Position, char *I )
    {
    typeMoveList List[512], *list;
    char T[512];
    int i;
    uint32 full;

    while( I[0] )
        {
        Mobility(Position);

        if( PosInCheck )
            {
            list = EvasionMoves(Position, List, 0xffffffffffffffff);
            list++;
            }
        else
            {
            list = CaptureMoves(Position, List, Position->OccupiedBW);
            list = OrdinaryMoves(Position, list);
            }
        full = FullMove(Position, (I[2] - 'a') + ((I[3] - '1') << 3) + ((I[0] - 'a') << 6) + ((I[1] - '1') << 9));
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

        for ( i = 0; i < list - List; i++ )
            {
            if( full == (List[i].move & 0x7fff) )
                {
                Make(Position, full);
                break;
                }
            }

        I += strlen(T) + 1;

        while( I[0] == ' ' )
            I++;
        }
    }

void InitPosition( typePos *Position, char *I )
    {
    char i[1024];
    NodeCheck = 0;
    sscanf(I, "%s", i);

    if( !strcmp(i, "startpos") )
        {
        ReadFEN(Position, StartPosition);
        I += strlen("startpos") + 1;
        }

    if( !strcmp(i, "fen") )
        {
        I += strlen("fen") + 1;
        I = ReadFEN(Position, I);
        }

    if( I[0] )
        {
        sscanf(I, "%s", i);

        if( !strcmp(i, "moves") )
            {
            I += strlen("moves") + 1;
            ReadMoves(Position, I);
            }
        }
    Position->height = 0;

    if( IsNewGame )
        ResetPositionalGain();
    }
