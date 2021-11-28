#include "firebird.h"
#include "pawn_eval.h"

void InitPawns()
    {
    int file, rank;
    int TARGET[8] =
        {
        FB, FB, FC, FD, FE, FF, FG, FG
        };

    int Switch[8] =
        {
        1, 1, 1, 1, -1, -1, -1, -1
        };

    for ( file = FA; file <= FH; file++ )
        {
        PawnPtr[file].Edge = FileArray[TARGET[file] - Switch[file]];
        PawnPtr[file].Middle = FileArray[TARGET[file]];
        PawnPtr[file].Center = FileArray[TARGET[file] + Switch[file]];
        }

    for ( rank = R1; rank <= R8; rank++ )
        {
        PawnPtr[FA].ShelterEdge[rank] = Shelter_aa[rank];
        PawnPtr[FA].StormEdge[rank] = Storm_aa[rank];
        PawnPtr[FA].ShelterMiddle[rank] = Shelter_ab[rank];
        PawnPtr[FA].StormMiddle[rank] = Storm_ab[rank];
        PawnPtr[FA].ShelterCenter[rank] = Shelter_ac[rank];
        PawnPtr[FA].StormCenter[rank] = Storm_ac[rank];
        PawnPtr[FH].ShelterEdge[rank] = Shelter_aa[rank];
        PawnPtr[FH].StormEdge[rank] = Storm_aa[rank];
        PawnPtr[FH].ShelterMiddle[rank] = Shelter_ab[rank];
        PawnPtr[FH].StormMiddle[rank] = Storm_ab[rank];
        PawnPtr[FH].ShelterCenter[rank] = Shelter_ac[rank];
        PawnPtr[FH].StormCenter[rank] = Storm_ac[rank];
        PawnPtr[FA].ShelterDiag[rank] = ShelterLongDiagA[rank];
        PawnPtr[FH].ShelterDiag[rank] = ShelterLongDiagA[rank];
        }

    for ( rank = R1; rank <= R8; rank++ )
        {
        PawnPtr[FB].ShelterEdge[rank] = Shelter_ba[rank];
        PawnPtr[FB].StormEdge[rank] = Storm_ba[rank];
        PawnPtr[FB].ShelterMiddle[rank] = Shelter_bb[rank];
        PawnPtr[FB].StormMiddle[rank] = Storm_bb[rank];
        PawnPtr[FB].ShelterCenter[rank] = Shelter_bc[rank];
        PawnPtr[FB].StormCenter[rank] = Storm_bc[rank];
        PawnPtr[FG].ShelterEdge[rank] = Shelter_ba[rank];
        PawnPtr[FG].StormEdge[rank] = Storm_ba[rank];
        PawnPtr[FG].ShelterMiddle[rank] = Shelter_bb[rank];
        PawnPtr[FG].StormMiddle[rank] = Storm_bb[rank];
        PawnPtr[FG].ShelterCenter[rank] = Shelter_bc[rank];
        PawnPtr[FG].StormCenter[rank] = Storm_bc[rank];
        PawnPtr[FB].ShelterDiag[rank] = ShelterLongDiagB[rank];
        PawnPtr[FG].ShelterDiag[rank] = ShelterLongDiagB[rank];
        }

    for ( rank = R1; rank <= R8; rank++ )
        {
        PawnPtr[FC].ShelterEdge[rank] = Shelter_cb[rank];
        PawnPtr[FC].StormEdge[rank] = Storm_cb[rank];
        PawnPtr[FC].ShelterMiddle[rank] = Shelter_cc[rank];
        PawnPtr[FC].StormMiddle[rank] = Storm_cc[rank];
        PawnPtr[FC].ShelterCenter[rank] = Shelter_cd[rank];
        PawnPtr[FC].StormCenter[rank] = Storm_cd[rank];
        PawnPtr[FF].ShelterEdge[rank] = Shelter_cb[rank];
        PawnPtr[FF].StormEdge[rank] = Storm_cb[rank];
        PawnPtr[FF].ShelterMiddle[rank] = Shelter_cc[rank];
        PawnPtr[FF].StormMiddle[rank] = Storm_cc[rank];
        PawnPtr[FF].ShelterCenter[rank] = Shelter_cd[rank];
        PawnPtr[FF].StormCenter[rank] = Storm_cd[rank];
        PawnPtr[FC].ShelterDiag[rank] = ShelterLongDiagC[rank];
        PawnPtr[FF].ShelterDiag[rank] = ShelterLongDiagC[rank];
        }

    for ( rank = R1; rank <= R8; rank++ )
        {
        PawnPtr[FD].ShelterEdge[rank] = Shelter_dc[rank];
        PawnPtr[FD].StormEdge[rank] = Storm_dc[rank];
        PawnPtr[FD].ShelterMiddle[rank] = Shelter_dd[rank];
        PawnPtr[FD].StormMiddle[rank] = Storm_dd[rank];
        PawnPtr[FD].ShelterCenter[rank] = Shelter_de[rank];
        PawnPtr[FD].StormCenter[rank] = Storm_de[rank];
        PawnPtr[FE].ShelterEdge[rank] = Shelter_dc[rank];
        PawnPtr[FE].StormEdge[rank] = Storm_dc[rank];
        PawnPtr[FE].ShelterMiddle[rank] = Shelter_dd[rank];
        PawnPtr[FE].StormMiddle[rank] = Storm_dd[rank];
        PawnPtr[FE].ShelterCenter[rank] = Shelter_de[rank];
        PawnPtr[FE].StormCenter[rank] = Storm_de[rank];
        PawnPtr[FD].ShelterDiag[rank] = ShelterLongDiagD[rank];
        PawnPtr[FE].ShelterDiag[rank] = ShelterLongDiagD[rank];
        }

    for ( file = FA; file <= FH; file++ )
        {
        PawnPtr[file].ZERO = PawnPtr[file].ShelterEdge[R2] + PawnPtr[file].ShelterMiddle[R2] + PawnPtr[file].ShelterCenter[R2];
        PawnPtr[file].VALU_ZERO = 10;
        }
    }
