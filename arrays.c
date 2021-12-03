#include "firebird.h"

const uint8 Left90[64] =
  {
    7, 15, 23, 31, 39, 47, 55, 63,
    6, 14, 22, 30, 38, 46, 54, 62,
    5, 13, 21, 29, 37, 45, 53, 61,
    4, 12, 20, 28, 36, 44, 52, 60,
    3, 11, 19, 27, 35, 43, 51, 59,
    2, 10, 18, 26, 34, 42, 50, 58,
    1, 9, 17, 25, 33, 41, 49, 57,
    0, 8, 16, 24, 32, 40, 48, 56
  };

const uint8 Left45[64] =
  {
    0, 2, 5, 9, 14, 20, 27, 35,
    1, 4, 8, 13, 19, 26, 34, 42,
    3, 7, 12, 18, 25, 33, 41, 48,
    6, 11, 17, 24, 32, 40, 47, 53,
    10, 16, 23, 31, 39, 46, 52, 57,
    15, 22, 30, 38, 45, 51, 56, 60,
    21, 29, 37, 44, 50, 55, 59, 62,
    28, 36, 43, 49, 54, 58, 61, 63
  };

const uint8 Right45[64] =
  {
    28, 21, 15, 10, 6, 3, 1, 0,
    36, 29, 22, 16, 11, 7, 4, 2,
    43, 37, 30, 23, 17, 12, 8, 5,
    49, 44, 38, 31, 24, 18, 13, 9,
    54, 50, 45, 39, 32, 25, 19, 14,
    58, 55, 51, 46, 40, 33, 26, 20,
    61, 59, 56, 52, 47, 41, 34, 27,
    63, 62, 60, 57, 53, 48, 42, 35
  };

static int LEFT54[64], RIGHT54[64], LEFT09[64];
static int SHIFT[64] =
  {
    1,
    2, 2,
    4, 4, 4,
    7, 7, 7, 7,
    11, 11, 11, 11, 11,
    16, 16, 16, 16, 16, 16,
    22, 22, 22, 22, 22, 22, 22,
    29, 29, 29, 29, 29, 29, 29, 29,
    37, 37, 37, 37, 37, 37, 37,
    44, 44, 44, 44, 44, 44,
    50, 50, 50, 50, 50,
    55, 55, 55, 55,
    59, 59, 59,
    62, 62,
    64
  };
static int LENGTH[64], WHERE[64];
static int HOP[8] =
    {
    6, 10, 15, 17, -6, -10, -15, -17
    };

static uint64 randkey = 1;
uint16 RAND16()
    {
    randkey = randkey * 8765432181103515245ULL + 1234567891ULL;
    return ((randkey >> 32) % 65536);
    }
static uint64 GET_RAND()
    {
	return (((uint64) RAND16()) << 48) |
		(((uint64) RAND16()) << 32) |
		(((uint64) RAND16()) << 16) |
		(((uint64) RAND16()) << 0);
    }

extern void InitRandom32( uint64 );
void InitZobrist()
    {
    int i, j;
    ZobristWTM = GET_RAND();
    ZobristCastling[0] = 0;
    ZobristCastling[1] = GET_RAND();
    ZobristCastling[2] = GET_RAND();
    ZobristCastling[4] = GET_RAND();
    ZobristCastling[8] = GET_RAND();

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
            HASH[i][j] = GET_RAND();

    for ( i = FA; i <= FH; i++ )
        ZobristEP[i] = GET_RAND();
    InitRandom32(GET_RAND());
    }

