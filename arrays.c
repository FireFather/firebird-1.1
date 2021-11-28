#include "firebird.h"

void InitZobrist()
    {
    int i, j;
    ZobristWTM = GetRand();
    ZobristCastling[0] = 0;
    ZobristCastling[1] = GetRand();
    ZobristCastling[2] = GetRand();
    ZobristCastling[4] = GetRand();
    ZobristCastling[8] = GetRand();

    for ( i = 0; i < 16; i++ )
        {
        if( POPCNT(i) < 2 )
            continue;
        ZobristCastling[i] = 0;

        for ( j = 1; j < 16; j <<= 1 )
            if( i & j )
                ZobristCastling[i] ^= ZobristCastling[j];
        }

    for ( i = 0; i < 16; i++ )
        for ( j = A1; j <= H8; j++ )
            Hash[i][j] = GetRand();

    for ( i = FA; i <= FH; i++ )
        ZobristEP[i] = GetRand();
    InitRandom32(GetRand());
    }

void InitArrays()
    {
    int sq2, l, w, i, sq = 0, j, u, file, rank, king, dir;
    uint64 T, b, s;

    for ( i = A1; i <= H8; i++ )
        {
        ShiftLeft45[i] = Shift[Left45[i]];
        ShiftRight45[i] = Shift[Right45[i]];
        }

    for ( i = A1; i <= H8; i++ )
        {
        ShiftAttack[i] = 1 + (i & 56);
        ShiftLeft90[i] = 1 + (Left90[i] & 56);
        }

    for ( i = 1; i <= 8; i++ )
        for ( j = 1; j <= i; j++ )
            {
            Length[sq] = i;
            Where[sq++] = j - 1;
            }

    for ( i = 7; i >= 1; i-- )
        for ( j = 1; j <= i; j++ )
            {
            Length[sq] = i;
            Where[sq++] = j - 1;
            }

    for ( i = A1; i <= H8; i++ )
        {
        Left54[Left45[i]] = i;
        Left09[Left90[i]] = i;
        Right54[Right45[i]] = i;
        }

    for ( i = A1; i <= H8; i++ )
        {
        SqSet[i] = 0;
        BitSet(i, SqSet[i]);
        SqClear[i] = ~SqSet[i];
        }

    for ( i = A1; i <= H8; i++ )
        {
        SetL90[i] = 0;
        BitSet(Left90[i], SetL90[i]);
        ClearL90[i] = ~SetL90[i];
        SetL45[i] = 0;
        BitSet(Left45[i], SetL45[i]);
        ClearL45[i] = ~SetL45[i];
        SetR45[i] = 0;
        BitSet(Right45[i], SetR45[i]);
        ClearR45[i] = ~SetR45[i];
        }

    for ( i = A1; i <= H8; i++ )
        {
        AttN[i] = 0;

        for ( j = 0; j < 8; j++ )
            {
            sq = i + Hop[j];

            if( (sq < A1) || (sq > H8) )
                continue;

            if( (FileDistance(i, sq) > 2) || (RankDistance(i, sq) > 2) )
                continue;

            BitSet(sq, AttN[i]);
            }
        }

    for ( i = A1; i <= H8; i++ )
        {
        AttK[i] = 0;

        for ( j = A1; j <= H8; j++ )
            {
            if( MAX(FileDistance(i, j), RankDistance(i, j)) == 1 )
                BitSet(j, AttK[i]);
            }
        }

    for ( i = A1; i <= H1; i++ )
        {
        AttPw[i] = 0;
        AttPb[i] = SqSet[i + 7] | SqSet[i + 9];
        }

    for ( i = A2; i <= H7; i++ )
        {
        AttPw[i] = SqSet[i - 7] | SqSet[i - 9];
        AttPb[i] = SqSet[i + 7] | SqSet[i + 9];
        }

    for ( i = A8; i <= H8; i++ )
        {
        AttPb[i] = 0;
        AttPw[i] = SqSet[i - 7] | SqSet[i - 9];
        }

    for ( i = A1; i <= A8; i += 8 )
        {
        AttPw[i] = SqSet[i - 7];
        AttPb[i] = SqSet[i + 9];
        }

    for ( i = H1; i <= H8; i += 8 )
        {
        AttPw[i] = SqSet[i - 9];
        AttPb[i] = SqSet[i + 7];
        }
    AttPw[A1] = 0;
    AttPw[A2] = SqSet[B1];
    AttPb[A7] = SqSet[B8];
    AttPb[A8] = 0;
    AttPw[H1] = 0;
    AttPw[H2] = SqSet[G1];
    AttPb[H7] = SqSet[G8];
    AttPb[H8] = 0;

    IsolatedFiles[FA] = FILEb;
    IsolatedFiles[FH] = FILEg;

    for ( file = FB; file <= FG; file++ )
        IsolatedFiles[file] = FileArray[file - 1] | FileArray[file + 1];

    for ( sq = A1; sq <= H8; sq++ )
        {
        IsolatedPawnW[sq] = 0;
        IsolatedPawnB[sq] = 0;
        file = FILE(sq);
        rank = RANK(sq);

        if( rank < R8 )
            IsolatedPawnW[sq] |= IsolatedFiles[file] & RankArray[rank + 1];

        if( rank < R7 )
            IsolatedPawnW[sq] |= IsolatedFiles[file] & RankArray[rank + 2];

        if( rank > R1 )
            IsolatedPawnB[sq] |= IsolatedFiles[file] & RankArray[rank - 1];

        if( rank > R2 )
            IsolatedPawnB[sq] |= IsolatedFiles[file] & RankArray[rank - 2];
        ConnectedPawns[sq] = IsolatedPawnW[sq] | IsolatedPawnB[sq] | (RankArray[rank]&IsolatedFiles[file]);
        }

    for ( rank = R1; rank <= R8; rank++ )
        {
        InFrontW[rank] = 0;

        for ( j = rank + 1; j <= R8; j++ )
            InFrontW[rank] |= RankArray[j];
        NotInFrontW[rank] = ~InFrontW[rank];
        }

    for ( rank = R8; rank >= R1; rank-- )
        {
        InFrontB[rank] = 0;

        for ( j = rank - 1; j >= R1; j-- )
            InFrontB[rank] |= RankArray[j];
        NotInFrontB[rank] = ~InFrontB[rank];
        }

    for ( u = 0; u < 128; u += 2 )
        for ( file = FA; file <= FH; file++ )
            {
            T = 0;

            if( file < 7 )
                {
                s = 1 << (file + 1);

                while( s < 256 )
                    {
                    T |= s;

                    if( u & s )
                        break;

                    s <<= 1;
                    }
                }

            if( file > 0 )
                {
                s = 1 << (file - 1);

                while( s > 0 )
                    {
                    T |= s;

                    if( u & s )
                        break;

                    s >>= 1;
                    }
                }

            for ( i = 0; i < 8; i++ )
                AttNormal[file + 8 * i][u >> 1] = T << (8 * i);
            }

    for ( sq = A1; sq <= H8; sq++ )
        {
        PassedPawnW[sq] = (IsolatedFiles[FILE(sq)] | FileArray[FILE(sq)]) & InFrontW[RANK(sq)];
        PassedPawnB[sq] = (IsolatedFiles[FILE(sq)] | FileArray[FILE(sq)]) & InFrontB[RANK(sq)];
        }

    for ( sq = A1; sq <= H8; sq++ )
        {
        if( FILE(sq) >= FC )
            Left2[sq] = SqSet[sq - 2];
        else
            Left2[sq] = 0;

        if( FILE(sq) <= FF )
            Right2[sq] = SqSet[sq + 2];
        else
            Right2[sq] = 0;

        if( FILE(sq) >= FB )
            Left1[sq] = SqSet[sq - 1];
        else
            Left1[sq] = 0;

        if( FILE(sq) <= FG )
            Right1[sq] = SqSet[sq + 1];
        else
            Right1[sq] = 0;
        Adjacent[sq] = Left1[sq] | Right1[sq];
        }

    for ( sq = A1; sq <= H8; sq++ )
        {
        ProtectedPawnW[sq] = (IsolatedFiles[FILE(sq)]) &NotInFrontW[RANK(sq)];
        ProtectedPawnB[sq] = (IsolatedFiles[FILE(sq)]) &NotInFrontB[RANK(sq)];
        }

    for ( sq = A1; sq <= H8; sq++ )
        {
        file = FILE(sq);
        rank = RANK(sq);
        LongDiag[sq] = 0;

        if( file <= FD )
            {
            while( file < FH && rank < R8 )
                {
                file++;
                rank++;
                LongDiag[sq] |= SqSet[8 * rank + file];
                }
            file = FILE(sq);
            rank = RANK(sq);

            while( file < FH && rank > R1 )
                {
                file++;
                rank--;
                LongDiag[sq] |= SqSet[8 * rank + file];
                }
            }
        else
            {
            while( file > FA && rank < R8 )
                {
                file--;
                rank++;
                LongDiag[sq] |= SqSet[8 * rank + file];
                }
            file = FILE(sq);
            rank = RANK(sq);

            while( file > FA && rank > R1 )
                {
                file--;
                rank--;
                LongDiag[sq] |= SqSet[8 * rank + file];
                }
            }
        }

    for ( sq = A1; sq <= H8; sq++ )
        OpenFileW[sq] = FileArray[FILE(sq)] & InFrontW[RANK(sq)];

    for ( sq = A1; sq <= H8; sq++ )
        OpenFileB[sq] = FileArray[FILE(sq)] & InFrontB[RANK(sq)];

    for ( sq = A1; sq <= H8; sq++ )
        Doubled[sq] = FileArray[FILE(sq)] ^ (1ULL << sq);

    for ( sq = A1; sq <= H8; sq++ )
        for ( i = 0; i < 64; i++ )
            {
            T = AttNormal[Left90[sq]][i];
            AttLeft90[sq][i] = 0;

            while( T )
                {
                b = LSB(T);
                AttLeft90[sq][i] |= SqSet[Left09[b]];
                BitClear(b, T);
                }
            }

    for ( u = 0; u < 128; u += 2 )
        for ( sq = A1; sq <= H8; sq++ )
            {
            T = 0;
            l = Length[sq];
            w = Where[sq];
            AttRight45[Right54[sq]][u >> 1] = 0;

            if( w < l )
                {
                s = 1 << (w + 1);

                while( s < (1 << l) )
                    {
                    T |= s;

                    if( u & s )
                        break;

                    s <<= 1;
                    }
                }

            if( w > 0 )
                {
                s = 1 << (w - 1);

                while( s > 0 )
                    {
                    T |= s;

                    if( u & s )
                        break;

                    s >>= 1;
                    }
                }
            T <<= (sq - w);

            while( T )
                {
                b = LSB(T);
                AttRight45[Right54[sq]][u >> 1] |= SqSet[Right54[b]];
                BitClear(b, T);
                }
            }

    for ( u = 0; u < 128; u += 2 )
        for ( sq = A1; sq <= H8; sq++ )
            {
            T = 0;
            l = Length[sq];
            w = Where[sq];
            AttLeft45[Left54[sq]][u >> 1] = 0;

            if( w < l )
                {
                s = 1 << (w + 1);

                while( s < (1 << l) )
                    {
                    T |= s;

                    if( u & s )
                        break;

                    s <<= 1;
                    }
                }

            if( w > 0 )
                {
                s = 1 << (w - 1);

                while( s > 0 )
                    {
                    T |= s;

                    if( u & s )
                        break;

                    s >>= 1;
                    }
                }
            T <<= (sq - w);

            while( T )
                {
                b = LSB(T);
                AttLeft45[Left54[sq]][u >> 1] |= SqSet[Left54[b]];
                BitClear(b, T);
                }
            }

#define Distance(i, j) (MAX (FileDistance (i, j), RankDistance (i, j)))

    for ( sq = A1; sq <= H8; sq++ )
        {
        QuadrantBKwtm[sq] = QuadrantBKbtm[sq] = 0;
        j = (sq & 7) + 56;

        if( RANK(sq) == R2 )
            sq2 = sq + 8;
        else
            sq2 = sq;

        for ( i = A1; i <= H8; i++ )
            {
            if( Distance(sq2, j) < Distance(j, i) - 1 )
                BitSet(i, QuadrantBKbtm[sq]);

            if( Distance(sq2, j) < Distance(j, i) )
                BitSet(i, QuadrantBKwtm[sq]);
            }
        }

    for ( sq = A1; sq <= H8; sq++ )
        {
        QuadrantWKwtm[sq] = QuadrantWKbtm[sq] = 0;
        j = (sq & 7);

        if( RANK(sq) == R7 )
            sq2 = sq - 8;
        else
            sq2 = sq;

        for ( i = A1; i <= H8; i++ )
            {
            if( Distance(sq2, j) < Distance(j, i) - 1 )
                BitSet(i, QuadrantWKwtm[sq]);

            if( Distance(sq2, j) < Distance(j, i) )
                BitSet(i, QuadrantWKbtm[sq]);
            }
        }

    for ( sq = A1; sq <= H8; sq++ )
        {
        ShepherdWK[sq] = ShepherdBK[sq] = 0;
        file = FILE(sq);

        if( file == FA || file == FH )
            T = IsolatedFiles[file];
        else
            T = IsolatedFiles[file] | FileArray[file];

        if( RANK(sq) >= R6 )
            ShepherdWK[sq] |= (T &RANK8);

        if( RANK(sq) >= R5 )
            ShepherdWK[sq] |= (T &RANK7);

        if( RANK(sq) <= R3 )
            ShepherdBK[sq] |= (T &RANK1);

        if( RANK(sq) <= R4 )
            ShepherdBK[sq] |= (T &RANK2);
        }

    for ( sq = A1; sq <= H8; sq++ )
        {
        NorthWest[sq] = (RANK(sq) != R8 && FILE(sq) != FA) ? SqSet[sq + 7] : 0;
        NorthEast[sq] = (RANK(sq) != R8 && FILE(sq) != FH) ? SqSet[sq + 9] : 0;
        SouthWest[sq] = (RANK(sq) != R1 && FILE(sq) != FA) ? SqSet[sq - 9] : 0;
        SouthEast[sq] = (RANK(sq) != R1 && FILE(sq) != FH) ? SqSet[sq - 7] : 0;
        }

    for ( sq = A1; sq <= H8; sq++ )
        for ( king = A1; king <= H8; king++ )
            {
            Evade[king][sq] = AttK[king];

            if( RANK(king) == RANK(sq) )
                {
                if( FILE(king) != FA )
                    Evade[king][sq] ^= SqSet[king - 1];

                if( FILE(king) != FH )
                    Evade[king][sq] ^= SqSet[king + 1];
                }

            if( FILE(king) == FILE(sq) )
                {
                if( RANK(king) != R1 )
                    Evade[king][sq] ^= SqSet[king - 8];

                if( RANK(king) != R8 )
                    Evade[king][sq] ^= SqSet[king + 8];
                }

            if( (RANK(king) - RANK(sq)) == (FILE(king) - FILE(sq)) )
                {
                if( RANK(king) != R8 && FILE(king) != FH )
                    Evade[king][sq] ^= SqSet[king + 9];

                if( RANK(king) != R1 && FILE(king) != FA )
                    Evade[king][sq] ^= SqSet[king - 9];
                }

            if( (RANK(king) - RANK(sq)) == (FILE(sq) - FILE(king)) )
                {
                if( RANK(king) != R8 && FILE(king) != FA )
                    Evade[king][sq] ^= SqSet[king + 7];

                if( RANK(king) != R1 && FILE(king) != FH )
                    Evade[king][sq] ^= SqSet[king - 7];
                }

            if( AttK[king] & SqSet[sq] )
                Evade[king][sq] |= SqSet[sq];
            }

    for ( file = FA; file <= FH; file++ )
        {
        FilesLeft[file] = FilesRight[file] = 0;

        for ( i = FA; i < file; i++ )
            FilesLeft[file] |= FileArray[i];

        for ( i = file + 1; i <= FH; i++ )
            FilesRight[file] |= FileArray[i];
        }

    for ( sq = A1; sq <= H8; sq++ )
        for ( king = A1; king <= H8; king++ )
            {
            Interpose[king][sq] = SqSet[sq];
            dir = 0;

            if( RANK(king) == RANK(sq) )
                {
                if( king > sq )
                    dir = 1;
                else
                    dir = -1;
                }

            if( FILE(king) == FILE(sq) )
                {
                if( king > sq )
                    dir = 8;
                else
                    dir = -8;
                }

            if( (RANK(king) - RANK(sq)) == (FILE(king) - FILE(sq)) )
                {
                if( king > sq )
                    dir = 9;
                else
                    dir = -9;
                }

            if( (RANK(king) - RANK(sq)) == (FILE(sq) - FILE(king)) )
                {
                if( king > sq )
                    dir = 7;
                else
                    dir = -7;
                }

            if( dir )
                for ( i = sq; i != king; i += dir )
                    BitSet(i, Interpose[king][sq]);
            }

    for ( sq = A1; sq <= H8; sq++ )
        {
        Ortho[sq] = RankArray[RANK(sq)] | FileArray[FILE(sq)];
        Diag[sq] = 0;

        for ( file = FILE(sq), rank = RANK(sq); file <= FH && rank <= R8; file++, rank++ )
            BitSet(8 * rank + file, Diag[sq]);

        for ( file = FILE(sq), rank = RANK(sq); file <= FH && rank >= R1; file++, rank-- )
            BitSet(8 * rank + file, Diag[sq]);

        for ( file = FILE(sq), rank = RANK(sq); file >= FA && rank <= R8; file--, rank++ )
            BitSet(8 * rank + file, Diag[sq]);

        for ( file = FILE(sq), rank = RANK(sq); file >= FA && rank >= R1; file--, rank-- )
            BitSet(8 * rank + file, Diag[sq]);
        Ortho[sq] &= SqClear[sq];
        Diag[sq] &= SqClear[sq];
        NonOrtho[sq] = ~Ortho[sq];
        NonDiag[sq] = ~Diag[sq];
        OrthoDiag[sq] = Ortho[sq] | Diag[sq];
        }

    for ( j = A1; j <= H8; j++ )
        for ( i = A1; i <= H8; i++ )
            {
            Line[i][j] = BadDirection;

            if( i == j )
                continue;

            if( RANK(j) == RANK(i) )
                Line[i][j] = Direction_horz;

            if( FILE(j) == FILE(i) )
                Line[i][j] = Direction_vert;

            if( (FILE(i) - FILE(j)) == (RANK(i) - RANK(j)) )
                Line[i][j] = Direction_a1h8;

            if( (FILE(j) - FILE(i)) == (RANK(i) - RANK(j)) )
                Line[i][j] = Direction_h1a8;
            }

    InitZobrist();
    }
