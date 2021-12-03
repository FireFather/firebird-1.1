#include "firebird.h"
#include "pawn_eval.h"

typedef struct
    {
    uint64 EDGE, MIDDLE, CENTER;
    uint8 ShelterEdge[8], ShelterMiddle[8], ShelterCenter[8];
    uint8 StormEdge[8], StormMiddle[8], StormCenter[8], ShelterDiag[8], ZERO, VALU_ZERO;
    } typePAWNptr;

typePAWNptr PAWNptr[8];

const uint8 SHELTERaa[8] =
    {
    30, 0, 5, 15, 20, 25, 25, 25
    };
const uint8 SHELTERab[8] =
    {
    55, 0, 15, 40, 50, 55, 55, 55
    };
const uint8 SHELTERac[8] =
    {
    30, 0, 10, 20, 25, 30, 30, 30
    };
const uint8 STORMaa[8] =
    {
    5, 0, 35, 15, 5, 0, 0, 0
    };
const uint8 STORMab[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 STORMac[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 SHELTERba[8] =
    {
    30, 0, 5, 15, 20, 25, 25, 25
    };
const uint8 SHELTERbb[8] =
    {
    55, 0, 15, 40, 50, 55, 55, 55
    };
const uint8 SHELTERbc[8] =
    {
    30, 0, 10, 20, 25, 30, 30, 30
    };
const uint8 STORMba[8] =
    {
    5, 0, 35, 15, 5, 0, 0, 0
    };
const uint8 STORMbb[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 STORMbc[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 SHELTERcb[8] =
    {
    30, 0, 5, 15, 20, 25, 25, 25
    };
const uint8 SHELTERcc[8] =
    {
    55, 0, 15, 40, 50, 55, 55, 55
    };
const uint8 SHELTERcd[8] =
    {
    30, 0, 10, 20, 25, 30, 30, 30
    };
const uint8 STORMcb[8] =
    {
    5, 0, 35, 15, 5, 0, 0, 0
    };
const uint8 STORMcc[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 STORMcd[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 SHELTERdc[8] =
    {
    30, 0, 5, 15, 20, 25, 25, 25
    };
const uint8 SHELTERdd[8] =
    {
    55, 0, 15, 40, 50, 55, 55, 55
    };
const uint8 SHELTERde[8] =
    {
    30, 0, 10, 20, 25, 30, 30, 30
    };
const uint8 STORMdc[8] =
    {
    5, 0, 35, 15, 5, 0, 0, 0
    };
const uint8 STORMdd[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 STORMde[8] =
    {
    10, 0, 50, 20, 10, 0, 0, 0
    };
const uint8 ShelterLongDiagA[8] =
    {
    10, 0, 2, 4, 6, 8, 10, 10
    };
const uint8 ShelterLongDiagB[8] =
    {
    8, 0, 2, 4, 6, 7, 8, 8
    };
const uint8 ShelterLongDiagC[8] =
    {
    6, 0, 2, 3, 4, 5, 6, 6
    };
const uint8 ShelterLongDiagD[8] =
    {
    4, 0, 1, 2, 3, 4, 4, 4
    };
void InitPawns()
    {
    int co, tr;
    int TARGET[8] =
        {
        FB, FB, FC, FD, FE, FF, FG, FG
        };

    int Switch[8] =
        {
        1, 1, 1, 1, -1, -1, -1, -1
        };

    for ( co = FA; co <= FH; co++ )
        {
        PAWNptr[co].EDGE = FileArray[TARGET[co] - Switch[co]];
        PAWNptr[co].MIDDLE = FileArray[TARGET[co]];
        PAWNptr[co].CENTER = FileArray[TARGET[co] + Switch[co]];
        }

    for ( tr = R1; tr <= R8; tr++ )
        {
        PAWNptr[FA].ShelterEdge[tr] = SHELTERaa[tr];
        PAWNptr[FA].StormEdge[tr] = STORMaa[tr];
        PAWNptr[FA].ShelterMiddle[tr] = SHELTERab[tr];
        PAWNptr[FA].StormMiddle[tr] = STORMab[tr];
        PAWNptr[FA].ShelterCenter[tr] = SHELTERac[tr];
        PAWNptr[FA].StormCenter[tr] = STORMac[tr];
        PAWNptr[FH].ShelterEdge[tr] = SHELTERaa[tr];
        PAWNptr[FH].StormEdge[tr] = STORMaa[tr];
        PAWNptr[FH].ShelterMiddle[tr] = SHELTERab[tr];
        PAWNptr[FH].StormMiddle[tr] = STORMab[tr];
        PAWNptr[FH].ShelterCenter[tr] = SHELTERac[tr];
        PAWNptr[FH].StormCenter[tr] = STORMac[tr];
        PAWNptr[FA].ShelterDiag[tr] = ShelterLongDiagA[tr];
        PAWNptr[FH].ShelterDiag[tr] = ShelterLongDiagA[tr];
        }

    for ( tr = R1; tr <= R8; tr++ )
        {
        PAWNptr[FB].ShelterEdge[tr] = SHELTERba[tr];
        PAWNptr[FB].StormEdge[tr] = STORMba[tr];
        PAWNptr[FB].ShelterMiddle[tr] = SHELTERbb[tr];
        PAWNptr[FB].StormMiddle[tr] = STORMbb[tr];
        PAWNptr[FB].ShelterCenter[tr] = SHELTERbc[tr];
        PAWNptr[FB].StormCenter[tr] = STORMbc[tr];
        PAWNptr[FG].ShelterEdge[tr] = SHELTERba[tr];
        PAWNptr[FG].StormEdge[tr] = STORMba[tr];
        PAWNptr[FG].ShelterMiddle[tr] = SHELTERbb[tr];
        PAWNptr[FG].StormMiddle[tr] = STORMbb[tr];
        PAWNptr[FG].ShelterCenter[tr] = SHELTERbc[tr];
        PAWNptr[FG].StormCenter[tr] = STORMbc[tr];
        PAWNptr[FB].ShelterDiag[tr] = ShelterLongDiagB[tr];
        PAWNptr[FG].ShelterDiag[tr] = ShelterLongDiagB[tr];
        }

    for ( tr = R1; tr <= R8; tr++ )
        {
        PAWNptr[FC].ShelterEdge[tr] = SHELTERcb[tr];
        PAWNptr[FC].StormEdge[tr] = STORMcb[tr];
        PAWNptr[FC].ShelterMiddle[tr] = SHELTERcc[tr];
        PAWNptr[FC].StormMiddle[tr] = STORMcc[tr];
        PAWNptr[FC].ShelterCenter[tr] = SHELTERcd[tr];
        PAWNptr[FC].StormCenter[tr] = STORMcd[tr];
        PAWNptr[FF].ShelterEdge[tr] = SHELTERcb[tr];
        PAWNptr[FF].StormEdge[tr] = STORMcb[tr];
        PAWNptr[FF].ShelterMiddle[tr] = SHELTERcc[tr];
        PAWNptr[FF].StormMiddle[tr] = STORMcc[tr];
        PAWNptr[FF].ShelterCenter[tr] = SHELTERcd[tr];
        PAWNptr[FF].StormCenter[tr] = STORMcd[tr];
        PAWNptr[FC].ShelterDiag[tr] = ShelterLongDiagC[tr];
        PAWNptr[FF].ShelterDiag[tr] = ShelterLongDiagC[tr];
        }

    for ( tr = R1; tr <= R8; tr++ )
        {
        PAWNptr[FD].ShelterEdge[tr] = SHELTERdc[tr];
        PAWNptr[FD].StormEdge[tr] = STORMdc[tr];
        PAWNptr[FD].ShelterMiddle[tr] = SHELTERdd[tr];
        PAWNptr[FD].StormMiddle[tr] = STORMdd[tr];
        PAWNptr[FD].ShelterCenter[tr] = SHELTERde[tr];
        PAWNptr[FD].StormCenter[tr] = STORMde[tr];
        PAWNptr[FE].ShelterEdge[tr] = SHELTERdc[tr];
        PAWNptr[FE].StormEdge[tr] = STORMdc[tr];
        PAWNptr[FE].ShelterMiddle[tr] = SHELTERdd[tr];
        PAWNptr[FE].StormMiddle[tr] = STORMdd[tr];
        PAWNptr[FE].ShelterCenter[tr] = SHELTERde[tr];
        PAWNptr[FE].StormCenter[tr] = STORMde[tr];
        PAWNptr[FD].ShelterDiag[tr] = ShelterLongDiagD[tr];
        PAWNptr[FE].ShelterDiag[tr] = ShelterLongDiagD[tr];
        }

    for ( co = FA; co <= FH; co++ )
        {
        PAWNptr[co].ZERO = PAWNptr[co].ShelterEdge[R2] + PAWNptr[co].ShelterMiddle[R2] + PAWNptr[co].ShelterCenter[R2];
        PAWNptr[co].VALU_ZERO = 10;
        }
    }
static int WhiteKingDanger( typePOS *POSITION, int wKs )
    {
    int e, RankWa, RankWb, RankWc, RankBa, RankBb, RankBc, v, tr = RANK(wKs);
    uint64 T, A = wBitboardP & NotInFrontB[tr];
    typePAWNptr Z = PAWNptr[FILE(wKs)];
    T = A & Z.EDGE;
    RankWa = LSB(T);

    if( !T )
        RankWa = 0;
    RankWa >>= 3;
    T = A & Z.MIDDLE;
    RankWb = LSB(T);

    if( !T )
        RankWb = 0;
    RankWb >>= 3;
    T = A & Z.CENTER;
    RankWc = LSB(T);

    if( !T )
        RankWc = 0;
    RankWc >>= 3;
    T = bBitboardP & Z.EDGE;
    RankBa = LSB(T);

    if( !T )
        RankBa = 0;
    RankBa >>= 3;
    T = bBitboardP & Z.MIDDLE;
    RankBb = LSB(T);

    if( !T )
        RankBb = 0;
    RankBb >>= 3;
    T = bBitboardP & Z.CENTER;
    RankBc = LSB(T);

    if( !T )
        RankBc = 0;
    RankBc >>= 3;
    v = (Z.ShelterEdge)[RankWa]+(Z.ShelterMiddle)[RankWb]+(Z.ShelterCenter)[RankWc];

    if( v == Z.ZERO )
        v = Z.VALU_ZERO;
    T = A & LONG_DIAG[wKs];
    e = LSB(T);

    if( !T )
        e = 0;
    e >>= 3;
    v += (Z.ShelterDiag)[e];
    e = (Z.StormEdge)[RankBa];

    if( RankBa == (RankWa + 1) )
        e >>= 1;
    v += e;
    e = (Z.StormMiddle)[RankBb];

    if( RankBb == (RankWb + 1) )
        e >>= 1;
    v += e;
    e = (Z.StormCenter)[RankBc];

    if( RankBc == (RankWc + 1) )
        e >>= 1;
    v += e;
    return v;
    }
static int BlackKingDanger( typePOS *POSITION, int bKs )
    {
    int e, RankWa, RankWb, RankWc, RankBa, RankBb, RankBc, v, tr = RANK(bKs);
    uint64 T, A = bBitboardP & NotInFrontW[tr];
    typePAWNptr Z = PAWNptr[FILE(bKs)];
    T = A & Z.EDGE;
    RankBa = MSB(T);

    if( !T )
        RankBa = 56;
    RankBa >>= 3;
    RankBa = 7 - RankBa;
    T = A & Z.MIDDLE;
    RankBb = MSB(T);

    if( !T )
        RankBb = 56;
    RankBb >>= 3;
    RankBb = 7 - RankBb;
    T = A & Z.CENTER;
    RankBc = MSB(T);

    if( !T )
        RankBc = 56;
    RankBc >>= 3;
    RankBc = 7 - RankBc;
    T = wBitboardP & Z.EDGE;
    RankWa = MSB(T);

    if( !T )
        RankWa = 56;
    RankWa >>= 3;
    RankWa = 7 - RankWa;
    T = wBitboardP & Z.MIDDLE;
    RankWb = MSB(T);

    if( !T )
        RankWb = 56;
    RankWb >>= 3;
    RankWb = 7 - RankWb;
    T = wBitboardP & Z.CENTER;
    RankWc = MSB(T);

    if( !T )
        RankWc = 56;
    RankWc >>= 3;
    RankWc = 7 - RankWc;
    v = (Z.ShelterEdge)[RankBa]+(Z.ShelterMiddle)[RankBb]+(Z.ShelterCenter)[RankBc];

    if( v == Z.ZERO )
        v = Z.VALU_ZERO;
    T = A & LONG_DIAG[bKs];
    e = MSB(T);

    if( !T )
        e = 56;
    e >>= 3;
    e = 7 - e;
    v += (Z.ShelterDiag)[e];
    e = (Z.StormEdge)[RankWa];

    if( RankWa == (RankBa + 1) )
        e >>= 1;
    v += e;
    e = (Z.StormMiddle)[RankWb];

    if( RankWb == (RankBb + 1) )
        e >>= 1;
    v += e;
    e = (Z.StormCenter)[RankWc];

    if( RankWc == (RankBc + 1) )
        e >>= 1;
    v += e;
    return v;
    }

void PawnEval( typePOS *POSITION, typePawnEval *RESULT )
    {
    int c, Value = 0, B, DistanceWhiteKing, DistanceBlackKing, BestWhiteKingDistance, BestBlackKingDistance;
    int wKs = POSITION->wKsq, bKs = POSITION->bKsq;
    int b, tr, co, v, ValuePassedPawn;
    uint64 T, U, V, CONNECTED;
    typePawnEval *ptr;

    RESULT->wPlight = RESULT->bPlight = RESULT->wPdark = RESULT->bPdark = 0;
    RESULT->wKdanger = RESULT->bKdanger = 0;
    RESULT->wPassedFiles = RESULT->bPassedFiles = 0;
    BestBlackKingDistance = BestWhiteKingDistance = 30000;
    CONNECTED = 0;

    c = 0;

    for ( co = FA; co <= FH; co++ )
        {
        if( (wBitboardP &FileArray[co]) == 0 )
            c = 0;
        else
            {
            if( c == 0 )
                {
                Value -= Islands;
                }
            c = 1;
            }
        }

    T = wBitboardP;

    while( T )
        {
        b = LSB(T);
        BitClear(b, T);
        tr = RANK(b);
        co = FILE(b);

        DistanceWhiteKing = WhiteKingPawnDistance(b, wKs);

        if( DistanceWhiteKing < BestWhiteKingDistance )
            BestWhiteKingDistance = DistanceWhiteKing;
        DistanceBlackKing = WhiteKingPawnDistance(b, bKs);

        if( DistanceBlackKing < BestBlackKingDistance )
            BestBlackKingDistance = DistanceBlackKing;

        if( SqSet[b] & LIGHT )
            {
            RESULT->wPlight += BlockedPawnValue[b];

            if( POSITION->sq[b + 8] == bEnumP )
                RESULT->wPlight += BlockedPawnValue[b];
            }
        else
            {
            RESULT->wPdark += BlockedPawnValue[b];

            if( POSITION->sq[b + 8] == bEnumP )
                RESULT->wPdark += BlockedPawnValue[b];
            }

        if( wBitboardP & LEFT2[b] && (wBitboardP &InFrontW[tr - 1]&FileArray[co - 1]) == 0 )
            {
            Value -= Hole;
            }

        if( (wBitboardP | bBitboardP) & OpenFileW[b] )
            {
            if( wBitboardP & DOUBLED[b] )
                {
                Value -= DoubledClosed;

                if( (wBitboardP &IsolatedFiles[co]) == 0 )
                    {
                    Value -= DoubledClosedIsolated;
                    }
                }

            if( (wBitboardP &IsolatedFiles[co]) == 0 )
                {
                Value -= IsolatedClosed;
                continue;
                }

            if( (wBitboardP &ProtectedPawnW[b]) == 0 )
                {
                B = b + 8;

                if( (wBitboardP &AttPb[b]) == 0 )
                    {
                    B += 8;

                    if( (wBitboardP &AttPb[b + 8]) == 0 )
                        B += 8;
                    }

                if( bBitboardP & AttPb[B] )
                    {
                    Value -= BackwardClosed;
                    }
                }
            continue;
            }

        if( wBitboardP & DOUBLED[b] )
            {
            Value -= DoubledOpen;

            if( (wBitboardP &IsolatedFiles[co]) == 0 )
                {
                Value -= DoubledOpenIsolated;
                }
            }

        if( (wBitboardP &IsolatedFiles[co]) == 0 )
            {
            Value -= IsolatedOpen;
            }
        else
            {
            if( (wBitboardP &ProtectedPawnW[b]) == 0 )
                {
                B = b + 8;

                if( (wBitboardP &AttPb[b]) == 0 )
                    {
                    B += 8;

                    if( (wBitboardP &AttPb[b + 8]) == 0 )
                        B += 8;
                    }

                if( bBitboardP & AttPb[B] )
                    {
                    Value -= BackwardOpen;
                    }
                }
            }

        if( (bBitboardP &PassedPawnW[b]) == 0 )
            goto PassedW;

        if( bBitboardP & PassedPawnW[b] & ~AttPb[b] )
            {
            Value += CandidatePawnValue[tr];
            continue;
            }

        if( POPCNT(AttPb[b] & bBitboardP) > POPCNT(AttPw[b] & wBitboardP) )
            {
            Value += CandidatePawnValue[tr];
            continue;
            }
        PassedW:
        ValuePassedPawn = PassedPawnValue[tr];

        if( wBitboardP & AttPw[b] )
            ValuePassedPawn += ProtectedPassedPawnValue[tr];

        if( (bBitboardP &FilesLeft[co]) == 0 || (bBitboardP &FilesRight[co]) == 0 )
            ValuePassedPawn += OutsidePassedPawnValue[tr];

        V = ConnectedPawns[b] & CONNECTED;
        CONNECTED |= SqSet[b];

        if( V )
            {
            ValuePassedPawn += ConnectedPassedPawnValue[tr] + ConnectedPassedPawnValue[RANK(LSB(V))];
            BitClear(0, V);

            if( V )
                ValuePassedPawn += ConnectedPassedPawnValue[tr] + ConnectedPassedPawnValue[RANK(LSB(V))];
            }
        Value += ValuePassedPawn;
        RESULT->wPassedFiles |= (uint8)(1 << co);

        if( b <= H3 )
            {
            continue;
            }
        Value += (WhiteKingPawnDistance(b + 8, bKs) * OppKingPawnDistance[RANK(b)]);
        Value -= (WhiteKingPawnDistance(b + 8, wKs) * MyKingPawnDistance[RANK(b)]);
        }

    c = 0;

    for ( co = FA; co <= FH; co++ )
        {
        if( (bBitboardP &FileArray[co]) == 0 )
            c = 0;
        else
            {
            if( c == 0 )
                {
                Value += Islands;
                }
            c = 1;
            }
        }

    CONNECTED = 0;
    T = bBitboardP;

    while( T )
        {
        b = LSB(T);
        BitClear(b, T);
        tr = RANK(b);
        co = FILE(b);

        DistanceBlackKing = BlackKingPawnDistance(b, bKs);

        if( DistanceBlackKing < BestBlackKingDistance )
            BestBlackKingDistance = DistanceBlackKing;
        DistanceWhiteKing = BlackKingPawnDistance(b, wKs);

        if( DistanceWhiteKing < BestWhiteKingDistance )
            BestWhiteKingDistance = DistanceWhiteKing;

        if( SqSet[b] & LIGHT )
            {
            RESULT->bPlight += BlockedPawnValue[b];

            if( POSITION->sq[b - 8] == wEnumP )
                RESULT->bPlight += BlockedPawnValue[b];
            }
        else
            {
            RESULT->bPdark += BlockedPawnValue[b];

            if( POSITION->sq[b - 8] == wEnumP )
                RESULT->bPdark += BlockedPawnValue[b];
            }

        if( bBitboardP & LEFT2[b] && (bBitboardP &InFrontB[tr + 1]&FileArray[co - 1]) == 0 )
            {
            Value += Hole;
            }

        if( (wBitboardP | bBitboardP) & OpenFileB[b] )
            {
            if( bBitboardP & DOUBLED[b] )
                {
                Value += DoubledClosed;

                if( (bBitboardP &IsolatedFiles[co]) == 0 )
                    {
                    Value += DoubledClosedIsolated;
                    }
                }

            if( (bBitboardP &IsolatedFiles[co]) == 0 )
                {
                Value += IsolatedClosed;
                continue;
                }

            if( (bBitboardP &ProtectedPawnB[b]) == 0 )
                {
                B = b - 8;

                if( (bBitboardP &AttPw[b]) == 0 )
                    {
                    B -= 8;

                    if( (bBitboardP &AttPw[b - 8]) == 0 )
                        B -= 8;
                    }

                if( wBitboardP & AttPw[B] )
                    {
                    Value += BackwardClosed;
                    }
                }
            continue;
            }

        if( bBitboardP & DOUBLED[b] )
            {
            Value += DoubledOpen;

            if( (bBitboardP &IsolatedFiles[co]) == 0 )
                {
                Value += DoubledOpenIsolated;
                }
            }

        if( (bBitboardP &IsolatedFiles[co]) == 0 )
            {
            Value += IsolatedOpen;
            }
        else
            {
            if( (bBitboardP &ProtectedPawnB[b]) == 0 )
                {
                B = b - 8;

                if( (bBitboardP &AttPw[b]) == 0 )
                    {
                    B -= 8;

                    if( (bBitboardP &AttPw[b - 8]) == 0 )
                        B -= 8;
                    }

                if( wBitboardP & AttPw[B] )
                    {
                    Value += BackwardOpen;
                    }
                }
            }

        if( (wBitboardP &PassedPawnB[b]) == 0 )
            goto PassedB;

        if( wBitboardP & PassedPawnB[b] & ~AttPw[b] )
            {
            Value -= CandidatePawnValue[7 - tr];
            continue;
            }

        if( POPCNT(AttPw[b] & wBitboardP) > POPCNT(AttPb[b] & bBitboardP) )
            {
            Value -= CandidatePawnValue[7 - tr];
            continue;
            }
        PassedB:
        ValuePassedPawn = PassedPawnValue[7 - tr];

        if( bBitboardP & AttPb[b] )
            ValuePassedPawn += ProtectedPassedPawnValue[7 - tr];

        if( (wBitboardP &FilesLeft[co]) == 0 || (wBitboardP &FilesRight[co]) == 0 )
            ValuePassedPawn += OutsidePassedPawnValue[7 - tr];

        V = ConnectedPawns[b] & CONNECTED;
        CONNECTED |= SqSet[b];

        if( V )
            {
            ValuePassedPawn += ConnectedPassedPawnValue[7 - tr] + ConnectedPassedPawnValue[7 - (LSB(V) >> 3)];
            BitClear(0, V);

            if( V )
                ValuePassedPawn += ConnectedPassedPawnValue[7 - tr] + ConnectedPassedPawnValue[7 - (LSB(V) >> 3)];
            }

        Value -= ValuePassedPawn;
        RESULT->bPassedFiles |= (uint8)(1 << co);

        if( b >= A6 )
            {
            continue;
            }
        Value -= (BlackKingPawnDistance(b - 8, wKs) * OppKingPawnDistance[R8 - RANK(b)]);
        Value += (BlackKingPawnDistance(b - 8, bKs) * MyKingPawnDistance[R8 - RANK(b)]);
        }

    T = 0;

    for ( tr = R2; tr <= R7; tr++ )
        T |= ((wBitboardP >> (8 * tr)) & 0xff);
    U = 0;

    for ( tr = R2; tr <= R7; tr++ )
        U |= ((bBitboardP >> (8 * tr)) & 0xff);
    RESULT->wPfile_count = POPCNT(T);
    RESULT->bPfile_count = POPCNT(U);
    RESULT->OpenFileCount = 8 - POPCNT(T | U);
    RESULT->wDrawWeight = OpposingPawnsMult[POPCNT(T & ~U)] * PawnCountMult[RESULT->wPfile_count];
    RESULT->bDrawWeight = OpposingPawnsMult[POPCNT(U & ~T)] * PawnCountMult[RESULT->bPfile_count];

    if( wBitboardP | bBitboardP )
        {
        Value += BestBlackKingDistance - BestWhiteKingDistance;
        }

    T = ((bBitboardP &( ~FILEa)) >> 9) | ((bBitboardP &( ~FILEh)) >> 7);

    if( ( ~T) &AttK[wKs] & bBitboardP )
        {
        Value += KingAttPawn;
        }

    if( POSITION->DYN->oo & 1 )
        {
        Value += KingOO;
        }

    if( POSITION->DYN->oo & 2 )
        {
        Value += KingOOO;
        }

    T = ((wBitboardP &( ~FILEa)) << 7) | ((wBitboardP &( ~FILEh)) << 9);

    if( ( ~T) &AttK[bKs] & wBitboardP )
        {
        Value -= KingAttPawn;
        }

    if( POSITION->DYN->oo & 4 )
        {
        Value -= KingOO;
        }

    if( POSITION->DYN->oo & 8 )
        {
        Value -= KingOOO;
        }

    RESULT->SCORE = Value;
    v = WhiteKingDanger(POSITION, wKs);

    if( WhiteOO )
        v = MIN(v, 5 + WhiteKingDanger(POSITION, G1));

    if( WhiteOOO )
        v = MIN(v, 5 + WhiteKingDanger(POSITION, C1));
    RESULT->wKdanger = SCORE(v, 0);
    v = BlackKingDanger(POSITION, bKs);

    if( BlackOO )
        v = MIN(v, 5 + BlackKingDanger(POSITION, G8));

    if( BlackOOO )
        v = MIN(v, 5 + BlackKingDanger(POSITION, C8));
    RESULT->bKdanger = SCORE(v, 0);
    RESULT->PAWN_HASH = POSITION->DYN->PAWN_HASH;
    RESULT->PAWN_HASH ^= (((uint64 *)(RESULT)) + 0x1)[0];
    RESULT->PAWN_HASH ^= (((uint64 *)(RESULT)) + 0x2)[0];
    RESULT->PAWN_HASH ^= (((uint64 *)(RESULT)) + 0x3)[0];

    ptr = PawnHash + (POSITION->DYN->PAWN_HASH &(PawnHashSize - 1));
    memcpy(ptr, RESULT, sizeof(typePawnEval));

    RESULT->PAWN_HASH ^= (((uint64 *)(RESULT)) + 0x1)[0];
    RESULT->PAWN_HASH ^= (((uint64 *)(RESULT)) + 0x2)[0];
    RESULT->PAWN_HASH ^= (((uint64 *)(RESULT)) + 0x3)[0];

    return;
    }