void InitArrays()
    {
    int sq2, l, w, i, sq = 0, j, u, co, trans, king, dir;
    uint64 T, b, s;

    for ( i = A1; i <= H8; i++ )
        {
        ShiftLeft45[i] = SHIFT[Left45[i]];
        ShiftRight45[i] = SHIFT[Right45[i]];
        }

    for ( i = A1; i <= H8; i++ )
        {
        ShiftAttack[i] = 1 + (i & 56);
        ShiftLeft90[i] = 1 + (Left90[i] & 56);
        }

    for ( i = 1; i <= 8; i++ )
        for ( j = 1; j <= i; j++ )
            {
            LENGTH[sq] = i;
            WHERE[sq++] = j - 1;
            }

    for ( i = 7; i >= 1; i-- )
        for ( j = 1; j <= i; j++ )
            {
            LENGTH[sq] = i;
            WHERE[sq++] = j - 1;
            }

    for ( i = A1; i <= H8; i++ )
        {
        LEFT54[Left45[i]] = i;
        LEFT09[Left90[i]] = i;
        RIGHT54[Right45[i]] = i;
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
            sq = i + HOP[j];

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

    for ( co = FB; co <= FG; co++ )
        IsolatedFiles[co] = FileArray[co - 1] | FileArray[co + 1];

    for ( sq = A1; sq <= H8; sq++ )
        {
        IsolatedPawnW[sq] = 0;
        IsolatedPawnB[sq] = 0;
        co = FILE(sq);
        trans = RANK(sq);

        if( trans < R8 )
            IsolatedPawnW[sq] |= IsolatedFiles[co] & RankArray[trans + 1];

        if( trans < R7 )
            IsolatedPawnW[sq] |= IsolatedFiles[co] & RankArray[trans + 2];

        if( trans > R1 )
            IsolatedPawnB[sq] |= IsolatedFiles[co] & RankArray[trans - 1];

        if( trans > R2 )
            IsolatedPawnB[sq] |= IsolatedFiles[co] & RankArray[trans - 2];
        ConnectedPawns[sq] = IsolatedPawnW[sq] | IsolatedPawnB[sq] | (RankArray[trans]&IsolatedFiles[co]);
        }

    for ( trans = R1; trans <= R8; trans++ )
        {
        InFrontW[trans] = 0;

        for ( j = trans + 1; j <= R8; j++ )
            InFrontW[trans] |= RankArray[j];
        NotInFrontW[trans] = ~InFrontW[trans];
        }

    for ( trans = R8; trans >= R1; trans-- )
        {
        InFrontB[trans] = 0;

        for ( j = trans - 1; j >= R1; j-- )
            InFrontB[trans] |= RankArray[j];
        NotInFrontB[trans] = ~InFrontB[trans];
        }

    for ( u = 0; u < 128; u += 2 )
        for ( co = FA; co <= FH; co++ )
            {
            T = 0;

            if( co < 7 )
                {
                s = 1 << (co + 1);

                while( s < 256 )
                    {
                    T |= s;

                    if( u & s )
                        break;

                    s <<= 1;
                    }
                }

            if( co > 0 )
                {
                s = 1 << (co - 1);

                while( s > 0 )
                    {
                    T |= s;

                    if( u & s )
                        break;

                    s >>= 1;
                    }
                }

            for ( i = 0; i < 8; i++ )
                AttNormal[co + 8 * i][u >> 1] = T << (8 * i);
            }

    for ( sq = A1; sq <= H8; sq++ )
        {
        PassedPawnW[sq] = (IsolatedFiles[FILE(sq)] | FileArray[FILE(sq)]) & InFrontW[RANK(sq)];
        PassedPawnB[sq] = (IsolatedFiles[FILE(sq)] | FileArray[FILE(sq)]) & InFrontB[RANK(sq)];
        }

    for ( sq = A1; sq <= H8; sq++ )
        {
        if( FILE(sq) >= FC )
            LEFT2[sq] = SqSet[sq - 2];
        else
            LEFT2[sq] = 0;

        if( FILE(sq) <= FF )
            RIGHT2[sq] = SqSet[sq + 2];
        else
            RIGHT2[sq] = 0;

        if( FILE(sq) >= FB )
            LEFT1[sq] = SqSet[sq - 1];
        else
            LEFT1[sq] = 0;

        if( FILE(sq) <= FG )
            RIGHT1[sq] = SqSet[sq + 1];
        else
            RIGHT1[sq] = 0;
        ADJACENT[sq] = LEFT1[sq] | RIGHT1[sq];
        }

    for ( sq = A1; sq <= H8; sq++ )
        {
        ProtectedPawnW[sq] = (IsolatedFiles[FILE(sq)]) &NotInFrontW[RANK(sq)];
        ProtectedPawnB[sq] = (IsolatedFiles[FILE(sq)]) &NotInFrontB[RANK(sq)];
        }

    for ( sq = A1; sq <= H8; sq++ )
        {
        co = FILE(sq);
        trans = RANK(sq);
        LONG_DIAG[sq] = 0;

        if( co <= FD )
            {
            while( co < FH && trans < R8 )
                {
                co++;
                trans++;
                LONG_DIAG[sq] |= SqSet[8 * trans + co];
                }
            co = FILE(sq);
            trans = RANK(sq);

            while( co < FH && trans > R1 )
                {
                co++;
                trans--;
                LONG_DIAG[sq] |= SqSet[8 * trans + co];
                }
            }
        else
            {
            while( co > FA && trans < R8 )
                {
                co--;
                trans++;
                LONG_DIAG[sq] |= SqSet[8 * trans + co];
                }
            co = FILE(sq);
            trans = RANK(sq);

            while( co > FA && trans > R1 )
                {
                co--;
                trans--;
                LONG_DIAG[sq] |= SqSet[8 * trans + co];
                }
            }
        }

    for ( sq = A1; sq <= H8; sq++ )
        OpenFileW[sq] = FileArray[FILE(sq)] & InFrontW[RANK(sq)];

    for ( sq = A1; sq <= H8; sq++ )
        OpenFileB[sq] = FileArray[FILE(sq)] & InFrontB[RANK(sq)];

    for ( sq = A1; sq <= H8; sq++ )
        DOUBLED[sq] = FileArray[FILE(sq)] ^ (1ULL << sq);

    for ( sq = A1; sq <= H8; sq++ )
        for ( i = 0; i < 64; i++ )
            {
            T = AttNormal[Left90[sq]][i];
            AttLeft90[sq][i] = 0;

            while( T )
                {
                b = LSB(T);
                AttLeft90[sq][i] |= SqSet[LEFT09[b]];
                BitClear(b, T);
                }
            }

    for ( u = 0; u < 128; u += 2 )
        for ( sq = A1; sq <= H8; sq++ )
            {
            T = 0;
            l = LENGTH[sq];
            w = WHERE[sq];
            AttRight45[RIGHT54[sq]][u >> 1] = 0;

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
                AttRight45[RIGHT54[sq]][u >> 1] |= SqSet[RIGHT54[b]];
                BitClear(b, T);
                }
            }

    for ( u = 0; u < 128; u += 2 )
        for ( sq = A1; sq <= H8; sq++ )
            {
            T = 0;
            l = LENGTH[sq];
            w = WHERE[sq];
            AttLeft45[LEFT54[sq]][u >> 1] = 0;

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
                AttLeft45[LEFT54[sq]][u >> 1] |= SqSet[LEFT54[b]];
                BitClear(b, T);
                }
            }