static int WhiteKingDanger( typePos *Position, int wKs )
    {
    int e, RankWa, RankWb, RankWc, RankBa, RankBb, RankBc, v, rank = RANK(wKs);
    uint64 T, A = wBitboardP & NotInFrontB[rank];
    typePawnPtr Z = PawnPtr[FILE(wKs)];
    T = A & Z.Edge;
    RankWa = LSB(T);

    if( !T )
        RankWa = 0;
    RankWa >>= 3;
    T = A & Z.Middle;
    RankWb = LSB(T);

    if( !T )
        RankWb = 0;
    RankWb >>= 3;
    T = A & Z.Center;
    RankWc = LSB(T);

    if( !T )
        RankWc = 0;
    RankWc >>= 3;
    T = bBitboardP & Z.Edge;
    RankBa = LSB(T);

    if( !T )
        RankBa = 0;
    RankBa >>= 3;
    T = bBitboardP & Z.Middle;
    RankBb = LSB(T);

    if( !T )
        RankBb = 0;
    RankBb >>= 3;
    T = bBitboardP & Z.Center;
    RankBc = LSB(T);

    if( !T )
        RankBc = 0;
    RankBc >>= 3;
    v = (Z.ShelterEdge)[RankWa]+(Z.ShelterMiddle)[RankWb]+(Z.ShelterCenter)[RankWc];

    if( v == Z.ZERO )
        v = Z.VALU_ZERO;
    T = A & LongDiag[wKs];
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
static int BlackKingDanger( typePos *Position, int bKs )
    {
    int e, RankWa, RankWb, RankWc, RankBa, RankBb, RankBc, v, rank = RANK(bKs);
    uint64 T, A = bBitboardP & NotInFrontW[rank];
    typePawnPtr Z = PawnPtr[FILE(bKs)];
    T = A & Z.Edge;
    RankBa = MSB(T);

    if( !T )
        RankBa = 56;
    RankBa >>= 3;
    RankBa = 7 - RankBa;
    T = A & Z.Middle;
    RankBb = MSB(T);

    if( !T )
        RankBb = 56;
    RankBb >>= 3;
    RankBb = 7 - RankBb;
    T = A & Z.Center;
    RankBc = MSB(T);

    if( !T )
        RankBc = 56;
    RankBc >>= 3;
    RankBc = 7 - RankBc;
    T = wBitboardP & Z.Edge;
    RankWa = MSB(T);

    if( !T )
        RankWa = 56;
    RankWa >>= 3;
    RankWa = 7 - RankWa;
    T = wBitboardP & Z.Middle;
    RankWb = MSB(T);

    if( !T )
        RankWb = 56;
    RankWb >>= 3;
    RankWb = 7 - RankWb;
    T = wBitboardP & Z.Center;
    RankWc = MSB(T);

    if( !T )
        RankWc = 56;
    RankWc >>= 3;
    RankWc = 7 - RankWc;
    v = (Z.ShelterEdge)[RankBa]+(Z.ShelterMiddle)[RankBb]+(Z.ShelterCenter)[RankBc];

    if( v == Z.ZERO )
        v = Z.VALU_ZERO;
    T = A & LongDiag[bKs];
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

void PawnEval( typePos *Position, typePawnEval *RESULT )
    {
    int c, Value = 0, B, DistanceWhiteKing, DistanceBlackKing, BestWhiteKingDistance, BestBlackKingDistance;
    int wKs = Position->wKsq, bKs = Position->bKsq;
    int b, rank, file, v, ValuePassedPawn;
    uint64 T, U, V, CONNECTED;
    typePawnEval *Ptr;

    RESULT->wPlight = RESULT->bPlight = RESULT->wPdark = RESULT->bPdark = 0;
    RESULT->wKdanger = RESULT->bKdanger = 0;
    RESULT->wPassedFiles = RESULT->bPassedFiles = 0;
    BestBlackKingDistance = BestWhiteKingDistance = 30000;
    CONNECTED = 0;

    c = 0;

    for ( file = FA; file <= FH; file++ )
        {
        if( (wBitboardP &FileArray[file]) == 0 )
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
        rank = RANK(b);
        file = FILE(b);

        DistanceWhiteKing = WhiteKingPawnDistance(b, wKs);

        if( DistanceWhiteKing < BestWhiteKingDistance )
            BestWhiteKingDistance = DistanceWhiteKing;
        DistanceBlackKing = WhiteKingPawnDistance(b, bKs);

        if( DistanceBlackKing < BestBlackKingDistance )
            BestBlackKingDistance = DistanceBlackKing;

        if( SqSet[b] & White )
            {
            RESULT->wPlight += BlockedPawnValue[b];

            if( Position->sq[b + 8] == bEnumP )
                RESULT->wPlight += BlockedPawnValue[b];
            }
        else
            {
            RESULT->wPdark += BlockedPawnValue[b];

            if( Position->sq[b + 8] == bEnumP )
                RESULT->wPdark += BlockedPawnValue[b];
            }

        if( wBitboardP & Left2[b] && (wBitboardP &InFrontW[rank - 1]&FileArray[file - 1]) == 0 )
            {
            Value -= Hole;
            }

        if( (wBitboardP | bBitboardP) & OpenFileW[b] )
            {
            if( wBitboardP & Doubled[b] )
                {
                Value -= DoubledClosed;

                if( (wBitboardP &IsolatedFiles[file]) == 0 )
                    {
                    Value -= DoubledClosedIsolated;
                    }
                }

            if( (wBitboardP &IsolatedFiles[file]) == 0 )
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

        if( wBitboardP & Doubled[b] )
            {
            Value -= DoubledOpen;

            if( (wBitboardP &IsolatedFiles[file]) == 0 )
                {
                Value -= DoubledOpenIsolated;
                }
            }

        if( (wBitboardP &IsolatedFiles[file]) == 0 )
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
            Value += CandidatePawnValue[rank];
            continue;
            }

        if( POPCNT(AttPb[b] & bBitboardP) > POPCNT(AttPw[b] & wBitboardP) )
            {
            Value += CandidatePawnValue[rank];
            continue;
            }
        PassedW:
        ValuePassedPawn = PassedPawnValue[rank];

        if( wBitboardP & AttPw[b] )
            ValuePassedPawn += ProtectedPassedPawnValue[rank];

        if( (bBitboardP &FilesLeft[file]) == 0 || (bBitboardP &FilesRight[file]) == 0 )
            ValuePassedPawn += OutsidePassedPawnValue[rank];

        V = ConnectedPawns[b] & CONNECTED;
        CONNECTED |= SqSet[b];

        if( V )
            {
            ValuePassedPawn += ConnectedPassedPawnValue[rank] + ConnectedPassedPawnValue[RANK(LSB(V))];
            BitClear(0, V);

            if( V )
                ValuePassedPawn += ConnectedPassedPawnValue[rank] + ConnectedPassedPawnValue[RANK(LSB(V))];
            }
        Value += ValuePassedPawn;
        RESULT->wPassedFiles |= (uint8)(1 << file);

        if( b <= H3 )
            {
            continue;
            }
        Value += (WhiteKingPawnDistance(b + 8, bKs) * OppKingPawnDistance[RANK(b)]);
        Value -= (WhiteKingPawnDistance(b + 8, wKs) * MyKingPawnDistance[RANK(b)]);
        }

    c = 0;

    for ( file = FA; file <= FH; file++ )
        {
        if( (bBitboardP &FileArray[file]) == 0 )
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
        rank = RANK(b);
        file = FILE(b);

        DistanceBlackKing = BlackKingPawnDistance(b, bKs);

        if( DistanceBlackKing < BestBlackKingDistance )
            BestBlackKingDistance = DistanceBlackKing;
        DistanceWhiteKing = BlackKingPawnDistance(b, wKs);

        if( DistanceWhiteKing < BestWhiteKingDistance )
            BestWhiteKingDistance = DistanceWhiteKing;

        if( SqSet[b] & White )
            {
            RESULT->bPlight += BlockedPawnValue[b];

            if( Position->sq[b - 8] == wEnumP )
                RESULT->bPlight += BlockedPawnValue[b];
            }
        else
            {
            RESULT->bPdark += BlockedPawnValue[b];

            if( Position->sq[b - 8] == wEnumP )
                RESULT->bPdark += BlockedPawnValue[b];
            }

        if( bBitboardP & Left2[b] && (bBitboardP &InFrontB[rank + 1]&FileArray[file - 1]) == 0 )
            {
            Value += Hole;
            }

        if( (wBitboardP | bBitboardP) & OpenFileB[b] )
            {
            if( bBitboardP & Doubled[b] )
                {
                Value += DoubledClosed;

                if( (bBitboardP &IsolatedFiles[file]) == 0 )
                    {
                    Value += DoubledClosedIsolated;
                    }
                }

            if( (bBitboardP &IsolatedFiles[file]) == 0 )
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

        if( bBitboardP & Doubled[b] )
            {
            Value += DoubledOpen;

            if( (bBitboardP &IsolatedFiles[file]) == 0 )
                {
                Value += DoubledOpenIsolated;
                }
            }

        if( (bBitboardP &IsolatedFiles[file]) == 0 )
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
            Value -= CandidatePawnValue[7 - rank];
            continue;
            }

        if( POPCNT(AttPw[b] & wBitboardP) > POPCNT(AttPb[b] & bBitboardP) )
            {
            Value -= CandidatePawnValue[7 - rank];
            continue;
            }
        PassedB:
        ValuePassedPawn = PassedPawnValue[7 - rank];

        if( bBitboardP & AttPb[b] )
            ValuePassedPawn += ProtectedPassedPawnValue[7 - rank];

        if( (wBitboardP &FilesLeft[file]) == 0 || (wBitboardP &FilesRight[file]) == 0 )
            ValuePassedPawn += OutsidePassedPawnValue[7 - rank];

        V = ConnectedPawns[b] & CONNECTED;
        CONNECTED |= SqSet[b];

        if( V )
            {
            ValuePassedPawn += ConnectedPassedPawnValue[7 - rank] + ConnectedPassedPawnValue[7 - (LSB(V) >> 3)];
            BitClear(0, V);

            if( V )
                ValuePassedPawn += ConnectedPassedPawnValue[7 - rank] + ConnectedPassedPawnValue[7 - (LSB(V) >> 3)];
            }

        Value -= ValuePassedPawn;
        RESULT->bPassedFiles |= (uint8)(1 << file);

        if( b >= A6 )
            {
            continue;
            }
        Value -= (BlackKingPawnDistance(b - 8, wKs) * OppKingPawnDistance[R8 - RANK(b)]);
        Value += (BlackKingPawnDistance(b - 8, bKs) * MyKingPawnDistance[R8 - RANK(b)]);
        }

    T = 0;

    for ( rank = R2; rank <= R7; rank++ )
        T |= ((wBitboardP >> (8 * rank)) & 0xff);
    U = 0;

    for ( rank = R2; rank <= R7; rank++ )
        U |= ((bBitboardP >> (8 * rank)) & 0xff);
    RESULT->wPfile_count = POPCNT(T);
    RESULT->bPfile_count = POPCNT(U);
    RESULT->OpenFileCount = 8 - POPCNT(T | U);
    RESULT->wDrawWeight = OpposingPawnsMult[POPCNT(T & ~U)] * PawnCountMult[RESULT->wPfile_count];
    RESULT->bDrawWeight = OpposingPawnsMult[POPCNT(U & ~T)] * PawnCountMult[RESULT->bPfile_count];

    if( wBitboardP | bBitboardP )
        {
        Value += BestBlackKingDistance - BestWhiteKingDistance;
        }

    T = ((bBitboardP &( ~FileA)) >> 9) | ((bBitboardP &( ~FileH)) >> 7);

    if( ( ~T) &AttK[wKs] & bBitboardP )
        {
        Value += KingAttPawn;
        }

    if( Position->Current->oo & 1 )
        {
        Value += KingOO;
        }

    if( Position->Current->oo & 2 )
        {
        Value += KingOOO;
        }

    T = ((wBitboardP &( ~FileA)) << 7) | ((wBitboardP &( ~FileH)) << 9);

    if( ( ~T) &AttK[bKs] & wBitboardP )
        {
        Value -= KingAttPawn;
        }

    if( Position->Current->oo & 4 )
        {
        Value -= KingOO;
        }

    if( Position->Current->oo & 8 )
        {
        Value -= KingOOO;
        }

    RESULT->score = Value;
    v = WhiteKingDanger(Position, wKs);

    if( WhiteOO )
        v = MIN(v, 5 + WhiteKingDanger(Position, G1));

    if( WhiteOOO )
        v = MIN(v, 5 + WhiteKingDanger(Position, C1));
    RESULT->wKdanger = Score(v, 0);
    v = BlackKingDanger(Position, bKs);

    if( BlackOO )
        v = MIN(v, 5 + BlackKingDanger(Position, G8));

    if( BlackOOO )
        v = MIN(v, 5 + BlackKingDanger(Position, C8));
    RESULT->bKdanger = Score(v, 0);
    RESULT->PawnHash = Position->Current->PawnHash;
    RESULT->PawnHash ^= (((uint64 *)(RESULT)) + 0x1)[0];
    RESULT->PawnHash ^= (((uint64 *)(RESULT)) + 0x2)[0];
    RESULT->PawnHash ^= (((uint64 *)(RESULT)) + 0x3)[0];

    Ptr = PawnHash + (Position->Current->PawnHash &(PawnHashSize - 1));
    memcpy(Ptr, RESULT, sizeof(typePawnEval));

    RESULT->PawnHash ^= (((uint64 *)(RESULT)) + 0x1)[0];
    RESULT->PawnHash ^= (((uint64 *)(RESULT)) + 0x2)[0];
    RESULT->PawnHash ^= (((uint64 *)(RESULT)) + 0x3)[0];

    return;
    }