#define DISTANCE(i, j) (MAX (FileDistance (i, j), RankDistance (i, j)))

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
            if( DISTANCE(sq2, j) < DISTANCE(j, i) - 1 )
                BitSet(i, QuadrantBKbtm[sq]);

            if( DISTANCE(sq2, j) < DISTANCE(j, i) )
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
            if( DISTANCE(sq2, j) < DISTANCE(j, i) - 1 )
                BitSet(i, QuadrantWKwtm[sq]);

            if( DISTANCE(sq2, j) < DISTANCE(j, i) )
                BitSet(i, QuadrantWKbtm[sq]);
            }
        }

    for ( sq = A1; sq <= H8; sq++ )
        {
        ShepherdWK[sq] = ShepherdBK[sq] = 0;
        co = FILE(sq);

        if( co == FA || co == FH )
            T = IsolatedFiles[co];
        else
            T = IsolatedFiles[co] | FileArray[co];

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
        NORTHWEST[sq] = (RANK(sq) != R8 && FILE(sq) != FA) ? SqSet[sq + 7] : 0;
        NORTHEAST[sq] = (RANK(sq) != R8 && FILE(sq) != FH) ? SqSet[sq + 9] : 0;
        SOUTHWEST[sq] = (RANK(sq) != R1 && FILE(sq) != FA) ? SqSet[sq - 9] : 0;
        SOUTHEAST[sq] = (RANK(sq) != R1 && FILE(sq) != FH) ? SqSet[sq - 7] : 0;
        }

    for ( sq = A1; sq <= H8; sq++ )
        for ( king = A1; king <= H8; king++ )
            {
            EVADE[king][sq] = AttK[king];

            if( RANK(king) == RANK(sq) )
                {
                if( FILE(king) != FA )
                    EVADE[king][sq] ^= SqSet[king - 1];

                if( FILE(king) != FH )
                    EVADE[king][sq] ^= SqSet[king + 1];
                }

            if( FILE(king) == FILE(sq) )
                {
                if( RANK(king) != R1 )
                    EVADE[king][sq] ^= SqSet[king - 8];

                if( RANK(king) != R8 )
                    EVADE[king][sq] ^= SqSet[king + 8];
                }

            if( (RANK(king) - RANK(sq)) == (FILE(king) - FILE(sq)) )
                {
                if( RANK(king) != R8 && FILE(king) != FH )
                    EVADE[king][sq] ^= SqSet[king + 9];

                if( RANK(king) != R1 && FILE(king) != FA )
                    EVADE[king][sq] ^= SqSet[king - 9];
                }

            if( (RANK(king) - RANK(sq)) == (FILE(sq) - FILE(king)) )
                {
                if( RANK(king) != R8 && FILE(king) != FA )
                    EVADE[king][sq] ^= SqSet[king + 7];

                if( RANK(king) != R1 && FILE(king) != FH )
                    EVADE[king][sq] ^= SqSet[king - 7];
                }

            if( AttK[king] & SqSet[sq] )
                EVADE[king][sq] |= SqSet[sq];
            }

    for ( co = FA; co <= FH; co++ )
        {
        FilesLeft[co] = FilesRight[co] = 0;

        for ( i = FA; i < co; i++ )
            FilesLeft[co] |= FileArray[i];

        for ( i = co + 1; i <= FH; i++ )
            FilesRight[co] |= FileArray[i];
        }

    for ( sq = A1; sq <= H8; sq++ )
        for ( king = A1; king <= H8; king++ )
            {
            INTERPOSE[king][sq] = SqSet[sq];
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
                    BitSet(i, INTERPOSE[king][sq]);
            }

    for ( sq = A1; sq <= H8; sq++ )
        {
        ORTHO[sq] = RankArray[RANK(sq)] | FileArray[FILE(sq)];
        DIAG[sq] = 0;

        for ( co = FILE(sq), trans = RANK(sq); co <= FH && trans <= R8; co++, trans++ )
            BitSet(8 * trans + co, DIAG[sq]);

        for ( co = FILE(sq), trans = RANK(sq); co <= FH && trans >= R1; co++, trans-- )
            BitSet(8 * trans + co, DIAG[sq]);

        for ( co = FILE(sq), trans = RANK(sq); co >= FA && trans <= R8; co--, trans++ )
            BitSet(8 * trans + co, DIAG[sq]);

        for ( co = FILE(sq), trans = RANK(sq); co >= FA && trans >= R1; co--, trans-- )
            BitSet(8 * trans + co, DIAG[sq]);
        ORTHO[sq] &= SqClear[sq];
        DIAG[sq] &= SqClear[sq];
        NON_ORTHO[sq] = ~ORTHO[sq];
        NON_DIAG[sq] = ~DIAG[sq];
        ORTHO_DIAG[sq] = ORTHO[sq] | DIAG[sq];
        }

    for ( j = A1; j <= H8; j++ )
        for ( i = A1; i <= H8; i++ )
            {
            LINE[i][j] = BAD_DIRECTION;

            if( i == j )
                continue;

            if( RANK(j) == RANK(i) )
                LINE[i][j] = Direction_horz;

            if( FILE(j) == FILE(i) )
                LINE[i][j] = Direction_vert;

            if( (FILE(i) - FILE(j)) == (RANK(i) - RANK(j)) )
                LINE[i][j] = Direction_a1h8;

            if( (FILE(j) - FILE(i)) == (RANK(i) - RANK(j)) )
                LINE[i][j] = Direction_h1a8;
            }

    InitZobrist();
    }
